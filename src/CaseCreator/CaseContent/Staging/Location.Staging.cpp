#include "Location.Staging.h"

#include "Condition.Staging.h"
#include "Conversation.Staging.h"
#include "Crowd.Staging.h"
#include "Encounter.Staging.h"
#include "FieldCharacter.Staging.h"
#include "ForegroundElement.Staging.h"
#include "HeightMap.Staging.h"
#include "HitBox.Staging.h"
#include "ZoomedView.Staging.h"

#include "CaseContentLoadingStager.h"

Staging::Location::Transition::~Transition()
{
    delete pHitBox;
    pHitBox = NULL;
    delete pCondition;
    pCondition = NULL;
    delete pEncounter;
    pEncounter = NULL;
}

void Staging::Location::Transition::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    pEncounter->AddDialogPaths(dialogIdToSavePathMap);
}

Staging::Location::Transition::Transition(XmlReader *pReader)
{
    pHitBox = NULL;
    pCondition = NULL;
    pEncounter = NULL;

    pReader->StartElement("Transition");
    TargetLocationId = pReader->ReadTextElement("TargetLocationId");
    TargetLocationName = pReader->ReadTextElement("TargetLocationName");

    pReader->StartElement("HitBox");
    pHitBox = new HitBox(pReader);
    pReader->EndElement();

    TransitionDirectionValue = StringToTransitionDirection(pReader->ReadTextElement("TransitionDirection"));

    pReader->StartElement("InteractionLocation");
    InteractionLocation = Vector2(pReader);
    pReader->EndElement();

    InteractFromAnywhere = pReader->ReadBooleanElement("InteractFromAnywhere");

    if (pReader->ElementExists("TransitionStartSfxId"))
    {
        TransitionStartSfxId = pReader->ReadTextElement("TransitionStartSfxId");
    }

    if (pReader->ElementExists("TransitionEndSfxId"))
    {
        TransitionEndSfxId = pReader->ReadTextElement("TransitionEndSfxId");
    }

    HideWhenLocked = pReader->ReadBooleanElement("HideWhenLocked");

    if (pReader->ElementExists("Condition"))
    {
        pReader->StartElement("Condition");
        pCondition = new Condition(pReader);
        pReader->EndElement();
    }

    if (pReader->ElementExists("Conversation"))
    {
        Conversation *pConversation = NULL;

        pReader->StartElement("Conversation");
        pConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();

        pEncounter = new Encounter();
        pEncounter->pOneShotConversation = pConversation;
        pEncounter->OwnsOneShotConversation = true;
    }

    pReader->EndElement();
}

Staging::Location::StartPosition::StartPosition(XmlReader *pReader)
{
    pReader->StartElement("StartPosition");

    pReader->StartElement("Position");
    Position = Vector2(pReader);
    pReader->EndElement();

    Direction = StringToCharacterDirection(pReader->ReadTextElement("Direction"));
    FieldDirection = StringToFieldCharacterDirection(pReader->ReadTextElement("FieldDirection"));
    pReader->EndElement();
}

