#include "DialogCharacter.Staging.h"

#include "CaseContentLoadingStager.h"

Staging::DialogCharacter::OneTimeEmotion::OneTimeEmotion(XmlReader *pReader)
{
    pReader->StartElement("OneTimeEmotion");

    if (pReader->ElementExists("TransitionToEmotion"))
    {
        TransitionToEmotion = pReader->ReadTextElement("TransitionToEmotion");
    }

    if (pReader->ElementExists("LeadInTime"))
    {
        LeadInTime = pReader->ReadIntElement("LeadInTime");
    }
    else
    {
        LeadInTime = 0;
    }

    VideoId = pReader->ReadTextElement("VideoId");
    pReader->EndElement();
}

void Staging::DialogCharacter::OneTimeEmotion::AddSpritePaths(QMap<QString, QString> &/*spriteIdToSavePathMap*/)
{

}

Staging::DialogCharacter::DialogCharacter(XmlReader *pReader, QString id)
{
    pReader->StartElement("DialogCharacter");
    Id = id;
    Name = pReader->ReadTextElement("Name").toUpper();
    DefaultEmotionId = pReader->ReadTextElement("DefaultEmotionId");

    pReader->StartElement("CharacterEmotionBaseSpriteIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString emotionId = pReader->ReadTextElement("EmotionId");
        QString spriteId = pReader->ReadTextElement("SpriteId");

        CharacterEmotionBaseSpriteIds[emotionId] = spriteId;
    }

    pReader->EndElement();

    pReader->StartElement("CharacterEmotionEyeSpriteIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString emotionId = pReader->ReadTextElement("EmotionId");
        CharacterEmotionEyeSpriteIds[emotionId] = QStringList();

        pReader->StartElement("SpriteIds");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            CharacterEmotionEyeSpriteIds[emotionId].push_back(pReader->ReadTextElement("SpriteId"));
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("CharacterEmotionMouthSpriteIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString emotionId = pReader->ReadTextElement("EmotionId");
        CharacterEmotionMouthSpriteIds[emotionId] = QStringList();

        pReader->StartElement("SpriteIds");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            CharacterEmotionMouthSpriteIds[emotionId].push_back(pReader->ReadTextElement("SpriteId"));
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("CharacterEmotionForegroundLayerIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString emotionId = pReader->ReadTextElement("EmotionId");
        CharacterEmotionForegroundLayerIds[emotionId] = QStringList();

        pReader->StartElement("AnimationIds");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            CharacterEmotionForegroundLayerIds[emotionId].push_back(pReader->ReadTextElement("AnimationId"));
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("CharacterOneTimeEmotions");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString emotionId = pReader->ReadTextElement("EmotionId");

        pReader->StartElement("OneTimeEmotion");
        CharacterOneTimeEmotions[emotionId] = new OneTimeEmotion(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    if (pReader->ElementExists("PresentCorrectEvidenceSpriteId"))
    {
        PresentCorrectEvidenceSpriteId = pReader->ReadTextElement("PresentCorrectEvidenceSpriteId");
    }

    if (pReader->ElementExists("PresentCorrectEvidenceSoundId"))
    {
        PresentCorrectEvidenceSoundId = pReader->ReadTextElement("PresentCorrectEvidenceSoundId");
    }

    if (pReader->ElementExists("PresentCorrectEvidencePromptedSoundId"))
    {
        PresentCorrectEvidencePromptedSoundId = pReader->ReadTextElement("PresentCorrectEvidencePromptedSoundId");
    }

    if (pReader->ElementExists("PresentIncorrectEvidenceSpriteId"))
    {
        PresentIncorrectEvidenceSpriteId = pReader->ReadTextElement("PresentIncorrectEvidenceSpriteId");
    }

    if (pReader->ElementExists("PresentIncorrectEvidenceSoundId"))
    {
        PresentIncorrectEvidenceSoundId = pReader->ReadTextElement("PresentIncorrectEvidenceSoundId");
    }

    if (pReader->ElementExists("PressStatementSpriteId"))
    {
        PressStatementSpriteId = pReader->ReadTextElement("PressStatementSpriteId");
    }

    if (pReader->ElementExists("PressStatementSoundId"))
    {
        PressStatementSoundId = pReader->ReadTextElement("PressStatementSoundId");
    }

    if (pReader->ElementExists("InterjectionSpriteId"))
    {
        InterjectionSpriteId = pReader->ReadTextElement("InterjectionSpriteId");
    }

    if (pReader->ElementExists("InterjectionSpriteId"))
    {
        InterjectionSoundId = pReader->ReadTextElement("InterjectionSoundId");
    }

    if (pReader->ElementExists("BreakdownVideoId"))
    {
        BreakdownVideoId = pReader->ReadTextElement("BreakdownVideoId");
    }

    if (pReader->ElementExists("HealthBackgroundSpriteId"))
    {
        HealthBackgroundSpriteId = pReader->ReadTextElement("HealthBackgroundSpriteId");
    }

    if (pReader->ElementExists("EntranceForegroundSpriteId"))
    {
        EntranceForegroundSpriteId = pReader->ReadTextElement("EntranceForegroundSpriteId");
    }

    if (pReader->ElementExists("BackgroundColor"))
    {
        pReader->StartElement("BackgroundColor");
        BackgroundColor = Color(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}

Staging::DialogCharacter::~DialogCharacter()
{
    for (QMap<QString, OneTimeEmotion *>::iterator iter = CharacterOneTimeEmotions.begin(); iter != CharacterOneTimeEmotions.end(); iter++)
    {
        delete iter.value();
    }

    CharacterOneTimeEmotions.clear();
}

void Staging::DialogCharacter::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap)
{
    QString baseDir = "Characters/" + Id + "/DialogSprites/";

    for (QMap<QString, QString>::iterator iter = CharacterEmotionBaseSpriteIds.begin(); iter != CharacterEmotionBaseSpriteIds.end(); iter++)
    {
        spriteIdToSavePathMap[iter.value()] = baseDir + iter.key() + "/Base.png";
    }

    for (QMap<QString, QStringList>::iterator iter = CharacterEmotionEyeSpriteIds.begin(); iter != CharacterEmotionEyeSpriteIds.end(); iter++)
    {
        for (int i = 0; i < iter.value().length(); i++)
        {
            char fileName[256];
            sprintf(fileName, "/Eyes%d.png", i + 1);
            spriteIdToSavePathMap[iter.value()[i]] = baseDir + iter.key() + QString(fileName);
        }
    }

    for (QMap<QString, QStringList>::iterator iter = CharacterEmotionMouthSpriteIds.begin(); iter != CharacterEmotionMouthSpriteIds.end(); iter++)
    {
        for (int i = 0; i < iter.value().length(); i++)
        {
            char fileName[256];
            sprintf(fileName, "/Mouth%d.png", i + 1);
            spriteIdToSavePathMap[iter.value()[i]] = baseDir + iter.key() + QString(fileName);
        }
    }

    for (QMap<QString, QStringList>::iterator iter = CharacterEmotionForegroundLayerIds.begin(); iter != CharacterEmotionForegroundLayerIds.end(); iter++)
    {
        for (int i = 0; i < iter.value().length(); i++)
        {
            char fileName[256];
            sprintf(fileName, "/ForegroundLayer%d", i + 1);
            CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[iter.value()[i]]->AddSpritePaths(spriteIdToSavePathMap, baseDir + iter.key() + QString(fileName));
        }
    }

    if (PresentCorrectEvidenceSpriteId.length() > 0)
    {
        spriteIdToSavePathMap[PresentCorrectEvidenceSpriteId] = baseDir + "/Interjections/PresentCorrectEvidence.png";
    }

    if (PresentIncorrectEvidenceSpriteId.length() > 0)
    {
        spriteIdToSavePathMap[PresentIncorrectEvidenceSpriteId] = baseDir + "/Interjections/PresentIncorrectEvidence.png";
    }

    if (PressStatementSpriteId.length() > 0)
    {
        spriteIdToSavePathMap[PressStatementSpriteId] = baseDir + "/Interjections/PressStatement.png";
    }

    if (InterjectionSpriteId.length() > 0)
    {
        spriteIdToSavePathMap[InterjectionSpriteId] = baseDir + "/Interjections/Interjection.png";
    }

    if (HealthBackgroundSpriteId.length() > 0)
    {
        spriteIdToSavePathMap[HealthBackgroundSpriteId] = baseDir + "/Misc/HealthBackground.png";
    }

    if (EntranceForegroundSpriteId.length() > 0)
    {
        spriteIdToSavePathMap[EntranceForegroundSpriteId] = baseDir + "/Misc/EntranceForeground.png";
    }
}
