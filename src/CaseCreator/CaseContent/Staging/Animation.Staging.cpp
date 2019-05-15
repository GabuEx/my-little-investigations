#include "Animation.Staging.h"

#include <QMap>

Staging::AnimationSound * Staging::AnimationSound::LoadFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("HoofstepSound"))
    {
        return new HoofstepSound(pReader);
    }
    else if (pReader->ElementExists("SpecifiedSound"))
    {
        return new SpecifiedSound(pReader);
    }
    else
    {
        throw MLIException("Invalid sound type.");
    }
}

Staging::HoofstepSound::HoofstepSound(XmlReader *pReader)
{
    pReader->StartElement("HoofstepSound");
    pReader->EndElement();
}

Staging::SpecifiedSound::SpecifiedSound(XmlReader *pReader)
{
    pReader->StartElement("SpecifiedSound");
    SfxId = pReader->ReadTextElement("SfxId");
    pReader->EndElement();
}

Staging::Animation::Animation(XmlReader *pReader)
{
    pReader->StartElement("Animation");

    pReader->StartList("Frame");

    while (pReader->MoveToNextListItem())
    {
        FrameList.push_back(new Animation::Frame(pReader));
    }

    pReader->EndElement();
}

Staging::Animation::~Animation()
{
    for (Animation::Frame *pFrame : FrameList)
    {
        delete pFrame;
    }

    FrameList.clear();
}

void Staging::Animation::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString basePath)
{
    for (int i = 0; i < FrameList.length(); i++)
    {
        char fileName[256];
        sprintf(fileName, "Frame%d.png", i + 1);
        spriteIdToSavePathMap[FrameList[i]->SpriteId] = basePath + QString(fileName);
    }
}

Staging::Animation::Frame::Frame(XmlReader *pReader)
{
    MsDuration = pReader->ReadIntElement("MsDuration");
    SpriteId = pReader->ReadTextElement("SpriteId");

    if (pReader->ElementExists("Sound"))
    {
        pReader->StartElement("Sound");
        pSound = AnimationSound::LoadFromXml(pReader);
        pReader->EndElement();
    }
    else
    {
        pSound = NULL;
    }
}

Staging::Animation::Frame::~Frame()
{
    delete pSound;
    pSound = NULL;
}
