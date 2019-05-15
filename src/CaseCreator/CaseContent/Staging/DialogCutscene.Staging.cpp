#include "DialogCutscene.Staging.h"

Staging::DialogCutscene::DialogCutscene(Staging::DialogCutsceneAnimation *pSourceAnimation, QMap<QString, DialogCutsceneAnimationElement *> &idToElementMap)
{
    Id = pSourceAnimation->Id;
    BackgroundSpriteId = pSourceAnimation->BackgroundSpriteId;

    for (QString elementId : pSourceAnimation->ElementIdList)
    {
        ElementIdToElementMap[elementId] = idToElementMap[elementId];
        idToElementMap.remove(elementId);
    }

    for (DialogCutsceneAnimationFrame *pFrame : pSourceAnimation->FrameList)
    {
        FrameList.push_back(pFrame);
    }

    pSourceAnimation->FrameList.clear();
}

Staging::DialogCutscene::~DialogCutscene()
{
    for (QMap<QString, DialogCutsceneAnimationElement *>::iterator iter = ElementIdToElementMap.begin(); iter != ElementIdToElementMap.end(); iter++)
    {
        delete iter.value();
    }

    ElementIdToElementMap.clear();

    for (DialogCutsceneAnimationFrame *pFrame : FrameList)
    {
        delete pFrame;
    }

    FrameList.clear();
}

void Staging::DialogCutscene::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap)
{
    QString baseDir = "Cutscenes/Dialog/" + Id + "/";
    unsigned int backgroundIndex = 1;
    unsigned int elementIndex = 1;

    if (BackgroundSpriteId.length() > 0)
    {
        char filename[256];
        sprintf(filename, "Background%d.png", backgroundIndex++);
        spriteIdToSavePathMap[BackgroundSpriteId] = baseDir + filename;
    }

    for (DialogCutsceneAnimationFrame *pFrame : FrameList)
    {
        char filename[256];
        sprintf(filename, "Background%d.png", backgroundIndex++);
        spriteIdToSavePathMap[pFrame->NewBackgroundSpriteId] = baseDir + filename;
    }

    for (QMap<QString, DialogCutsceneAnimationElement *>::iterator iter = ElementIdToElementMap.begin(); iter != ElementIdToElementMap.end(); iter++)
    {
        char filename[256];
        sprintf(filename, "Element%d.png", elementIndex++);
        spriteIdToSavePathMap[iter.value()->SpriteId] = baseDir + filename;
    }
}

Staging::DialogCutsceneAnimation::DialogCutsceneAnimation(XmlReader *pReader)
{
    pReader->StartElement("DialogCutsceneAnimation");

    Id = pReader->ReadTextElement("Id");
    BackgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");

    pReader->StartElement("ElementIdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ElementIdList.push_back(pReader->ReadTextElement("ElementId"));
    }

    pReader->EndElement();

    pReader->StartElement("FrameList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        FrameList.push_back(new Staging::DialogCutsceneAnimationFrame(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

Staging::DialogCutsceneAnimation::~DialogCutsceneAnimation()
{
    for (Staging::DialogCutsceneAnimationFrame *pFrame : FrameList)
    {
        delete pFrame;
    }

    FrameList.clear();
}

Staging::DialogCutsceneAnimationElement::DialogCutsceneAnimationElement(XmlReader *pReader)
{
    pReader->StartElement("DialogCutsceneAnimationElement");
    SpriteId = pReader->ReadTextElement("SpriteId");
    OriginalXPosition = pReader->ReadIntElement("OriginalXPosition");
    OriginalYPosition = pReader->ReadIntElement("OriginalYPosition");
    OriginalATint = pReader->ReadIntElement("OriginalATint");
    OriginalRTint = pReader->ReadIntElement("OriginalRTint");
    OriginalGTint = pReader->ReadIntElement("OriginalGTint");
    OriginalBTint = pReader->ReadIntElement("OriginalBTint");
    pReader->EndElement();
}

Staging::DialogCutsceneAnimationFrame::DialogCutsceneAnimationFrame(XmlReader *pReader)
{
    pReader->StartElement("DialogCutsceneAnimationFrame");
    MsDuration = pReader->ReadIntElement("MsDuration");

    pReader->StartElement("PositionChangeHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");

        pReader->StartElement("PositionChange");
        PositionChangeHashMap[id] = Vector2(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("TintChangeHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");

        pReader->StartElement("TintChange");
        TintChangeHashMap[id] = Color(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    if (pReader->ElementExists("NewBackgroundSpriteId"))
    {
        NewBackgroundSpriteId = pReader->ReadTextElement("NewBackgroundSpriteId");
    }

    pReader->EndElement();
}
