#include "FieldCutscene.Staging.h"

#include "Encounter.Staging.h"
#include "FieldCharacter.Staging.h"

int Staging::FieldCutscene::FieldCutsceneSetBackground::BackgroundCount = 0;

Staging::FieldCutscene::FieldCutscene(XmlReader *pReader)
{
    pReader->StartElement("FieldCutscene");

    Id = pReader->ReadTextElement("Id");

    pReader->StartElement("PhaseList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        PhaseList.push_back(GetPhaseForNextElement(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("IdToCharacterDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        IdToCharacterMap[id] = new FieldCharacter(pReader);

        CharacterToOriginalCharacterDirectionMap[IdToCharacterMap[id]] = IdToCharacterMap[id]->Direction;
        CharacterToOriginalFieldCharacterDirectionMap[IdToCharacterMap[id]] = IdToCharacterMap[id]->SpriteDirection;
        CharacterToOriginalPositionMap[IdToCharacterMap[id]] = IdToCharacterMap[id]->Position;
    }

    pReader->EndElement();

    IsEnabled = pReader->ReadBooleanElement("IsEnabled");
    PreventsInitialBgm = pReader->ReadBooleanElement("PreventInitialBgm");

    if (pReader->ElementExists("InitialBgmReplacement"))
    {
        InitialBgmReplacement = pReader->ReadTextElement("InitialBgmReplacement");
    }

    PreventsInitialAmbiance = pReader->ReadBooleanElement("PreventInitialAmbiance");

    if (pReader->ElementExists("InitialAmbianceReplacement"))
    {
        InitialAmbianceReplacement = pReader->ReadTextElement("InitialAmbianceReplacement");
    }

    pReader->StartElement("InitialCameraPosition");
    InitialCameraPosition = Vector2(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Staging::FieldCutscene::~FieldCutscene()
{
    for (FieldCutscenePhase *pPhase : PhaseList)
    {
        delete pPhase;
    }

    PhaseList.clear();

    for (QMap<QString, FieldCharacter *>::iterator iter = IdToCharacterMap.begin(); iter != IdToCharacterMap.end(); ++iter)
    {
        delete iter.value();
    }

    IdToCharacterMap.clear();
}

void Staging::FieldCutscene::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap)
{
    QString baseDir = "Cutscenes/Field/" + Id + "/";
    Staging::FieldCutscene::FieldCutsceneSetBackground::BackgroundCount = 0;

    for (FieldCutscenePhase *pPhase : PhaseList)
    {
        pPhase->AddSpritePaths(spriteIdToSavePathMap, baseDir);
    }
}

void Staging::FieldCutscene::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    for (FieldCutscenePhase *pPhase : PhaseList)
    {
        pPhase->AddDialogPaths(dialogIdToSavePathMap);
    }
}

