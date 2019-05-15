#ifndef CASECONTENTLOADINGSTAGER_H
#define CASECONTENTLOADINGSTAGER_H

#include "DialogCharacter.Staging.h"
#include "DialogCutscene.Staging.h"
#include "Evidence.Staging.h"
#include "FieldCharacter.Staging.h"
#include "FieldCutscene.Staging.h"
#include "Location.Staging.h"

#include "Encounter.Staging.h"
#include "Conversation.Staging.h"
#include "Animation.Staging.h"
#include "Sprite.Staging.h"

#include "Flag.Staging.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QDir>

// To make things easier for us, we'll declare a macro that'll enable us
// to easily do things involving all of the root staging helper types.
// When adding a staging helper type, make sure that you add an entry here as well;
// the macros using this macro will take care of the rest.

// Only some of these represent root types with sprites to be saved,
// so we'll define those types separately.
#define STAGING_HELPER_ROOT_TYPES_OPERATION(OPERATION) \
    OPERATION(DialogCharacter) \
    OPERATION(FieldCharacter) \
    OPERATION(DialogCutscene) \
    OPERATION(FieldCutscene) \
    OPERATION(Evidence) \
    OPERATION(Location) \

#define STAGING_HELPER_TYPES_OPERATION(OPERATION) \
    STAGING_HELPER_ROOT_TYPES_OPERATION(OPERATION) \
    OPERATION(Encounter) \
    OPERATION(Conversation) \
    OPERATION(Interrogation) \
    OPERATION(Confrontation) \
    OPERATION(Animation) \
    OPERATION(Sprite)

// We'll also define the names of maps and loading methods in the CaseContentLoadingStager object
// as <TypeName>IdTo<TypeName>Map and Load<TypeName>s(), for ease of reference.
#define STAGING_HELPER_MAP_NAME(TYPE) TYPE##IdTo##TYPE##Map
#define STAGING_HELPER_LOAD_METHOD_NAME(TYPE) Load##TYPE##s

// First, we'll want to declare the maps themselves.
#define DECLARE_STAGING_HELPER_MAP(TYPE) \
    QMap<QString, Staging::TYPE *> STAGING_HELPER_MAP_NAME(TYPE);

// First, we'll want to declare the maps themselves.
#define DECLARE_STAGING_HELPER_LOAD_METHOD(TYPE) \
    void STAGING_HELPER_LOAD_METHOD_NAME(TYPE)(XmlReader *pReader);

// Next, we'll want to declare entries in the constructor
// and destructor that will clear the maps and delete their contents.
#define DECLARE_STAGING_HELPER_CONSTRUCTOR_ENTRY(TYPE) \
    STAGING_HELPER_MAP_NAME(TYPE).clear();

#define DECLARE_STAGING_HELPER_DESTRUCTOR_ENTRY(TYPE) \
    for (QMap<QString, TYPE *>::iterator iter = STAGING_HELPER_MAP_NAME(TYPE).begin(); iter != STAGING_HELPER_MAP_NAME(TYPE).end(); iter++) \
    { \
        delete iter.value(); \
    } \
    \
    STAGING_HELPER_MAP_NAME(TYPE).clear();

class ArchiveReader;

class CaseContentLoadingStager : public QObject
{
    Q_OBJECT

public:
    static CaseContentLoadingStager * GetCurrent()
    {
        return pCurrentInstance;
    }

    static void BeginStaging(const QString &projectFilePath, const QString &projectFileName)
    {
        delete pCurrentInstance;
        pCurrentInstance = new CaseContentLoadingStager(projectFilePath, projectFileName);
    }

    static void EndStaging()
    {
        delete pCurrentInstance;
        pCurrentInstance = NULL;
    }

    void LoadFromCaseFile(QString caseFilePath);

    QString GetSpriteFilePathFromId(QString spriteId);

    STAGING_HELPER_TYPES_OPERATION(DECLARE_STAGING_HELPER_MAP)
    DECLARE_STAGING_HELPER_MAP(Flag)
    QMap<QString, QString> SpriteIdToFilePathMap;

    QString PlayerCharacterId;
    QString InitialLocationId;

    QMap<Staging::EvidenceIdPair, Staging::Encounter *> EvidenceIdPairToCombinationEncounterMap;
    QList<QString> EvidenceIdList;
    QMap<Staging::Evidence *, bool> EvidenceToOriginalEnabledStateMap;
    Staging::Encounter *pWrongEvidenceCombinationEncounter;

    QMap<Staging::FieldCutscene *, bool> FieldCutsceneToOriginalEnabledStateMap;

    QMap<Staging::Conversation *, bool> ConversationToOriginalEnabledStateMap;
    QMap<Staging::Conversation *, bool> ConversationToOriginalHasBeenCompletedStateMap;

    QMap<QString, QString> BgmIdToBgmFilePathMap;
    QMap<QString, QString> SfxIdToSfxFilePathMap;
    QMap<QString, QString> DialogIdToDialogFilePathMap;
    QMap<QString, QList<QString> > HoofstepSoundIdListByTextureMap;

    QDir ProjectFileDir;
    QString ProjectFilePath;

signals:
    void ProgressStageUpdated(QString currentStage);
    void ProgressPercentUpdated(int percentDone);
    void ProgressCurrentFileUpdated(QString currentFile);

private:
    static CaseContentLoadingStager *pCurrentInstance;

    CaseContentLoadingStager(const QString &projectFilePath, const QString &projectFileName)
        : ProjectFileDir(QDir(projectFilePath))
        , ProjectFilePath(QDir(projectFilePath).absoluteFilePath(projectFileName))
        , projectImagesFileDir(QDir(projectFilePath))
        , projectAudioFileDir(QDir(projectFilePath))
    {
        STAGING_HELPER_TYPES_OPERATION(DECLARE_STAGING_HELPER_CONSTRUCTOR_ENTRY)
        DECLARE_STAGING_HELPER_CONSTRUCTOR_ENTRY(Flag)

        ProjectFileDir.mkpath(".");

        projectImagesFileDir.mkpath("Images");
        projectImagesFileDir.cd("Images");

        projectAudioFileDir.mkpath("Audio");
        projectAudioFileDir.cd("Audio");

        lastPercentDone = 0;
    }

    ~CaseContentLoadingStager()
    {
        STAGING_HELPER_TYPES_OPERATION(DECLARE_STAGING_HELPER_CONSTRUCTOR_ENTRY)
        DECLARE_STAGING_HELPER_CONSTRUCTOR_ENTRY(Flag)
    }

    void UpdateProgressPercent(int percentDone);

    STAGING_HELPER_TYPES_OPERATION(DECLARE_STAGING_HELPER_LOAD_METHOD)
    void LoadSpriteSheets(XmlReader *pReader);

    void SaveSprites(ArchiveReader *pReader);

    void LoadAudio(XmlReader *pReader);
    void SaveAudio(ArchiveReader *pReader);
    void SaveBGM(QString id, QString filePathBase, ArchiveReader *pReader);
    void SaveSFXOrDialog(QString id, QString filePathBase, ArchiveReader *pReader, bool isDialog);

    QMap<QString, QString> spriteSheetIdToFilePathMap;

    QDir projectImagesFileDir;
    QDir projectAudioFileDir;

    int lastPercentDone;
};

#endif // CASECONTENTLOADINGSTAGER_H
