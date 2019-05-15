#include "CaseContent.h"

#include "Staging/CaseContentLoadingStager.h"
#include "CaseCreator/Utilities/ArchiveWriter.h"
#include "CaseCreator/Utilities/Utilities.h"

#include "Staging/ForegroundElement.Staging.h"

#define DECLARE_CASE_CONTENT_ADD_FROM_STAGER_CALL(TYPE) \
    pInstance->Add##TYPE##sFromStager();

#define DECLARE_CASE_CONTENT_ADD_FROM_PROJECT_FILE_CALL(TYPE) \
    pInstance->Add##TYPE##sFromProjectFile(&projectFileReader);

#define DECLARE_CASE_CONTENT_SAVE_TO_PROJECT_FILE_CALL(TYPE) \
   pInstance->Save##TYPE##sToProjectFile(&projectFileWriter);

#define DECLARE_CASE_CONTENT_MAP_OPERATION_FUNCTIONS(TYPE) \
template <> \
void CaseContent::Add<TYPE>(QString id, TYPE *pObject, void *) \
{ \
    CASE_CONTENT_MAP_NAME(TYPE)[id] = pObject; \
} \
\
template <> \
TYPE * CaseContent::GetById<TYPE>(QString id, void *) \
{ \
    if (CASE_CONTENT_MAP_NAME(TYPE).contains(id)) \
    { \
        return CASE_CONTENT_MAP_NAME(TYPE)[id]; \
    } \
    else \
    { \
        return NULL; \
    } \
} \
\
template <> \
QStringList CaseContent::GetIds<TYPE>(void *) \
{ \
    return CASE_CONTENT_MAP_NAME(TYPE).keys(); \
} \
\
template <> \
QStringList CaseContent::GetDisplayNames<TYPE>(void *) \
{ \
    QStringList displayNameList; \
    \
    for (QMap<QString, TYPE *>::iterator iter = CASE_CONTENT_MAP_NAME(TYPE).begin(); iter != CASE_CONTENT_MAP_NAME(TYPE).end(); iter++) \
    { \
        displayNameList.push_back(iter.value()->GetDisplayName()); \
    } \
    \
    return displayNameList; \
}

#define DECLARE_CASE_CONTENT_ADD_FROM_PROJECT_FILE_FUNCTION(TYPE) \
void CaseContent::Add##TYPE##sFromProjectFile(XmlReader *pReader) \
{ \
    if (pReader->ElementExists(#TYPE "s")) \
    { \
        pReader->StartElement(#TYPE "s"); \
        pReader->StartList(#TYPE); \
     \
        while (pReader->MoveToNextListItem()) \
        { \
            TYPE *pObject = new TYPE(pReader); \
            Add(pObject->GetId(), pObject); \
        } \
     \
        pReader->EndElement(); \
    } \
}

#define DECLARE_CASE_CONTENT_SAVE_TO_PROJECT_FILE_FUNCTION(TYPE) \
void CaseContent::Save##TYPE##sToProjectFile(XmlWriter *pWriter) \
{ \
    pWriter->StartElement(#TYPE "s"); \
 \
    for (QMap<QString, TYPE *>::iterator iter = CASE_CONTENT_MAP_NAME(TYPE).begin(); iter != CASE_CONTENT_MAP_NAME(TYPE).end(); iter++) \
    { \
        pWriter->StartElement(#TYPE); \
        (*iter)->SaveToXml(pWriter); \
        pWriter->EndElement(); \
    } \
 \
    pWriter->EndElement(); \
}

CaseContent *CaseContent::pInstance = NULL;

CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_MAP_OPERATION_FUNCTIONS)
CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_ADD_FROM_PROJECT_FILE_FUNCTION)
CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_SAVE_TO_PROJECT_FILE_FUNCTION)

void CaseContent::ReplaceInstanceWithStager()
{
    delete pInstance;
    pInstance = new CaseContent();

    pInstance->projectFileDir = CaseContentLoadingStager::GetCurrent()->ProjectFileDir;
    pInstance->projectFilePath = CaseContentLoadingStager::GetCurrent()->ProjectFilePath;

    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_ADD_FROM_STAGER_CALL)
}

void CaseContent::ReplaceInstanceFromProjectFile(QString filePath)
{
    delete pInstance;
    pInstance = new CaseContent();

    pInstance->projectFileDir = filePath.left(filePath.lastIndexOf('/'));
    pInstance->projectFilePath = filePath;

    XmlReader projectFileReader(filePath.toStdString().c_str());

    projectFileReader.StartElement("CaseContent");
    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_ADD_FROM_PROJECT_FILE_CALL)
    projectFileReader.EndElement();
}

void CaseContent::SaveToProjectFile()
{
    XmlWriter projectFileWriter(projectFilePath.toStdString().c_str(), NULL, true /* makeHumanReadable */, 2 /* formattingVersion */);

    projectFileWriter.StartElement("CaseContent");
    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_SAVE_TO_PROJECT_FILE_CALL)
    projectFileWriter.EndElement();
}