Staging::FieldCutscene::FieldCutscenePhase * Staging::FieldCutscene::GetPhaseForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("FieldCutsceneConversation"))
    {
        return new FieldCutsceneConversation(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneConcurrentMovements"))
    {
        return new FieldCutsceneConcurrentMovements(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneMovement"))
    {
        return new FieldCutsceneMovement(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneOrient"))
    {
        return new FieldCutsceneOrient(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneSetBackground"))
    {
        return new FieldCutsceneSetBackground(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneCenterCamera"))
    {
        return new FieldCutsceneCenterCamera(pReader);
    }
    else if (pReader->ElementExists("FieldCutscenePause"))
    {
        return new FieldCutscenePause(pReader);
    }
    else if (pReader->ElementExists("FieldCutscenePlayBgm"))
    {
        return new FieldCutscenePlayBgm(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneStopBgm"))
    {
        return new FieldCutsceneStopBgm(pReader);
    }
    else if (pReader->ElementExists("FieldCutscenePlayAmbiance"))
    {
        return new FieldCutscenePlayAmbiance(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneStopAmbiance"))
    {
        return new FieldCutsceneStopAmbiance(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneEndCase"))
    {
        return new FieldCutsceneEndCase(pReader);
    }
    else
    {
        throw new MLIException("Unknown phase type.");
    }
}

Staging::FieldCutscene::FieldCutsceneConversation::~FieldCutsceneConversation()
{
    delete pEncounter;
    pEncounter = NULL;
}

void Staging::FieldCutscene::FieldCutsceneConversation::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    pEncounter->AddDialogPaths(dialogIdToSavePathMap);
}

Staging::FieldCutscene::FieldCutsceneConversation::FieldCutsceneConversation(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneConversation");
    MillisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");
    pEncounter = new Encounter(pReader);
    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutsceneConcurrentMovements::~FieldCutsceneConcurrentMovements()
{
    for (FieldCutscenePhase *pMovementPhase : MovementList)
    {
        delete pMovementPhase;
    }

    MovementList.clear();
}

Staging::FieldCutscene::FieldCutsceneConcurrentMovements::FieldCutsceneConcurrentMovements(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneConcurrentMovements");
    MillisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");

    pReader->StartElement("MovementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        MovementList.push_back(GetPhaseForNextElement(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutscenePhase * Staging::FieldCutscene::FieldCutsceneConcurrentMovements::GetPhaseForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("FieldCutsceneConversation"))
    {
        return new FieldCutsceneConversation(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneConcurrentMovements"))
    {
        return new FieldCutsceneConcurrentMovements(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneMovement"))
    {
        return new FieldCutsceneMovement(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneOrient"))
    {
        return new FieldCutsceneOrient(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneSetBackground"))
    {
        return new FieldCutsceneSetBackground(pReader);
    }
    else if (pReader->ElementExists("FieldCutsceneCenterCamera"))
    {
        return new FieldCutsceneCenterCamera(pReader);
    }
    else
    {
        throw new MLIException("Unknown phase type.");
    }
}

void Staging::FieldCutscene::FieldCutsceneConcurrentMovements::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString baseDir)
{
    for (FieldCutscenePhase *pMovementPhase : MovementList)
    {
        pMovementPhase->AddSpritePaths(spriteIdToSavePathMap, baseDir);
    }
}

Staging::FieldCutscene::FieldCutsceneMovement::FieldCutsceneMovement(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneMovement");
    MillisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");
    CharacterId = pReader->ReadTextElement("CharacterId");

    pReader->StartElement("TargetPosition");
    TargetPosition = Vector2(pReader);
    pReader->EndElement();

    MovementState = StringToFieldCharacterState(pReader->ReadTextElement("MovementState"));

    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutsceneOrient::FieldCutsceneOrient(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneOrient");
    MillisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");
    CharacterId = pReader->ReadTextElement("CharacterId");
    Direction = StringToCharacterDirection(pReader->ReadTextElement("Direction"));
    SpriteDirection = StringToFieldCharacterDirection(pReader->ReadTextElement("SpriteDirection"));
    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutsceneSetBackground::FieldCutsceneSetBackground(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneSetBackground");

    if (pReader->ElementExists("BackgroundSpriteId"))
    {
        BackgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");
    }

    MsFadeDuration = pReader->ReadIntElement("MsFadeDuration");
    pReader->EndElement();
}

void Staging::FieldCutscene::FieldCutsceneSetBackground::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString baseDir)
{
    char fileName[256];
    sprintf(fileName, "Background%d.png", (Staging::FieldCutscene::FieldCutsceneSetBackground::BackgroundCount + 1));
    Staging::FieldCutscene::FieldCutsceneSetBackground::BackgroundCount++;

    spriteIdToSavePathMap[BackgroundSpriteId] = baseDir + fileName;
}

Staging::FieldCutscene::FieldCutsceneCenterCamera::FieldCutsceneCenterCamera(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneCenterCamera");

    MillisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");

    if (pReader->ElementExists("CharacterId"))
    {
        CharacterId = pReader->ReadTextElement("CharacterId");
    }
    else
    {
        pReader->StartElement("CameraCenterPosition");
        CameraCenterPosition = Vector2(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutscenePause::FieldCutscenePause(XmlReader *pReader)
{
    pReader->StartElement("FieldCutscenePause");
    MsPauseDuration = pReader->ReadIntElement("MsPauseDuration");
    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutscenePlayBgm::FieldCutscenePlayBgm(XmlReader *pReader)
{
    pReader->StartElement("FieldCutscenePlayBgm");
    BgmId = pReader->ReadTextElement("BgmId");
    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutsceneStopBgm::FieldCutsceneStopBgm(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneStopBgm");
    IsInstant = pReader->ReadBooleanElement("IsInstant");
    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutscenePlayAmbiance::FieldCutscenePlayAmbiance(XmlReader *pReader)
{
    pReader->StartElement("FieldCutscenePlayAmbiance");
    AmbianceSfxId = pReader->ReadTextElement("AmbianceSfxId");
    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutsceneStopAmbiance::FieldCutsceneStopAmbiance(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneStopAmbiance");
    IsInstant = pReader->ReadBooleanElement("IsInstant");
    pReader->EndElement();
}

Staging::FieldCutscene::FieldCutsceneEndCase::FieldCutsceneEndCase(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneEndCase");
    CompletesCase = pReader->ReadBooleanElement("CompletesCase");
    pReader->EndElement();
}
