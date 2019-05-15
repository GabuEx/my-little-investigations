#include "CaseContentLoadingStager.h"

#include "Conversation.Staging.h"

#include "../CaseContent.h"

#include "CaseCreator/Utilities/ArchiveReader.h"
#include "XmlReader.h"

#include <QPainter>

#define DECLARE_STAGING_HELPER_LOAD_FROM_XAML_ENTRY(TYPE) \
    STAGING_HELPER_LOAD_METHOD_NAME(TYPE)(&caseFileReader);

#define DECLARE_STAGING_HELPER_SAVE_SPRITES_ENTRY(TYPE) \
    for (QMap<QString, Staging::TYPE *>::iterator iter = STAGING_HELPER_MAP_NAME(TYPE).begin(); iter != STAGING_HELPER_MAP_NAME(TYPE).end(); iter++) \
    { \
        iter.value()->AddSpritePaths(SpriteIdToFilePathMap); \
    }

CaseContentLoadingStager *CaseContentLoadingStager::pCurrentInstance = NULL;

void CaseContentLoadingStager::LoadFromCaseFile(QString caseFilePath)
{
    ArchiveReader caseArchiveReader;

    if (caseArchiveReader.Init(caseFilePath) == false)
    {
        return;
    }

    ProgressStageUpdated("");
    UpdateProgressPercent(0);

    XmlReader caseFileReader;
    caseFileReader.ParseXmlContent(caseArchiveReader.LoadText(QString("case.xml")));

    caseFileReader.StartElement("Case");

    STAGING_HELPER_TYPES_OPERATION(DECLARE_STAGING_HELPER_LOAD_FROM_XAML_ENTRY)
    LoadSpriteSheets(&caseFileReader);
    LoadAudio(&caseFileReader);

    caseFileReader.EndElement();

    STAGING_HELPER_ROOT_TYPES_OPERATION(DECLARE_STAGING_HELPER_SAVE_SPRITES_ENTRY)
    SaveSprites(&caseArchiveReader);
    SaveAudio(&caseArchiveReader);
}

QString CaseContentLoadingStager::GetSpriteFilePathFromId(QString spriteId)
{
    QString suggestedFilePath = "";

    if (SpriteIdToFilePathMap.contains(spriteId) &&
            QFileInfo(suggestedFilePath = CaseContent::GetInstance()->RelativePathToAbsolutePath(SpriteIdToFilePathMap[spriteId])).isFile())
    {
        return suggestedFilePath;
    }
    else
    {
        return "";
    }
}

