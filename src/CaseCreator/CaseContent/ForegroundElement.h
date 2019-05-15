#ifndef FOREGROUNDELEMENT_H
#define FOREGROUNDELEMENT_H

#include "Animation.h"

#include "InteractionLocation.h"
#include "CaseCreator/Utilities/Interfaces.h"
#include "CaseCreator/UIComponents/ManipulationViewBox.h"
#include "Polygon.h"
#include "Vector2.h"
#include "XmlStorableObject.h"

#include <QString>
#include <QList>
#include <QPixmap>
#include <QUndoCommand>

namespace Staging
{
    class ForegroundElementAnimation;
    class ForegroundElement;
}

class ForegroundElementAnimation : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(ForegroundElementAnimation)
        XML_STORABLE_CUSTOM_OBJECT(pAnimation, Animation::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT(position, Vector2::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    ForegroundElementAnimation()
    {
        pAnimation = NULL;
        position = Vector2();
    }

    ForegroundElementAnimation(Staging::ForegroundElementAnimation *pStagingForegroundElementAnimation);
    ~ForegroundElementAnimation();

    static ForegroundElementAnimation * CreateFromXml(XmlReader *pReader)
    {
        return new ForegroundElementAnimation(pReader);
    }

    class DrawingView : public IDrawingView<ForegroundElementAnimation>
    {
    public:
        DrawingView(ForegroundElementAnimation *pAnimation, IDrawable *pParent);
        ~DrawingView();

        void DrawCore(QGraphicsScene *, QList<QGraphicsItem *> &) { }
        void DrawChildren(QGraphicsScene *pScene);
        void UpdateChildren();
        void ResetChildren();

        Vector2 GetPosition()
        {
            return pObject->position;
        }

        RectangleWH GetBoundingRect()
        {
            RectangleWH animationBoundingRect = pAnimationView->GetBoundingRect();
            return RectangleWH(pObject->position.GetX(), pObject->position.GetY(), animationBoundingRect.GetWidth(), animationBoundingRect.GetHeight());
        }

        qreal GetZOrder()
        {
            // Ensure that ForegroundElementAnimations are always on top
            // of their ForegroundElements.
            return pParent->GetZOrder() + 1;
        }

    private:
        Animation::DrawingView *pAnimationView;
    };

    GET_DRAWING_VIEW_DEFINITION(ForegroundElementAnimation)

private:
    Animation *pAnimation;
    Vector2 position;
};

class Encounter;
class HitBox;
class Condition;

class ObjectManipulatorSlots;

class ForegroundElement : public InteractionLocation
{
    BEGIN_DERIVED_XML_STORABLE_OBJECT(ForegroundElement, InteractionLocation)
        XML_STORABLE_CUSTOM_OBJECT(position, Vector2::CreateFromXml)
        XML_STORABLE_FILE_PATH(spriteFilePath)
        XML_STORABLE_INT(anchorPosition)
        XML_STORABLE_CUSTOM_OBJECT_LIST(foregroundElementAnimationList, ForegroundElementAnimation::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    ForegroundElement() : InteractionLocation() { }
    ForegroundElement(QString name, QString spriteFilePath);
    ForegroundElement(Staging::ForegroundElement *pStagingForegroundElement, int indexInScene);
    ~ForegroundElement();

    static ForegroundElement * CreateFromXml(XmlReader *pReader)
    {
        return new ForegroundElement(pReader);
    }

    void WriteToCaseXml(XmlWriter *pWriter);

    virtual ObjectManipulatorSlots * GetManipulationWidget();

    static QString GetObjectAdditionString() { return QString("foreground element"); }
    static QString GetListTitle() { return QString("Foreground Elements"); }
    static bool GetIsMainList() { return false; }
    static bool GetAllowsNewObjects() { return true; }

    virtual void SetIdFromIndex(int indexInScene) override;

    QString GetSpriteFilePath() const { return this->spriteFilePath; }
    void SetSpriteFilePath(QString spriteFilePath) { this->spriteFilePath = spriteFilePath; }

    virtual Vector2 GetPosition() const { return this->position; }
    virtual void SetPosition(Vector2 position);

    int GetAnchorPosition() const { return this->anchorPosition; }
    void SetAnchorPosition(int anchorPosition) { this->anchorPosition = anchorPosition; }

    class ManipulationView : public InteractionLocation::ManipulationView
    {
        friend class ForegroundElement;

    public:
        ManipulationView(ForegroundElement *pForegroundElement, IDrawable *pParent, UndoCommandSink *pUndoCommandSink);
        virtual ~ManipulationView();

        void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        void DrawChildren(QGraphicsScene *pScene);
        void UpdateCore();
        void UpdateChildren();
        void ResetChildren();

        RectangleWH GetBoundingRect();

        qreal GetZOrder()
        {
            // Multiply by 100 in order to allow intraelement granularity -
            // e.g., enabling ForegroundElementAnimations to always be drawn on top of
            // their ForegroundElements without causing them to also be drawn on top of
            // other ForegroundElements.
            return 100 * (pObjectAsFE->position.GetY() + pObjectAsFE->anchorPosition);
        }

    private:
        // Needed since this derives from
        ForegroundElement *pObjectAsFE;

        QPixmap spritePixmap;
        QList<ForegroundElementAnimation::DrawingView *> animationDrawingViewList;
    };

    GET_MANIPULATION_VIEW_DEFINITION(ForegroundElement)

protected:
    Vector2 position;

    int anchorPosition;

private:
    QString spriteFilePath;

    QList<ForegroundElementAnimation *> foregroundElementAnimationList;
};

class HiddenForegroundElement : public ForegroundElement
{
    BEGIN_DERIVED_XML_STORABLE_OBJECT(HiddenForegroundElement, ForegroundElement)
        XML_STORABLE_TEXT(requiredPartnerId)
        XML_STORABLE_BOOL(isRequired)
    END_XML_STORABLE_OBJECT()

public:
    HiddenForegroundElement() : ForegroundElement()
    {
        isRequired = false;
    }

    HiddenForegroundElement(QString spriteFilePath);
    HiddenForegroundElement(QString spriteFilePath, int anchorPosition);

    static HiddenForegroundElement * CreateFromXml(XmlReader *pReader)
    {
        return new HiddenForegroundElement(pReader);
    }

    QString GetRequiredPartnerId() const { return this->requiredPartnerId; }

    bool GetIsRequired() const { return this->isRequired; }
    void SetIsRequired(bool isRequired) { this->isRequired = isRequired; }

private:
    QString requiredPartnerId;
    bool isRequired;
};

#endif // FOREGROUNDELEMENT_H
