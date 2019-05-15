#ifndef DIALOGCHARACTER_STAGING_H
#define DIALOGCHARACTER_STAGING_H

#include "Color.h"
#include "XmlReader.h"

#include <QString>
#include <QStringList>
#include <QMap>

namespace Staging
{

class DialogCharacter
{
public:
    class OneTimeEmotion
    {
    public:
        OneTimeEmotion(XmlReader *pReader);

        void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap);

        QString TransitionToEmotion;
        int LeadInTime;
        QString VideoId;
    };

    DialogCharacter(XmlReader *pReader, QString id);
    ~DialogCharacter();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap);

    QMap<QString, QString> CharacterEmotionBaseSpriteIds;
    QMap<QString, QStringList> CharacterEmotionEyeSpriteIds;
    QMap<QString, QStringList> CharacterEmotionMouthSpriteIds;
    QMap<QString, QStringList> CharacterEmotionForegroundLayerIds;
    QMap<QString, OneTimeEmotion *> CharacterOneTimeEmotions;

    QString Id;
    QString Name;
    QString DefaultEmotionId;

    QString PresentCorrectEvidenceSpriteId;
    QString PresentCorrectEvidenceSoundId;
    QString PresentCorrectEvidencePromptedSoundId;
    QString PresentIncorrectEvidenceSpriteId;
    QString PresentIncorrectEvidenceSoundId;
    QString PressStatementSpriteId;
    QString PressStatementSoundId;
    QString InterjectionSpriteId;
    QString InterjectionSoundId;

    QString BreakdownVideoId;

    QString HealthBackgroundSpriteId;
    QString EntranceForegroundSpriteId;
    Color BackgroundColor;
};

}

#endif // DIALOGCHARACTER_H
