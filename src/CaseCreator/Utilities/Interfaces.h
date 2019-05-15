#ifndef INTERFACES_H
#define INTERFACES_H

#include "Vector2.h"
#include "Rectangle.h"

#include "CaseCreator/UIComponents/MouseOverText.h"

#include <QObject>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsItem>
#include <QList>
#include <QString>
#include <QUndoCommand>
#include <QUndoStack>
#include <QWidget>

#include <limits>

class ICaseLoadedCallback
{
public:
    virtual void CaseLoaded() = 0;
};

class UndoCommandSink
{
public:
    void AddUndoCommand(QUndoCommand *pCommand)
    {
        undoStack.push(pCommand);
    }

    bool Undo()
    {
        if (undoStack.canUndo())
        {
            undoStack.undo();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Redo()
    {
        if (undoStack.canRedo())
        {
            undoStack.redo();
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    QUndoStack undoStack;
};

class ObjectManipulatorSlots;

class IManipulatableObject : public QObject
{
    Q_OBJECT

public:
    virtual ObjectManipulatorSlots * GetManipulationWidget() = 0;

signals:
    void PropertyChanged(QString propertyName);
};

template<class T>
IManipulatableObject * GetManipulatableObjectFrom(T *pObject);

class IManipulatable;

// The way in which this class is linked together is fairly complex,
// but it's set up so that it will all more or less work auto-magically
// without having to manually set things up in a way where something
// could be missed and cause problems down the line.
// This comment will explain how it all works together.
//
// All instances of IDrawable contain references to their parents
// in the logical visual tree, if one exists.  For example,
// a Location-linked drawable will have a ForegroundElement-linked drawable,
// which in turn will have a ForegroundElementAnimation-linked drawable,
// and finally that will have an Animation-linked drawable.
//
// IDrawable contains overrideable methods that allow a drawable to specify
// both its position relative to its parent and its absolute z-order.
// By default, its position relative to its parent is (0, 0)
// (i.e., the same location as its parent), whereas its z-order defaults
// either to its parent's z-order, or if no parent exists, the minimum possible z-order
// (i.e., drawn underneath everything else).
//
// However, these can be overridden by implementers of this interface
// that need to specify values other than this.
//
// In addition to that, IDrawable also supports both animations and manual updates of state.
// If a subclass of IDrawable sets a nonzero value for msTimeoutDuration, that will cause
// IDrawable, once it draws itself, to automatically set a timer that will call HandleTimeout()
// once msTimeoutDuration milliseconds elapses, which will call UpdateAfterTimeout() and then Draw().
// UpdateAfterTimeout() is a method that can be overridden, and is provided to enable subclasses
// to easily specify what should happen after a timeout happens.
// Subclasses that set msTimeoutDuration should also always override UpdateAfterTimeout().
//
// Finally, Draw(), which is used to add a drawable's drawable content to a QGraphicsScene,
// also stores the last QGraphicsScene it received, as well as accumulating parent positions
// down the logical visual tree.  This ensures that when Draw() is called on a drawable,
// it knows exactly where its parents are, so if we update a child element, we can simply
// re-draw it exactly where it was before without needing to know anything about its parents.
// This way, for example, if a user clicks and drags to move a ForegroundElementAnimation
// in a ForegroundElement, we can simply draw the ForegroundElementAnimation at its new position
// within the ForegroundElement without needing to worry about where the ForegroundElement is located.
class IDrawable : public QObject
{
    Q_OBJECT

public:
    IDrawable(IDrawable *pParent)
    {
        this->pParent = pParent;

        msTimeoutDuration = 0;
        pendingMsTimeoutDuration = 0;

        pLastScene = NULL;
        cumulativeOffsetVector = Vector2();

        lastGraphicsItems.clear();

        updateTimer.setSingleShot(true);
        QObject::connect(&updateTimer, SIGNAL(timeout()), this, SLOT(HandleTimeout()));

        timerCallbackConnected = true;
        hasBeenDrawn = false;
        updatesBlocked = false;
    }

    virtual ~IDrawable()
    {
        Reset();
    }

    void Draw(QGraphicsScene *pScene)
    {
        if (pScene == NULL)
        {
            return;
        }

        if (!timerCallbackConnected)
        {
            QObject::connect(&updateTimer, SIGNAL(timeout()), this, SLOT(HandleTimeout()));
            timerCallbackConnected = true;

            if (msTimeoutDuration == 0 && pendingMsTimeoutDuration > 0)
            {
                msTimeoutDuration = pendingMsTimeoutDuration;
            }
        }

        pLastScene = pScene;

        QList<QGraphicsItem *> graphicsItemsToRemove = lastGraphicsItems;
        lastGraphicsItems.clear();

        for (QGraphicsItem *pItem : graphicsItemsToRemove)
        {
            pScene->removeItem(pItem);
        }

        DrawCore(pScene, lastGraphicsItems);
        DrawChildren(pScene);

        for (QGraphicsItem *pItem : graphicsItemsToRemove)
        {
            if (!lastGraphicsItems.contains(pItem))
            {
                delete pItem;
            }
        }

        hasBeenDrawn = true;

        Update();
    }

    void Redraw()
    {
        if (pLastScene != NULL)
        {
            Draw(pLastScene);
        }
    }

    void DrawAdditionalItem(IDrawable *pDrawable)
    {
        if (pLastScene != NULL)
        {
            pDrawable->Draw(pLastScene);
        }
    }

    void RemoveItem(IDrawable *pDrawable)
    {
        if (pLastScene != NULL)
        {
            pDrawable->Draw(pLastScene);
        }
    }

    void Update()
    {
        if (!hasBeenDrawn || updatesBlocked)
        {
            return;
        }

        cumulativeOffsetVector = GetPosition() + GetParentPosition();

        for (QGraphicsItem *pItem : lastGraphicsItems)
        {
            qreal scale = GetScale();
            QPointF translation = (cumulativeOffsetVector * scale).ToQPointF();
            qreal xAdjustment = scale * (GetFlipHorizontal() ? pItem->boundingRect().x() + pItem->boundingRect().width() : 0);

            QTransform transform;
            transform.setMatrix(GetFlipHorizontal() ? -scale : scale, 0, 0,
                                0, scale, 0,
                                translation.x() + xAdjustment, translation.y(), 1);

            pItem->setTransform(transform);

            pItem->setZValue(GetZOrder());
            pItem->setOpacity(GetOpacity());
        }

        UpdateCore();
        UpdateChildren();

        if (msTimeoutDuration > 0)
        {
            updateTimer.start(msTimeoutDuration);

            // We've started the timer, so we'll set msTimeoutDuration to 0
            // so we don't start it again until it gets reset by the object
            // subclassing us.
            // We'll save the value of msTimeoutDuration until the timeout occurs
            // in case we need to reset and then redraw, necessitating a timeout reset.
            pendingMsTimeoutDuration = msTimeoutDuration;
            msTimeoutDuration = 0;
        }
    }

    void Reset()
    {
        hasBeenDrawn = false;

        if (timerCallbackConnected)
        {
            QObject::disconnect(&updateTimer, SIGNAL(timeout()), this, SLOT(HandleTimeout()));
            timerCallbackConnected = false;
        }

        msTimeoutDuration = 0;
        cumulativeOffsetVector = Vector2();

        for (QGraphicsItem *pItem : lastGraphicsItems)
        {
            if (pLastScene != NULL)
            {
                pLastScene->removeItem(pItem);
            }

            delete pItem;
        }

        pLastScene = NULL;
        lastGraphicsItems.clear();

        ResetChildren();
    }

    void ClearLastGraphicsItems()
    {
        lastGraphicsItems.clear();
    }

    virtual Vector2 GetPosition()
    {
        return Vector2();
    }

    virtual RectangleWH GetBoundingRect()
    {
        // Default to a 20x20 rectangle to ensure that there will always be *something* to manipulate.
        return RectangleWH(0, 0, 20, 20);
    }

    virtual qreal GetZOrder()
    {
        if (pParent != NULL)
        {
            return pParent->GetZOrder();
        }

        return std::numeric_limits<qreal>::min();
    }

    virtual qreal GetOpacity()
    {
        if (pParent != NULL)
        {
            return pParent->GetOpacity();
        }

        return 1.0;
    }

    virtual bool GetFlipHorizontal()
    {
        if (pParent != NULL)
        {
            return pParent->GetFlipHorizontal();
        }

        return false;
    }

    virtual qreal GetScale()
    {
        if (pParent != NULL)
        {
            return pParent->GetScale();
        }

        return 1.0;
    }

    void BlockUpdates()
    {
        updatesBlocked = true;
    }

    void AllowUpdates()
    {
        updatesBlocked = false;
    }

signals:
    // These are needed to support subclasses - they must go here because
    // Qt does not support virtual inheritance involving QObject.
    void SelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);
    void PositionOverrideChanged(Vector2 newPositionOverride);
    void WindowRectChanged(RectangleWH newWindowRect);
    void CameraRectChanged(RectangleWH newCameraRect);
    void CharacterPositionChanged(Vector2 newCharacterPosition);

public slots:
    void HandleTimeout()
    {
        pendingMsTimeoutDuration = 0;

        UpdateOnTimeout();
        Update();
    }

public slots:
    // These are needed to support subclasses - they must go here because
    // Qt does not support virtual inheritance involving QObject.
    void CameraManipulatableWindowRectChanged(RectangleWH newWindowRect) { CameraManipulatableWindowRectChangedImpl(newWindowRect); }
    void CharacterManipulatablePositionOverrideChanged(Vector2 newPositionOverride) { CharacterManipulatablePositionOverrideChangedImpl(newPositionOverride); }

protected:
    virtual void CameraManipulatableWindowRectChangedImpl(RectangleWH /*newWindowRect*/) { }
    virtual void CharacterManipulatablePositionOverrideChangedImpl(Vector2 /*newPositionOverride*/) { }

protected:
    virtual void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems) = 0;
    virtual void DrawChildren(QGraphicsScene *) { }
    virtual void UpdateCore() { }
    virtual void UpdateChildren() { }
    virtual void UpdateOnTimeout() { }
    virtual void ResetChildren() { }

    IDrawable *pParent;

    int msTimeoutDuration;
    int pendingMsTimeoutDuration;

private:
    Vector2 GetParentPosition()
    {
        if (pParent != NULL)
        {
            return pParent->cumulativeOffsetVector;
        }

        return Vector2();
    }

    QGraphicsScene *pLastScene;
    Vector2 cumulativeOffsetVector;

    QList<QGraphicsItem *> lastGraphicsItems;

    QTimer updateTimer;

    bool timerCallbackConnected;
    bool hasBeenDrawn;
    bool updatesBlocked;
};

template <class T>
class IDrawingView : virtual public IDrawable
{
protected:
    IDrawingView(T *pObject, IDrawable *pParent)
        : IDrawable(pParent)
    {
        this->pObject = pObject;
    }

    virtual ~IDrawingView() { }

    T *pObject;
};

class IManipulatable : virtual public IDrawable
{
    friend class IManipulationSurface;

public:
    IManipulatable(IDrawable *pParent, UndoCommandSink *pUndoCommandSink = NULL)
        : IDrawable(pParent)
    {
        this->pUndoCommandSink = pUndoCommandSink;
        this->isEnabled = true;
    }

    virtual QString GetMouseOverText()
    {
        return "";
    }

    virtual bool GetShouldDimIfDisabled()
    {
        return true;
    }

    bool IsEnabled()
    {
        return isEnabled;
    }

    void SetIsEnabled(bool isEnabled)
    {
        this->isEnabled = isEnabled;
    }

    virtual qreal GetOpacity()
    {
        if (IsEnabled() || !GetShouldDimIfDisabled())
        {
            return IDrawable::GetOpacity();
        }
        else
        {
            return 0.333;
        }
    }

protected:
    virtual void OnGotFocus() { }
    virtual void OnLostFocus() { }

    virtual void OnMouseEnter() { }
    virtual void OnMouseLeave() { }

    virtual void OnMousePress() { }
    virtual void OnMouseDrag(Vector2) { }
    virtual void OnMouseRelease() { }

    virtual void OnInteractedWith() { }

    virtual bool IsHitTestable()
    {
        return IsEnabled();
    }

    virtual IManipulatableObject * GetManipulatableObject()
    {
        return NULL;
    }

    void AddUndoCommand(QUndoCommand *pUndoCommand)
    {
        if (pUndoCommandSink != NULL)
        {
            pUndoCommandSink->AddUndoCommand(pUndoCommand);
        }
    }

private:
    bool IsPointWithinBounds(Vector2 point)
    {
        return (GetBoundingRect() + GetPosition()).ContainsPoint(point);
    }

    UndoCommandSink *pUndoCommandSink;
    bool isEnabled;
};

template <class T>
class IManipulationView : public IDrawingView<T>, public IManipulatable
{
protected:
    IManipulationView(T *pObject, IDrawable *pParent, UndoCommandSink *pUndoCommandSink = NULL)
        : IDrawable(pParent)
        , IDrawingView<T>(pObject, pParent)
        , IManipulatable(pParent, pUndoCommandSink)
    {
    }

    virtual ~IManipulationView() { }

public:
    virtual void SetPosition(Vector2 position) = 0;
    virtual void SetPositionOverride(Vector2 /*position*/) { }

protected:
    virtual IManipulatableObject * GetManipulatableObject()
    {
        return GetManipulatableObjectFrom(this->pObject);
    }

    class MoveUndoCommand : public QUndoCommand
    {
    public:
        MoveUndoCommand(IManipulationView<T> *pManipulationView, Vector2 oldPosition, Vector2 newPosition, bool isPositionOverride = false)
            : pManipulationView(pManipulationView)
            , oldPosition(oldPosition)
            , newPosition(newPosition)
            , isPositionOverride(isPositionOverride)
        {
        }

        void undo()
        {
            if (isPositionOverride)
            {
                pManipulationView->SetPositionOverride(oldPosition);
            }
            else
            {
                pManipulationView->SetPosition(oldPosition);
            }
        }

        void redo()
        {
            if (isPositionOverride)
            {
                pManipulationView->SetPositionOverride(newPosition);
            }
            else
            {
                pManipulationView->SetPosition(newPosition);
            }
        }

    private:
        IManipulationView<T> *pManipulationView;

        Vector2 oldPosition;
        Vector2 newPosition;

        bool isPositionOverride;
    };
};

class IManipulationSurface : virtual public IDrawable, public UndoCommandSink
{
protected:
    IManipulationSurface(IDrawable *pParent)
        : IDrawable(pParent)
    {
        pMouseOverText = new MouseOverText();

        pCurrentHoverElement = NULL;
        pCurrentSelectedElement = NULL;
        pLastSelectedElement = NULL;
    }

    virtual ~IManipulationSurface()
    {
        delete pMouseOverText;
        pMouseOverText = NULL;
    }

public:
    virtual void OnMousePress(Vector2 position)
    {
        lastMousePosition = position;
        IManipulatable *pHitElement = DoHitTest(position);

        if (pCurrentSelectedElement != NULL)
        {
            if (pCurrentSelectedElement == pHitElement)
            {
                pCurrentSelectedElement->OnMousePress();
                return;
            }
            else
            {
                pCurrentSelectedElement->OnLostFocus();
            }
        }

        pCurrentSelectedElement = pHitElement;

        if (pCurrentSelectedElement != NULL)
        {
            pCurrentSelectedElement->OnGotFocus();
            pCurrentSelectedElement->OnMousePress();
        }
    }

    virtual void OnMouseMove(Vector2 position, bool isMouseDown)
    {
        Vector2 delta = position - lastMousePosition;
        lastMousePosition = position;

        if (isMouseDown)
        {
            pMouseOverText->SetText("");

            if (pCurrentSelectedElement != NULL)
            {
                pCurrentSelectedElement->OnMouseDrag(delta);
            }
        }
        else
        {
            IManipulatable *pMouseOverElement = DoHitTest(position);

            if (pMouseOverElement != NULL && pMouseOverElement->GetMouseOverText().length() > 0)
            {
                pMouseOverText->SetText(pMouseOverElement->GetMouseOverText());
                pMouseOverText->SetPositionFromMousePosition(position);
            }
            else
            {
                pMouseOverText->SetText("");
            }

            if (pCurrentHoverElement == pMouseOverElement)
            {
                return;
            }

            if (pCurrentHoverElement != NULL)
            {
                pCurrentHoverElement->OnMouseLeave();
            }

            pCurrentHoverElement = pMouseOverElement;

            if (pCurrentHoverElement != NULL)
            {
                pCurrentHoverElement->OnMouseEnter();
            }
        }
    }

    virtual void OnMouseRelease(Vector2)
    {
        if (pCurrentSelectedElement != NULL)
        {
            pCurrentSelectedElement->OnMouseRelease();
        }

        if (pCurrentSelectedElement != pLastSelectedElement)
        {
            emit SelectionChanged(
                        pCurrentSelectedElement,
                        pCurrentSelectedElement != NULL ? pCurrentSelectedElement->GetManipulatableObject() : NULL);

            pLastSelectedElement = pCurrentSelectedElement;
        }
    }

    virtual void OnMouseEnter()
    {
    }

    virtual void OnMouseLeave()
    {
        pMouseOverText->SetText("");

        if (pCurrentHoverElement != NULL)
        {
            pCurrentHoverElement->OnMouseLeave();
        }

        pCurrentHoverElement = NULL;
    }

    virtual void OnMouseDoubleClick(Vector2 position)
    {
        IManipulatable *pHitElement = DoHitTest(position);

        if (pHitElement != NULL)
        {
            pHitElement->OnInteractedWith();
        }
    }

protected:
    virtual void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
    {
        pMouseOverText->Draw(pScene, addedItems);
    }

    virtual void UpdateCore()
    {
        pMouseOverText->Update();
    }

    virtual QList<IManipulatable *> & GetManipulatableList() = 0;

private:
    static bool ManipulatableOrderDescending(IManipulatable *p1, IManipulatable *p2)
    {
        return p1->GetZOrder() > p2->GetZOrder();
    }

    IManipulatable * DoHitTest(Vector2 point)
    {
        QList<IManipulatable *> orderedManipulationViewList = GetManipulatableList();
        IManipulatable *pHitElement = NULL;

        qSort(orderedManipulationViewList.begin(), orderedManipulationViewList.end(), IManipulationSurface::ManipulatableOrderDescending);

        // We want to hit-test the smallest element that is under our mouse.
        // Otherwse, since we'll be manipulating things with the mouse, it'll
        // be hard to get at the smaller elements underneath the bigger elements.
        int minArea = INT_MAX;

        for (IManipulatable *pManipulatable : orderedManipulationViewList)
        {
            if (pManipulatable->IsHitTestable() && pManipulatable->IsPointWithinBounds(point))
            {
                int area = pManipulatable->GetBoundingRect().GetWidth() * pManipulatable->GetBoundingRect().GetHeight();

                if (area < minArea)
                {
                    minArea = area;
                }
            }
        }

        for (IManipulatable *pManipulatable : orderedManipulationViewList)
        {
            if (pManipulatable->IsHitTestable() && pManipulatable->IsPointWithinBounds(point))
            {
                int area = pManipulatable->GetBoundingRect().GetWidth() * pManipulatable->GetBoundingRect().GetHeight();

                if (area == minArea)
                {
                    pHitElement = pManipulatable;
                    break;
                }
            }
        }

        return pHitElement;
    }

private:
    MouseOverText *pMouseOverText;

    IManipulatable *pCurrentHoverElement;
    IManipulatable *pCurrentSelectedElement;
    IManipulatable *pLastSelectedElement;

    Vector2 lastMousePosition;
};

template <class T>
class IManipulationSurfaceView : public IDrawingView<T>, public IManipulationSurface
{
protected:
    IManipulationSurfaceView(T *pObject, IDrawable *pParent)
        : IDrawable(pParent)
        , IDrawingView<T>(pObject, pParent)
        , IManipulationSurface(pParent)
    {
    }

    virtual ~IManipulationSurfaceView() { }
};

#define GET_DRAWING_VIEW_DEFINITION(TYPE) \
    TYPE::DrawingView * GetDrawingView(IDrawable *pParent) \
    { \
        return new TYPE::DrawingView(this, pParent); \
    }

#define GET_MANIPULATION_VIEW_DEFINITION(TYPE) \
    TYPE::ManipulationView * GetManipulationView(IDrawable *pParent, UndoCommandSink *pUndoCommandSink = NULL) \
    { \
        return new TYPE::ManipulationView(this, pParent, pUndoCommandSink); \
    } \

#define GET_MANIPULATABLE_OBJECT_FROM_DEFINITION(TYPE) \
    template<> \
    IManipulatableObject * GetManipulatableObjectFrom<TYPE>(TYPE *pObject) \
    { \
        return pObject; \
    }

#endif // INTERFACES_H
