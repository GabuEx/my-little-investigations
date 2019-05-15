#ifndef DIALOGCUTSCENE_STAGING_H
#define DIALOGCUTSCENE_STAGING_H

#include "Vector2.h"
#include "Color.h"
#include "XmlReader.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

namespace Staging
{

class DialogCutsceneAnimation;
class DialogCutsceneAnimationElement;
class DialogCutsceneAnimationFrame;

class DialogCutscene
{
public:
    DialogCutscene(DialogCutsceneAnimation *pSourceAnimation, QMap<QString, DialogCutsceneAnimationElement *> &idToElementMap);
    ~DialogCutscene();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap);

    QString Id;
    QString BackgroundSpriteId;

    QMap<QString, DialogCutsceneAnimationElement *> ElementIdToElementMap;
    QList<DialogCutsceneAnimationFrame *> FrameList;
};

class DialogCutsceneAnimation
{
public:
    DialogCutsceneAnimation(XmlReader *pReader);
    ~DialogCutsceneAnimation();

    QStringList ElementIdList;
    QList<DialogCutsceneAnimationFrame *> FrameList;

    QString Id;
    QString BackgroundSpriteId;
};

class DialogCutsceneAnimationElement
{
public:
    DialogCutsceneAnimationElement(XmlReader *pReader);

    QString SpriteId;

    int OriginalXPosition;
    int OriginalYPosition;

    int OriginalATint;
    int OriginalRTint;
    int OriginalGTint;
    int OriginalBTint;
};

class DialogCutsceneAnimationFrame
{
public:
    DialogCutsceneAnimationFrame(XmlReader *pReader);

    int MsDuration;
    QString NewBackgroundSpriteId;
    QMap<QString, Vector2> PositionChangeHashMap;
    QMap<QString, Color> TintChangeHashMap;
};

}

#endif // DIALOGCUTSCENE_STAGING_H
