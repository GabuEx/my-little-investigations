#include "ForegroundElement.h"

#include "Animation.h"
#include "XmlReader.h"
#include "XmlWriter.h"
#include "CaseCreator/CaseContent/CaseContent.h"

#include "Staging/CaseContentLoadingStager.h"
#include "Staging/ForegroundElement.Staging.h"

#include "CaseCreator/UIComponents/LocationTab/ForegroundElementManipulator.h"

#include <limits>

ForegroundElementAnimation::ForegroundElementAnimation(Staging::ForegroundElementAnimation *pStagingForegroundElementAnimation)
    : ForegroundElementAnimation()
{
    pAnimation = new Animation(CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[pStagingForegroundElementAnimation->AnimationId]);
    position = pStagingForegroundElementAnimation->Position;
}

ForegroundElementAnimation::~ForegroundElementAnimation()
{
    delete pAnimation;
    pAnimation = NULL;
}

ForegroundElementAnimation::DrawingView::DrawingView(ForegroundElementAnimation *pAnimation, IDrawable *pParent)
    : IDrawable(pParent)
    , IDrawingView<ForegroundElementAnimation>(pAnimation, pParent)
{
    pAnimationView = pObject->pAnimation->GetDrawingView(this);
}

ForegroundElementAnimation::DrawingView::~DrawingView()
{
    delete pAnimationView;
    pAnimationView = NULL;
}

void ForegroundElementAnimation::DrawingView::DrawChildren(QGraphicsScene *pScene)
{
    pAnimationView->Draw(pScene);
}

void ForegroundElementAnimation::DrawingView::UpdateChildren()
{
    pAnimationView->Update();
}

void ForegroundElementAnimation::DrawingView::ResetChildren()
{
    pAnimationView->Reset();
}

ForegroundElement::ForegroundElement(QString name, QString spriteFilePath)
    : InteractionLocation(name)
{
    this->spriteFilePath = spriteFilePath;
}

ForegroundElement::ForegroundElement(Staging::ForegroundElement *pStagingForegroundElement, int indexInScene)
    : InteractionLocation(pStagingForegroundElement, indexInScene)
{
    SetPosition(pStagingForegroundElement->Position);
    SetSpriteFilePath(CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(pStagingForegroundElement->SpriteId));
    SetAnchorPosition(pStagingForegroundElement->AnchorPosition);

    for (Staging::ForegroundElementAnimation *pForegroundElementAnimation : pStagingForegroundElement->ForegroundElementAnimationList)
    {
        foregroundElementAnimationList.push_back(new ForegroundElementAnimation(pForegroundElementAnimation));
    }
}

ForegroundElement::~ForegroundElement()
{

}

void ForegroundElement::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("ForegroundElement");
    pWriter->WriteTextElement("Name", name);

    pWriter->StartElement("Position");
    position.SaveToXml(pWriter);
    pWriter->EndElement();

    pWriter->WriteIntElement("AnchorPosition", anchorPosition);

    pWriter->StartElement("InteractionLocation");
    interactionLocation.SaveToXml(pWriter);
    pWriter->EndElement();

    // Placeholder
    pWriter->WriteBooleanElement("InteractFromAnywhere", false);

    pWriter->StartElement("ClickPolygon");
    clickPolygon.SaveToXml(pWriter);
    pWriter->EndElement();

    if (pEncounter != NULL)
    {
        pWriter->StartElement("Encounter");
        pEncounter->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->StartElement("Animations");
    // Placeholder
    pWriter->EndElement();

    pWriter->EndElement();
}

ObjectManipulatorSlots * ForegroundElement::GetManipulationWidget()
{
    ForegroundElementManipulator* pManipulator = ForegroundElementManipulator::Create();
    pManipulator->Init(this);
    return pManipulator;
}

void ForegroundElement::SetIdFromIndex(int indexInScene)
{
    char id[256];
    sprintf(id, "ForegroundElement%d", indexInScene + 1);
    this->id = id;

    emit PropertyChanged("Id");
}

void ForegroundElement::SetPosition(Vector2 position)
{
    this->position = position;
    emit PropertyChanged("Position");
}

