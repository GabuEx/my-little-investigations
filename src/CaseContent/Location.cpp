/**
 * Handles a single area in the game.
 *
 * @author GabuEx, dawnmew
 * @since 1.0
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2014 Equestrian Dreamers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Location.h"
#include "../FileFunctions.h"
#include "../Game.h"
#include "../globals.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../PositionalSound.h"
#include "../TransitionRequest.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"
#include "../CaseInformation/Case.h"
#include "../CaseInformation/CommonCaseResources.h"
#include "../Screens/MLIScreen.h"
#include <algorithm>
#include <limits>
#include <math.h>

#ifdef __OSX
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

const int MinDistanceToRevealHiddenElements = 200; // px
const int MaxDistanceToStartInteractions = 150; // px
const int FadeAnimationDuration = 500; // ms

const int CursorExtremeThreshold = 40; // px
const int CursorHighThreshold = 100; // px
const int CursorMidThreshold = 200; // px
const int CursorLowThreshold = 300; // px

const int WalkingSpeed = 300; // px / s
const int RunningSpeed = 600; // px / s

const bool bSkipIntroCutscene = false;

const double KeyboardMovementVectorLength = 50.0;   //In this case, fairly arbitrary, as we're moving the player directly

Image *Location::pFadeSprite = NULL;
FieldCharacter *Location::pCurrentPlayerCharacter = NULL;
string Location::pendingTransitionEndSfxId = "";

Location::PathfindingThreadParameters::PathfindingThreadParameters(Location *pLocation, FieldCharacter *pCharacter, Vector2 startPosition, Vector2 endPosition, FieldCharacterState characterStateIfMoving, int threadId)
    : pLocation(pLocation)
    , pCharacter(pCharacter)
    , startPosition(startPosition)
    , endPosition(endPosition)
    , characterStateIfMoving(characterStateIfMoving)
    , threadId(threadId)
{
}

Location * Location::Transition::GetTargetLocation()
{
    if (pTargetLocation == NULL)
    {
        pTargetLocation = Case::GetInstance()->GetContentManager()->GetLocationFromId(targetLocationId);
    }

    return pTargetLocation;
}

void Location::Transition::BeginInteraction(Location *pLocation)
{
    if (GetCondition() == NULL || GetCondition()->IsTrue())
    {
        pLocation->BeginTransition(
            GetTargetLocation(),
            pLocation->id);

        if (transitionStartSfxId.length() > 0)
        {
            playSound(transitionStartSfxId);
        }

        pendingTransitionEndSfxId = transitionEndSfxId;
    }
    else if (GetEncounter() != NULL)
    {
        pLocation->pCurrentEncounter = GetEncounter();
        pLocation->pCurrentEncounter->Begin();
    }

    pLocation->characterStateMap[pLocation->pPlayerCharacter] = FieldCharacterStateStanding;
    pLocation->pTransitionAtPlayer = this;
}

Location::Transition::Transition(XmlReader *pReader)
{
    pTargetLocation = NULL;
    pHitBox = NULL;
    pCondition = NULL;
    pEncounter = NULL;

    pReader->StartElement("Transition");
    targetLocationId = pReader->ReadTextElement("TargetLocationId");
    targetLocationName = pReader->ReadTextElement("TargetLocationName");

    pReader->StartElement("HitBox");
    pHitBox = new HitBox(pReader);
    pReader->EndElement();

    transitionDirection = StringToTransitionDirection(pReader->ReadTextElement("TransitionDirection"));

    pReader->StartElement("InteractionLocation");
    interactionLocation = Vector2(pReader);
    pReader->EndElement();

    interactFromAnywhere = pReader->ReadBooleanElement("InteractFromAnywhere");

    if (pReader->ElementExists("TransitionStartSfxId"))
    {
        transitionStartSfxId = pReader->ReadTextElement("TransitionStartSfxId");
    }

    if (pReader->ElementExists("TransitionEndSfxId"))
    {
        transitionEndSfxId = pReader->ReadTextElement("TransitionEndSfxId");
    }

    hideWhenLocked = pReader->ReadBooleanElement("HideWhenLocked");

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
        pConversation = new Conversation(pReader);
        pReader->EndElement();

        pEncounter = new Encounter();
        pEncounter->SetOneShotConversation(pConversation);
        pEncounter->SetOwnsOneShotConversation(true /* ownsOneShotConversation */);
    }

    pReader->EndElement();
}

Location::Transition::Transition(const Location::Transition &other)
    : targetLocationId(other.targetLocationId)
    , targetLocationName(other.targetLocationName)
{
    this->pTargetLocation = NULL;
    this->pHitBox = other.pHitBox != NULL ? other.pHitBox->Clone() : NULL;
    this->transitionDirection = other.transitionDirection;
    this->hideWhenLocked = other.hideWhenLocked;
    this->pCondition = other.pCondition->Clone();
    this->pEncounter = NULL;
}

Location::StartPosition::StartPosition(XmlReader *pReader)
{
    pReader->StartElement("StartPosition");

    pReader->StartElement("Position");
    position = Vector2(pReader);
    pReader->EndElement();

    direction = StringToCharacterDirection(pReader->ReadTextElement("Direction"));
    fieldDirection = StringToFieldCharacterDirection(pReader->ReadTextElement("FieldDirection"));
    pReader->EndElement();
}

