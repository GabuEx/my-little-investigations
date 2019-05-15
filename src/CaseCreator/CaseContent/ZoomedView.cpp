#include "ZoomedView.h"

#include "CaseContent.h"
#include "Location.h"

#include "Staging/ZoomedView.Staging.h"
#include "Staging/CaseContentLoadingStager.h"

ZoomedView::ZoomedView(Staging::ZoomedView *pStagingZoomedView)
{
    id = pStagingZoomedView->Id;
    backgroundFilePath = CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(pStagingZoomedView->BackgroundSpriteId);

    for (Staging::ForegroundElement *pStagingForegroundElement : pStagingZoomedView->ForegroundElementList)
    {
        foregroundElementList.append(new ForegroundElement(pStagingForegroundElement, foregroundElementList.length()));
    }
}

ZoomedView::~ZoomedView()
{
    for (ForegroundElement *pForegroundElement : foregroundElementList)
    {
        delete pForegroundElement;
    }

    foregroundElementList.clear();
}

ZoomedView::OverviewView::OverviewView(ZoomedView *pLocation)
    : IDrawable(NULL)
    , IManipulationSurfaceView<ZoomedView>(pLocation, NULL)
{
    backgroundPixmap = QPixmap(pObject->backgroundFilePath, "png");

    for (QList<ForegroundElement *>::iterator iter = pObject->foregroundElementList.begin(); iter != pObject->foregroundElementList.end(); iter++)
    {
        IManipulatable *pForegroundElementManipulatable = (*iter)->GetManipulationView(this, this);

        manipulatableList.push_back(pForegroundElementManipulatable);
        foregroundElementManipulatableList.push_back(pForegroundElementManipulatable);
    }
}

ZoomedView::OverviewView::~OverviewView()
{
    backgroundPixmap = QPixmap();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        delete pManipulatable;
    }

    manipulatableList.clear();
}

void ZoomedView::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("ZoomedView");
    pWriter->WriteTextElement("Id", id);
    pWriter->WriteTextElement("BackgroundSpriteId", "placeholder");

    pWriter->StartElement("ForegroundElementList");

    for (ForegroundElement *pForegroundElement : foregroundElementList)
    {
        pWriter->StartElement("Entry");
        pForegroundElement->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();

    pWriter->EndElement();
}

void ZoomedView::OverviewView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    IManipulationSurfaceView::DrawCore(pScene, addedItems);

    addedItems.push_back(pScene->addPixmap(backgroundPixmap));
}

void ZoomedView::OverviewView::DrawChildren(QGraphicsScene *pScene)
{
    IManipulationSurfaceView::DrawChildren(pScene);

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Draw(pScene);
    }
}

void ZoomedView::OverviewView::UpdateChildren()
{
    IManipulationSurfaceView::UpdateChildren();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Update();
    }
}

void ZoomedView::OverviewView::ResetChildren()
{
    IManipulationSurfaceView::ResetChildren();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Reset();
    }
}

template <>
void CaseContent::Add<ZoomedView>(QString, ZoomedView *pZoomedView, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        pLocation->AddZoomedView(pZoomedView);
    }
}

template <>
ZoomedView * CaseContent::GetById<ZoomedView>(QString id, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetZoomedViewById(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<ZoomedView>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetZoomedViewIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<ZoomedView>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetZoomedViewDisplayNames();
    }
    else
    {
        return QStringList();
    }
}
