#ifndef CASECONTENT_H
#define CASECONTENT_H

#include <QMap>
#include <QString>
#include <QStringList>

#include <QObject>
#include <QDir>

#include "Character.h"
#include "Conversation.h"
#include "Encounter.h"
#include "Location.h"
#include "Flag.h"
#include "Evidence.h"
#include "BackgroundMusic.h"
#include "SoundEffect.h"

#include "../Utilities/ArchiveReader.h"
#include "../Utilities/Interfaces.h"
#include "XmlReader.h"
#include "XmlWriter.h"

// To make things easier for us, we'll declare a macro that'll enable us
// to easily do things involving all of the case content types.
// When adding a case content type, make sure that you add an entry here as well;
// the macros using this macro will take care of the rest.
#define CASE_CONTENT_TYPES_OPERATION(OPERATION) \
    OPERATION(Character) \
    OPERATION(Encounter) \
    OPERATION(Location) \
    OPERATION(Flag) \
    OPERATION(Evidence) \
    OPERATION(BackgroundMusic) \
    OPERATION(SoundEffect)

// We'll also define the names of maps in the CaseContent object
// as <TypeName>IdTo<TypeName>Map (e.g., CharacterIdToCharacterMap), for ease of reference.
#define CASE_CONTENT_MAP_NAME(TYPE) TYPE##IdTo##TYPE##Map

// First, we'll want to declare the maps themselves.
#define DECLARE_CASE_CONTENT_MAP(TYPE) \
    QMap<QString, TYPE *> CASE_CONTENT_MAP_NAME(TYPE);

// Next, we'll want to declare entries in the constructor
// and destructor that will clear the maps and delete their contents.
#define DECLARE_CASE_CONTENT_CONSTRUCTOR_ENTRY(TYPE) \
    CASE_CONTENT_MAP_NAME(TYPE).clear();

#define DECLARE_CASE_CONTENT_DESTRUCTOR_ENTRY(TYPE) \
    for (QMap<QString, TYPE *>::iterator iter = CASE_CONTENT_MAP_NAME(TYPE).begin(); iter != CASE_CONTENT_MAP_NAME(TYPE).end(); iter++) \
    { \
        delete iter.value(); \
    } \
    \
    CASE_CONTENT_MAP_NAME(TYPE).clear();

// We'll also want to declare methods to add types from a case loading stager
// and from a project file, and to save types to a project file.
#define DECLARE_CASE_CONTENT_ADD_FROM_STAGER_PROTOTYPE(TYPE) \
    void Add##TYPE##sFromStager();

#define DECLARE_CASE_CONTENT_ADD_FROM_PROJECT_FILE_PROTOTYPE(TYPE) \
    void Add##TYPE##sFromProjectFile(XmlReader *pReader);

#define DECLARE_CASE_CONTENT_SAVE_TO_PROJECT_FILE_PROTOTYPE(TYPE) \
    void Save##TYPE##sToProjectFile(XmlWriter *pWriter);

#define DECLARE_CASE_CONTENT_SAVE_TO_CASE_FILE_PROTOTYPE(TYPE) \
    void Save##TYPE##sToCaseFile(XmlWriter *pWriter);

class CaseContentLoadingStager;

class CaseContent
{
public:
    static CaseContent * GetInstance()
    {
        if (pInstance == NULL)
        {
            pInstance = new CaseContent();
        }

        return pInstance;
    }

    static void ReplaceInstance()
    {
        delete pInstance;
        pInstance = new CaseContent();
    }

    static void ReplaceInstanceWithStager();
    static void ReplaceInstanceFromProjectFile(QString filePath);

    void SaveToProjectFile();
    void CompileToCaseFile(QString filePath);

    void WriteAnimationManager(XmlWriter *pWriter);
    void WriteAudioManager(XmlWriter *pWriter);
    void WriteContent(XmlWriter *pWriter);
    void WriteDialogCharacterManager(XmlWriter *pWriter);
    void WriteDialogCutsceneManager(XmlWriter *pWriter);
    void WriteEvidenceManager(XmlWriter *pWriter);
    void WriteFieldCharacterManager(XmlWriter *pWriter);
    void WriteFieldCutsceneManager(XmlWriter *pWriter);
    void WriteFlagManager(XmlWriter *pWriter);
    void WritePartnerManager(XmlWriter *pWriter);
    void WriteSpriteManager(XmlWriter *pWriter);
    void WriteParentLocationListsBySpriteSheetId(XmlWriter *pWriter);
    void WriteParentLocationListsByVideoId(XmlWriter *pWriter);

    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_ADD_FROM_STAGER_PROTOTYPE)
    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_ADD_FROM_PROJECT_FILE_PROTOTYPE)
    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_SAVE_TO_PROJECT_FILE_PROTOTYPE)
    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_SAVE_TO_CASE_FILE_PROTOTYPE)

    // NOTE: Passing in the pointer from which to retrieve or to add things is super ugly,
    // but I can't think of a better way of doing it - there's no way to make a templated
    // virtual function in an interface, so this is the best I could come up with.
    //
    // If anyone wants to have a go at making this less hacky, be my guest.
    //
    template <class T>
    void Add(QString id, T *pObject, void *pParentObject = NULL);

    template <class T>
    T * GetById(QString id, void *pParentObject = NULL);

    template <class T>
    QStringList GetIds(void *pParentObject = NULL);

    template <class T>
    QStringList GetDisplayNames(void *pParentObject = NULL);

    QString RelativePathToAbsolutePath(QString relativePath);
    QString AbsolutePathToRelativePath(QString absolutePath);

private:
    static CaseContent *pInstance;

    CaseContent()
    {
        CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_CONSTRUCTOR_ENTRY)
    }

    ~CaseContent()
    {
        CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_DESTRUCTOR_ENTRY)
    }

    CASE_CONTENT_TYPES_OPERATION(DECLARE_CASE_CONTENT_MAP)

    QDir projectFileDir;
    QString projectFilePath;
};

#endif // CASECONTENT_H