ForegroundElement::ManipulationView::ManipulationView(ForegroundElement *pForegroundElement, IDrawable *pParent, UndoCommandSink *pUndoCommandSink)
    : IDrawable(pParent)
    , InteractionLocation::ManipulationView(pForegroundElement, pParent, pUndoCommandSink)
{
    pObjectAsFE = pForegroundElement;
    spritePixmap = QPixmap(pObjectAsFE->spriteFilePath);

    for (ForegroundElementAnimation *pForegroundElementAnimation : pObjectAsFE->foregroundElementAnimationList)
    {
        animationDrawingViewList.push_back(pForegroundElementAnimation->GetDrawingView(this));
    }
}

ForegroundElement::ManipulationView::~ManipulationView()
{
    spritePixmap = QPixmap();

    for (ForegroundElementAnimation::DrawingView *pForegroundElementAnimationView : animationDrawingViewList)
    {
        delete pForegroundElementAnimationView;
    }

    animationDrawingViewList.clear();
}

void ForegroundElement::ManipulationView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    InteractionLocation::ManipulationView::DrawCore(pScene, addedItems);
    addedItems.append(pScene->addPixmap(spritePixmap));
}

void ForegroundElement::ManipulationView::DrawChildren(QGraphicsScene *pScene)
{
    InteractionLocation::ManipulationView::DrawChildren(pScene);

    for (ForegroundElementAnimation::DrawingView *pForegroundElementAnimationView : animationDrawingViewList)
    {
        pForegroundElementAnimationView->Draw(pScene);
    }
}

void ForegroundElement::ManipulationView::UpdateCore()
{
    InteractionLocation::ManipulationView::UpdateCore();
}

void ForegroundElement::ManipulationView::UpdateChildren()
{
    InteractionLocation::ManipulationView::UpdateChildren();

    for (ForegroundElementAnimation::DrawingView *pForegroundElementAnimationView : animationDrawingViewList)
    {
        pForegroundElementAnimationView->Update();
    }
}

void ForegroundElement::ManipulationView::ResetChildren()
{
    InteractionLocation::ManipulationView::ResetChildren();

    for (ForegroundElementAnimation::DrawingView *pForegroundElementAnimationView : animationDrawingViewList)
    {
        pForegroundElementAnimationView->Reset();
    }
}

RectangleWH ForegroundElement::ManipulationView::GetBoundingRect()
{
    if (pObjectAsFE->GetSpriteFilePath().length() > 0 &&
            spritePixmap.width() > 0 &&
            spritePixmap.height() > 0)
    {
        return RectangleWH(0, 0, spritePixmap.width(), spritePixmap.height());
    }
    else if (!animationDrawingViewList.empty())
    {
        RectangleWH cumulativeBoundingRect = RectangleWH(0, 0, 0, 0);

        for (ForegroundElementAnimation::DrawingView *pForegroundElementAnimationView : animationDrawingViewList)
        {
            RectangleWH animationBoundingRect = pForegroundElementAnimationView->GetBoundingRect();

            if (animationBoundingRect.GetX() < cumulativeBoundingRect.GetX())
            {
                cumulativeBoundingRect.SetX(animationBoundingRect.GetX());
            }

            if (animationBoundingRect.GetY() < cumulativeBoundingRect.GetY())
            {
                cumulativeBoundingRect.SetY(animationBoundingRect.GetY());
            }

            if (animationBoundingRect.GetX() + animationBoundingRect.GetWidth() > cumulativeBoundingRect.GetWidth())
            {
                cumulativeBoundingRect.SetWidth(animationBoundingRect.GetX() + animationBoundingRect.GetWidth());
            }

            if (animationBoundingRect.GetY() + animationBoundingRect.GetHeight() > cumulativeBoundingRect.GetHeight())
            {
                cumulativeBoundingRect.SetHeight(animationBoundingRect.GetY() + animationBoundingRect.GetHeight());
            }
        }

        return cumulativeBoundingRect;
    }
    else
    {
        return InteractionLocation::ManipulationView::GetBoundingRect();
    }
}

template <>
void CaseContent::Add<ForegroundElement>(QString, ForegroundElement *pForegroundElement, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        pLocation->AddForegroundElement(pForegroundElement);
    }
}

template <>
ForegroundElement * CaseContent::GetById<ForegroundElement>(QString id, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetForegroundElementById(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<ForegroundElement>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetForegroundElementIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<ForegroundElement>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetForegroundElementDisplayNames();
    }
    else
    {
        return QStringList();
    }
}
