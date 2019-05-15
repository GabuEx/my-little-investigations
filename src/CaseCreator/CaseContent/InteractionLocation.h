#ifndef INTERACTIONLOCATION_H
#define INTERACTIONLOCATION_H

#include "Animation.h"

#include "Condition.h"
#include "Encounter.h"
#include "HitBox.h"
#include "CaseCreator/Utilities/Interfaces.h"
#include "CaseCreator/UIComponents/ManipulationViewBox.h"
#include "Polygon.h"
#include "XmlStorableObject.h"

#include <QString>
#include <QList>
#include <QPixmap>
#include <QUndoCommand>

namespace Staging
{
    class ForegroundElement;
}

class ObjectManipulatorSlots;

class InteractionLocation : public IManipulatableObject, public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(InteractionLocation)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_TEXT(name)
        XML_STORABLE_CUSTOM_OBJECT(clickPolygon, GeometricPolygon::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT(pHitBox, HitBox::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT(pCondition, Condition::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT(pEncounter, Encounter::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    InteractionLocation()
    {
        pEncounter = NULL;
        pHitBox = NULL;
        pCondition = NULL;
    }

    InteractionLocation(QString name);
    InteractionLocation(Staging::ForegroundElement *pStagingForegroundElement, int indexInScene);
    virtual ~InteractionLocation();

    static InteractionLocation * CreateFromXml(XmlReader *pReader)
    {
        return new InteractionLocation(pReader);
    }

    virtual ObjectManipulatorSlots * GetManipulationWidget();

    static QString GetObjectAdditionString() { return QString("interaction location"); }
    static QString GetListTitle() { return QString("Interaction Locations"); }
    static bool GetIsMainList() { return false; }
    static bool GetAllowsNewObjects() { return true; }

    QString GetDisplayName() { return this->name.length() > 0 ? this->name : this->id; }

    QString GetId() { return this->id; }
    virtual void SetIdFromIndex(int indexInScene);

    QString GetName() const { return this->name; }
    void SetName(QString name);

    virtual Vector2 GetPosition() const;
    virtual void SetPosition(Vector2 position);

    Encounter * GetEncounter() { return pEncounter; }
    HitBox * GetHitBox() { return pHitBox; }
    Condition * GetCondition() { return pCondition; }

    class ManipulationView : public IManipulationView<InteractionLocation>
    {
        friend class InteractionLocation;

    public:
        ManipulationView(InteractionLocation *pInteractionLocation, IDrawable *pParent, UndoCommandSink *pUndoCommandSink);
        virtual ~ManipulationView();

        virtual void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        virtual void DrawChildren(QGraphicsScene *pScene);
        virtual void UpdateCore();
        virtual void UpdateChildren();
        virtual void ResetChildren();

        virtual QString GetMouseOverText()
        {
            return pObject->GetDisplayName();
        }

        Vector2 GetPosition()
        {
            return pObject->GetPosition();
        }

        virtual RectangleWH GetBoundingRect();

        virtual qreal GetZOrder()
        {
            // Multiply by 100 in order to allow intraelement granularity -
            // e.g., enabling ForegroundElementAnimations to always be drawn on top of
            // their ForegroundElements without causing them to also be drawn on top of
            // other ForegroundElements.
            return 100 * pObject->GetPosition().GetY();
        }

        qreal GetOpacity()
        {
            if (IsEnabled())
            {
                return pParent->GetOpacity() * (pManipulationViewBox->GetIsBeingDragged() ? 0.5 : 1.0);
            }
            else
            {
                return IManipulatable::GetOpacity();
            }
        }

    protected:
        void OnGotFocus();
        void OnLostFocus();

        void OnMouseEnter();
        void OnMouseLeave();

        void OnMousePress();
        void OnMouseDrag(Vector2 delta);
        void OnMouseRelease();

        void SetPosition(Vector2 position);

    private:
        void SetIsMouseOver(bool isMouseOver);
        void SetIsSelected(bool isSelected);
        void SetIsBeingDragged(bool isBeingDragged);

        ManipulationViewBox *pManipulationViewBox;

        Vector2 positionBeforeDrag;
    };

    GET_MANIPULATION_VIEW_DEFINITION(InteractionLocation)

protected:
    QString id;
    QString name;
    GeometricPolygon clickPolygon;

    Encounter *pEncounter;
    QString zoomedViewId;

    HitBox *pHitBox;

    Vector2 interactionLocation;

    Condition *pCondition;
};

#endif // INTERACTIONLOCATION_H