Location::LoopingSound::LoopingSound(XmlReader *pReader)
{
    pReader->StartElement("LoopingSound");
    this->soundId = pReader->ReadTextElement("SoundId");

    pReader->StartElement("Origin");
    this->origin = Vector2(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Location::Location(XmlReader *pReader)
    : bounds(RectangleWH(0, 0, 0, 0))
{
    pBackgroundSprite = NULL;
    pPlayerCharacter = NULL;
    pPartnerCharacter = NULL;
    pTransitionAtPlayer = NULL;
    pCurrentEncounter = NULL;
    pCurrentInteractiveElement = NULL;
    pCurrentInteractiveForegroundElement = NULL;
    pCurrentInteractiveCrowd = NULL;
    pCurrentZoomedView = NULL;
    pCurrentCutscene = NULL;
    pNextCutscene = NULL;
    pInEasePartner = NULL;
    pOutEasePartner = NULL;
    pTargetInteractiveElement = NULL;
    pAreaHitBox = NULL;
    animationOffsetPartner = gScreenWidth;
    movingDirectly = false;
    pPathfindingValuesSemaphore = SDL_CreateSemaphore(1);
    lastPathfindingThreadId = 0;

    pEvidenceTab = new Tab(gScreenWidth - 3 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/EvidenceText"), false /* useCancelClickSoundEffect */, TabRowBottom, true /* canPulse */);
    pEvidenceSelector = new EvidenceSelector(true /* isCancelable */, true /* isForCombination */);
    pPartnerTab = new Tab(gScreenWidth - 2 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/PartnerText"), false /* useCancelClickSoundEffect */, TabRowBottom);
    pInEasePartner = new LinearEase(gScreenWidth, 0, AnimationDuration);
    pOutEasePartner = new LinearEase(0, -gScreenWidth, AnimationDuration);
    pPartnerAbilityTab = new Tab((gScreenWidth - TabWidth) / 2, true /* isClickable */, "", false /* useCancelClickSoundEffect */, TabRowTop, true /* canPulse */);
    pFadeInEase = new LinearEase(1.0, 0.0, FadeAnimationDuration);
    pFadeOutEase = new LinearEase(0.0, 1.0, FadeAnimationDuration);

    fadeOpacity = 0;
    pTargetLocation = NULL;
    isTransitioning = false;
    isFinishing = false;
    overlayId = "";
    wasInOverlay = false;

    pOkTab = new Tab(gScreenWidth - (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/OKText"));
    pSaveTab = new Tab(gScreenWidth - 5 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/SaveText"), false /* useCancelClickSoundEffect */, TabRowBottom);
    pLoadTab = new Tab(gScreenWidth - 4 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/LoadText"), false /* useCancelClickSoundEffect */, TabRowBottom);
    pOptionsTab = new Tab(7, true /* isClickable */, pgLocalizableContent->GetText("Location/OptionsText"), false /* useCancelClickSoundEffect */, TabRowTop);
    pQuitTab = new Tab(gScreenWidth - (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/QuitText"), false /* useCancelClickSoundEffect */, TabRowTop);

    pQuitConfirmOverlay = new PromptOverlay(pgLocalizableContent->GetText("Location/QuitPromptText"), false /* allowsTextEntry */);
    pQuitConfirmOverlay->AddButton(pgLocalizableContent->GetText("Location/YesText"));
    pQuitConfirmOverlay->AddButton(pgLocalizableContent->GetText("Location/NoText"));
    pQuitConfirmOverlay->FinalizeButtons();

    shouldAutosave = false;

    pReader->StartElement("Location");

    id = pReader->ReadTextElement("Id");
    backgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");

    if (pReader->ElementExists("Bgm"))
    {
        bgm = pReader->ReadTextElement("Bgm");
    }
    else
    {
        bgm = "";
    }

    if (pReader->ElementExists("AmbianceSfxId"))
    {
        ambianceSfxId = pReader->ReadTextElement("AmbianceSfxId");
    }
    else
    {
        ambianceSfxId = "";
    }

    pAreaHitBox = new HitBox(pReader);

    pReader->StartElement("CutsceneIdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        cutsceneIdList.push_back(pReader->ReadTextElement("CutsceneId"));
    }

    pReader->EndElement();

    pReader->StartElement("ForegroundElementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        foregroundElementList.push_back(new ForegroundElement(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("HiddenForegroundElementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        hiddenForegroundElementList.push_back(new HiddenForegroundElement(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("ZoomedViewList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ZoomedView *pZoomedView = new ZoomedView(pReader);
        zoomedViewsByIdMap[pZoomedView->GetId()] = pZoomedView;
    }

    pReader->EndElement();

    pReader->StartElement("CharacterList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        characterList.push_back(new FieldCharacter(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("CrowdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        crowdList.push_back(new Crowd(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("TransitionList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        transitionList.push_back(new Transition(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("PreviousLocationIdToStartPositionDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string locationId = pReader->ReadTextElement("LocationId");
        transitionIdToStartPositionMap[locationId] = StartPosition(pReader);
    }

    pReader->EndElement();

    if (pReader->ElementExists("StartPositionFromMap"))
    {
        pReader->StartElement("StartPositionFromMap");
        startPositionFromMap = StartPosition(pReader);
        pReader->EndElement();
    }
    else
    {
        // This will only happen when we're loading a game from a save in a location
        // with no default start position.  In this case, we'll create a dummy start position.
        // This will be overwritten with the contents of the save file anyway.
        startPositionFromMap = StartPosition(Vector2(0, 0), CharacterDirectionLeft, FieldCharacterDirectionSide);
    }

    pReader->StartElement("PreviousLocationIdToPartnerStartPositionDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string locationId = pReader->ReadTextElement("LocationId");
        transitionIdToPartnerStartPositionMap[locationId] = StartPosition(pReader);
    }

    pReader->EndElement();

    pReader->StartElement("HeightMapList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        heightMapList.push_back(HeightMap::LoadFromXml(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("LoopingSoundList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        loopingSoundList.push_back(new LoopingSound(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();

    fadeOpacity = 1;

    pEvidenceTab->SetIsHidden(true, false);
    pPartnerTab->SetIsHidden(true, false);
    pPartnerAbilityTab->SetIsHidden(true, false);
    pOkTab->SetIsHidden(true, false);
    pSaveTab->SetIsHidden(true, false);
    pLoadTab->SetIsHidden(true, false);
    pOptionsTab->SetIsHidden(true, false);
    pQuitTab->SetIsHidden(true, false);

    EventProviders::GetPromptOverlayEventProvider()->ClearListener(this);
    EventProviders::GetPromptOverlayEventProvider()->RegisterListener(this);
}

Location::Location(const Location &other)
    : bounds(RectangleWH(0, 0, 0, 0))
{
    pBackgroundSprite = NULL;
    pPlayerCharacter = NULL;
    pPartnerCharacter = NULL;
    pTransitionAtPlayer = NULL;
    pCurrentEncounter = NULL;
    pCurrentInteractiveElement = NULL;
    pCurrentInteractiveForegroundElement = NULL;
    pCurrentInteractiveCrowd = NULL;
    pCurrentZoomedView = NULL;
    pCurrentCutscene = NULL;
    pNextCutscene = NULL;
    pInEasePartner = NULL;
    pOutEasePartner = NULL;
    pTargetInteractiveElement = NULL;
    pAreaHitBox = NULL;
    animationOffsetPartner = gScreenWidth;
    movingDirectly = false;
    pPathfindingValuesSemaphore = SDL_CreateSemaphore(1);
    lastPathfindingThreadId = 0;

    pEvidenceTab = new Tab(gScreenWidth - 3 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/EvidenceText"), false /* useCancelClickSoundEffect */, TabRowBottom, true /* canPulse */);
    pEvidenceSelector = new EvidenceSelector(true /* isCancelable */, true /* isForCombination */);
    pPartnerTab = new Tab(gScreenWidth - 2 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/PartnerText"), false /* useCancelClickSoundEffect */, TabRowBottom);
    pInEasePartner = new LinearEase(gScreenWidth, 0, AnimationDuration);
    pOutEasePartner = new LinearEase(0, -gScreenWidth, AnimationDuration);
    pPartnerAbilityTab = new Tab((gScreenWidth - TabWidth) / 2, true /* isClickable */, "", false /* useCancelClickSoundEffect */, TabRowTop, true /* canPulse */);
    pFadeInEase = new LinearEase(1.0, 0.0, FadeAnimationDuration);
    pFadeOutEase = new LinearEase(0.0, 1.0, FadeAnimationDuration);

    fadeOpacity = 0;
    pTargetLocation = NULL;
    isTransitioning = false;
    isFinishing = false;
    overlayId = "";
    wasInOverlay = false;

    pOkTab = new Tab(gScreenWidth - (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/OKText"));
    pSaveTab = new Tab(gScreenWidth - 5 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/SaveText"), false /* useCancelClickSoundEffect */, TabRowBottom);
    pLoadTab = new Tab(gScreenWidth - 4 * (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/LoadText"), false /* useCancelClickSoundEffect */, TabRowBottom);
    pOptionsTab = new Tab(7, true /* isClickable */, pgLocalizableContent->GetText("Location/OptionsText"), false /* useCancelClickSoundEffect */, TabRowTop);
    pQuitTab = new Tab(gScreenWidth - (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Location/QuitText"), false /* useCancelClickSoundEffect */, TabRowTop);

    pQuitConfirmOverlay = new PromptOverlay(pgLocalizableContent->GetText("Location/QuitPromptText"), false /* allowsTextEntry */);
    pQuitConfirmOverlay->AddButton(pgLocalizableContent->GetText("Location/YesText"));
    pQuitConfirmOverlay->AddButton(pgLocalizableContent->GetText("Location/NoText"));

    backgroundSpriteId = other.backgroundSpriteId;
    bgm = other.bgm;
    pAreaHitBox = other.pAreaHitBox->Clone();

    for (unsigned int i = 0; i < other.cutsceneIdList.size(); i++)
    {
        cutsceneIdList.push_back(other.cutsceneIdList[i]);
    }

    fadeOpacity = 1;
    shouldAutosave = false;

    pEvidenceTab->SetIsHidden(true, false);
    pPartnerTab->SetIsHidden(true, false);
    pPartnerAbilityTab->SetIsHidden(true, false);
    pOkTab->SetIsHidden(true, false);
    pSaveTab->SetIsHidden(true, false);
    pLoadTab->SetIsHidden(true, false);
    pOptionsTab->SetIsHidden(true, false);
    pQuitTab->SetIsHidden(true, false);

    EventProviders::GetPromptOverlayEventProvider()->ClearListener(this);
    EventProviders::GetPromptOverlayEventProvider()->RegisterListener(this);
}

Location::~Location()
{
    delete pPartnerCharacter;
    pPartnerCharacter = NULL;

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        delete characterList[i];
    }

    for (unsigned int i = 0; i < crowdList.size(); i++)
    {
        delete crowdList[i];
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        delete foregroundElementList[i];
    }

    for (unsigned int i = 0; i < hiddenForegroundElementList.size(); i++)
    {
        delete hiddenForegroundElementList[i];
    }

    for (map<string, ZoomedView *>::iterator iter = zoomedViewsByIdMap.begin(); iter != zoomedViewsByIdMap.end(); ++iter)
    {
        delete iter->second;
    }

    for (unsigned int i = 0; i < transitionList.size(); i++)
    {
        delete transitionList[i];
    }

    for (unsigned int i = 0; i < heightMapList.size(); i++)
    {
        delete heightMapList[i];
    }

    for (unsigned int i = 0; i < loopingSoundList.size(); i++)
    {
        delete loopingSoundList[i];
    }

    delete pInEasePartner;
    pInEasePartner = NULL;
    delete pOutEasePartner;
    pOutEasePartner = NULL;
    delete pFadeInEase;
    pFadeInEase = NULL;
    delete pFadeOutEase;
    pFadeOutEase = NULL;
    delete pAreaHitBox;
    pAreaHitBox = NULL;
    delete pEvidenceSelector;
    pEvidenceSelector = NULL;

    delete pEvidenceTab;
    pEvidenceTab = NULL;
    delete pPartnerTab;
    pPartnerTab = NULL;
    delete pPartnerAbilityTab;
    pPartnerAbilityTab = NULL;
    delete pOkTab;
    pOkTab = NULL;
    delete pSaveTab;
    pSaveTab = NULL;
    delete pLoadTab;
    pLoadTab = NULL;
    delete pOptionsTab;
    pOptionsTab = NULL;
    delete pQuitTab;
    pQuitTab = NULL;
    delete pQuitConfirmOverlay;
    pQuitConfirmOverlay = NULL;

    SDL_DestroySemaphore(pPathfindingValuesSemaphore);

    EventProviders::GetPromptOverlayEventProvider()->ClearListener(this);
}

void Location::Initialize(Image *pFadeSprite)
{
    Location::pFadeSprite = pFadeSprite;
}

string Location::GetBgm()
{
    bool bgmPrevented = false;
    string initialBgmReplacement = "";

    for (unsigned int i = 0; i < GetCutsceneList()->size(); i++)
    {
        FieldCutscene *pCutscene = (*GetCutsceneList())[i];

        if (pCutscene->GetIsEnabled() && !pCutscene->GetHasCompleted() && pCutscene->GetPreventsInitialBgm() && (id != Case::GetInstance()->GetContentManager()->GetInitialLocationId() || !bSkipIntroCutscene))
        {
            bgmPrevented = true;
            initialBgmReplacement = pCutscene->GetInitialBgmReplacement();
            break;
        }
    }

    if (bgmPrevented)
    {
        return initialBgmReplacement;
    }
    else
    {
        return bgm;
    }
}

string Location::GetAmbianceSfxId()
{
    bool ambiancePrevented = false;
    string initialAmbianceReplacement = "";

    for (unsigned int i = 0; i < GetCutsceneList()->size(); i++)
    {
        FieldCutscene *pCutscene = (*GetCutsceneList())[i];

        if (pCutscene->GetIsEnabled() && !pCutscene->GetHasCompleted() && pCutscene->GetPreventsInitialAmbiance() && (id != Case::GetInstance()->GetContentManager()->GetInitialLocationId() || !bSkipIntroCutscene))
        {
            ambiancePrevented = true;
            initialAmbianceReplacement = pCutscene->GetInitialAmbianceReplacement();
            break;
        }
    }

    if (ambiancePrevented)
    {
        return initialAmbianceReplacement;
    }
    else
    {
        return ambianceSfxId;
    }
}

vector<FieldCutscene *> * Location::GetCutsceneList()
{
    if (cutsceneList.size() == 0)
    {
        for (unsigned int i = 0; i < GetCutsceneIdList()->size(); i++)
        {
            cutsceneList.push_back(Case::GetInstance()->GetFieldCutsceneManager()->GetCutsceneFromId((*GetCutsceneIdList())[i]));
        }
    }

    return &cutsceneList;
}

bool Location::GetAcceptsUserInput()
{
    return fadeOpacity == 0 && !pQuitConfirmOverlay->GetIsShowing();
}

void Location::UpdateLoadedTextures(bool waitUntilLoaded)
{
    for (unsigned int i = 0; i < GetCutsceneList()->size(); i++)
    {
        FieldCutscene *pCutscene = (*GetCutsceneList())[i];

        if (pCutscene->GetIsEnabled() && !pCutscene->GetHasCompleted() && (id != Case::GetInstance()->GetContentManager()->GetInitialLocationId() || !bSkipIntroCutscene))
        {
            Case::GetInstance()->UpdateLoadedTextures(pCutscene->GetId());

            return;
        }
    }

    Case::GetInstance()->UpdateLoadedTextures(GetId(), waitUntilLoaded);
}

void Location::Begin(const string &transitionId)
{
    if (pPlayerCharacter == NULL)
    {
        pPlayerCharacter = Case::GetInstance()->GetFieldCharacterManager()->GetPlayerCharacter();
    }

    pCurrentPlayerCharacter = pPlayerCharacter;

    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() == 0)
    {
        delete pPartnerCharacter;
        pPartnerCharacter = NULL;
    }
    else
    {
        delete pPartnerCharacter;
        pPartnerCharacter = Case::GetInstance()->GetFieldCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId());
    }

    previousPartnerCharacterId = Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId();

    StartPosition startPosition = GetStartPositionFromTransitionId(transitionId);
    StartPosition *pPartnerStartPosition = GetPartnerStartPositionFromTransitionId(transitionId);

    pPlayerCharacter->Begin();

    pPlayerCharacter->SetPosition(startPosition.GetPosition());
    pPlayerCharacter->SetState(FieldCharacterStateStanding);
    pPlayerCharacter->SetDirection(startPosition.GetDirection());
    pPlayerCharacter->SetSpriteDirection(startPosition.GetFieldDirection());

    if (pPartnerCharacter != NULL)
    {
        pPartnerCharacter->Begin();

        // If we have a custom partner start position, then we'll use that.
        // Otherwise, we'll set things up such that the partner character is
        // behind the player character.
        if (pPartnerStartPosition != NULL)
        {
            pPartnerCharacter->SetPosition(pPartnerStartPosition->GetPosition());
            pPartnerCharacter->SetState(FieldCharacterStateStanding);
            pPartnerCharacter->SetDirection(pPartnerStartPosition->GetDirection());
            pPartnerCharacter->SetSpriteDirection(pPartnerStartPosition->GetFieldDirection());
        }
        else
        {
            Vector2 partnerOffset = (pPlayerCharacter->GetVectorAnchorPosition() - pPlayerCharacter->GetPosition()) - (pPartnerCharacter->GetVectorAnchorPosition() - pPartnerCharacter->GetPosition());
            int separationDisplacement = (int)(pPlayerCharacter->GetClickRect().GetWidth() / 2 + pPartnerCharacter->GetClickRect().GetWidth() / 2);
            int directionMultiplier = pPlayerCharacter->GetDirection() == CharacterDirectionLeft ? 1 : -1;

            switch (pPlayerCharacter->GetSpriteDirection())
            {
            case FieldCharacterDirectionUp:
                partnerOffset += Vector2(0, separationDisplacement);
                break;

            case FieldCharacterDirectionDiagonalUp:
                partnerOffset += Vector2(directionMultiplier * separationDisplacement / sqrt(2.0), separationDisplacement / sqrt(2.0));
                break;

            case FieldCharacterDirectionSide:
                partnerOffset += Vector2(directionMultiplier * separationDisplacement, 0);
                break;

            case FieldCharacterDirectionDiagonalDown:
                partnerOffset += Vector2(directionMultiplier * separationDisplacement / sqrt(2.0), -separationDisplacement / sqrt(2.0));
                break;

            case FieldCharacterDirectionDown:
                partnerOffset += Vector2(0, -separationDisplacement);
                break;

            default:
                // We should only get to this point if we're loading, in which case
                // the partner's position will be set when we load from the save file.
                // Nothing to do in that case.
                break;
            }

            pPartnerCharacter->SetPosition(pPlayerCharacter->GetPosition() + partnerOffset);
            pPartnerCharacter->SetState(pPlayerCharacter->GetState());
            pPartnerCharacter->SetDirection(pPlayerCharacter->GetDirection());
            pPartnerCharacter->SetSpriteDirection(pPlayerCharacter->GetSpriteDirection());
        }
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        characterList[i]->Begin();
    }

    for (unsigned int i = 0; i < crowdList.size(); i++)
    {
        crowdList[i]->Begin();
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        foregroundElementList[i]->Begin();
    }

    pTargetInteractiveElement = NULL;

    vector<FieldCharacter *> fieldCharacterList;

    fieldCharacterList.push_back(pPlayerCharacter);

    if (pPartnerCharacter != NULL)
    {
        fieldCharacterList.push_back(pPartnerCharacter);
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        FieldCharacter *pCharacter = characterList[i];
        bool isPartnerCharacter = pPartnerCharacter != NULL && pPartnerCharacter->GetId() == pCharacter->GetId();

        if (!isPartnerCharacter)
        {
            fieldCharacterList.push_back(pCharacter);
        }
    }

    for (unsigned int i = 0; i < fieldCharacterList.size(); i++)
    {
        FieldCharacter *pCharacter = fieldCharacterList[i];
        characterStateMap[pCharacter] = FieldCharacterStateStanding;
        characterTargetPositionQueueMap[pCharacter] = queue<Vector2>();
        characterTargetPositionMap[pCharacter] = Vector2(-1, -1);
    }

    drawingOffsetVector = pPlayerCharacter->GetMidPoint() - (Vector2(gScreenWidth, gScreenHeight) * 0.5);

    // If this would take us off the edge of the map, however,
    // clamp this to the bounds of the background.
    if (drawingOffsetVector.GetX() < 0)
    {
        drawingOffsetVector.SetX(0);
    }
    else if (drawingOffsetVector.GetX() > GetBackgroundSprite()->GetWidth() - gScreenWidth)
    {
        drawingOffsetVector.SetX(GetBackgroundSprite()->GetWidth() - gScreenWidth);
    }

    if (drawingOffsetVector.GetY() < 0)
    {
        drawingOffsetVector.SetY(0);
    }
    else if (drawingOffsetVector.GetY() > GetBackgroundSprite()->GetHeight() - gScreenHeight)
    {
        drawingOffsetVector.SetY(GetBackgroundSprite()->GetHeight() - gScreenHeight);
    }

    pTargetLocation = NULL;
    pTransitionAtPlayer = NULL;
    this->transitionId = "";
    isTransitioning = false;
    isFinishing = false;
    overlayId = "";
    wasInOverlay = false;
    pFadeOutEase->Reset();
    pFadeInEase->Begin();

    fadeOpacity = 1;

    if (pendingTransitionEndSfxId.length() > 0)
    {
        playSound(pendingTransitionEndSfxId);
        pendingTransitionEndSfxId = "";
    }

    stopLoopingSounds();
    SetLoopingSoundLevels();

    for (unsigned int i = 0; i < loopingSoundList.size(); i++)
    {
        playLoopingSound(loopingSoundList[i]->soundId, i);
    }

    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
    {
        Case::GetInstance()->GetPartnerManager()->SetCursor(FieldCustomCursorStateNone);
    }

    pEvidenceTab->SetIsHidden(true, false);
    pPartnerTab->SetIsHidden(true, false);
    pPartnerAbilityTab->SetIsHidden(true, false);
    pOkTab->SetIsHidden(true, false);
    pSaveTab->SetIsHidden(true, false);
    pLoadTab->SetIsHidden(true, false);
    pOptionsTab->SetIsHidden(true, false);
    pQuitTab->SetIsHidden(true, false);

    // We should clear the flag for autosaving when we newly arrive at a location,
    // in case anything caused it to be erroneously left as true, e.g. if a cutscene finished,
    // setting it true, and then another cutscene transported us to a new location.
    shouldAutosave = false;

    CheckForTransitionUnderPlayer();
}

void Location::Update(int delta)
{
    if (gIsQuitting)
    {
        SaveDialogsSeenListForCase(Case::GetInstance()->GetUuid());
    }

    vector<PositionalSound> soundsToPlayList;

    if (pendingBgm.length() > 0)
    {
        if (pendingBgm != "None")
        {
            Case::GetInstance()->GetAudioManager()->PlayBgmWithId(pendingBgm);
        }
        else
        {
            Case::GetInstance()->GetAudioManager()->StopCurrentBgm(true /* isInstant */);
        }

        pendingBgm = "";
    }

    if (pendingAmbianceSfxId.length() > 0)
    {
        if (pendingAmbianceSfxId != "None")
        {
            Case::GetInstance()->GetAudioManager()->PlayAmbianceWithId(pendingAmbianceSfxId);
        }
        else
        {
            Case::GetInstance()->GetAudioManager()->StopCurrentAmbiance(true /* isInstant */);
        }

        pendingAmbianceSfxId = "";
    }

    if (pCurrentEncounter != NULL)
    {
        pCurrentEncounter->Update(delta);

        if (pCurrentEncounter->GetIsFinished())
        {
            pCurrentEncounter->Reset();
            pCurrentInteractiveElement = NULL;

            if (pCurrentEncounter->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationNewLocation)
            {
                BeginTransition(
                    Case::GetInstance()->GetContentManager()->GetLocationFromId(pCurrentEncounter->GetTransitionRequest().GetNewLocationRequestedId()),
                    pCurrentEncounter->GetTransitionRequest().GetTransitionId());

                // If the player is transitioning for a reason other than having walked into a transition's hitbox,
                // then it's unlikely that we'd want to maintain using the field ability, so we'll disable it in that circumstance.
                if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
                {
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->ToggleIsUsingFieldAbility();
                }

                pCurrentEncounter = NULL;
                return;
            }
            else if (pCurrentEncounter->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationZoomedView)
            {
                pCurrentZoomedView = zoomedViewsByIdMap[pCurrentEncounter->GetTransitionRequest().GetZoomedViewId()];
                pCurrentZoomedView->Begin();

                pCurrentEncounter = NULL;
                return;
            }

            shouldAutosave = true;
            pCurrentEncounter = NULL;
            MouseHelper::HandleClick();
        }
        else
        {
            return;
        }
    }

    if (pCurrentCutscene != NULL && pCurrentCutscene->GetHasBegun())
    {
        bool acceptsUserInput = GetAcceptsUserInput();

        if (pFadeOutEase->GetIsFinished())
        {
            if (isTransitioning)
            {
                OnExited(pTargetLocation, transitionId);
                return;
            }
            else
            {
                if (pNextCutscene != NULL)
                {
                    pCurrentCutscene = pNextCutscene;
                    pNextCutscene = NULL;
                }
                else
                {
                    CheckForTransitionUnderPlayer();
                    shouldAutosave = true;
                    pCurrentInteractiveElement = NULL;
                }

                if (pCurrentCutscene != NULL && !pCurrentCutscene->GetIsFinished())
                {
                    previousPartnerCharacterId = Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId();
                    pCurrentCutscene->Begin(pPartnerCharacter);
                    Case::GetInstance()->UpdateLoadedTextures(pCurrentCutscene->GetId());

                    pFadeInEase->Begin();
                    pFadeOutEase->Reset();
                    return;
                }
                else
                {
                    pFadeInEase->Begin();
                }
            }
        }
        else if (pFadeOutEase->GetIsStarted())
        {
            pFadeOutEase->Update(delta);
            fadeOpacity = pFadeOutEase->GetCurrentValue();
        }
        else if (pFadeInEase->GetIsFinished())
        {
            fadeOpacity = 0;
            pFadeInEase->Reset();
        }
        else if (pFadeInEase->GetIsStarted())
        {
            pFadeInEase->Update(delta);
            fadeOpacity = pFadeInEase->GetCurrentValue();
        }

        pCurrentCutscene->UpdateCharacters(delta, &heightMapList);

        if (acceptsUserInput)
        {
            pCurrentCutscene->UpdatePhase(delta);
        }

        drawingOffsetVector = pCurrentCutscene->GetCurrentCameraPosition() - (Vector2(gScreenWidth, gScreenHeight) * 0.5);

        // If this would take us off the edge of the map, however,
        // clamp this to the bounds of the background.
        if (drawingOffsetVector.GetX() < 0)
        {
            drawingOffsetVector.SetX(0);
        }
        else if (drawingOffsetVector.GetX() > GetBackgroundSprite()->GetWidth() - gScreenWidth)
        {
            drawingOffsetVector.SetX(GetBackgroundSprite()->GetWidth() - gScreenWidth);
        }

        if (drawingOffsetVector.GetY() < 0)
        {
            drawingOffsetVector.SetY(0);
        }
        else if (drawingOffsetVector.GetY() > GetBackgroundSprite()->GetHeight() - gScreenHeight)
        {
            drawingOffsetVector.SetY(GetBackgroundSprite()->GetHeight() - gScreenHeight);
        }

        SetLoopingSoundLevels();

        if (pCurrentCutscene->GetIsFinished())
        {
            if (pCurrentCutscene->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationNewLocation)
            {
                BeginTransition(
                    Case::GetInstance()->GetContentManager()->GetLocationFromId(pCurrentCutscene->GetTransitionRequest().GetNewLocationRequestedId()),
                    pCurrentCutscene->GetTransitionRequest().GetTransitionId());

                // If the player is transitioning for a reason other than having walked into a transition's hitbox,
                // then it's unlikely that we'd want to maintain using the field ability, so we'll disable it in that circumstance.
                if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
                {
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->ToggleIsUsingFieldAbility();
                }

                pCurrentCutscene->SetTransitionRequest(TransitionRequest());
                return;
            }
            else if (pCurrentCutscene->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationZoomedView)
            {
                pCurrentZoomedView = zoomedViewsByIdMap[pCurrentCutscene->GetTransitionRequest().GetZoomedViewId()];
                pCurrentZoomedView->Begin();

                pCurrentCutscene->SetTransitionRequest(TransitionRequest());
                return;
            }
            else
            {
                if (pNextCutscene == NULL)
                {
                    for (unsigned int i = 0; i < GetCutsceneList()->size(); i++)
                    {
                        FieldCutscene *pCutscene = (*GetCutsceneList())[i];

                        if (pCutscene->GetIsEnabled() && !pCutscene->GetHasCompleted() && (id != Case::GetInstance()->GetContentManager()->GetInitialLocationId() || !bSkipIntroCutscene))
                        {
                            pNextCutscene = pCutscene;
                            break;
                        }
                    }
                }

                if (!pFadeOutEase->GetIsStarted())
                {
                    pFadeOutEase->Begin();
                    return;
                }
                else if (pFadeInEase->GetIsStarted())
                {
                    pFadeOutEase->Reset();
                    pCurrentCutscene = NULL;
                    MouseHelper::HandleClick();
                    Case::GetInstance()->UpdateLoadedTextures(GetId());
                    return;
                }
            }
        }

        return;
    }
    else if (pCurrentCutscene == NULL)
    {
        for (unsigned int i = 0; i < GetCutsceneList()->size(); i++)
        {
            FieldCutscene *pCutscene = (*GetCutsceneList())[i];

            if (pCutscene->GetIsEnabled() && !pCutscene->GetHasCompleted() && (id != Case::GetInstance()->GetContentManager()->GetInitialLocationId() || !bSkipIntroCutscene))
            {
                pCurrentCutscene = pCutscene;

                if (GetAcceptsUserInput())
                {
                    pFadeInEase->Reset();
                    pFadeOutEase->Begin();
                }
                else
                {
                    pCurrentCutscene->Begin(pPartnerCharacter);
                }

                return;
            }
        }
    }

    if (pCurrentInteractiveForegroundElement != NULL)
    {
        pCurrentInteractiveForegroundElement->UpdateInteraction(delta);

        if (pCurrentInteractiveForegroundElement->GetIsInteractionFinished())
        {
            pCurrentInteractiveElement = NULL;

            if (pCurrentInteractiveForegroundElement->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationNewLocation)
            {
                BeginTransition(
                    Case::GetInstance()->GetContentManager()->GetLocationFromId(pCurrentInteractiveForegroundElement->GetTransitionRequest().GetNewLocationRequestedId()),
                    pCurrentInteractiveForegroundElement->GetTransitionRequest().GetTransitionId());

                // If the player is transitioning for a reason other than having walked into a transition's hitbox,
                // then it's unlikely that we'd want to maintain using the field ability, so we'll disable it in that circumstance.
                if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
                {
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->ToggleIsUsingFieldAbility();
                }

                pCurrentInteractiveForegroundElement = NULL;
                return;
            }
            else if (pCurrentInteractiveForegroundElement->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationZoomedView)
            {
                pCurrentZoomedView = zoomedViewsByIdMap[pCurrentInteractiveForegroundElement->GetTransitionRequest().GetZoomedViewId()];
                pCurrentZoomedView->Begin();

                pCurrentInteractiveForegroundElement = NULL;
                return;
            }

            shouldAutosave = true;
            pCurrentInteractiveForegroundElement = NULL;
            MouseHelper::HandleClick();
        }
        else
        {
            return;
        }
    }

    if (pCurrentInteractiveCrowd != NULL)
    {
        pCurrentInteractiveCrowd->UpdateInteraction(delta);

        if (pCurrentInteractiveCrowd->GetIsInteractionFinished())
        {
            pCurrentInteractiveElement = NULL;

            if (pCurrentInteractiveCrowd->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationNewLocation)
            {
                BeginTransition(
                    Case::GetInstance()->GetContentManager()->GetLocationFromId(pCurrentInteractiveCrowd->GetTransitionRequest().GetNewLocationRequestedId()),
                    pCurrentInteractiveCrowd->GetTransitionRequest().GetTransitionId());

                // If the player is transitioning for a reason other than having walked into a transition's hitbox,
                // then it's unlikely that we'd want to maintain using the field ability, so we'll disable it in that circumstance.
                if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
                {
                    Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->ToggleIsUsingFieldAbility();
                }

                pCurrentInteractiveCrowd = NULL;
                return;
            }
            else if (pCurrentInteractiveCrowd->GetTransitionRequest().GetTransitionDestination() == TransitionDestinationZoomedView)
            {
                pCurrentZoomedView = zoomedViewsByIdMap[pCurrentInteractiveCrowd->GetTransitionRequest().GetZoomedViewId()];
                pCurrentZoomedView->Begin();

                pCurrentInteractiveCrowd = NULL;
                return;
            }

            shouldAutosave = true;
            pCurrentInteractiveCrowd = NULL;
            MouseHelper::HandleClick();
        }
        else
        {
            return;
        }
    }

    if (pCurrentZoomedView != NULL)
    {
        pCurrentZoomedView->Update(delta);

        if (pCurrentZoomedView->GetIsFinished())
        {
            shouldAutosave = true;
            pCurrentZoomedView = NULL;
            MouseHelper::HandleClick();
        }
        else
        {
            return;
        }
    }

    if (pEvidenceSelector->GetIsShowing())
    {
        pEvidenceSelector->Update(delta);

        if (pEvidenceSelector->GetIsShowing())
        {
            return;
        }
        else if (pEvidenceSelector->GetEvidenceCombinationEncounter() != NULL)
        {
            pCurrentEncounter = pEvidenceSelector->GetEvidenceCombinationEncounter();
            pCurrentEncounter->Begin();

            return;
        }
    }

    if (pInEasePartner->GetIsStarted())
    {
        if (!pInEasePartner->GetIsFinished())
        {
            pInEasePartner->Update(delta);
            animationOffsetPartner = (int)pInEasePartner->GetCurrentValue();
            return;
        }
        else if (pOutEasePartner->GetIsStarted() && !pOutEasePartner->GetIsFinished())
        {
            pOutEasePartner->Update(delta);
            animationOffsetPartner = (int)pOutEasePartner->GetCurrentValue();
            return;
        }
        else if (pOutEasePartner->GetIsFinished())
        {
            pInEasePartner->Reset();
            pOutEasePartner->Reset();
        }
        else
        {
            pOkTab->Update();

            if (pOkTab->GetIsClicked())
            {
                pOutEasePartner->Begin();
            }

            return;
        }
    }

    // Don't check for transitions if we're already in the middle
    // of a transition.
    if (!isTransitioning)
    {
        bool transitionCollisionFound = false;

        for (unsigned int i = 0; i < transitionList.size(); i++)
        {
            CollisionParameter param;

            if (pPlayerCharacter->IsCollision(transitionList[i]->GetHitBox(), &param))
            {
                transitionCollisionFound = true;

                if (pTransitionAtPlayer != transitionList[i])
                {
                    transitionList[i]->BeginInteraction(this);
                    return;
                }
            }
        }

        if (!transitionCollisionFound)
        {
            pTransitionAtPlayer = NULL;
        }
    }

    bool acceptsUserInput = GetAcceptsUserInput();

    if (pFadeOutEase->GetIsFinished())
    {
        if (isFinishing)
        {
            Case::GetInstance()->SetIsFinished(true);
            return;
        }
        else if (overlayId.length() > 0)
        {
            if (!wasInOverlay)
            {
                wasInOverlay = true;
                Game::GetInstance()->SetOverlayById(overlayId);
                return;
            }
            else
            {
                wasInOverlay = false;
                overlayId = "";
                pFadeInEase->Begin();
                pFadeOutEase->Reset();
            }
        }
        else if (isTransitioning)
        {
            OnExited(pTargetLocation, transitionId);
            return;
        }
        else
        {
            if (pNextCutscene != NULL)
            {
                pCurrentCutscene = pNextCutscene;
                pNextCutscene = NULL;
            }

            if (pCurrentCutscene != NULL && !pCurrentCutscene->GetIsFinished())
            {
                previousPartnerCharacterId = Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId();
                pCurrentCutscene->Begin(pPartnerCharacter);
                Case::GetInstance()->UpdateLoadedTextures(pCurrentCutscene->GetId());
            }

            pFadeInEase->Begin();
            pFadeOutEase->Reset();
        }
    }
    else if (pFadeOutEase->GetIsStarted())
    {
        pFadeOutEase->Update(delta);
        fadeOpacity = pFadeOutEase->GetCurrentValue();
    }
    else if (pFadeInEase->GetIsFinished())
    {
        fadeOpacity = 0;
        pFadeInEase->Reset();
    }
    else if (pFadeInEase->GetIsStarted())
    {
        pFadeInEase->Update(delta);
        fadeOpacity = pFadeInEase->GetCurrentValue();
    }

    string currentPartnerId = Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId();

    if (previousPartnerCharacterId != currentPartnerId)
    {
        previousPartnerCharacterId = currentPartnerId;

        if (currentPartnerId.length() == 0)
        {
            characterStateMap.erase(pPartnerCharacter);
            characterTargetPositionQueueMap.erase(pPartnerCharacter);
            characterTargetPositionMap.erase(pPartnerCharacter);

            delete pPartnerCharacter;
            pPartnerCharacter = NULL;
        }
        else
        {
            if (pPartnerCharacter != NULL)
            {
                characterStateMap.erase(pPartnerCharacter);
                characterTargetPositionQueueMap.erase(pPartnerCharacter);
                characterTargetPositionMap.erase(pPartnerCharacter);

                delete pPartnerCharacter;
            }

            pPartnerCharacter = Case::GetInstance()->GetFieldCharacterManager()->GetCharacterFromId(currentPartnerId);

            // If this partner character came from this scene, then make its starting position
            // where the field character used to be.  Otherwise, start it at the player character.
            FieldCharacter *pPartnerCharacterInField = NULL;

            for (unsigned int i = 0; i < characterList.size(); i++)
            {
                FieldCharacter *pFieldCharacter = characterList[i];

                if (pFieldCharacter->GetId() == currentPartnerId)
                {
                    pPartnerCharacterInField = pFieldCharacter;
                    break;
                }
            }

            if (pPartnerCharacterInField != NULL)
            {
                pPartnerCharacter->SetPosition(pPartnerCharacterInField->GetPosition());
                pPartnerCharacter->SetState(pPartnerCharacterInField->GetState());
                pPartnerCharacter->SetDirection(pPartnerCharacterInField->GetDirection());
                pPartnerCharacterInField->Reset();
            }
            else
            {
                pPartnerCharacter->SetPosition(pPlayerCharacter->GetPosition());
                pPartnerCharacter->SetState(pPlayerCharacter->GetState());
                pPartnerCharacter->SetDirection(pPlayerCharacter->GetDirection());
            }

            pPartnerCharacter->Begin();

            characterStateMap[pPartnerCharacter] = pPartnerCharacter->GetState();
            characterTargetPositionQueueMap[pPartnerCharacter] = queue<Vector2>();
            characterTargetPositionMap[pPartnerCharacter] = Vector2(-1, -1);

            // Load any new images needed for the partner if they're not already loaded.
            // This can include things such as the partner's field ability custom cursor.
            UpdateLoadedTextures(false);
        }
    }

    pEvidenceTab->SetIsEnabled(Case::GetInstance()->GetEvidenceManager()->GetHasEvidence());
    pPartnerTab->SetIsEnabled(Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0);
    pPartnerAbilityTab->SetIsEnabled(Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetFieldAbilityName().length() > 0);

    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
        Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetFieldAbilityName().length() > 0)
    {
        pPartnerAbilityTab->SetText(Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetFieldAbilityName());
        pPartnerAbilityTab->SetTabImage(Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIconSprite());
    }
    else
    {
        pPartnerAbilityTab->SetText("");
        pPartnerAbilityTab->SetTabImage(NULL);
    }

    if (SaveFileExists())
    {
        pLoadTab->SetIsEnabled(true);
    }
    else
    {
        pLoadTab->SetIsEnabled(false);
    }

    if (acceptsUserInput)
    {
        // We're at the point where we can respond to user input.
        // If we've reached this point, then this is where we want to autosave
        // if we need to.
        if (shouldAutosave)
        {
            // Make sure that a cutscene isn't pending - if one is, then we should not be autosaving.
            bool cutscenePending = false;

            for (unsigned int i = 0; i < GetCutsceneList()->size(); i++)
            {
                FieldCutscene *pCutscene = (*GetCutsceneList())[i];

                if (pCutscene->GetIsEnabled() && !pCutscene->GetHasCompleted())
                {
                    cutscenePending = true;
                    break;
                }
            }

            if (!cutscenePending)
            {
                Case::GetInstance()->Autosave();
            }

            SaveDialogsSeenListForCase(Case::GetInstance()->GetUuid());
            shouldAutosave = false;
        }

        pEvidenceTab->SetIsPulsing(Case::GetInstance()->GetEvidenceManager()->GetAreEvidenceCombinations() && gEnableHints);
        pEvidenceTab->Update(delta);

        if (pEvidenceTab->GetIsClicked())
        {
            pEvidenceSelector->Begin();
            pEvidenceSelector->Show();

            return;
        }

        pPartnerTab->Update();

        if (pPartnerTab->GetIsClicked())
        {
            partnerInformation.SetPartnerId(Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId());
            pInEasePartner->Begin();

            return;
        }

        if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
            Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetFieldAbilityName().length() > 0)
        {
            bool partnerAbilityNeedsUsing = false;

            if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() == 0 || !Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
            {
                for (unsigned int i = 0; i < hiddenForegroundElementList.size(); i++)
                {
                    HiddenForegroundElement *pHiddenForegroundElement = hiddenForegroundElementList[i];

                    if (pHiddenForegroundElement->GetIsRequired() && !pHiddenForegroundElement->GetIsDiscovered() && pHiddenForegroundElement->IsPresent())
                    {
                        partnerAbilityNeedsUsing = true;
                        break;
                    }
                }
            }

            pPartnerAbilityTab->SetIsPulsing(partnerAbilityNeedsUsing && gEnableHints);
            pPartnerAbilityTab->Update(delta);

            if (pPartnerAbilityTab->GetIsClicked())
            {
                Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->ToggleIsUsingFieldAbility();

                return;
            }
        }

        pLoadTab->Update();

        if (pLoadTab->GetIsClicked())
        {
            overlayId = LOAD_SCREEN_ID;
            pFadeOutEase->Begin();
            return;
        }

        pSaveTab->Update();

        if (pSaveTab->GetIsClicked())
        {
            overlayId = SAVE_SCREEN_ID;
            pFadeOutEase->Begin();
            return;
        }

        pOptionsTab->Update();

        if (pOptionsTab->GetIsClicked())
        {
            overlayId = OPTIONS_SCREEN_ID;
            pFadeOutEase->Begin();
            return;
        }

        pQuitTab->Update();

        if (pQuitTab->GetIsClicked())
        {
            pQuitConfirmOverlay->Begin();
            return;
        }
    }

    list<ZOrderableObject *> interactiveElementsByZOrder;

    for (unsigned int i = 0; i < transitionList.size(); i++)
    {
        interactiveElementsByZOrder.push_back(transitionList[i]);
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        interactiveElementsByZOrder.push_back(foregroundElementList[i]);
    }

    for (unsigned int i = 0; i < hiddenForegroundElementList.size(); i++)
    {
        interactiveElementsByZOrder.push_back(hiddenForegroundElementList[i]);
    }

    if (pPartnerCharacter != NULL)
    {
        interactiveElementsByZOrder.push_back(pPartnerCharacter);
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        interactiveElementsByZOrder.push_back(characterList[i]);
    }

    for (unsigned int i = 0; i < crowdList.size(); i++)
    {
        interactiveElementsByZOrder.push_back(crowdList[i]);
    }

    interactiveElementsByZOrder.sort(CompareByZOrderDescending);

    bool elementWithMouseOverFound = false;
    int minDistanceToHiddenElement = numeric_limits<int>::max();
    HiddenForegroundElement *pClosestHiddenForegroundElement = NULL;

    for (list<ZOrderableObject *>::iterator iter = interactiveElementsByZOrder.begin(); iter != interactiveElementsByZOrder.end(); ++iter)
    {
        ZOrderableObject *pZOrderableObject = *iter;
        FieldCharacter *pFieldCharacter = dynamic_cast<FieldCharacter *>(pZOrderableObject);
        Crowd *pCrowd = dynamic_cast<Crowd *>(pZOrderableObject);
        HiddenForegroundElement *pHiddenForegroundElement = dynamic_cast<HiddenForegroundElement *>(pZOrderableObject);
        ForegroundElement *pForegroundElement = dynamic_cast<ForegroundElement *>(pZOrderableObject);
        Location::Transition *pTransition = dynamic_cast<Location::Transition *>(pZOrderableObject);

        if (pTransition != NULL)
        {
            if (pTransition->GetHitBox()->ContainsPoint(Vector2(0, 0), MouseHelper::GetMousePosition() + drawingOffsetVector) &&
                (pTransition->GetCondition() == NULL || pTransition->GetCondition()->IsTrue() || !pTransition->GetHideWhenLocked()) &&
                !MouseHelper::PressedAndHeldAnywhere() && !MouseHelper::DoublePressedAndHeldAnywhere() &&
                !elementWithMouseOverFound)
            {
                CursorType cursorType = CursorTypeNormal;

                if (pTransition->GetCondition() != NULL && !pTransition->GetCondition()->IsTrue())
                {
                    cursorType = CursorTypeNoExit;
                }
                else
                {
                    switch (pTransition->GetTransitionDirection())
                    {
                        case TransitionDirectionNorth:
                            cursorType = CursorTypeExitNorth;
                            break;

                        case TransitionDirectionNorthEast:
                            cursorType = CursorTypeExitNorthEast;
                            break;

                        case TransitionDirectionEast:
                            cursorType = CursorTypeExitEast;
                            break;

                        case TransitionDirectionSouthEast:
                            cursorType = CursorTypeExitSouthEast;
                            break;

                        case TransitionDirectionSouth:
                            cursorType = CursorTypeExitSouth;
                            break;

                        case TransitionDirectionSouthWest:
                            cursorType = CursorTypeExitSouthWest;
                            break;

                        case TransitionDirectionWest:
                            cursorType = CursorTypeExitWest;
                            break;

                        case TransitionDirectionNorthWest:
                            cursorType = CursorTypeExitNorthWest;
                            break;

                        case TransitionDirectionDoor:
                            cursorType = CursorTypeExitDoor;
                            break;
                    }
                }

                MouseHelper::SetCursorType(cursorType);

                char mouseOverText[256];
                sprintf(mouseOverText, pgLocalizableContent->GetText("Location/LocationTransitionFormatText").c_str(), pTransition->GetTargetLocationName().c_str());

                MouseHelper::SetMouseOverText(string(mouseOverText));
                elementWithMouseOverFound = true;
            }

            if (pTransition->GetHitBox()->ContainsPoint(Vector2(0, 0), MouseHelper::GetMousePosition() + drawingOffsetVector) &&
                (pTransition->GetCondition() == NULL || pTransition->GetCondition()->IsTrue() || !pTransition->GetHideWhenLocked()) &&
                pTransition->HasInteractionLocation() &&
                (MouseHelper::ClickedAnywhere() || MouseHelper::DoubleClickedAnywhere() || KeyboardHelper::ClickPressed()))
            {
                if (acceptsUserInput)
                {
                    // If the player is already in the transition hitbox, then setting the target interactive element will do nothing -
                    // instead, we should just immediately play the interaction.
                    if (pTransitionAtPlayer != pTransition)
                    {
                        SetTargetInteractiveElement(pTransition, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
                    }
                    else
                    {
                        pPlayerCharacter->UpdateDirection((MouseHelper::GetMousePosition() + drawingOffsetVector) - pPlayerCharacter->GetMidPoint());
                        pTransition->BeginInteraction(this);
                        return;
                    }
                }
            }
        }
        else if (pZOrderableObject == pPartnerCharacter)
        {
            if ((Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() == 0 || !Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility()) &&
                !elementWithMouseOverFound)
            {
                pPartnerCharacter->UpdateClickState(drawingOffsetVector);
                elementWithMouseOverFound = pPartnerCharacter->GetIsMouseOver();

                if (acceptsUserInput)
                {
                    if (pPartnerCharacter->GetIsClicked())
                    {
                        SetTargetInteractiveElement(pPartnerCharacter, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
                        pPartnerCharacter->SetIsClicked(false);
                    }
                }
            }
        }
        else if (pFieldCharacter != NULL)
        {
            bool isPartnerCharacter = pPartnerCharacter != NULL && pPartnerCharacter->GetId() == pFieldCharacter->GetId();

            if (!isPartnerCharacter && !pFieldCharacter->GetIsPresent())
            {
                continue;
            }

            if (!isPartnerCharacter)
            {
                pFieldCharacter->UpdateAnimation(delta);

                if ((Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() == 0 || !Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility()) &&
                    !elementWithMouseOverFound)
                {
                    pFieldCharacter->UpdateClickState(drawingOffsetVector);
                    elementWithMouseOverFound = pFieldCharacter->GetIsMouseOver();
                }

                if (acceptsUserInput)
                {
                    if (pFieldCharacter->GetIsClicked())
                    {
                        SetTargetInteractiveElement(pFieldCharacter, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
                        pFieldCharacter->SetIsClicked(false);
                    }
                }
            }
        }
        else if (pCrowd != NULL)
        {
            pCrowd->UpdateAnimation(delta);

            if ((Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() == 0 || !Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility()) &&
                !elementWithMouseOverFound)
            {
                pCrowd->UpdateClickState(drawingOffsetVector);
                elementWithMouseOverFound = pCrowd->GetIsMouseOver();

                if (acceptsUserInput)
                {
                    if (pCrowd->GetIsClicked())
                    {
                        SetTargetInteractiveElement(pCrowd, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
                        pCrowd->SetIsClicked(false);
                    }
                }
            }
        }
        else if (pHiddenForegroundElement != NULL)
        {
            if (!pHiddenForegroundElement->IsPresent())
            {
                continue;
            }

            if (pHiddenForegroundElement->GetIsDiscovered())
            {
                pHiddenForegroundElement->UpdateAnimation(delta);

                if (!elementWithMouseOverFound)
                {
                    pHiddenForegroundElement->UpdateClickState(drawingOffsetVector);
                    elementWithMouseOverFound = pHiddenForegroundElement->GetIsMouseOver();

                    if (pHiddenForegroundElement->GetIsClicked() && pHiddenForegroundElement->GetIsInteractive())
                    {
                        SetTargetInteractiveElement(pHiddenForegroundElement, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
                        pHiddenForegroundElement->SetIsClicked(false);
                    }
                }
            }
            else if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
            {
                int distance = (int)(pHiddenForegroundElement->GetInexactCenterPoint() - drawingOffsetVector - MouseHelper::GetMousePosition()).Length();

                if (minDistanceToHiddenElement > distance)
                {
                    minDistanceToHiddenElement = distance;
                    pClosestHiddenForegroundElement = pHiddenForegroundElement;
                }
            }
        }
        else if (pForegroundElement != NULL)
        {
            if (!pForegroundElement->IsPresent())
            {
                continue;
            }

            pForegroundElement->UpdateAnimation(delta);

            if ((Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() == 0 || !Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility()) &&
                !elementWithMouseOverFound)
            {
                pForegroundElement->UpdateClickState(drawingOffsetVector);
                elementWithMouseOverFound = pForegroundElement->GetIsMouseOver();

                if (acceptsUserInput)
                {
                    if (pForegroundElement->GetIsClicked() && pForegroundElement->GetIsInteractive())
                    {
                        SetTargetInteractiveElement(pForegroundElement, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
                        pForegroundElement->SetIsClicked(false);
                    }
                }
            }
        }
    }

    if (pClosestHiddenForegroundElement != NULL)
    {
        if (minDistanceToHiddenElement <= CursorExtremeThreshold)
        {
            Case::GetInstance()->GetPartnerManager()->SetCursor(FieldCustomCursorStateExtreme);

            pClosestHiddenForegroundElement->Update(delta, drawingOffsetVector);

            if (acceptsUserInput)
            {
                if (pClosestHiddenForegroundElement->GetIsDiscovered())
                {
                    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
                    {
                        bool exitSearchMode = true;

                        for (unsigned int i = 0; i < hiddenForegroundElementList.size(); i++)
                        {
                            HiddenForegroundElement *pHiddenForegroundElement = hiddenForegroundElementList[i];

                            if (pHiddenForegroundElement->GetIsRequired() && !pHiddenForegroundElement->GetIsDiscovered())
                            {
                                exitSearchMode = false;
                                break;
                            }
                        }

                        if (exitSearchMode)
                        {
                            Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->ToggleIsUsingFieldAbility();
                        }
                    }
                }

                if (pClosestHiddenForegroundElement->GetIsClicked() && pClosestHiddenForegroundElement->GetIsInteractive())
                {
                    SetTargetInteractiveElement(pClosestHiddenForegroundElement, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
                }
            }
        }
        else if (minDistanceToHiddenElement <= CursorHighThreshold)
        {
            Case::GetInstance()->GetPartnerManager()->SetCursor(FieldCustomCursorStateHigh);
        }
        else if (minDistanceToHiddenElement <= CursorMidThreshold)
        {
            Case::GetInstance()->GetPartnerManager()->SetCursor(FieldCustomCursorStateMid);
        }
        else if (minDistanceToHiddenElement <= CursorLowThreshold)
        {
            Case::GetInstance()->GetPartnerManager()->SetCursor(FieldCustomCursorStateLow);
        }
        else
        {
            Case::GetInstance()->GetPartnerManager()->SetCursor(FieldCustomCursorStateNone);
        }
    }
    else if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
    {
        Case::GetInstance()->GetPartnerManager()->SetCursor(FieldCustomCursorStateNone);
    }

    if (acceptsUserInput)
    {
        pEvidenceTab->Update(delta);
        pPartnerTab->Update();

        if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
            Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetFieldAbilityName().length() > 0)
        {
            pPartnerAbilityTab->Update(delta);
        }

        pQuitTab->Update();
        pLoadTab->Update();
        pSaveTab->Update();
        pOptionsTab->Update();

        if (!pFadeOutEase->GetIsStarted())
        {
            if (MouseHelper::ClickedAnywhere() || MouseHelper::DoubleClickedAnywhere())
            {
                pTargetInteractiveElement = NULL;

                Vector2 clickPoint = MouseHelper::GetMousePosition() + drawingOffsetVector;
                Vector2 endPosition = clickPoint;

                for (unsigned int i = 0; i < heightMapList.size(); i++)
                {
                    HeightMap *pHeightMap = heightMapList[i];
                    Vector2 heightMapOffset = pHeightMap->GetBasePointOffsetFromHeightenedPoint(endPosition);

                    if (heightMapOffset.GetX() >= 0)
                    {
                        endPosition += heightMapOffset;
                    }
                }

                StartCharacterOnPath(pPlayerCharacter, endPosition, MouseHelper::DoubleClickedAnywhere() ? FieldCharacterStateRunning : FieldCharacterStateWalking);
            }
            else if (MouseHelper::PressedAndHeldAnywhere())
            {
                Vector2 endPosition = MouseHelper::GetMousePosition() + drawingOffsetVector + Vector2(0, pPlayerCharacter->GetExtraHeight());

                // If the end position is closer to the player than the distance the player would walk in five frames (just a heuristic),
                // then we won't bother moving her anywhere, since doing so can cause crazy stuff as the math
                // tries to keep up with the direction of the tiny vectors between the mouse cursor and the player character.
                if ((endPosition - pPlayerCharacter->GetVectorAnchorPosition()).Length() < (WalkingSpeed * 5) / gFramerate)
                {
                    endPosition = pPlayerCharacter->GetVectorAnchorPosition();
                }

                SDL_SemWait(pPathfindingValuesSemaphore);

                movingDirectly = true;
                pTargetInteractiveElement = NULL;
                characterTargetPositionQueueMap[pPlayerCharacter] = queue<Vector2>();
                characterTargetPositionMap[pPlayerCharacter] = endPosition;
                characterStateMap[pPlayerCharacter] = FieldCharacterStateWalking;

                if (!characterTargetPositionQueueMap[pPartnerCharacter].empty())
                {
                    characterTargetPositionQueueMap[pPartnerCharacter] = queue<Vector2>();
                    characterTargetPositionMap[pPartnerCharacter] = Vector2(-1, -1);
                    characterStateMap[pPartnerCharacter] = FieldCharacterStateStanding;
                }

                SDL_SemPost(pPathfindingValuesSemaphore);
            }
            else if (MouseHelper::DoublePressedAndHeldAnywhere())
            {
                Vector2 endPosition = MouseHelper::GetMousePosition() + drawingOffsetVector + Vector2(0, pPlayerCharacter->GetExtraHeight());

                // If the end position is closer to the player than the distance the player would run in in five frames (just a heuristic),
                // then we won't bother moving her anywhere, since doing so can cause crazy stuff as the math
                // tries to keep up with the direction of the tiny vectors between the mouse cursor and the player character.
                if ((endPosition - pPlayerCharacter->GetVectorAnchorPosition()).Length() < (RunningSpeed * 5) / gFramerate)
                {
                    endPosition = pPlayerCharacter->GetVectorAnchorPosition();
                }

                SDL_SemWait(pPathfindingValuesSemaphore);

                movingDirectly = true;
                pTargetInteractiveElement = NULL;
                characterTargetPositionQueueMap[pPlayerCharacter] = queue<Vector2>();
                characterTargetPositionMap[pPlayerCharacter] = endPosition;
                characterStateMap[pPlayerCharacter] = FieldCharacterStateRunning;

                if (!characterTargetPositionQueueMap[pPartnerCharacter].empty())
                {
                    characterTargetPositionQueueMap[pPartnerCharacter] = queue<Vector2>();
                    characterTargetPositionMap[pPartnerCharacter] = Vector2(-1, -1);
                    characterStateMap[pPartnerCharacter] = FieldCharacterStateStanding;
                }

                SDL_SemPost(pPathfindingValuesSemaphore);
            }
            else if (KeyboardHelper::GetMoving())
            {
                Vector2 pressedDirection(0,0);
                if(KeyboardHelper::GetLeftState())
                {
                    pressedDirection.SetX(pressedDirection.GetX() - KeyboardMovementVectorLength);
                }
                if(KeyboardHelper::GetRightState())
                {
                    pressedDirection.SetX(pressedDirection.GetX() + KeyboardMovementVectorLength);
                }
                if(KeyboardHelper::GetUpState())
                {
                    pressedDirection.SetY(pressedDirection.GetY() - KeyboardMovementVectorLength);
                }
                if(KeyboardHelper::GetDownState())
                {
                    pressedDirection.SetY(pressedDirection.GetY() + KeyboardMovementVectorLength);
                }
                Vector2 endPosition = pPlayerCharacter->GetCenterPoint() + pressedDirection;

                SDL_SemWait(pPathfindingValuesSemaphore);

                movingDirectly = true;
                pTargetInteractiveElement = NULL;
                characterTargetPositionQueueMap[pPlayerCharacter] = queue<Vector2>();
                characterTargetPositionMap[pPlayerCharacter] = endPosition;

                if (KeyboardHelper::GetRunState())
                {
                    characterStateMap[pPlayerCharacter] = FieldCharacterStateRunning;
                }
                else
                {
                    characterStateMap[pPlayerCharacter] = FieldCharacterStateWalking;
                }

                if (!characterTargetPositionQueueMap[pPartnerCharacter].empty())
                {
                    characterTargetPositionQueueMap[pPartnerCharacter] = queue<Vector2>();
                    characterTargetPositionMap[pPartnerCharacter] = Vector2(-1, -1);
                    characterStateMap[pPartnerCharacter] = FieldCharacterStateStanding;
                }

                SDL_SemPost(pPathfindingValuesSemaphore);
            }
            else if (movingDirectly)
            {
                // If moving directly, then we should stop as soon as the player
                // has lifted the mouse button.
                characterStateMap[pPlayerCharacter] = FieldCharacterStateStanding;
            }
        }
    }

    SDL_SemWait(pPathfindingValuesSemaphore);

    if (pTargetInteractiveElement != NULL)
    {
        // If we're close enough to the interactive element,
        // then we should begin the interaction.
        if (GetIsPlayerCharacterCloseToInteractiveElement())
        {
            pPlayerCharacter->UpdateDirection(clickPoint - pPlayerCharacter->GetMidPoint());

            if (pPartnerCharacter != NULL)
            {
                if (pTargetInteractiveElement != pPartnerCharacter)
                {
                    pPartnerCharacter->UpdateDirection(clickPoint - pPartnerCharacter->GetMidPoint());
                }
                else
                {
                    pPartnerCharacter->UpdateDirection(pPlayerCharacter->GetMidPoint() - pPartnerCharacter->GetMidPoint());
                }
            }

            pTargetInteractiveElement->BeginInteraction(this);

            characterStateMap[pPlayerCharacter] = FieldCharacterStateStanding;
            pTargetInteractiveElement = NULL;
        }
    }

    vector<FieldCharacter *> fieldCharacterList;

    fieldCharacterList.push_back(pPlayerCharacter);

    if (pPartnerCharacter != NULL)
    {
        fieldCharacterList.push_back(pPartnerCharacter);
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        FieldCharacter *pCharacter = characterList[i];
        bool isPartnerCharacter = pPartnerCharacter != NULL && pPartnerCharacter->GetId() == pCharacter->GetId();

        if (!isPartnerCharacter)
        {
            fieldCharacterList.push_back(pCharacter);
        }
    }

    map<FieldCharacter *, double> characterSpeedMap;

    for (unsigned int i = 0; i < fieldCharacterList.size(); i++)
    {
        FieldCharacter *pCharacter = fieldCharacterList[i];
        FieldCharacterState characterState = characterStateMap[pCharacter];

        if (characterState == FieldCharacterStateWalking)
        {
            characterSpeedMap[pCharacter] = WalkingSpeed;
        }
        else if (characterState == FieldCharacterStateRunning)
        {
            characterSpeedMap[pCharacter] = RunningSpeed;
        }
        else
        {
            characterSpeedMap[pCharacter] = 0;
        }
    }

    for (unsigned int i = 0; i < fieldCharacterList.size(); i++)
    {
        FieldCharacter *pCharacter = fieldCharacterList[i];

        if (characterStateMap[pCharacter] != FieldCharacterStateStanding &&
            ((characterTargetPositionMap[pCharacter].GetX() >= 0 && characterTargetPositionMap[pCharacter].GetY() >= 0) ||
             (pCharacter == pPlayerCharacter && movingDirectly)))
        {
            Vector2 characterPosition = pCharacter->GetVectorAnchorPosition();
            Vector2 targetPosition = characterTargetPositionMap[pCharacter];
            bool reachedTargetPosition = true;

            if (targetPosition != characterPosition)
            {
                Vector2 characterDelta = (targetPosition - characterPosition).Normalize() * (characterSpeedMap[pCharacter] * delta / 1000);

                Vector2 newPosition = Vector2(pCharacter->GetPosition().GetX() + characterDelta.GetX(), pCharacter->GetPosition().GetY() + characterDelta.GetY());

                pCharacter->SetPosition(newPosition);
                pCharacter->SetState(characterStateMap[pCharacter]);

                CollisionParameter param;

                if (pCharacter->IsCollision(GetAreaHitBox(), &param))
                {
                    param.OverlapAxis = param.OverlapAxis.Normalize();
                    newPosition =
                        Vector2(pCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                pCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                    pCharacter->SetPosition(newPosition);
                }

                for (unsigned int j = 0; j < fieldCharacterList.size(); j++)
                {
                    FieldCharacter *pOtherCharacter = fieldCharacterList[j];

                    // Don't collide characters with themselves, or anyone with the partner and player characters,
                    // or anyone with someone else who's moving.
                    if (pCharacter != pOtherCharacter &&
                        pOtherCharacter != pPlayerCharacter &&
                        pOtherCharacter != pPartnerCharacter &&
                        pOtherCharacter->GetIsPresent())
                    {
                        if (pCharacter->IsCollision(pOtherCharacter, &param))
                        {
                            param.OverlapAxis = param.OverlapAxis.Normalize();
                            newPosition =
                                Vector2(pCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                        pCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                            pCharacter->SetPosition(newPosition);
                        }
                    }
                }

                for (unsigned int j = 0; j < foregroundElementList.size(); j++)
                {
                    ForegroundElement *pElement = foregroundElementList[j];

                    if (pElement->IsPresent() &&
                        pCharacter->IsCollision(pElement, &param))
                    {
                        param.OverlapAxis = param.OverlapAxis.Normalize();
                        newPosition =
                            Vector2(pCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                    pCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                        pCharacter->SetPosition(newPosition);
                    }
                }

                for (unsigned int i = 0; i < crowdList.size(); i++)
                {
                    Crowd *pCrowd = crowdList[i];

                    if (pCharacter->IsCollision(pCrowd, &param))
                    {
                        param.OverlapAxis = param.OverlapAxis.Normalize();
                        newPosition =
                            Vector2(pCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                    pCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                        pCharacter->SetPosition(newPosition);
                    }
                }

                Vector2 directionVector = (targetPosition - pCharacter->GetVectorAnchorPosition()).Normalize();

                // If the angle between the two normal vectors is pi / 2 or greater,
                // then they're not pointing in the same direction, and we've gone past the target position.
                // We'll back up to the target position and flag that we're done.
                double angleBetween = acos(min(max(directionVector * characterDelta.Normalize(), -1.0), 1.0));
                reachedTargetPosition = angleBetween >= M_PI / 2;

                // If this is the partner character, then she's reached the target position additionally
                // if the separation between her and the player character is sufficiently short.
                if (!reachedTargetPosition && pCharacter == pPartnerCharacter)
                {
                    double maximumSeparationDisplacement = pPlayerCharacter->GetClickRect().GetWidth() / 2 + pPartnerCharacter->GetClickRect().GetWidth() / 2 + 20;
                    double separationDisplacement = (targetPosition - pPartnerCharacter->GetVectorAnchorPosition()).Length();

                    queue<Vector2> targetPositionQueue = characterTargetPositionQueueMap[pCharacter];
                    Vector2 lastPosition = targetPosition;

                    while (!targetPositionQueue.empty())
                    {
                        Vector2 newTargetPosition = targetPositionQueue.front();
                        targetPositionQueue.pop();

                        separationDisplacement += (lastPosition - newTargetPosition).Length();
                        lastPosition = newTargetPosition;
                    }

                    if (separationDisplacement <= maximumSeparationDisplacement)
                    {
                        reachedTargetPosition = true;

                        // If the partner character is now close enough to the player character,
                        // we can ignore any other movements she would've made.
                        while (!characterTargetPositionQueueMap[pCharacter].empty())
                        {
                            characterTargetPositionQueueMap[pCharacter].pop();
                        }
                    }
                }

                if (!reachedTargetPosition)
                {
                    pCharacter->UpdateDirection(directionVector);

                    if ((!movingDirectly || pCharacter != pPlayerCharacter) && (pCharacter->GetVectorAnchorPosition() - characterPosition).Length() < 1)
                    {
                        // If the player character moved less than a pixel this frame, she's probably stuck.
                        // If we're moving through pathfinding, we should stop moving as a result.
                        pCharacter->SetState(FieldCharacterStateStanding);
                        characterStateMap[pCharacter] = FieldCharacterStateStanding;
                    }
                }
            }
            else
            {
                characterTargetPositionMap[pCharacter] = Vector2(-1, -1);
                pCharacter->SetState(FieldCharacterStateStanding);
                characterStateMap[pCharacter] = FieldCharacterStateStanding;
            }

            if (reachedTargetPosition)
            {
                if (characterTargetPositionQueueMap[pCharacter].empty())
                {
                    if (pCharacter != pPartnerCharacter)
                    {
                        pCharacter->SetPositionByAnchorPosition(targetPosition);
                    }

                    if (!movingDirectly || pCharacter != pPlayerCharacter)
                    {
                        characterTargetPositionMap[pCharacter] = Vector2(-1, -1);
                        pCharacter->SetState(FieldCharacterStateStanding);
                        characterStateMap[pCharacter] = FieldCharacterStateStanding;
                    }
                }
                else
                {
                    characterTargetPositionMap[pCharacter] = characterTargetPositionQueueMap[pCharacter].front();
                    characterTargetPositionQueueMap[pCharacter].pop();
                }
            }
        }
        else if (characterStateMap[pCharacter] == FieldCharacterStateStanding)
        {
            pCharacter->SetState(FieldCharacterStateStanding);

            while (!characterTargetPositionQueueMap[pCharacter].empty())
            {
                characterTargetPositionQueueMap[pCharacter].pop();
            }

            characterTargetPositionMap[pCharacter] = Vector2(-1, -1);
        }
    }

    pPlayerCharacter->Update(delta);

    bool shouldDoPartnerPathfinding = false;

    if (pPartnerCharacter != NULL && pCurrentInteractiveElement == NULL && acceptsUserInput)
    {
        Vector2 playerCharacterAnchorPoint = pPlayerCharacter->GetVectorAnchorPosition();
        Vector2 partnerCharacterAnchorPoint = pPartnerCharacter->GetVectorAnchorPosition();

        Vector2 separationDisplacement = playerCharacterAnchorPoint - partnerCharacterAnchorPoint;
        int maximumSeparationDisplacement = (int)(pPlayerCharacter->GetClickRect().GetWidth() / 2 + pPartnerCharacter->GetClickRect().GetWidth() / 2 + 20);

        if (separationDisplacement.Length() > maximumSeparationDisplacement)
        {
            if (pPlayerCharacter->GetState() == FieldCharacterStateStanding)
            {
                shouldDoPartnerPathfinding = true;
            }
            else
            {
                characterSpeedMap[pPartnerCharacter] = 0;
                characterStateMap[pPartnerCharacter] = FieldCharacterStateStanding;

                if (characterStateMap[pPlayerCharacter] != FieldCharacterStateStanding)
                {
                    characterStateMap[pPartnerCharacter] = characterStateMap[pPlayerCharacter];
                    characterSpeedMap[pPartnerCharacter] = characterSpeedMap[pPlayerCharacter];
                }
                else
                {
                    characterStateMap[pPartnerCharacter] = FieldCharacterStateWalking;
                    characterSpeedMap[pPartnerCharacter] = WalkingSpeed;
                }

                characterTargetPositionQueueMap[pPartnerCharacter] = queue<Vector2>();
                characterTargetPositionMap[pPartnerCharacter] = Vector2(-1, -1);

                pPartnerCharacter->SetState(characterStateMap[pPartnerCharacter]);

                double movementDistance = characterSpeedMap[pPartnerCharacter] * (double)delta / 1000.0;

                if (separationDisplacement.Length() - movementDistance < maximumSeparationDisplacement)
                {
                    movementDistance = separationDisplacement.Length() - maximumSeparationDisplacement;

                    if (movementDistance < 0.001)
                    {
                        movementDistance = 0.001;
                    }
                }

                Vector2 movementDisplacement = separationDisplacement.Normalize() * movementDistance;
                pPartnerCharacter->SetPosition(pPartnerCharacter->GetPosition() + movementDisplacement);
                pPartnerCharacter->UpdateDirection(movementDisplacement.Normalize());

                CollisionParameter param;
                Vector2 newPosition = pPartnerCharacter->GetPosition();

                if (pPartnerCharacter->IsCollision(GetAreaHitBox(), &param))
                {
                    param.OverlapAxis = param.OverlapAxis.Normalize();
                    newPosition =
                        Vector2(pPartnerCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                pPartnerCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                    pPartnerCharacter->SetPosition(newPosition);
                }

                for (unsigned int i = 0; i < fieldCharacterList.size(); i++)
                {
                    FieldCharacter *pOtherCharacter = fieldCharacterList[i];

                    // Don't collide characters with themselves, or anyone with the partner and player characters,
                    // or anyone with someone else who's moving.
                    if (pPartnerCharacter != pOtherCharacter &&
                        pOtherCharacter != pPlayerCharacter &&
                        pOtherCharacter != pPartnerCharacter &&
                        pOtherCharacter->GetIsPresent())
                    {
                        if (pPartnerCharacter->IsCollision(pOtherCharacter, &param))
                        {
                            param.OverlapAxis = param.OverlapAxis.Normalize();
                            newPosition =
                                Vector2(pPartnerCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                        pPartnerCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                            pPartnerCharacter->SetPosition(newPosition);
                        }
                    }
                }

                for (unsigned int j = 0; j < foregroundElementList.size(); j++)
                {
                    ForegroundElement *pElement = foregroundElementList[j];

                    if (pElement->IsPresent() &&
                        pPartnerCharacter->IsCollision(pElement, &param))
                    {
                        param.OverlapAxis = param.OverlapAxis.Normalize();
                        newPosition =
                            Vector2(pPartnerCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                    pPartnerCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                        pPartnerCharacter->SetPosition(newPosition);
                    }
                }

                for (unsigned int i = 0; i < crowdList.size(); i++)
                {
                    Crowd *pCrowd = crowdList[i];

                    if (pPartnerCharacter->IsCollision(pCrowd, &param))
                    {
                        param.OverlapAxis = param.OverlapAxis.Normalize();
                        newPosition =
                            Vector2(pPartnerCharacter->GetPosition().GetX() + param.OverlapAxis.GetX() * param.OverlapDistance,
                                    pPartnerCharacter->GetPosition().GetY() + param.OverlapAxis.GetY() * param.OverlapDistance);
                        pPartnerCharacter->SetPosition(newPosition);
                    }
                }

                Vector2 newPartnerCharacterAnchorPoint = pPartnerCharacter->GetVectorAnchorPosition();

                Vector2 newSeparationDisplacement = playerCharacterAnchorPoint - newPartnerCharacterAnchorPoint;
                int minimumSeparationDisplacementForWarp = (int)(pPlayerCharacter->GetClickRect().GetWidth() / 2 + pPartnerCharacter->GetClickRect().GetWidth() / 2 + 200);

                if (newSeparationDisplacement.Length() >= minimumSeparationDisplacementForWarp)
                {
                    pPartnerCharacter->SetPosition(
                        pPlayerCharacter->GetPosition() +
                        Vector2(
                            pPlayerCharacter->GetClickRect().GetX() + pPlayerCharacter->GetClickRect().GetWidth() / 2 - (pPartnerCharacter->GetClickRect().GetX() + pPartnerCharacter->GetClickRect().GetWidth() / 2),
                            pPlayerCharacter->GetClickRect().GetY() + pPlayerCharacter->GetClickRect().GetHeight() / 2 - (pPartnerCharacter->GetClickRect().GetY() + pPartnerCharacter->GetClickRect().GetHeight() / 2)));
                    pPartnerCharacter->SetState(FieldCharacterStateStanding);
                }
            }
        }
        else
        {
            pPartnerCharacter->UpdateDirection(pPlayerCharacter->GetMidPoint() - pPartnerCharacter->GetMidPoint());
            pPartnerCharacter->SetState(FieldCharacterStateStanding);
        }
    }
    else if (pPartnerCharacter != NULL && !acceptsUserInput)
    {
        pPartnerCharacter->SetState(FieldCharacterStateStanding);
    }

    SDL_SemPost(pPathfindingValuesSemaphore);

    if (pPartnerCharacter != NULL)
    {
        pPartnerCharacter->UpdateAnimation(delta);
    }

    if (shouldDoPartnerPathfinding)
    {
        StartCharacterOnPath(pPartnerCharacter, pPlayerCharacter->GetVectorAnchorPosition(), FieldCharacterStateWalking, false /* doAsync */);
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        FieldCharacter *pCharacter = characterList[i];

        if (pPartnerCharacter == NULL || pPartnerCharacter->GetId() != pCharacter->GetId())
        {
            pCharacter->SetExtraHeightFromHeightMaps(&heightMapList);
        }
    }

    pPlayerCharacter->SetExtraHeightFromHeightMaps(&heightMapList);

    if (pPartnerCharacter != NULL)
    {
        pPartnerCharacter->SetExtraHeightFromHeightMaps(&heightMapList);
    }

    drawingOffsetVector = pPlayerCharacter->GetMidPoint() - (Vector2(gScreenWidth, gScreenHeight) * 0.5);

    // If this would take us off the edge of the map, however,
    // clamp this to the bounds of the background.
    if (drawingOffsetVector.GetX() < 0)
    {
        drawingOffsetVector.SetX(0);
    }
    else if (drawingOffsetVector.GetX() > GetBackgroundSprite()->GetWidth() - gScreenWidth)
    {
        drawingOffsetVector.SetX(GetBackgroundSprite()->GetWidth() - gScreenWidth);
    }

    if (drawingOffsetVector.GetY() < 0)
    {
        drawingOffsetVector.SetY(0);
    }
    else if (drawingOffsetVector.GetY() > GetBackgroundSprite()->GetHeight() - gScreenHeight)
    {
        drawingOffsetVector.SetY(GetBackgroundSprite()->GetHeight() - gScreenHeight);
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        FieldCharacter *pCharacter = characterList[i];

        if (pCharacter == pPlayerCharacter || pCharacter == pPartnerCharacter)
        {
            continue;
        }

        AnimationSound *pSoundToPlay = pCharacter->GetSoundToPlay();

        if (pSoundToPlay != NULL)
        {
            soundsToPlayList.push_back(PositionalSound(pSoundToPlay, pCharacter->GetMidPoint()));
        }
    }

    AnimationSound *pSoundToPlay = pPlayerCharacter->GetSoundToPlay();

    if (pSoundToPlay != NULL)
    {
        soundsToPlayList.push_back(PositionalSound(pSoundToPlay, pPlayerCharacter->GetMidPoint()));
    }

    if (pPartnerCharacter != NULL)
    {
        pSoundToPlay = pPartnerCharacter->GetSoundToPlay();

        if (pSoundToPlay != NULL)
        {
            soundsToPlayList.push_back(PositionalSound(pSoundToPlay, pPartnerCharacter->GetMidPoint()));
        }
    }

    for (unsigned int i = 0; i < soundsToPlayList.size(); i++)
    {
        AnimationSound *pSoundToPlay = soundsToPlayList[i].pSound;
        Vector2 soundOriginPosition = soundsToPlayList[i].location;

        double distanceToOrigin = (soundOriginPosition - (drawingOffsetVector + (Vector2(gScreenWidth, gScreenHeight) * 0.5))).Length();
        double zeroDistance = max(gScreenWidth, gScreenHeight);//sqrt(gScreenWidth * gScreenWidth + gScreenHeight * gScreenHeight);
        double volume = max(0.0, 1.0 - distanceToOrigin / zeroDistance);

        pSoundToPlay->Play(volume);
    }

    SetLoopingSoundLevels();

    if (pQuitConfirmOverlay->GetIsShowing())
    {
        pQuitConfirmOverlay->Update(delta);
    }
}

void Location::UpdateTabPositions(int delta)
{
    bool shouldShowTabs =
        GetAcceptsUserInput() &&
        pCurrentEncounter == NULL &&
        pCurrentCutscene == NULL &&
        !pEvidenceSelector->GetIsShowing() &&
        pCurrentZoomedView == NULL &&
        pCurrentInteractiveForegroundElement == NULL &&
        pCurrentInteractiveCrowd == NULL &&
        !pInEasePartner->GetIsStarted() &&
        !isTransitioning;

    pEvidenceTab->SetIsHidden(!shouldShowTabs);
    pPartnerTab->SetIsHidden(!shouldShowTabs);
    pPartnerAbilityTab->SetIsHidden(!shouldShowTabs);
    pOkTab->SetIsHidden(!pInEasePartner->GetIsStarted() || pOutEasePartner->GetIsStarted());
    pSaveTab->SetIsHidden(!shouldShowTabs);
    pLoadTab->SetIsHidden(!shouldShowTabs);
    pOptionsTab->SetIsHidden(!shouldShowTabs);
    pQuitTab->SetIsHidden(!shouldShowTabs);

    pEvidenceTab->UpdatePosition(delta);
    pPartnerTab->UpdatePosition(delta);
    pPartnerAbilityTab->UpdatePosition(delta);
    pOkTab->UpdatePosition(delta);
    pSaveTab->UpdatePosition(delta);
    pLoadTab->UpdatePosition(delta);
    pOptionsTab->UpdatePosition(delta);
    pQuitTab->UpdatePosition(delta);
}

void Location::Draw()
{
    if (fadeOpacity == 1)
    {
        return;
    }

    if (pCurrentZoomedView == NULL)
    {
        GetBackgroundSprite()->DrawClipped(Vector2(0, 0), RectangleWH(drawingOffsetVector.GetX(), drawingOffsetVector.GetY(), gScreenWidth, gScreenHeight));

        list<ZOrderableObject *> objectsInZOrder;

        for (unsigned int i = 0; i < foregroundElementList.size(); i++)
        {
            ForegroundElement *pForegroundElement = foregroundElementList[i];

            if (pForegroundElement->IsVisible())
            {
                objectsInZOrder.push_back(pForegroundElement);
            }
        }

        for (unsigned int i = 0; i < hiddenForegroundElementList.size(); i++)
        {
            HiddenForegroundElement *pHiddenForegroundElement = hiddenForegroundElementList[i];

            if (pHiddenForegroundElement->IsVisible() && pHiddenForegroundElement->GetIsDiscovered())
            {
                objectsInZOrder.push_back(pHiddenForegroundElement);
            }
        }

        if (pCurrentCutscene != NULL && pCurrentCutscene->GetHasBegun())
        {
            pCurrentCutscene->Draw(drawingOffsetVector, &objectsInZOrder);
            pFadeSprite->Draw(Vector2(0, 0), Color(fadeOpacity, 1.0, 1.0, 1.0));
            return;
        }

        if (pPartnerCharacter != NULL)
        {
            objectsInZOrder.push_back(pPartnerCharacter);
        }

        for (unsigned int i = 0; i < characterList.size(); i++)
        {
            FieldCharacter *pCharacter = characterList[i];

            if ((pPartnerCharacter == NULL || pPartnerCharacter->GetId() != pCharacter->GetId()) && pCharacter->GetIsPresent())
            {
                objectsInZOrder.push_back(pCharacter);
            }
        }

        for (unsigned int i = 0; i < crowdList.size(); i++)
        {
            Crowd *pCrowd = crowdList[i];
            objectsInZOrder.push_back(pCrowd);
        }

        objectsInZOrder.push_back(pPlayerCharacter);
        objectsInZOrder.sort(CompareByZOrder);

        for (list<ZOrderableObject *>::iterator iter = objectsInZOrder.begin(); iter != objectsInZOrder.end(); ++iter)
        {
            (*iter)->Draw(drawingOffsetVector);
        }

        #ifdef MLI_DEBUG
            #ifdef MLI_DEBUG_DRAW_HITBOXES
                pAreaHitBox->Draw(Vector2(0, 0) - drawingOffsetVector);

                for (unsigned int i = 0; i < objectsInZOrder.size(); i++)
                {
                    FieldCharacter *pCharacter = dynamic_cast<FieldCharacter *>(objectsInZOrder[i]);

                    if (pCharacter != NULL)
                    {
                        pCharacter->GetHitBox()->Draw(pCharacter->GetPosition() - drawingOffsetVector);
                    }
                }
            #endif
        #endif

        if (pEvidenceSelector->GetIsShowing())
        {
            pEvidenceSelector->Draw();
        }
        else if (pInEasePartner->GetIsStarted())
        {
            partnerInformation.Draw(animationOffsetPartner);
            pOkTab->Draw();
        }

        pPartnerTab->Draw();

        pLoadTab->Draw();
        pSaveTab->Draw();

        pOptionsTab->Draw();
        pQuitTab->Draw();

        if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 &&
            Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetFieldAbilityName().length() > 0)
        {
            pPartnerAbilityTab->Draw();
        }

        pEvidenceTab->Draw();

        if (pQuitConfirmOverlay->GetIsShowing())
        {
            pQuitConfirmOverlay->Draw();
        }

        pFadeSprite->Draw(Vector2(0, 0), Color(fadeOpacity, 1.0, 1.0, 1.0));
    }

    if (pCurrentInteractiveForegroundElement != NULL)
    {
        pCurrentInteractiveForegroundElement->DrawInteraction();
        return;
    }

    if (pCurrentInteractiveCrowd != NULL)
    {
        pCurrentInteractiveCrowd->DrawInteraction();
        return;
    }

    if (pCurrentEncounter != NULL)
    {
        pCurrentEncounter->Draw();
        return;
    }

    if (pCurrentZoomedView != NULL)
    {
        pCurrentZoomedView->Draw();
    }
}

void Location::DrawForScreenshot()
{
    if (pCurrentZoomedView == NULL)
    {
        GetBackgroundSprite()->DrawClipped(Vector2(0, 0), RectangleWH(drawingOffsetVector.GetX(), drawingOffsetVector.GetY(), gScreenWidth, gScreenHeight));

        list<ZOrderableObject *> objectsInZOrder;

        for (unsigned int i = 0; i < foregroundElementList.size(); i++)
        {
            ForegroundElement *pForegroundElement = foregroundElementList[i];

            if (pForegroundElement->IsVisible())
            {
                objectsInZOrder.push_back(pForegroundElement);
            }
        }

        for (unsigned int i = 0; i < hiddenForegroundElementList.size(); i++)
        {
            HiddenForegroundElement *pHiddenForegroundElement = hiddenForegroundElementList[i];

            if (pHiddenForegroundElement->IsVisible() && pHiddenForegroundElement->GetIsDiscovered())
            {
                objectsInZOrder.push_back(pHiddenForegroundElement);
            }
        }

        if (pCurrentCutscene != NULL && pCurrentCutscene->GetHasBegun())
        {
            pCurrentCutscene->Draw(drawingOffsetVector, &objectsInZOrder);
            pFadeSprite->Draw(Vector2(0, 0), Color(fadeOpacity, 1.0, 1.0, 1.0));
            return;
        }

        if (pPartnerCharacter != NULL)
        {
            objectsInZOrder.push_back(pPartnerCharacter);
        }

        for (unsigned int i = 0; i < characterList.size(); i++)
        {
            FieldCharacter *pCharacter = characterList[i];

            if ((pPartnerCharacter == NULL || pPartnerCharacter->GetId() != pCharacter->GetId()) && pCharacter->GetIsPresent())
            {
                objectsInZOrder.push_back(pCharacter);
            }
        }

        for (unsigned int i = 0; i < crowdList.size(); i++)
        {
            Crowd *pCrowd = crowdList[i];
            objectsInZOrder.push_back(pCrowd);
        }

        objectsInZOrder.push_back(pPlayerCharacter);
        objectsInZOrder.sort(CompareByZOrder);

        for (list<ZOrderableObject *>::iterator iter = objectsInZOrder.begin(); iter != objectsInZOrder.end(); ++iter)
        {
            (*iter)->Draw(drawingOffsetVector);
        }

        #ifdef MLI_DEBUG
            #ifdef MLI_DEBUG_DRAW_HITBOXES
                pAreaHitBox->Draw(Vector2(0, 0) - drawingOffsetVector);

                for (unsigned int i = 0; i < objectsInZOrder.size(); i++)
                {
                    FieldCharacter *pCharacter = dynamic_cast<FieldCharacter *>(objectsInZOrder[i]);

                    if (pCharacter != NULL)
                    {
                        pCharacter->GetHitBox()->Draw(pCharacter->GetPosition() - drawingOffsetVector);
                    }
                }
            #endif
        #endif
    }
}

void Location::Reset()
{
    pCurrentEncounter = NULL;
    pCurrentCutscene = NULL;
    pNextCutscene = NULL;
    pCurrentInteractiveElement = NULL;
    pCurrentInteractiveForegroundElement = NULL;
    pCurrentInteractiveCrowd = NULL;
}

void Location::BeginTransition(Location *pTargetLocation, const string &transitionId)
{
    this->pTargetLocation = pTargetLocation;
    this->transitionId = transitionId;
    isTransitioning = true;
    pFadeOutEase->Begin();
}

void Location::CheckForTransitionUnderPlayer()
{
    // If the player starts in a transition hitbox, we shouldn't activate it
    // until the player has moved off of it.
    for (unsigned int i = 0; i < transitionList.size(); i++)
    {
        CollisionParameter param;

        if (pPlayerCharacter->IsCollision(transitionList[i]->GetHitBox(), &param))
        {
            pTransitionAtPlayer = transitionList[i];
            break;
        }
    }
}

void Location::SaveToSaveFile(XmlWriter *pWriter)
{
    pWriter->WriteTextElement("CurrentLocationId", id);
    pWriter->StartElement("PlayerCharacterPosition");
    pWriter->WriteDoubleElement("X", pPlayerCharacter->GetPosition().GetX());
    pWriter->WriteDoubleElement("Y", pPlayerCharacter->GetPosition().GetY());
    pWriter->EndElement();

    pWriter->WriteTextElement("PlayerCharacterDirection", CharacterDirectionToString(pPlayerCharacter->GetDirection()));
    pWriter->WriteTextElement("PlayerFieldCharacterDirection", FieldCharacterDirectionToString(pPlayerCharacter->GetSpriteDirection()));

    if (pPartnerCharacter != NULL)
    {
        pWriter->StartElement("PartnerCharacterPosition");
        pWriter->WriteDoubleElement("X", pPartnerCharacter->GetPosition().GetX());
        pWriter->WriteDoubleElement("Y", pPartnerCharacter->GetPosition().GetY());
        pWriter->EndElement();

        pWriter->WriteTextElement("PartnerCharacterDirection", CharacterDirectionToString(pPartnerCharacter->GetDirection()));
        pWriter->WriteTextElement("PartnerFieldCharacterDirection", FieldCharacterDirectionToString(pPartnerCharacter->GetSpriteDirection()));
    }

    if (getPlayingMusic().length() > 0)
    {
        pWriter->WriteTextElement("CurrentBgm", getPlayingMusic());
    }
    else
    {
        pWriter->WriteTextElement("CurrentBgm", "None");
    }

    if (getPlayingAmbiance().length() > 0)
    {
        pWriter->WriteTextElement("CurrentAmbiance", getPlayingAmbiance());
    }
    else
    {
        pWriter->WriteTextElement("CurrentAmbiance", "None");
    }
}

void Location::LoadFromSaveFile(XmlReader *pReader)
{
    if (pPlayerCharacter == NULL)
    {
        pPlayerCharacter = Case::GetInstance()->GetFieldCharacterManager()->GetPlayerCharacter();
    }

    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() == 0)
    {
        delete pPartnerCharacter;
        pPartnerCharacter = NULL;
    }
    else
    {
        delete pPartnerCharacter;
        pPartnerCharacter = Case::GetInstance()->GetFieldCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId());

        // If the player is loading, it's unlikely that we'd want to maintain using the field ability,
        // so we'll disable it in that circumstance.
        if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetIsUsingFieldAbility())
        {
            Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->ToggleIsUsingFieldAbility();
        }
    }

    pPlayerCharacter->Begin();

    pReader->StartElement("PlayerCharacterPosition");
    pPlayerCharacter->SetPosition(Vector2(pReader->ReadDoubleElement("X"), pReader->ReadDoubleElement("Y")));
    pReader->EndElement();

    pPlayerCharacter->SetState(FieldCharacterStateStanding);
    pPlayerCharacter->SetDirection(StringToCharacterDirection(pReader->ReadTextElement("PlayerCharacterDirection")));
    pPlayerCharacter->SetSpriteDirection(StringToFieldCharacterDirection(pReader->ReadTextElement("PlayerFieldCharacterDirection")));

    if (pPartnerCharacter != NULL)
    {
        pPartnerCharacter->Begin();

        pReader->StartElement("PartnerCharacterPosition");
        pPartnerCharacter->SetPosition(Vector2(pReader->ReadDoubleElement("X"), pReader->ReadDoubleElement("Y")));
        pReader->EndElement();

        pPartnerCharacter->SetState(FieldCharacterStateStanding);
        pPartnerCharacter->SetDirection(StringToCharacterDirection(pReader->ReadTextElement("PartnerCharacterDirection")));
        pPartnerCharacter->SetSpriteDirection(StringToFieldCharacterDirection(pReader->ReadTextElement("PartnerFieldCharacterDirection")));
    }

    if (pReader->ElementExists("CurrentBgm"))
    {
        pendingBgm = pReader->ReadTextElement("CurrentBgm");
    }

    if (pReader->ElementExists("CurrentAmbiance"))
    {
        pendingAmbianceSfxId = pReader->ReadTextElement("CurrentAmbiance");
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        characterList[i]->Begin();
    }

    for (unsigned int i = 0; i < crowdList.size(); i++)
    {
        crowdList[i]->Begin();
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        foregroundElementList[i]->Begin();
    }

    pTargetInteractiveElement = NULL;

    vector<FieldCharacter *> fieldCharacterList;

    fieldCharacterList.push_back(pPlayerCharacter);

    if (pPartnerCharacter != NULL)
    {
        fieldCharacterList.push_back(pPartnerCharacter);
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        FieldCharacter *pCharacter = characterList[i];
        bool isPartnerCharacter = pPartnerCharacter != NULL && pPartnerCharacter->GetId() == pCharacter->GetId();

        if (!isPartnerCharacter)
        {
            fieldCharacterList.push_back(pCharacter);
        }
    }

    for (unsigned int i = 0; i < fieldCharacterList.size(); i++)
    {
        FieldCharacter *pCharacter = fieldCharacterList[i];
        characterStateMap[pCharacter] = FieldCharacterStateStanding;
        characterTargetPositionQueueMap[pCharacter] = queue<Vector2>();
        characterTargetPositionMap[pCharacter] = Vector2(-1, -1);
    }

    drawingOffsetVector = pPlayerCharacter->GetMidPoint() - (Vector2(gScreenWidth, gScreenHeight) * 0.5);

    // If this would take us off the edge of the map, however,
    // clamp this to the bounds of the background.
    if (drawingOffsetVector.GetX() < 0)
    {
        drawingOffsetVector.SetX(0);
    }
    else if (drawingOffsetVector.GetX() > GetBackgroundSprite()->GetWidth() - gScreenWidth)
    {
        drawingOffsetVector.SetX(GetBackgroundSprite()->GetWidth() - gScreenWidth);
    }

    if (drawingOffsetVector.GetY() < 0)
    {
        drawingOffsetVector.SetY(0);
    }
    else if (drawingOffsetVector.GetY() > GetBackgroundSprite()->GetHeight() - gScreenHeight)
    {
        drawingOffsetVector.SetY(GetBackgroundSprite()->GetHeight() - gScreenHeight);
    }

    pTargetLocation = NULL;
    transitionId = "";
    isTransitioning = false;
    isFinishing = false;
    overlayId = "";
    wasInOverlay = false;
    pFadeOutEase->Reset();
    pFadeInEase->Begin();

    fadeOpacity = 1;

    pEvidenceTab->SetIsHidden(true, false);
    pPartnerTab->SetIsHidden(true, false);
    pPartnerAbilityTab->SetIsHidden(true, false);
    pOkTab->SetIsHidden(true, false);
    pSaveTab->SetIsHidden(true, false);
    pLoadTab->SetIsHidden(true, false);
    pOptionsTab->SetIsHidden(true, false);
    pQuitTab->SetIsHidden(true, false);

    CheckForTransitionUnderPlayer();
}

void Location::BeginCharacterInteraction(FieldCharacter *pCharacter)
{
    pCurrentEncounter = pCharacter->GetClickEncounter();
    pCurrentInteractiveElement = pCharacter;

    if (pCurrentEncounter != NULL)
    {
        previousPartnerCharacterId = Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId();
        pCurrentEncounter->Begin();
    }
    else
    {
        pCurrentCutscene = pCharacter->GetClickCutscene();

        if (pCurrentCutscene != NULL)
        {
            previousPartnerCharacterId = Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId();

            if (GetAcceptsUserInput())
            {
                pFadeInEase->Reset();
                pFadeOutEase->Begin();
            }
            else
            {
                pCurrentCutscene->Begin(pPartnerCharacter);
            }
        }
    }
}

void Location::BeginForegroundElementInteraction(ForegroundElement *pForegroundElement)
{
    pForegroundElement->BeginInteraction();
    pCurrentInteractiveElement = pForegroundElement;
    pCurrentInteractiveForegroundElement = pForegroundElement;
}

void Location::BeginCrowdInteraction(Crowd *pCrowd)
{
    pCrowd->BeginInteraction();
    pCurrentInteractiveElement = pCrowd;
    pCurrentInteractiveCrowd = pCrowd;
}

void Location::OnPromptOverlayValueReturned(PromptOverlay *pSender, const string &value)
{
    if (pSender == pQuitConfirmOverlay && value == pgLocalizableContent->GetText("Location/YesText"))
    {
        Case::GetInstance()->Autosave();
        SaveDialogsSeenListForCase(Case::GetInstance()->GetUuid());

        isFinishing = true;
        pFadeOutEase->Begin();
        pQuitConfirmOverlay->KeepOpen();
    }
}

void Location::SetLoopingSoundLevels()
{
    for (unsigned int i = 0; i < loopingSoundList.size(); i++)
    {
        Vector2 soundOriginPosition = loopingSoundList[i]->origin;

        double distanceToOrigin = (soundOriginPosition - (drawingOffsetVector + (Vector2(gScreenWidth, gScreenHeight) * 0.5))).Length();
        double zeroDistance = max(gScreenWidth, gScreenHeight);
        double volume = max(0.0, 1.0 - distanceToOrigin / zeroDistance);

        setLoopingSoundVolume(i, volume);
    }
}

Sprite * Location::GetBackgroundSprite()
{
    if (pBackgroundSprite == NULL)
    {
        pBackgroundSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(GetBackgroundSpriteId());
    }

    return pBackgroundSprite;
}

RectangleWH Location::GetBounds()
{
    if (bounds.GetWidth() == 0)
    {
        Sprite *pBackgroundSprite = GetBackgroundSprite();
        bounds = RectangleWH(0, 0, pBackgroundSprite->GetWidth(), pBackgroundSprite->GetHeight());
    }

    return bounds;
}

Location::StartPosition Location::GetStartPositionFromTransitionId(const string &transitionId)
{
    if (transitionId.length() > 0)
    {
        return transitionIdToStartPositionMap[transitionId];
    }
    else
    {
        return startPositionFromMap;
    }
}

Location::StartPosition * Location::GetPartnerStartPositionFromTransitionId(const string &transitionId)
{
    if (transitionId.length() > 0 && transitionIdToPartnerStartPositionMap.count(transitionId) > 0)
    {
        return &transitionIdToPartnerStartPositionMap[transitionId];
    }
    else
    {
        return NULL;
    }
}

void Location::SetTargetInteractiveElement(InteractiveElement *pInteractiveElement, FieldCharacterState characterStateIfMoving)
{
    clickPoint = MouseHelper::GetMousePosition() + drawingOffsetVector;
    pTargetInteractiveElement = pInteractiveElement;

    // If we're not already close enough to the target interactive element,
    // then move to its location.
    if (!GetIsPlayerCharacterCloseToInteractiveElement())
    {
        StartCharacterOnPath(pPlayerCharacter, pInteractiveElement->GetCenterPoint(), characterStateIfMoving);
    }

    MouseHelper::HandleClick();
    MouseHelper::HandleDoubleClick();
}

bool Location::GetIsPlayerCharacterCloseToInteractiveElement()
{
    if (pTargetInteractiveElement == NULL || pPlayerCharacter == NULL)
    {
        return false;
    }

    // If the player can interact with the element from anywhere,
    // then the player is always close enough.
    if (pTargetInteractiveElement->GetCanInteractFromAnywhere())
    {
        return true;
    }

    RectangleWH boundsForInteraction = pTargetInteractiveElement->GetBoundsForInteraction();
    Vector2 playerAnchorPosition = pPlayerCharacter->GetVectorAnchorPosition();
    int allowedDistanceOffset = pTargetInteractiveElement->IsInteractionPointExact() ? 20 : MaxDistanceToStartInteractions;

    return
        (((playerAnchorPosition.GetX() <= boundsForInteraction.GetX() && playerAnchorPosition.GetX() >= boundsForInteraction.GetX() - allowedDistanceOffset) ||
            (playerAnchorPosition.GetX() >= boundsForInteraction.GetX() && playerAnchorPosition.GetX() <= boundsForInteraction.GetX() + boundsForInteraction.GetWidth() + allowedDistanceOffset)) &&
        ((playerAnchorPosition.GetY() <= boundsForInteraction.GetY() && playerAnchorPosition.GetY() >= boundsForInteraction.GetY() - allowedDistanceOffset) ||
            (playerAnchorPosition.GetY() >= boundsForInteraction.GetY() && playerAnchorPosition.GetY() <= boundsForInteraction.GetY() + boundsForInteraction.GetHeight() + allowedDistanceOffset)));
}

void Location::StartCharacterOnPath(FieldCharacter *pCharacter, Vector2 endPosition, FieldCharacterState characterStateIfMoving, bool doAsync)
{
    Vector2 currentPosition = pCharacter->GetVectorAnchorPosition();

    if (doAsync)
    {
        SDL_SemWait(pPathfindingValuesSemaphore);
    }

    if (pCharacter == pPlayerCharacter)
    {
        movingDirectly = false;
    }

    lastPathfindingThreadId++;

    if (doAsync)
    {
        SDL_Thread *pThread = SDL_CreateThread(Location::PerformPathfindingStatic, "PathfindingThread", new PathfindingThreadParameters(this, pCharacter, currentPosition, endPosition, characterStateIfMoving, lastPathfindingThreadId));
        SDL_DetachThread(pThread);
        SDL_SemPost(pPathfindingValuesSemaphore);
    }
    else
    {
        PerformPathfinding(pCharacter, currentPosition, endPosition, characterStateIfMoving, lastPathfindingThreadId);
    }

    MouseHelper::HandleClick();
    MouseHelper::HandleDoubleClick();
}

void Location::OnExited(Location *pLocation, const string &transitionId)
{
    EventProviders::GetLocationEventProvider()->RaiseExited(this, pLocation, transitionId);
}

int Location::PerformPathfindingStatic(void *pData)
{
    PathfindingThreadParameters *pParams = reinterpret_cast<PathfindingThreadParameters *>(pData);

    Location *pThis = pParams->pLocation;
    FieldCharacter *pCharacter = pParams->pCharacter;
    Vector2 startPosition = pParams->startPosition;
    Vector2 endPosition = pParams->endPosition;
    FieldCharacterState characterStateIfMoving = pParams->characterStateIfMoving;
    int threadId = pParams->threadId;

    delete pParams;

    pThis->PerformPathfinding(pCharacter, startPosition, endPosition, characterStateIfMoving, threadId);

    return 0;
}

void Location::PerformPathfinding(FieldCharacter *pCharacter, Vector2 startPosition, Vector2 endPosition, FieldCharacterState characterStateIfMoving, int threadId)
{
    queue<Vector2> targetPositionQueue;
    Vector2 targetPosition;

    endPosition = FindClosestPassablePositionForCharacter(pCharacter, endPosition);
    targetPositionQueue = GetPathForCharacterBetweenPoints(pCharacter, startPosition, endPosition);

    if (targetPositionQueue.empty())
    {
        return;
    }

    targetPositionQueue = RemoveUnnecessaryStepsFromPath(pCharacter, startPosition, targetPositionQueue);

    SDL_SemWait(pPathfindingValuesSemaphore);

    if ((!movingDirectly || pCharacter != pPlayerCharacter) && lastPathfindingThreadId == threadId)
    {
        targetPosition = targetPositionQueue.front();
        characterTargetPositionQueueMap[pCharacter] = targetPositionQueue;
        characterTargetPositionMap[pCharacter] = targetPosition;
        characterTargetPositionQueueMap[pCharacter].pop();

        if (characterTargetPositionMap[pCharacter].GetX() >= 0)
        {
            Vector2 displacementDelta = characterTargetPositionMap[pCharacter] - startPosition;

            if (displacementDelta.Length() > 0.001)
            {
                if (displacementDelta.GetX() < 0)
                {
                    pCharacter->SetDirection(CharacterDirectionLeft);
                }
                else if (displacementDelta.GetX() > 0)
                {
                    pCharacter->SetDirection(CharacterDirectionRight);
                }

                characterStateMap[pCharacter] = characterStateIfMoving;
            }
        }
        else
        {
            characterStateMap[pCharacter] = FieldCharacterStateStanding;
        }
    }

    SDL_SemPost(pPathfindingValuesSemaphore);
}

queue<Vector2> Location::RemoveUnnecessaryStepsFromPath(FieldCharacter *pCharacter, Vector2 startPosition, queue<Vector2> pathPositionQueue)
{
    queue<Vector2> optimizedPath;
    bool optimizationOccurred = true;

    while (pathPositionQueue.size() > 1 && optimizationOccurred)
    {
        Vector2 previousPosition = startPosition;
        Vector2 currentPosition = pathPositionQueue.front();
        pathPositionQueue.pop();
        Vector2 nextPosition = pathPositionQueue.front();

        optimizationOccurred = false;
        optimizedPath = queue<Vector2>();

        while (!pathPositionQueue.empty())
        {
            // If a collision occurs when we remove the current step,
            // then this step is needed in the path, so we'll add it
            // to the optimized path.  Otherwise, we'll leave it out,
            // and note that an optimization occurred.
            if (IsCollisionBetweenTwoPositions(pCharacter, previousPosition, nextPosition))
            {
                optimizedPath.push(currentPosition);
                previousPosition = currentPosition;
                currentPosition = pathPositionQueue.front();
                pathPositionQueue.pop();
            }
            else
            {
                optimizationOccurred = true;

                if (pathPositionQueue.size() > 1)
                {
                    optimizedPath.push(nextPosition);
                    previousPosition = pathPositionQueue.front();
                    pathPositionQueue.pop();
                    currentPosition = pathPositionQueue.front();
                    pathPositionQueue.pop();
                }
                else
                {
                    // If we've only got one item left in the queue,
                    // set it to the current position so it'll get
                    // added to the optimized path.
                    currentPosition = pathPositionQueue.front();
                    pathPositionQueue.pop();
                }
            }

            if (!pathPositionQueue.empty())
            {
                nextPosition = pathPositionQueue.front();
            }
        }

        // We always need the last step to be in the path.
        optimizedPath.push(currentPosition);
        pathPositionQueue = optimizedPath;
    }

    return pathPositionQueue;
}

bool Location::IsCollisionBetweenTwoPositions(FieldCharacter *pCharacter, Vector2 startPosition, Vector2 endPosition)
{
    double totalDistance = (endPosition - startPosition).Length();
    // TODO: Reconsider this magic number.
    double stepSize = 10;
    Vector2 directionVector = (endPosition - startPosition).Normalize() * stepSize;
    Vector2 currentPosition = startPosition;

    for (double i = 0; i < totalDistance; i += stepSize)
    {
        if (TestCollisionWithLocationElements(pCharacter, currentPosition))
        {
            return true;
        }
        else
        {
            currentPosition += directionVector;
        }
    }

    return false;
}

Vector2 Location::FindClosestPassablePositionForCharacter(FieldCharacter *pCharacter, Vector2 position)
{
    deque<OverlapEntry> overlapEntries;
    stack<Vector2> positions;
    list<Vector2> possiblePositions;

    positions.push(position);

    FindClosestPassablePositionForCharacter(pCharacter, position, &overlapEntries, &positions, &possiblePositions);

    Vector2 closestPosition = pCharacter->GetVectorAnchorPosition();
    double closestPositionDistance = numeric_limits<double>::infinity();

    for (list<Vector2>::iterator iter = possiblePositions.begin(); iter != possiblePositions.end(); ++iter)
    {
        Vector2 possiblePosition = *iter;
        double possiblePositionDistance = (possiblePosition - position).Length();

        if (possiblePositionDistance < closestPositionDistance)
        {
            closestPositionDistance = possiblePositionDistance;
            closestPosition = possiblePosition;
        }
    }

    return closestPosition;
}

void Location::FindClosestPassablePositionForCharacter(FieldCharacter *pCharacter, Vector2 position, deque<OverlapEntry> *pOverlapEntriesThusFar, stack<Vector2> *pPositionsThusFar, list<Vector2> *pPossiblePositions)
{
    CollisionParameter param;

    if (!TestCollisionWithLocationElements(pCharacter, position, &param))
    {
        pPossiblePositions->push_back(position);
        pPossiblePositions->unique();
    }
    else
    {
        for (unsigned int i = 0; i < param.OverlapEntryList.size(); i++)
        {
            OverlapEntry overlapEntry = param.OverlapEntryList[i];

            if (find(pOverlapEntriesThusFar->begin(), pOverlapEntriesThusFar->end(), overlapEntry) == pOverlapEntriesThusFar->end())
            {
                Vector2 newPosition = pPositionsThusFar->top() + (overlapEntry.OverlapAxis * overlapEntry.OverlapDistance);
                RectangleWH locationBounds = GetBounds();

                if (newPosition.GetX() >= locationBounds.GetX() &&
                        newPosition.GetX() < locationBounds.GetX() + locationBounds.GetWidth() &&
                        newPosition.GetY() >= locationBounds.GetY() &&
                        newPosition.GetY() < locationBounds.GetY() + locationBounds.GetHeight())
                {
                    pPositionsThusFar->push(newPosition);
                    pOverlapEntriesThusFar->push_back(overlapEntry);

                    FindClosestPassablePositionForCharacter(pCharacter, newPosition, pOverlapEntriesThusFar, pPositionsThusFar, pPossiblePositions);

                    pOverlapEntriesThusFar->pop_back();
                    pPositionsThusFar->pop();
                }
            }
        }
    }
}

queue<Vector2> Location::GetPathForCharacterBetweenPoints(FieldCharacter *pCharacter, Vector2 start, Vector2 goal)
{
    list<Vector2> closedSet;
    list<Vector2> openSet;
    map<Vector2, Vector2> cameFrom;

    // Initialize the open set to have the start position.
    openSet.push_back(start);

    map<Vector2, double> gScore;
    map<Vector2, double> hScore;
    map<Vector2, double> fScore;

    gScore[start] = 0.0;
    hScore[start] = HeuristicCostEstimate(start, goal);
    fScore[start] = gScore[start] + hScore[start];

    double tileSize = 20;

    double closestPointDistance = numeric_limits<double>::infinity();
    Vector2 closestPoint(0, 0);

    while (!openSet.empty())
    {
        Vector2 current = GetVectorWithLowestFScore(openSet, fScore);

        if (IsPointInTileAtPoint(goal, current, tileSize))
        {
            closestPoint = current;
            break;
        }
        else if ((goal - current).Length() < closestPointDistance)
        {
            closestPointDistance = (goal - current).Length();
            closestPoint = current;
        }

        openSet.remove(current);
        closedSet.push_back(current);

        list<Vector2> neighborNodes = GetNeighbors(pCharacter, current, goal, tileSize);

        for (list<Vector2>::iterator iter = neighborNodes.begin(); iter != neighborNodes.end(); ++iter)
        {
            Vector2 neighborNode = *iter;

            if (find(closedSet.begin(), closedSet.end(), neighborNode) != closedSet.end())
            {
                continue;
            }

            double tentativeGScore = gScore[current] + (neighborNode - current).Length();
            bool tentativeIsBetter = false;

            if (find(openSet.begin(), openSet.end(), neighborNode) == openSet.end())
            {
                openSet.push_back(neighborNode);
                hScore[neighborNode] = HeuristicCostEstimate(neighborNode, goal);
                tentativeIsBetter = true;
            }
            else if (tentativeGScore < gScore[neighborNode])
            {
                tentativeIsBetter = true;
            }

            if (tentativeIsBetter)
            {
                cameFrom[neighborNode] = current;
                gScore[neighborNode] = tentativeGScore;
                fScore[neighborNode] = tentativeGScore + hScore[neighborNode];
            }
        }
    }

    return ReconstructPath(&cameFrom, closestPoint, goal);
}

double Location::HeuristicCostEstimate(Vector2 start, Vector2 end)
{
    return (end - start).Length();
}

bool Location::IsPointInTileAtPoint(Vector2 point, Vector2 tilePoint, double tileSize)
{
    return
        point.GetX() - tilePoint.GetX() < tileSize / 2 && point.GetX() - tilePoint.GetX() >= -tileSize / 2 &&
        point.GetY() - tilePoint.GetY() < tileSize / 2 && point.GetY() - tilePoint.GetY() >= -tileSize / 2;
}

Vector2 Location::GetVectorWithLowestFScore(list<Vector2> openSet, map<Vector2, double> fScore)
{
    double lowestFScore = numeric_limits<double>::infinity();
    Vector2 vectorWithLowestFScore(0, 0);

    for (list<Vector2>::iterator iter = openSet.begin(); iter != openSet.end(); ++iter)
    {
        Vector2 currentVector = *iter;
        double currentFScore = fScore[currentVector];

        if (currentFScore < lowestFScore)
        {
            lowestFScore = currentFScore;
            vectorWithLowestFScore = currentVector;
        }
    }

    return vectorWithLowestFScore;
}

list<Vector2> Location::GetNeighbors(FieldCharacter *pCharacter, Vector2 position, Vector2 goal, double tileSize)
{
    list<Vector2> neighborNodes;

    Vector2 topLeftNeighbor(position.GetX() - tileSize, position.GetY() - tileSize);

    if (IsPointInTileAtPoint(goal, topLeftNeighbor, tileSize))
    {
        topLeftNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, topLeftNeighbor))
    {
        neighborNodes.push_back(topLeftNeighbor);
    }

    Vector2 topNeighbor(position.GetX(), position.GetY() - tileSize);

    if (IsPointInTileAtPoint(goal, topNeighbor, tileSize))
    {
        topNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, topNeighbor))
    {
        neighborNodes.push_back(topNeighbor);
    }

    Vector2 topRightNeighbor(position.GetX() + tileSize, position.GetY() - tileSize);

    if (IsPointInTileAtPoint(goal, topRightNeighbor, tileSize))
    {
        topRightNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, topRightNeighbor))
    {
        neighborNodes.push_back(topRightNeighbor);
    }

    Vector2 leftNeighbor(position.GetX() - tileSize, position.GetY());

    if (IsPointInTileAtPoint(goal, leftNeighbor, tileSize))
    {
        topNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, leftNeighbor))
    {
        neighborNodes.push_back(leftNeighbor);
    }

    Vector2 rightNeighbor(position.GetX() + tileSize, position.GetY());

    if (IsPointInTileAtPoint(goal, rightNeighbor, tileSize))
    {
        rightNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, rightNeighbor))
    {
        neighborNodes.push_back(rightNeighbor);
    }

    Vector2 bottomLeftNeighbor(position.GetX() - tileSize, position.GetY() + tileSize);

    if (IsPointInTileAtPoint(goal, bottomLeftNeighbor, tileSize))
    {
        bottomLeftNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, bottomLeftNeighbor))
    {
        neighborNodes.push_back(bottomLeftNeighbor);
    }

    Vector2 bottomNeighbor(position.GetX(), position.GetY() + tileSize);

    if (IsPointInTileAtPoint(goal, bottomNeighbor, tileSize))
    {
        bottomNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, bottomNeighbor))
    {
        neighborNodes.push_back(bottomNeighbor);
    }

    Vector2 bottomRightNeighbor(position.GetX() + tileSize, position.GetY() + tileSize);

    if (IsPointInTileAtPoint(goal, bottomRightNeighbor, tileSize))
    {
        bottomRightNeighbor = goal;
    }

    if (!TestCollisionWithLocationElements(pCharacter, bottomRightNeighbor))
    {
        neighborNodes.push_back(bottomRightNeighbor);
    }

    return neighborNodes;
}

bool Location::TestCollisionWithLocationElements(FieldCharacter *pCharacter, Vector2 position)
{
    CollisionParameter param;
    position -= pCharacter->GetVectorAnchorPosition() - pCharacter->GetPosition();

    if (pCharacter->TestCollisionAtPosition(position, GetAreaHitBox(), &param))
    {
        return true;
    }

    if (pCharacter != pPlayerCharacter &&
        pCharacter != pPartnerCharacter &&
        pCharacter->IsVisible() &&
        pCharacter->TestCollisionAtPosition(position, pPlayerCharacter, &param))
    {
        return true;
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        FieldCharacter *pCharacterToTest = characterList[i];

        if (pCharacterToTest->IsVisible() &&
            pCharacter != pCharacterToTest &&
            pCharacter != pTargetInteractiveElement &&
            pCharacterToTest != pTargetInteractiveElement &&
            (pPartnerCharacter == NULL || pCharacterToTest->GetId() != pPartnerCharacter->GetId()))
        {
            if (pCharacter->TestCollisionAtPosition(position, pCharacterToTest, &param))
            {
                return true;
            }
        }
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        ForegroundElement *pElement = foregroundElementList[i];

        if (pElement->IsPresent() &&
            pCharacter != pTargetInteractiveElement &&
            pElement != pTargetInteractiveElement)
        {
            if (pCharacter->TestCollisionAtPosition(position, pElement, &param))
            {
                return true;
            }
        }
    }

    for (unsigned int i = 0; i < crowdList.size(); i++)
    {
        Crowd *pCrowd = crowdList[i];

        if (pCharacter != pTargetInteractiveElement &&
            pCrowd != pTargetInteractiveElement)
        {
            if (pCharacter->TestCollisionAtPosition(position, pCrowd, &param))
            {
                return true;
            }
        }
    }

    return false;
}

bool Location::TestCollisionWithLocationElements(FieldCharacter *pCharacter, Vector2 position, CollisionParameter *pParam)
{
    position -= pCharacter->GetVectorAnchorPosition() - pCharacter->GetPosition();

    if (pCharacter->TestCollisionAtPosition(position, GetAreaHitBox(), pParam))
    {
        return true;
    }

    if (pCharacter != pPlayerCharacter &&
        pCharacter != pPartnerCharacter &&
        pCharacter->IsVisible() &&
        pCharacter->TestCollisionAtPosition(position, pPlayerCharacter, pParam))
    {
        return true;
    }

    for (unsigned int i = 0; i < characterList.size(); i++)
    {
        FieldCharacter *pCharacterToTest = characterList[i];

        if (pCharacterToTest->IsVisible() &&
            pCharacter != pCharacterToTest &&
            pCharacter != pTargetInteractiveElement &&
            pCharacterToTest != pTargetInteractiveElement &&
            (pPartnerCharacter == NULL || pCharacterToTest->GetId() != pPartnerCharacter->GetId()))
        {
            if (pCharacter->TestCollisionAtPosition(position, pCharacterToTest, pParam))
            {
                return true;
            }
        }
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        ForegroundElement *pElement = foregroundElementList[i];

        if (pElement->IsPresent() &&
            pCharacter != pTargetInteractiveElement &&
            pElement != pTargetInteractiveElement)
        {
            if (pCharacter->TestCollisionAtPosition(position, pElement, pParam))
            {
                return true;
            }
        }
    }

    for (unsigned int i = 0; i < crowdList.size(); i++)
    {
        Crowd *pCrowd = crowdList[i];

        if (pCharacter != pTargetInteractiveElement &&
            pCrowd != pTargetInteractiveElement)
        {
            if (pCharacter->TestCollisionAtPosition(position, pCrowd, pParam))
            {
                return true;
            }
        }
    }

    return false;
}

queue<Vector2> Location::ReconstructPath(map<Vector2, Vector2> *pCameFrom, Vector2 currentNode, Vector2 goalNode)
{
    queue<Vector2> currentQueue;

    if (pCameFrom->count(currentNode) > 0)
    {
        currentQueue = ReconstructPath(pCameFrom, (*pCameFrom)[currentNode]);
        currentQueue.push(goalNode);
    }
    // Don't add the current node here - there's no reason to
    // add the start node to the queue, as we're already there.

    return currentQueue;
}

queue<Vector2> Location::ReconstructPath(map<Vector2, Vector2> *pCameFrom, Vector2 currentNode)
{
    queue<Vector2> currentQueue;

    if (pCameFrom->count(currentNode) > 0)
    {
        currentQueue = ReconstructPath(pCameFrom, (*pCameFrom)[currentNode]);
        currentQueue.push(currentNode);
    }
    // Don't add the current node here - there's no reason to
    // add the start node to the queue, as we're already there.

    return currentQueue;
}