void CaseContent::CompileToCaseFile(QString filePath)
{
    ArchiveWriter caseArchiveWriter;

    if (!caseArchiveWriter.Init(projectFileDir.absolutePath() + "/" + filePath))
    {
        return;
    }

    int spriteIndex = 0;

    for (QMap<QString, Location *>::iterator iter = LocationIdToLocationMap.begin(); iter != LocationIdToLocationMap.end(); iter++)
    {
        char filename[256];
        sprintf(filename, "Images/Background%d.png", spriteIndex + 1);
        spriteIndex++;

        if (!caseArchiveWriter.SaveFile(iter.value()->GetBackgroundFilePath(), filename))
        {
            return;
        }
    }

    XmlWriter caseFileWriter(NULL, NULL, true /* makeHumanReadable */, 2 /* formattingVersion */);
    caseFileWriter.StartElement("Case");

    WriteAnimationManager(&caseFileWriter);
    WriteAudioManager(&caseFileWriter);
    WriteContent(&caseFileWriter);
    WriteDialogCharacterManager(&caseFileWriter);
    WriteDialogCutsceneManager(&caseFileWriter);
    WriteEvidenceManager(&caseFileWriter);
    WriteFieldCharacterManager(&caseFileWriter);
    WriteFieldCutsceneManager(&caseFileWriter);
    WriteFlagManager(&caseFileWriter);
    WritePartnerManager(&caseFileWriter);
    WriteSpriteManager(&caseFileWriter);
    WriteParentLocationListsBySpriteSheetId(&caseFileWriter);
    WriteParentLocationListsByVideoId(&caseFileWriter);

    caseFileWriter.EndElement();
    caseArchiveWriter.SaveToFile(caseFileWriter.GetXmlString(), "case.xml");
    caseArchiveWriter.Close();
}

void CaseContent::WriteAnimationManager(XmlWriter *pWriter)
{
    pWriter->StartElement("AnimationManager");
    pWriter->EndElement();
}

void CaseContent::WriteAudioManager(XmlWriter *pWriter)
{
    pWriter->StartElement("AudioManager");
    pWriter->EndElement();
}

void CaseContent::WriteContent(XmlWriter *pWriter)
{
    pWriter->StartElement("Content");
    pWriter->StartElement("LocationByIdHashMap");

    for (QMap<QString, Location *>::iterator iter = LocationIdToLocationMap.begin(); iter != LocationIdToLocationMap.end(); iter++)
    {
        pWriter->StartElement("Entry");
        pWriter->WriteTextElement("Id", iter.key());
        pWriter->EndElement();

        iter.value()->WriteToCaseXml(pWriter);
    }

    pWriter->EndElement();
    pWriter->EndElement();
}

void CaseContent::WriteDialogCharacterManager(XmlWriter *pWriter)
{
    pWriter->StartElement("DialogCharacterManager");
    pWriter->EndElement();
}

void CaseContent::WriteDialogCutsceneManager(XmlWriter *pWriter)
{
    pWriter->StartElement("DialogCutsceneManager");
    pWriter->EndElement();
}

void CaseContent::WriteEvidenceManager(XmlWriter *pWriter)
{
    pWriter->StartElement("EvidenceManager");
    pWriter->EndElement();
}

void CaseContent::WriteFieldCharacterManager(XmlWriter *pWriter)
{
    pWriter->StartElement("FieldCharacterManager");
    pWriter->EndElement();
}

void CaseContent::WriteFieldCutsceneManager(XmlWriter *pWriter)
{
    pWriter->StartElement("FieldCutsceneManager");
    pWriter->EndElement();
}

void CaseContent::WriteFlagManager(XmlWriter *pWriter)
{
    pWriter->StartElement("FlagManager");
    pWriter->EndElement();
}

void CaseContent::WritePartnerManager(XmlWriter *pWriter)
{
    pWriter->StartElement("PartnerManager");
    pWriter->EndElement();
}

void CaseContent::WriteSpriteManager(XmlWriter *pWriter)
{
    pWriter->StartElement("SpriteManager");
    pWriter->EndElement();
}

void CaseContent::WriteParentLocationListsBySpriteSheetId(XmlWriter *pWriter)
{
    pWriter->StartElement("ParentLocationListsBySpriteSheetId");
    pWriter->EndElement();
}

void CaseContent::WriteParentLocationListsByVideoId(XmlWriter *pWriter)
{
    pWriter->StartElement("ParentLocationListsByVideoId");
    pWriter->EndElement();
}

void CaseContent::AddCharactersFromStager()
{
    for (QMap<QString, Staging::DialogCharacter *>::iterator iter = CaseContentLoadingStager::GetCurrent()->DialogCharacterIdToDialogCharacterMap.begin();
         iter != CaseContentLoadingStager::GetCurrent()->DialogCharacterIdToDialogCharacterMap.end();
         iter++)
    {
        Add(iter.key(), new Character(iter.value(), CaseContentLoadingStager::GetCurrent()->FieldCharacterIdToFieldCharacterMap[iter.key()]));
    }
}