Staging::Location::LoopingSound::LoopingSound(XmlReader *pReader)
{
    pReader->StartElement("LoopingSound");
    SoundId = pReader->ReadTextElement("SoundId");

    pReader->StartElement("Origin");
    Origin = Vector2(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Staging::Location::Location(XmlReader *pReader)
{
    pReader->StartElement("Location");

    Id = pReader->ReadTextElement("Id");
    BackgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");

    if (pReader->ElementExists("Bgm"))
    {
        Bgm = pReader->ReadTextElement("Bgm");
    }
    else
    {
        Bgm = "";
    }

    if (pReader->ElementExists("AmbianceSfxId"))
    {
        AmbianceSfxId = pReader->ReadTextElement("AmbianceSfxId");
    }
    else
    {
        AmbianceSfxId = "";
    }

    pAreaHitBox = new Staging::HitBox(pReader);

    pReader->StartElement("CutsceneIdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        CutsceneIdList.push_back(pReader->ReadTextElement("CutsceneId"));
    }

    pReader->EndElement();

    pReader->StartElement("ForegroundElementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ForegroundElementList.push_back(new Staging::ForegroundElement(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("HiddenForegroundElementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        HiddenForegroundElementList.push_back(new Staging::HiddenForegroundElement(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("ZoomedViewList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ZoomedView *pZoomedView = new Staging::ZoomedView(pReader);
        ZoomedViewsByIdMap[pZoomedView->Id] = pZoomedView;
    }

    pReader->EndElement();

    pReader->StartElement("CharacterList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        CharacterList.push_back(new Staging::FieldCharacter(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("CrowdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        CrowdList.push_back(new Staging::Crowd(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("TransitionList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        TransitionList.push_back(new Staging::Location::Transition(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("PreviousLocationIdToStartPositionDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString locationId = pReader->ReadTextElement("LocationId");
        TransitionIdToStartPositionMap[locationId] = Staging::Location::StartPosition(pReader);
    }

    pReader->EndElement();

    if (pReader->ElementExists("StartPositionFromMap"))
    {
        pReader->StartElement("StartPositionFromMap");
        StartPositionFromMap = Staging::Location::StartPosition(pReader);
        pReader->EndElement();
    }
    else
    {
        // This will only happen when we're loading a game from a save in a location
        // with no default start position.  In this case, we'll create a dummy start position.
        // This will be overwritten with the contents of the save file anyway.
        StartPositionFromMap = Staging::Location::StartPosition(Vector2(0, 0), CharacterDirection_Left, FieldCharacterDirection_Side);
    }

    pReader->StartElement("PreviousLocationIdToPartnerStartPositionDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString locationId = pReader->ReadTextElement("LocationId");
        TransitionIdToPartnerStartPositionMap[locationId] = Staging::Location::StartPosition(pReader);
    }

    pReader->EndElement();

    pReader->StartElement("HeightMapList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        HeightMapList.push_back(Staging::HeightMap::LoadFromXml(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("LoopingSoundList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        LoopingSoundList.push_back(new LoopingSound(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

Staging::Location::~Location()
{
    delete pAreaHitBox;
    pAreaHitBox = NULL;

    for (ForegroundElement *pForegroundElement : ForegroundElementList)
    {
        delete pForegroundElement;
    }

    ForegroundElementList.clear();

    for (HiddenForegroundElement *pHiddenForegroundElement : HiddenForegroundElementList)
    {
        delete pHiddenForegroundElement;
    }

    HiddenForegroundElementList.clear();

    for (QMap<QString, ZoomedView *>::iterator iter = ZoomedViewsByIdMap.begin(); iter != ZoomedViewsByIdMap.end(); iter++)
    {
        delete iter.value();
    }

    ZoomedViewsByIdMap.clear();

    for (FieldCharacter *pFieldCharacter : CharacterList)
    {
        delete pFieldCharacter;
    }

    CharacterList.clear();

    for (Crowd *pCrowd : CrowdList)
    {
        delete pCrowd;
    }

    CrowdList.clear();

    for (Location::Transition *pTransition : TransitionList)
    {
        delete pTransition;
    }

    TransitionList.clear();

    for (HeightMap *pHeightMap : HeightMapList)
    {
        delete pHeightMap;
    }

    HeightMapList.clear();

    for (LoopingSound *pLoopingSound : LoopingSoundList)
    {
        delete pLoopingSound;
    }

    LoopingSoundList.clear();
}

void Staging::Location::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap)
{
    QString baseDir = "Locations/" + Id + "/";

    spriteIdToSavePathMap[BackgroundSpriteId] = baseDir + "Background.png";

    for (int i = 0; i < ForegroundElementList.size(); i++)
    {
        char fileName[256];
        sprintf(fileName, "ForegroundElement%d", (i + 1));

        ForegroundElementList[i]->AddSpritePaths(spriteIdToSavePathMap, baseDir + fileName);
    }

    for (int i = 0; i < HiddenForegroundElementList.size(); i++)
    {
        char fileName[256];
        sprintf(fileName, "HiddenForegroundElement%d", (i + 1));

        HiddenForegroundElementList[i]->AddSpritePaths(spriteIdToSavePathMap, baseDir + fileName);
    }

    for (QString zoomedViewId : ZoomedViewsByIdMap.keys())
    {
        ZoomedViewsByIdMap[zoomedViewId]->AddSpritePaths(spriteIdToSavePathMap, baseDir);
    }
}

void Staging::Location::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    for (ForegroundElement *pForegroundElement : ForegroundElementList)
    {
        pForegroundElement->AddDialogPaths(dialogIdToSavePathMap);
    }

    for (HiddenForegroundElement *pHiddenForegroundElement : HiddenForegroundElementList)
    {
        pHiddenForegroundElement->AddDialogPaths(dialogIdToSavePathMap);
    }

    for (Crowd *pCrowd : CrowdList)
    {
        pCrowd->AddDialogPaths(dialogIdToSavePathMap);
    }

    for (Location::Transition *pTransition : TransitionList)
    {
        pTransition->AddDialogPaths(dialogIdToSavePathMap);
    }
}