void CaseContentLoadingStager::LoadDialogCharacters(XmlReader *pReader)
{
    pReader->StartElement("DialogCharacterManager");

    pReader->StartElement("CharacterByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        DialogCharacterIdToDialogCharacterMap[id] = new Staging::DialogCharacter(pReader, id);
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadFieldCharacters(XmlReader *pReader)
{
    pReader->StartElement("FieldCharacterManager");

    pReader->StartElement("CharacterByIdDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        FieldCharacterIdToFieldCharacterMap[id] = new Staging::FieldCharacter(pReader);
    }

    pReader->EndElement();

    PlayerCharacterId = pReader->ReadTextElement("PlayerCharacterId");

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadDialogCutscenes(XmlReader *pReader)
{
    QMap<QString, Staging::DialogCutsceneAnimation *> idToAnimationMap;
    QMap<QString, Staging::DialogCutsceneAnimationElement *> idToElementMap;

    pReader->StartElement("DialogCutsceneManager");

    pReader->StartElement("IdToAnimationHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        idToAnimationMap[id] = new Staging::DialogCutsceneAnimation(pReader);
    }

    pReader->EndElement();

    pReader->StartElement("IdToElementHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        idToElementMap[id] = new Staging::DialogCutsceneAnimationElement(pReader);
    }

    pReader->EndElement();

    pReader->EndElement();

    for (QMap<QString, Staging::DialogCutsceneAnimation *>::iterator iter = idToAnimationMap.begin(); iter != idToAnimationMap.end(); iter++)
    {
        DialogCutsceneIdToDialogCutsceneMap[iter.key()] = new Staging::DialogCutscene(iter.value(), idToElementMap);
        delete iter.value();
    }

    idToAnimationMap.clear();

    for (QMap<QString, Staging::DialogCutsceneAnimationElement *>::iterator iter = idToElementMap.begin(); iter != idToElementMap.end(); iter++)
    {
        delete iter.value();
    }

    idToElementMap.clear();
}

void CaseContentLoadingStager::LoadFieldCutscenes(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneManager");

    pReader->StartElement("IdToCutsceneHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        FieldCutsceneIdToFieldCutsceneMap[id] = new Staging::FieldCutscene(pReader);
        FieldCutsceneToOriginalEnabledStateMap[FieldCutsceneIdToFieldCutsceneMap[id]] = FieldCutsceneIdToFieldCutsceneMap[id]->IsEnabled;

        FieldCutsceneIdToFieldCutsceneMap[id]->AddDialogPaths(DialogIdToDialogFilePathMap);
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadEvidences(XmlReader *pReader)
{
    pReader->StartElement("EvidenceManager");

    pReader->StartElement("IdToItemDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        EvidenceIdToEvidenceMap[id] = new Staging::Evidence(id, pReader);

        EvidenceToOriginalEnabledStateMap[EvidenceIdToEvidenceMap[id]] = EvidenceIdToEvidenceMap[id]->IsEnabled;
    }

    pReader->EndElement();

    pReader->StartElement("IdPairToCombinationConversationDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        Staging::Encounter *pEncounter = new Staging::Encounter();
        pEncounter->pOneShotConversation = Staging::Conversation::LoadFromXml(pReader);
        pEncounter->OwnsOneShotConversation = true;
        EvidenceIdPairToCombinationEncounterMap[Staging::EvidenceIdPair(pReader)] = pEncounter;
    }

    pReader->EndElement();

    pReader->StartElement("IdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        EvidenceIdList.push_back(pReader->ReadTextElement("Id"));
    }

    pReader->EndElement();

    pReader->StartElement("WrongCombinationConversation");
    pWrongEvidenceCombinationEncounter = new Staging::Encounter();
    pWrongEvidenceCombinationEncounter->pOneShotConversation = Staging::Conversation::LoadFromXml(pReader);
    pWrongEvidenceCombinationEncounter->OwnsOneShotConversation = true;
    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadLocations(XmlReader *pReader)
{
    pReader->StartElement("Content");

    pReader->StartElement("LocationByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        LocationIdToLocationMap[id] = new Staging::Location(pReader);
    }

    pReader->EndElement();

    InitialLocationId = pReader->ReadTextElement("InitialLocationId");

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadEncounters(XmlReader *pReader)
{
    pReader->StartElement("Content");

    pReader->StartElement("EncounterByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        EncounterIdToEncounterMap[id] = new Staging::Encounter(pReader);

        EncounterIdToEncounterMap[id]->AddDialogPaths(DialogIdToDialogFilePathMap);
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadConversations(XmlReader *pReader)
{
    pReader->StartElement("Content");

    pReader->StartElement("ConversationByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");

        Staging::Conversation *pConversation = Staging::Conversation::LoadFromXml(pReader);
        Staging::Interrogation *pInterrogation = dynamic_cast<Staging::Interrogation *>(pConversation);
        Staging::Confrontation *pConfrontation = dynamic_cast<Staging::Confrontation *>(pConversation);

        if (pConfrontation == NULL && pInterrogation == NULL)
        {
            ConversationIdToConversationMap[id] = pConversation;
            ConversationToOriginalEnabledStateMap[pConversation] = pConversation->IsEnabled;
            ConversationToOriginalHasBeenCompletedStateMap[pConversation] = pConversation->HasBeenCompleted;

            pConversation->AddDialogPaths(DialogIdToDialogFilePathMap);
        }
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadInterrogations(XmlReader *pReader)
{
    pReader->StartElement("Content");

    pReader->StartElement("ConversationByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");

        Staging::Conversation *pConversation = Staging::Conversation::LoadFromXml(pReader);
        Staging::Interrogation *pInterrogation = dynamic_cast<Staging::Interrogation *>(pConversation);
        Staging::Confrontation *pConfrontation = dynamic_cast<Staging::Confrontation *>(pConversation);

        if (pConfrontation == NULL && pInterrogation != NULL)
        {
            InterrogationIdToInterrogationMap[id] = pInterrogation;
            ConversationToOriginalEnabledStateMap[pInterrogation] = pInterrogation->IsEnabled;
            ConversationToOriginalHasBeenCompletedStateMap[pInterrogation] = pInterrogation->HasBeenCompleted;

            pInterrogation->AddDialogPaths(DialogIdToDialogFilePathMap);
        }
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadConfrontations(XmlReader *pReader)
{
    pReader->StartElement("Content");

    pReader->StartElement("ConversationByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");

        Staging::Conversation *pConversation = Staging::Conversation::LoadFromXml(pReader);
        Staging::Confrontation *pConfrontation = dynamic_cast<Staging::Confrontation *>(pConversation);

        if (pConfrontation != NULL)
        {
            ConfrontationIdToConfrontationMap[id] = pConfrontation;
            ConversationToOriginalEnabledStateMap[pConfrontation] = pConfrontation->IsEnabled;
            ConversationToOriginalHasBeenCompletedStateMap[pConfrontation] = pConfrontation->HasBeenCompleted;

            pConfrontation->AddDialogPaths(DialogIdToDialogFilePathMap);
        }
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadAnimations(XmlReader *pReader)
{
    pReader->StartElement("AnimationManager");

    pReader->StartElement("AnimationByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        AnimationIdToAnimationMap[id] = new Staging::Animation(pReader);
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadSprites(XmlReader *pReader)
{
    pReader->StartElement("SpriteManager");

    pReader->StartElement("SpriteByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        SpriteIdToSpriteMap[id] = new Staging::Sprite(pReader);
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::LoadSpriteSheets(XmlReader *pReader)
{
    pReader->StartElement("SpriteManager");

    pReader->StartElement("SpriteSheetIdToFilePathHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString spriteSheetId = pReader->ReadTextElement("SpriteSheetId");
        QString filePath = pReader->ReadTextElement("FilePath");

        spriteSheetIdToFilePathMap[spriteSheetId] = filePath;
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::SaveSprites(ArchiveReader *pReader)
{
    QMap<QString, QString> spriteIdToSpriteSheetIdMap;
    QStringList neededSpriteSheetIds;
    QMap<QString, QImage> spriteSheetIdToImageMap;

    int totalImageCount = SpriteIdToFilePathMap.count();
    int currentImageCount = 0;

    ProgressStageUpdated("Writing sprites to disk...");
    UpdateProgressPercent(0);

    for (QMap<QString, QString>::iterator iter = SpriteIdToFilePathMap.begin(); iter != SpriteIdToFilePathMap.end(); iter++)
    {
        if (!SpriteIdToSpriteMap.contains(iter.key()))
        {
            totalImageCount--;
            continue;
        }

        QString spriteSheetId = SpriteIdToSpriteMap[iter.key()]->SpriteSheetImageId;

        spriteIdToSpriteSheetIdMap[iter.key()] = spriteSheetId;

        if (!neededSpriteSheetIds.contains(spriteSheetId))
        {
            neededSpriteSheetIds.push_back(spriteSheetId);
        }
    }

    for (QString neededSpriteSheetId : neededSpriteSheetIds)
    {
        spriteSheetIdToImageMap[neededSpriteSheetId] = pReader->LoadImage(spriteSheetIdToFilePathMap[neededSpriteSheetId]);
    }

    for (QMap<QString, QString>::iterator iter = SpriteIdToFilePathMap.begin(); iter != SpriteIdToFilePathMap.end(); iter++)
    {
        if (!SpriteIdToSpriteMap.contains(iter.key()))
        {
            continue;
        }

        Staging::Sprite *pSprite = SpriteIdToSpriteMap[iter.key()];
        QImage sourceSpriteSheet = spriteSheetIdToImageMap[pSprite->SpriteSheetImageId];
        QImage fullSpriteSheetImage;

        // If the original size is zero, that means that this sprite was entirely transparent.
        // We'll just create a dummy 1x1 transparent image for that.
        if (pSprite->OriginalSize.GetX() == 0 && pSprite->OriginalSize.GetY() == 0)
        {
            fullSpriteSheetImage = QImage(QSize(1, 1), sourceSpriteSheet.format());
            fullSpriteSheetImage.fill(Qt::transparent);
        }
        else
        {
            fullSpriteSheetImage = QImage(pSprite->OriginalSize.ToQSize(), sourceSpriteSheet.format());
            fullSpriteSheetImage.fill(Qt::transparent);

            QPainter spriteSheetPainter(&fullSpriteSheetImage);
            spriteSheetPainter.drawImage(pSprite->SpriteDrawOffset.ToQPoint(), sourceSpriteSheet.copy(pSprite->SpriteClipRect.ToQRect()));
        }

        {
            QDir saveDir(projectImagesFileDir);
            QString saveDirString = iter.value().left(iter.value().lastIndexOf('/'));
            QString saveNameString = iter.value().right(iter.value().length() - iter.value().lastIndexOf('/'));

            saveDir.mkpath(saveDirString);
            saveDir.cd(saveDirString);

            ProgressCurrentFileUpdated(saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameString + QString("..."));

            fullSpriteSheetImage.save(saveDir.absolutePath() + saveNameString, "png");

            SpriteIdToFilePathMap[iter.key()] = saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameString;
        }

        currentImageCount++;
        UpdateProgressPercent(100 * currentImageCount / totalImageCount);
    }
}

void CaseContentLoadingStager::LoadAudio(XmlReader *pReader)
{
    BgmIdToBgmFilePathMap.clear();
    SfxIdToSfxFilePathMap.clear();
    HoofstepSoundIdListByTextureMap.clear();

    pReader->StartElement("AudioManager");

    pReader->StartElement("BgmIdToFilePathDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString bgmId = pReader->ReadTextElement("BgmId");
        QString filePath = pReader->ReadTextElement("FilePath");
        BgmIdToBgmFilePathMap[bgmId] = filePath;
    }

    pReader->EndElement();

    pReader->StartElement("SfxIdToFilePathDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString sfxId = pReader->ReadTextElement("SfxId");
        QString filePath = pReader->ReadTextElement("FilePath");
        SfxIdToSfxFilePathMap[sfxId] = filePath;
    }

    pReader->EndElement();

    pReader->StartElement("HoofstepSoundIdListByTexture");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString textureName = pReader->ReadTextElement("TextureName");

        pReader->StartElement("HoofstepSoundIdList");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            QString soundId = pReader->ReadTextElement("SoundId");

            HoofstepSoundIdListByTextureMap[textureName].push_back(soundId);
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->EndElement();
}

void CaseContentLoadingStager::SaveAudio(ArchiveReader *pReader)
{
    ProgressStageUpdated("Writing audio to disk...");
    UpdateProgressPercent(0);

    int totalAudioCount = BgmIdToBgmFilePathMap.count() + SfxIdToSfxFilePathMap.count() + DialogIdToDialogFilePathMap.count();
    int currentAudioCount = 0;

    for (QMap<QString, QString>::iterator iter = BgmIdToBgmFilePathMap.begin(); iter != BgmIdToBgmFilePathMap.end(); iter++)
    {
        SaveBGM(iter.key(), iter.value(), pReader);

        currentAudioCount++;
        UpdateProgressPercent(100 * currentAudioCount / totalAudioCount);
    }

    for (QMap<QString, QString>::iterator iter = SfxIdToSfxFilePathMap.begin(); iter != SfxIdToSfxFilePathMap.end(); iter++)
    {
        SaveSFXOrDialog(iter.key(), iter.value(), pReader, false /* isDialog */);

        currentAudioCount++;
        UpdateProgressPercent(100 * currentAudioCount / totalAudioCount);
    }

    for (QMap<QString, QString>::iterator iter = DialogIdToDialogFilePathMap.begin(); iter != DialogIdToDialogFilePathMap.end(); iter++)
    {
        SaveSFXOrDialog(iter.key(), iter.value(), pReader, true /* isDialog */);

        currentAudioCount++;
        UpdateProgressPercent(100 * currentAudioCount / totalAudioCount);
    }
}

void CaseContentLoadingStager::SaveBGM(QString id, QString filePathBase, ArchiveReader *pReader)
{
    QDir saveDir(projectAudioFileDir);

    QString saveDirString = filePathBase.left(filePathBase.lastIndexOf('/'));
    QString saveNameStringBase = filePathBase.right(filePathBase.length() - filePathBase.lastIndexOf('/'));

    saveDir.mkpath(saveDirString);
    saveDir.cd(saveDirString);

    unsigned int fileSizeA = 0;
    char *pFileContentsA = reinterpret_cast<char *>(pReader->LoadFile(filePathBase + "A.ogg", &fileSizeA));

    QFile savedFileA(saveDir.absolutePath() + saveNameStringBase + "A.ogg");

    ProgressCurrentFileUpdated(saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameStringBase + "A.ogg" + QString("..."));

    savedFileA.open(QIODevice::WriteOnly);
    savedFileA.write(pFileContentsA, (qint64)fileSizeA);
    savedFileA.close();

    free(pFileContentsA);

    unsigned int fileSizeB = 0;
    char *pFileContentsB = reinterpret_cast<char *>(pReader->LoadFile(filePathBase + "B.ogg", &fileSizeB));

    QFile savedFileB(saveDir.absolutePath() + saveNameStringBase + "B.ogg");

    ProgressCurrentFileUpdated(saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameStringBase + "B.ogg" + QString("..."));

    savedFileB.open(QIODevice::WriteOnly);
    savedFileB.write(pFileContentsB, (qint64)fileSizeB);
    savedFileB.close();

    free(pFileContentsB);

    BgmIdToBgmFilePathMap[id] = saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameStringBase;
}

void CaseContentLoadingStager::SaveSFXOrDialog(QString id, QString filePathBase, ArchiveReader *pReader, bool isDialog)
{
    QDir saveDir(projectAudioFileDir);

    QString saveDirString = filePathBase.left(filePathBase.lastIndexOf('/'));
    QString saveNameStringBase = filePathBase.right(filePathBase.length() - filePathBase.lastIndexOf('/'));

    saveDir.mkpath(saveDirString);
    saveDir.cd(saveDirString);

    unsigned int fileSize = 0;
    char *pFileContents = reinterpret_cast<char *>(pReader->LoadFile(filePathBase + ".ogg", &fileSize));

    QFile savedFile(saveDir.absolutePath() + saveNameStringBase + ".ogg");

    ProgressCurrentFileUpdated(saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameStringBase + ".ogg" + QString("..."));

    savedFile.open(QIODevice::WriteOnly);
    savedFile.write(pFileContents, (qint64)fileSize);
    savedFile.close();

    free(pFileContents);

    if (isDialog)
    {
        DialogIdToDialogFilePathMap[id] = saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameStringBase;
    }
    else
    {
        SfxIdToSfxFilePathMap[id] = saveDir.absolutePath().right(saveDir.absolutePath().length() - ProjectFileDir.absolutePath().length() - 1) + saveNameStringBase;
    }
}

void CaseContentLoadingStager::UpdateProgressPercent(int percentDone)
{
    if (lastPercentDone == percentDone)
    {
        return;
    }

    lastPercentDone = percentDone;
    ProgressPercentUpdated(percentDone);
}