void CaseContent::AddLocationsFromStager()
{
    for (QMap<QString, Staging::Location *>::iterator iter = CaseContentLoadingStager::GetCurrent()->LocationIdToLocationMap.begin();
         iter != CaseContentLoadingStager::GetCurrent()->LocationIdToLocationMap.end();
         iter++)
    {
        Staging::Location *pStagingLocation = iter.value();
        Location *pLocation = new Location(pStagingLocation);

        for (QString cutsceneId : pStagingLocation->CutsceneIdList)
        {
            pLocation->AddFieldCutscene(new FieldCutscene(CaseContentLoadingStager::GetCurrent()->FieldCutsceneIdToFieldCutsceneMap[cutsceneId]));
        }

        Add(iter.key(), pLocation);
    }
}

void CaseContent::AddEncountersFromStager()
{
    for (QMap<QString, Staging::Encounter *>::iterator iter = CaseContentLoadingStager::GetCurrent()->EncounterIdToEncounterMap.begin();
         iter != CaseContentLoadingStager::GetCurrent()->EncounterIdToEncounterMap.end();
         iter++)
    {
        Staging::Encounter *pStagingEncounter = iter.value();
        Encounter *pEncounter = new Encounter(pStagingEncounter);

        for (QString conversationId : pStagingEncounter->ConversationIdList)
        {
            QString realConversationId;

            SplitConversationIdFromCaseFile(conversationId, NULL, &realConversationId);

            pEncounter->AddConversation(
                        Conversation::CreateFromStaging(CaseContentLoadingStager::GetCurrent()->ConversationIdToConversationMap[conversationId]),
                        realConversationId);
        }

        for (QString interrogationId : pStagingEncounter->InterrogationIdList)
        {
            QString realInterrogationId;

            SplitConversationIdFromCaseFile(interrogationId, NULL, &realInterrogationId);

            pEncounter->AddInterrogation(
                        Interrogation::CreateFromStaging(CaseContentLoadingStager::GetCurrent()->InterrogationIdToInterrogationMap[interrogationId]),
                        realInterrogationId);
        }

        for (QString confrontationId : pStagingEncounter->ConfrontationIdList)
        {
            QString realConfrontationId;

            SplitConversationIdFromCaseFile(confrontationId, NULL, &realConfrontationId);

            pEncounter->AddConfrontation(
                        Confrontation::CreateFromStaging(CaseContentLoadingStager::GetCurrent()->ConfrontationIdToConfrontationMap[confrontationId]),
                        realConfrontationId);
        }

        if (pStagingEncounter->OneShotConversationId.length() > 0 && pStagingEncounter->pOneShotConversation == NULL)
        {
            pEncounter->SetOneShotConversation(
                        Conversation::CreateFromStaging(CaseContentLoadingStager::GetCurrent()->ConversationIdToConversationMap[pStagingEncounter->OneShotConversationId]));
        }

        Add(iter.key(), pEncounter);
    }
}

void CaseContent::AddFlagsFromStager()
{
    for (QMap<QString, Staging::Flag *>::iterator iter = CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.begin();
         iter != CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.end();
         iter++)
    {
        Add(iter.key(), new Flag(iter.value()));
    }
}

void CaseContent::AddEvidencesFromStager()
{
    for (QMap<QString, Staging::Evidence *>::iterator iter = CaseContentLoadingStager::GetCurrent()->EvidenceIdToEvidenceMap.begin();
         iter != CaseContentLoadingStager::GetCurrent()->EvidenceIdToEvidenceMap.end();
         iter++)
    {
        Add(iter.key(), new Evidence(iter.value()));
    }
}

void CaseContent::AddBackgroundMusicsFromStager()
{
    for (QMap<QString, QString>::iterator iter = CaseContentLoadingStager::GetCurrent()->BgmIdToBgmFilePathMap.begin();
         iter != CaseContentLoadingStager::GetCurrent()->BgmIdToBgmFilePathMap.end();
         iter++)
    {
        Add(iter.key(), new BackgroundMusic(iter.key(), iter.value()));
    }
}

void CaseContent::AddSoundEffectsFromStager()
{
    for (QMap<QString, QString>::iterator iter = CaseContentLoadingStager::GetCurrent()->SfxIdToSfxFilePathMap.begin();
         iter != CaseContentLoadingStager::GetCurrent()->SfxIdToSfxFilePathMap.end();
         iter++)
    {
        Add(iter.key(), new SoundEffect(iter.key(), iter.value()));
    }
}

QString CaseContent::RelativePathToAbsolutePath(QString relativePath)
{
    return QDir(projectFileDir.absolutePath() + "/" + relativePath).canonicalPath();
}

QString CaseContent::AbsolutePathToRelativePath(QString absolutePath)
{
    return projectFileDir.relativeFilePath(absolutePath);
}
