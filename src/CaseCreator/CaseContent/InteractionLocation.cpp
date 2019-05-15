#include "InteractionLocation.h"

#include "Animation.h"
#include "HitBox.h"
#include "Condition.h"
#include "XmlReader.h"
#include "XmlWriter.h"
#include "CaseCreator/CaseContent/CaseContent.h"

#include "Staging/CaseContentLoadingStager.h"
#include "Staging/ForegroundElement.Staging.h"

#include "CaseCreator/UIComponents/LocationTab/InteractionLocationManipulator.h"

#include <limits>

InteractionLocation::InteractionLocation(QString name)
    : InteractionLocation()
{
    this->name = name;
}

InteractionLocation::InteractionLocation(Staging::ForegroundElement *pStagingForegroundElement, int indexInScene)
    : InteractionLocation()
{
    pEncounter = NULL;
    pHitBox = NULL;
    pCondition = NULL;

    SetIdFromIndex(indexInScene);
    SetName(pStagingForegroundElement->Name);
    SetPosition(pStagingForegroundElement->Position);

    this->clickPolygon = pStagingForegroundElement->ClickPolygon;

    if (pStagingForegroundElement->pHitBox != NULL)
    {
        this->pHitBox = new HitBox(pStagingForegroundElement->pHitBox);
    }

    if (pStagingForegroundElement->pCondition != NULL)
    {
        this->pCondition = new Condition(pStagingForegroundElement->pCondition);
    }

    if (pStagingForegroundElement->pEncounter != NULL)
    {
        this->pEncounter = new Encounter(pStagingForegroundElement->pEncounter);
    }
}

InteractionLocation::~InteractionLocation()
{
    delete pEncounter;
    pEncounter = NULL;
    delete pHitBox;
    pHitBox = NULL;
    delete pCondition;
    pCondition = NULL;
}

ObjectManipulatorSlots *InteractionLocation::GetManipulationWidget()
{
    InteractionLocationManipulator *pManipulator = InteractionLocationManipulator::Create();
    pManipulator->Init(this);
    return pManipulator;
}

void InteractionLocation::SetIdFromIndex(int indexInScene)
{
    char id[256];
    sprintf(id, "InteractionLocation%d", indexInScene + 1);
    this->id = id;

    emit PropertyChanged("Id");
}

void InteractionLocation::SetName(QString name)
{
    this->name = name;
    emit PropertyChanged("Name");
}

Vector2 InteractionLocation::GetPosition() const
{
    if (!clickPolygon.Empty())
    {
        return Vector2(clickPolygon.GetBoundingBox().GetX(), clickPolygon.GetBoundingBox().GetY());
    }
    else
    {
        return Vector2(0, 0);
    }
}

void InteractionLocation::SetPosition(Vector2 position)
{
    Vector2 delta = position - GetPosition();

    if (!clickPolygon.Empty())
    {
        clickPolygon += delta;
    }

    emit PropertyChanged("Position");
}

InteractionLocation::ManipulationView::ManipulationView(InteractionLocation *pInteractionLocation, IDrawable *pParent, UndoCommandSink *pUndoCommandSink)
    : IDrawable(pParent)
    , IManipulationView<InteractionLocation>(pInteractionLocation, pParent, pUndoCommandSink)
{
    pManipulationViewBox = new ManipulationViewBox(this);
}

InteractionLocation::ManipulationView::~ManipulationView()
{
    delete pManipulationViewBox;
    pManipulationViewBox = NULL;
}

void InteractionLocation::ManipulationView::DrawCore(QGraphicsScene */*pScene*/, QList<QGraphicsItem *> &/*addedItems*/)
{
}

void InteractionLocation::ManipulationView::DrawChildren(QGraphicsScene *pScene)
{
    pManipulationViewBox->SetBoxRect(GetBoundingRect());
    pManipulationViewBox->Draw(pScene);
}

void InteractionLocation::ManipulationView::UpdateCore()
{
}

void InteractionLocation::ManipulationView::UpdateChildren()
{
    pManipulationViewBox->Update();
}

void InteractionLocation::ManipulationView::ResetChildren()
{
    pManipulationViewBox->Reset();
}

RectangleWH InteractionLocation::ManipulationView::GetBoundingRect()
{
    if (!pObject->clickPolygon.Empty())
    {
        return pObject->clickPolygon.GetBoundingBox() - GetPosition();
    }
    else
    {
        return IDrawable::GetBoundingRect();
    }
}

void InteractionLocation::ManipulationView::OnGotFocus()
{
    SetIsSelected(true);
}

void InteractionLocation::ManipulationView::OnLostFocus()
{
    SetIsSelected(false);
}

void InteractionLocation::ManipulationView::OnMouseEnter()
{
    SetIsMouseOver(true);
}

void InteractionLocation::ManipulationView::OnMouseLeave()
{
    SetIsMouseOver(false);
}

void InteractionLocation::ManipulationView::OnMousePress()
{
    positionBeforeDrag = this->pObject->GetPosition();
}

void InteractionLocation::ManipulationView::OnMouseDrag(Vector2 delta)
{
    SetIsBeingDragged(true);
    SetPosition(this->pObject->GetPosition() + delta);
}

void InteractionLocation::ManipulationView::OnMouseRelease()
{
    SetIsBeingDragged(false);

    if (this->pObject->GetPosition() != positionBeforeDrag)
    {
        AddUndoCommand(new MoveUndoCommand(this, positionBeforeDrag, this->pObject->GetPosition()));
    }
}

void InteractionLocation::ManipulationView::SetPosition(Vector2 position)
{
    this->pObject->SetPosition(position);
    Update();
}

void InteractionLocation::ManipulationView::SetIsMouseOver(bool isMouseOver)
{
    pManipulationViewBox->SetIsMouseOver(isMouseOver);
}

void InteractionLocation::ManipulationView::SetIsSelected(bool isSelected)
{
    pManipulationViewBox->SetIsSelected(isSelected);
}

void InteractionLocation::ManipulationView::SetIsBeingDragged(bool isBeingDragged)
{
    pManipulationViewBox->SetIsBeingDragged(isBeingDragged);
}

template <>
void CaseContent::Add<InteractionLocation>(QString, InteractionLocation *pInteractionLocation, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        pLocation->AddInteractionLocation(pInteractionLocation);
    }
}

template <>
InteractionLocation * CaseContent::GetById<InteractionLocation>(QString id, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetInteractionLocationById(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<InteractionLocation>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetInteractionLocationIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<InteractionLocation>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetInteractionLocationDisplayNames();
    }
    else
    {
        return QStringList();
    }
}

GET_MANIPULATABLE_OBJECT_FROM_DEFINITION(InteractionLocation)
