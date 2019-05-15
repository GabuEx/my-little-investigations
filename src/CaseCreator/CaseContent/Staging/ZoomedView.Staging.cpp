#include "ZoomedView.Staging.h"

#include "ForegroundElement.Staging.h"

Staging::ZoomedView::ZoomedView(XmlReader *pReader)
{
    pReader->StartElement("ZoomedView");
    Id = pReader->ReadTextElement("Id");
    BackgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");

    pReader->StartElement("ForegroundElementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ForegroundElementList.push_back(new ForegroundElement(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

Staging::ZoomedView::~ZoomedView()
{
    for (ForegroundElement *pForegroundElement : ForegroundElementList)
    {
        delete pForegroundElement;
    }

    ForegroundElementList.clear();
}

void Staging::ZoomedView::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString baseDir)
{
    baseDir += "ZoomedViews/" + Id + "/";

    spriteIdToSavePathMap[BackgroundSpriteId] = baseDir + "Background.png";

    for (int i = 0; i < ForegroundElementList.size(); i++)
    {
        char fileName[256];
        sprintf(fileName, "ForegroundElement%d", (i + 1));

        ForegroundElementList[i]->AddSpritePaths(spriteIdToSavePathMap, baseDir + fileName);
    }
}
