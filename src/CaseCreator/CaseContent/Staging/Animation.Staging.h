#ifndef ANIMATION_STAGING_H
#define ANIMATION_STAGING_H

#include "Enums.Staging.h"

#include "XmlReader.h"

#include <QString>
#include <QList>

namespace Staging
{

class AnimationSound
{
public:
    virtual ~AnimationSound() { }

    static AnimationSound * LoadFromXml(XmlReader *pReader);

    virtual AnimationSoundType GetType() = 0;
};

class HoofstepSound : public AnimationSound
{
public:
    HoofstepSound(XmlReader *pReader);

    AnimationSoundType GetType()
    {
        return AnimationSoundType_Hoofstep;
    }
};

class SpecifiedSound : public AnimationSound
{
public:
    SpecifiedSound(XmlReader *pReader);

    AnimationSoundType GetType()
    {
        return AnimationSoundType_Specified;
    }

    QString SfxId;
};

class Animation
{
public:
    Animation(XmlReader *pReader);
    ~Animation();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString basePath);

    class Frame
    {
    public:
        Frame(XmlReader *pReader);
        ~Frame();

        int MsDuration;
        QString SpriteId;
        AnimationSound *pSound;
    };

    QList<Frame *> FrameList;
};

}

#endif // ANIMATION_H
