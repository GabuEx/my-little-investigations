/**
 * Represents a cutscene that plays out in the field.
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

#include "FieldCutscene.h"
#include "../FileFunctions.h"
#include "../globals.h"
#include "../Interfaces.h"
#include "../PositionalSound.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"
#include "../CaseInformation/Case.h"

const int WalkingSpeed = 300;
const int RunningSpeed = 600;
const int CenterCameraDuration = 500;

FieldCutscene::FieldCutscene()
{
    pActualPlayerCharacter = NULL;
    pActualPartnerCharacter = NULL;
    pCurrentPhase = NULL;
    currentPhaseId = 0;

    pCurrentConcurrentMovements = NULL;

    pBackgroundSprite = NULL;
    backgroundSpriteOpacity = 0;
    pReplacementBackgroundSprite = NULL;
    replacementBackgroundSpriteOpacity = 0;

    isEnabled = false;
    isFinished = false;
    hasBegun = false;
    hasCompleted = false;
    preventsInitialBgm = false;
    initialBgmReplacement = "";
    preventsInitialAmbiance = false;
    initialAmbianceReplacement = "";
}

FieldCutscene::FieldCutscene(XmlReader *pReader)
{
    pActualPlayerCharacter = NULL;
    pActualPartnerCharacter = NULL;
    pCurrentPhase = NULL;
    currentPhaseId = 0;

    pCurrentConcurrentMovements = NULL;

    pBackgroundSprite = NULL;
    backgroundSpriteOpacity = 0;
    pReplacementBackgroundSprite = NULL;
    replacementBackgroundSpriteOpacity = 0;

    isEnabled = false;
    isFinished = false;
    hasBegun = false;
    hasCompleted = false;
    preventsInitialBgm = false;
    initialBgmReplacement = "";
    preventsInitialAmbiance = false;
    initialAmbianceReplacement = "";

    pReader->StartElement("FieldCutscene");

    id = pReader->ReadTextElement("Id");

    pReader->StartElement("PhaseList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        phaseList.push_back(GetPhaseForNextElement(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("IdToCharacterDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        idToCharacterMap[id] = new FieldCharacter(pReader);

        characterToOriginalCharacterDirectionMap[idToCharacterMap[id]] = idToCharacterMap[id]->GetDirection();
        characterToOriginalFieldCharacterDirectionMap[idToCharacterMap[id]] = idToCharacterMap[id]->GetSpriteDirection();
        characterToOriginalPositionMap[idToCharacterMap[id]] = idToCharacterMap[id]->GetPosition();
        characterToOriginalStateMap[idToCharacterMap[id]] = idToCharacterMap[id]->GetState();
    }

    pReader->EndElement();

    isEnabled = pReader->ReadBooleanElement("IsEnabled");
    preventsInitialBgm = pReader->ReadBooleanElement("PreventInitialBgm");

    if (pReader->ElementExists("InitialBgmReplacement"))
    {
        initialBgmReplacement = pReader->ReadTextElement("InitialBgmReplacement");
    }

    preventsInitialAmbiance = pReader->ReadBooleanElement("PreventInitialAmbiance");

    if (pReader->ElementExists("InitialAmbianceReplacement"))
    {
        initialAmbianceReplacement = pReader->ReadTextElement("InitialAmbianceReplacement");
    }

    pReader->StartElement("InitialCameraPosition");
    initialCameraPosition = Vector2(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

FieldCutscene::~FieldCutscene()
{
    for (unsigned int i = 0; i < phaseList.size(); i++)
    {
        delete phaseList[i];
        phaseList[i] = NULL;
    }

    for (map<string, FieldCharacter *>::iterator iter = idToCharacterMap.begin(); iter != idToCharacterMap.end(); ++iter)
    {
        if (iter->second != pActualPlayerCharacter && iter->second != pActualPartnerCharacter)
        {
            delete iter->second;
        }
    }
}

void FieldCutscene::Begin(FieldCharacter *pPartnerCharacter)
{
    SetHasBegun(true);
    SetIsFinished(false);
    currentPhaseId = 0;
    currentCameraPosition = initialCameraPosition;
    transitionRequest = TransitionRequest();

    if (idToCharacterMap.count("PlayerCharacter") > 0)
    {
        // We store a copy of the player character when compiling the case,
        // so we need to replace it with the actual player character.
        if (pActualPlayerCharacter == NULL || pActualPlayerCharacter != Case::GetInstance()->GetFieldCharacterManager()->GetPlayerCharacter())
        {
            FieldCharacter *pPlayerCharacterStored = idToCharacterMap["PlayerCharacter"];
            pActualPlayerCharacter = Case::GetInstance()->GetFieldCharacterManager()->GetPlayerCharacter();

            CharacterDirection originalCharacterDirection = characterToOriginalCharacterDirectionMap[pPlayerCharacterStored];
            FieldCharacterDirection originalFieldCharacterDirection = characterToOriginalFieldCharacterDirectionMap[pPlayerCharacterStored];
            Vector2 originalPosition = characterToOriginalPositionMap[pPlayerCharacterStored];
            FieldCharacterState originalState = characterToOriginalStateMap[pPlayerCharacterStored];

            characterToOriginalCharacterDirectionMap.erase(pPlayerCharacterStored);
            characterToOriginalFieldCharacterDirectionMap.erase(pPlayerCharacterStored);
            characterToOriginalPositionMap.erase(pPlayerCharacterStored);
            characterToOriginalStateMap.erase(pPlayerCharacterStored);

            characterToOriginalCharacterDirectionMap[pActualPlayerCharacter] = originalCharacterDirection;
            characterToOriginalFieldCharacterDirectionMap[pActualPlayerCharacter] = originalFieldCharacterDirection;
            characterToOriginalPositionMap[pActualPlayerCharacter] = originalPosition;
            characterToOriginalStateMap[pActualPlayerCharacter] = originalState;

            delete pPlayerCharacterStored;
            idToCharacterMap["PlayerCharacter"] = pActualPlayerCharacter;
        }

        pActualPlayerCharacter->SetPosition(characterToOriginalPositionMap[pActualPlayerCharacter]);
        pActualPlayerCharacter->SetDirection(characterToOriginalCharacterDirectionMap[pActualPlayerCharacter]);
        pActualPlayerCharacter->SetSpriteDirection(characterToOriginalFieldCharacterDirectionMap[pActualPlayerCharacter]);
    }

    if (idToCharacterMap.count("PartnerCharacter") > 0)
    {
        // We store a copy of the player character when compiling the case,
        // so we need to replace it with the actual player character.
        if (pActualPartnerCharacter == NULL || pActualPartnerCharacter != pPartnerCharacter)
        {
            FieldCharacter *pPartnerCharacterStored = idToCharacterMap["PartnerCharacter"];
            pActualPartnerCharacter = pPartnerCharacter;

            CharacterDirection originalCharacterDirection = characterToOriginalCharacterDirectionMap[pPartnerCharacterStored];
            FieldCharacterDirection originalFieldCharacterDirection = characterToOriginalFieldCharacterDirectionMap[pPartnerCharacterStored];
            Vector2 originalPosition = characterToOriginalPositionMap[pPartnerCharacterStored];
            FieldCharacterState originalState = characterToOriginalStateMap[pPartnerCharacterStored];

            characterToOriginalCharacterDirectionMap.erase(pPartnerCharacterStored);
            characterToOriginalFieldCharacterDirectionMap.erase(pPartnerCharacterStored);
            characterToOriginalPositionMap.erase(pPartnerCharacterStored);
            characterToOriginalStateMap.erase(pPartnerCharacterStored);

            characterToOriginalCharacterDirectionMap[pActualPartnerCharacter] = originalCharacterDirection;
            characterToOriginalFieldCharacterDirectionMap[pActualPartnerCharacter] = originalFieldCharacterDirection;
            characterToOriginalPositionMap[pActualPartnerCharacter] = originalPosition;
            characterToOriginalStateMap[pActualPartnerCharacter] = originalState;

            delete pPartnerCharacterStored;
            idToCharacterMap["PartnerCharacter"] = pActualPartnerCharacter;
        }

        pActualPartnerCharacter->SetPosition(characterToOriginalPositionMap[pActualPartnerCharacter]);
        pActualPartnerCharacter->SetDirection(characterToOriginalCharacterDirectionMap[pActualPartnerCharacter]);
        pActualPartnerCharacter->SetSpriteDirection(characterToOriginalFieldCharacterDirectionMap[pActualPartnerCharacter]);
    }

    for (map<string, FieldCharacter *>::iterator iter = idToCharacterMap.begin(); iter != idToCharacterMap.end(); ++iter)
    {
        iter->second->Begin();
    }

    StartNextPhase();
}

void FieldCutscene::UpdateCharacters(int delta, vector<HeightMap *> *pHeightMapList)
{
    vector<PositionalSound> soundsToPlayList;

    if (GetIsFinished() || (pCurrentPhase != NULL && pCurrentPhase->GetAllowsCharacterUpdates()))
    {
        for (map<string, FieldCharacter *>::iterator iter = idToCharacterMap.begin(); iter != idToCharacterMap.end(); ++iter)
        {
            iter->second->UpdateAnimation(delta);
            iter->second->SetExtraHeightFromHeightMaps(pHeightMapList);
            AnimationSound *pSoundToPlay = iter->second->GetSoundToPlay();

            if (pSoundToPlay != NULL)
            {
                soundsToPlayList.push_back(PositionalSound(pSoundToPlay, iter->second->GetMidPoint()));
            }
        }
    }

    for (unsigned int i = 0; i < soundsToPlayList.size(); i++)
    {
        AnimationSound *pSoundToPlay = soundsToPlayList[i].pSound;
        Vector2 soundOriginPosition = soundsToPlayList[i].location;

        double distanceToOrigin = (soundOriginPosition - currentCameraPosition).Length();
        double zeroDistance = max(gScreenWidth, gScreenHeight);//sqrt(gScreenWidth * gScreenWidth + gScreenHeight * gScreenHeight);
        double volume = max(0.0, 1.0 - distanceToOrigin / zeroDistance);

        pSoundToPlay->Play(volume);
    }
}

void FieldCutscene::UpdatePhase(int delta)
{
    if (GetIsFinished())
    {
        return;
    }

    if (pCurrentPhase == NULL || pCurrentPhase->GetIsFinished())
    {
        StartNextPhase();

        if (GetIsFinished())
        {
            return;
        }
    }

    pCurrentPhase->Update(delta);
}

void FieldCutscene::Draw(Vector2 offsetVector, list<ZOrderableObject *> *pObjectsFromLocation)
{
    if (pBackgroundSprite == NULL || backgroundSpriteOpacity < 1)
    {
        list<ZOrderableObject *> objectsInZOrder;

        for (list<ZOrderableObject *>::iterator iter = pObjectsFromLocation->begin(); iter != pObjectsFromLocation->end(); ++iter)
        {
            objectsInZOrder.push_back(*iter);
        }

        for (map<string, FieldCharacter *>::iterator iter = idToCharacterMap.begin(); iter != idToCharacterMap.end(); ++iter)
        {
            objectsInZOrder.push_back(iter->second);
        }

        objectsInZOrder.sort(CompareByZOrder);

        for (list<ZOrderableObject *>::iterator iter = objectsInZOrder.begin(); iter != objectsInZOrder.end(); ++iter)
        {
            (*iter)->Draw(offsetVector);
        }
    }

    if (pBackgroundSprite != NULL)
    {
        pBackgroundSprite->Draw(Vector2(0, 0), Color(backgroundSpriteOpacity, 1.0, 1.0, 1.0));
    }

    if (pReplacementBackgroundSprite != NULL)
    {
        pReplacementBackgroundSprite->Draw(Vector2(0, 0), Color(replacementBackgroundSpriteOpacity, 1.0, 1.0, 1.0));
    }

    if (GetIsFinished())
    {
        return;
    }

    if (pCurrentPhase != NULL)
    {
        pCurrentPhase->Draw();
    }
}

void FieldCutscene::Reset()
{
    pBackgroundSprite = NULL;
    pReplacementBackgroundSprite = NULL;
    hasBegun = false;
    currentPhaseId = 0;
    pCurrentPhase = NULL;

    for (map<string, FieldCharacter *>::iterator iter = idToCharacterMap.begin(); iter != idToCharacterMap.end(); ++iter)
    {
        iter->second->Reset(
            characterToOriginalCharacterDirectionMap[iter->second],
            characterToOriginalFieldCharacterDirectionMap[iter->second],
            characterToOriginalPositionMap[iter->second],
            characterToOriginalStateMap[iter->second]);
    }
}

void FieldCutscene::StartNextPhase()
{
    if (currentPhaseId < phaseList.size())
    {
        pCurrentPhase = phaseList[currentPhaseId];
        pCurrentPhase->Begin(this);

        if (pCurrentPhase->GetShouldUpdateImmediately())
        {
            pCurrentPhase->Update(0);
        }

        currentPhaseId++;
    }
    else
    {
        SetIsFinished(true);
        SetHasCompleted(true);
    }
}

FieldCutscene::FieldCutscenePhase * FieldCutscene::GetPhaseForNextElement(XmlReader *pReader)
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
        throw MLIException("Unknown phase type.");
    }
}

void FieldCutscene::FieldCutsceneConversation::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);
    pEncounter->Begin();
}

void FieldCutscene::FieldCutsceneConversation::Update(int delta)
{
    pEncounter->Update(delta);

    if (pEncounter->GetIsFinished())
    {
        if (pEncounter->GetTransitionRequest().GetNewAreaRequestedId().length() > 0)
        {
            pParentCutscene->SetTransitionRequest(pEncounter->GetTransitionRequest());
        }

        SetIsFinished(true);
    }
}

void FieldCutscene::FieldCutsceneConversation::Draw()
{
    pEncounter->Draw();
}

FieldCutscene::FieldCutsceneConversation::FieldCutsceneConversation(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneConversation");
    millisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");
    pEncounter = new Encounter(pReader);
    pReader->EndElement();
}

FieldCutscene::FieldCutsceneConcurrentMovements::~FieldCutsceneConcurrentMovements()
{
    for (unsigned int i = 0; i < movementList.size(); i++)
    {
        delete movementList[i];
        movementList[i] = NULL;
    }
}

void FieldCutscene::FieldCutsceneConcurrentMovements::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);

    for (unsigned int i = 0; i < movementList.size(); i++)
    {
        movementList[i]->Begin(pParentCutscene);
    }

    millisecondsElapsed = 0;
}

void FieldCutscene::FieldCutsceneConcurrentMovements::Update(int delta)
{
    bool allMovementsComplete = true;
    millisecondsElapsed += delta;

    for (unsigned int i = 0; i < movementList.size(); i++)
    {
        if (millisecondsElapsed >= movementList[i]->GetMillisecondDelayBeforeBegin() && !movementList[i]->GetIsFinished())
        {
            movementList[i]->Update(delta);
        }

        if (!movementList[i]->GetIsFinished())
        {
            allMovementsComplete = false;
        }
    }

    if (allMovementsComplete)
    {
        SetIsFinished(true);
    }
}

void FieldCutscene::FieldCutsceneConcurrentMovements::Draw()
{
    // Characters are drawn by the cutscene itself, so nothing to draw here.
}

FieldCutscene::FieldCutsceneConcurrentMovements::FieldCutsceneConcurrentMovements(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneConcurrentMovements");
    millisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");

    pReader->StartElement("MovementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        movementList.push_back(GetPhaseForNextElement(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

FieldCutscene::FieldCutscenePhase * FieldCutscene::FieldCutsceneConcurrentMovements::GetPhaseForNextElement(XmlReader *pReader)
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
        throw MLIException("Unknown phase type.");
    }
}

void FieldCutscene::FieldCutsceneMovement::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);
    pMovingCharacter = pParentCutscene->idToCharacterMap[characterId];
    normalizedMovementVector = (targetPosition - pMovingCharacter->GetPosition()).Normalize();
    lastPosition = pMovingCharacter->GetPosition();
}

void FieldCutscene::FieldCutsceneMovement::Update(int delta)
{
    Vector2 newPosition = lastPosition + (normalizedMovementVector * ((movementState == FieldCharacterStateWalking ? WalkingSpeed : RunningSpeed) * delta / 1000.0));
    pMovingCharacter->SetPosition(newPosition);
    pMovingCharacter->SetState(movementState);
    pMovingCharacter->UpdateDirection(normalizedMovementVector);
    lastPosition = newPosition;

    double angleBetween = acos((targetPosition - newPosition).Normalize() * normalizedMovementVector);

    // If the angle between the two normal vectors is greater than pi / 2,
    // then they're not pointing in the same direction, and we've gone past the target position.
    // We'll back up to the target position and flag that we're done.
    if (angleBetween > M_PI / 2)
    {
        pMovingCharacter->SetPosition(targetPosition);
        pMovingCharacter->SetState(FieldCharacterStateStanding);
        SetIsFinished(true);
    }
}

void FieldCutscene::FieldCutsceneMovement::Draw()
{
    // Characters are drawn by the cutscene itself, so nothing to draw here.
}

FieldCutscene::FieldCutsceneMovement::FieldCutsceneMovement(XmlReader *pReader)
{
    pMovingCharacter = NULL;

    pReader->StartElement("FieldCutsceneMovement");
    millisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");
    characterId = pReader->ReadTextElement("CharacterId");

    pReader->StartElement("TargetPosition");
    targetPosition = Vector2(pReader);
    pReader->EndElement();

    movementState = StringToFieldCharacterState(pReader->ReadTextElement("MovementState"));

    pReader->EndElement();
}

void FieldCutscene::FieldCutsceneOrient::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);
    pCharacter = pParentCutscene->idToCharacterMap[characterId];
}

void FieldCutscene::FieldCutsceneOrient::Update(int delta)
{
    if (!GetIsFinished())
    {
        pCharacter->SetDirection(direction);
        pCharacter->SetSpriteDirection(spriteDirection);
        SetIsFinished(true);
    }
}

void FieldCutscene::FieldCutsceneOrient::Draw()
{
    // Characters are drawn by the cutscene itself, so nothing to draw here.
}

FieldCutscene::FieldCutsceneOrient::FieldCutsceneOrient(XmlReader *pReader)
{
    pCharacter = NULL;

    pReader->StartElement("FieldCutsceneOrient");
    millisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");
    characterId = pReader->ReadTextElement("CharacterId");
    direction = StringToCharacterDirection(pReader->ReadTextElement("Direction"));
    spriteDirection = StringToFieldCharacterDirection(pReader->ReadTextElement("SpriteDirection"));
    pReader->EndElement();
}

FieldCutscene::FieldCutsceneSetBackground::~FieldCutsceneSetBackground()
{
    delete pBackgroundSpriteOpacityEase;
    pBackgroundSpriteOpacityEase = NULL;
}

void FieldCutscene::FieldCutsceneSetBackground::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);

    if (backgroundSpriteId.length() > 0)
    {
        pBackgroundSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(backgroundSpriteId);
        pParentCutscene->SetReplacementBackgroundSprite(pBackgroundSprite);
        pParentCutscene->SetReplacementBackgroundSpriteOpacity(0);
    }

    if (msFadeDuration > 0)
    {
        pBackgroundSpriteOpacityEase =
            new LinearEase(
                backgroundSpriteId.length() > 0 ? 0 : 1,
                backgroundSpriteId.length() > 0 ? 1 : 0,
                msFadeDuration);
        pBackgroundSpriteOpacityEase->Begin();
    }
}

void FieldCutscene::FieldCutsceneSetBackground::Update(int delta)
{
    if (pBackgroundSpriteOpacityEase != NULL && !pBackgroundSpriteOpacityEase->GetIsFinished())
    {
        pBackgroundSpriteOpacityEase->Update(delta);

        if (pBackgroundSpriteOpacityEase->GetIsFinished())
        {
            if (backgroundSpriteId.length() > 0)
            {
                pParentCutscene->SetBackgroundSprite(pBackgroundSprite);
                pParentCutscene->SetBackgroundSpriteOpacity(1);
                pParentCutscene->SetReplacementBackgroundSprite(NULL);
                pParentCutscene->SetReplacementBackgroundSpriteOpacity(0);
            }
            else
            {
                pParentCutscene->SetBackgroundSprite(NULL);
                pParentCutscene->SetBackgroundSpriteOpacity(0);
                pParentCutscene->SetReplacementBackgroundSprite(NULL);
                pParentCutscene->SetReplacementBackgroundSpriteOpacity(0);
            }

            SetIsFinished(true);
        }
        else
        {
            if (backgroundSpriteId.length() > 0)
            {
                pParentCutscene->SetReplacementBackgroundSpriteOpacity(pBackgroundSpriteOpacityEase->GetCurrentValue());
            }
            else
            {
                pParentCutscene->SetBackgroundSpriteOpacity(pBackgroundSpriteOpacityEase->GetCurrentValue());
            }
        }
    }
    else
    {
        if (backgroundSpriteId.length() > 0)
        {
            pParentCutscene->SetBackgroundSprite(pBackgroundSprite);
            pParentCutscene->SetBackgroundSpriteOpacity(1);
            pParentCutscene->SetReplacementBackgroundSprite(NULL);
            pParentCutscene->SetReplacementBackgroundSpriteOpacity(0);
        }
        else
        {
            pParentCutscene->SetBackgroundSprite(NULL);
            pParentCutscene->SetBackgroundSpriteOpacity(0);
            pParentCutscene->SetReplacementBackgroundSprite(NULL);
            pParentCutscene->SetReplacementBackgroundSpriteOpacity(0);
        }

        SetIsFinished(true);
    }
}

void FieldCutscene::FieldCutsceneSetBackground::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutsceneSetBackground::FieldCutsceneSetBackground(XmlReader *pReader)
{
    pBackgroundSprite = NULL;
    pBackgroundSpriteOpacityEase = NULL;

    pReader->StartElement("FieldCutsceneSetBackground");

    if (pReader->ElementExists("BackgroundSpriteId"))
    {
        backgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");
    }

    msFadeDuration = pReader->ReadIntElement("MsFadeDuration");
    pReader->EndElement();
}

FieldCutscene::FieldCutsceneCenterCamera::~FieldCutsceneCenterCamera()
{
    delete pCameraMovementXEase;
    pCameraMovementXEase = NULL;
    delete pCameraMovementYEase;
    pCameraMovementYEase = NULL;
}

void FieldCutscene::FieldCutsceneCenterCamera::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);

    if (characterId.length() > 0)
    {
        cameraCenterPosition = pParentCutscene->idToCharacterMap[characterId]->GetMidPoint();
    }

    delete pCameraMovementXEase;
    pCameraMovementXEase = new SCurveEase(pParentCutscene->GetCurrentCameraPosition().GetX(), cameraCenterPosition.GetX(), CenterCameraDuration);
    delete pCameraMovementYEase;
    pCameraMovementYEase = new SCurveEase(pParentCutscene->GetCurrentCameraPosition().GetY(), cameraCenterPosition.GetY(), CenterCameraDuration);

    pCameraMovementXEase->Begin();
    pCameraMovementYEase->Begin();
}

void FieldCutscene::FieldCutsceneCenterCamera::Update(int delta)
{
    pCameraMovementXEase->Update(delta);
    pCameraMovementYEase->Update(delta);

    pParentCutscene->currentCameraPosition = Vector2(pCameraMovementXEase->GetCurrentValue(), pCameraMovementYEase->GetCurrentValue());

    if (pCameraMovementXEase->GetIsFinished() && pCameraMovementYEase->GetIsFinished())
    {
        SetIsFinished(true);
    }
}

void FieldCutscene::FieldCutsceneCenterCamera::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutsceneCenterCamera::FieldCutsceneCenterCamera(XmlReader *pReader)
{
    pCameraMovementXEase = NULL;
    pCameraMovementYEase = NULL;

    pReader->StartElement("FieldCutsceneCenterCamera");

    millisecondDelayBeforeBegin = pReader->ReadIntElement("MillisecondDelayBeforeBegin");

    if (pReader->ElementExists("CharacterId"))
    {
        characterId = pReader->ReadTextElement("CharacterId");
    }
    else
    {
        pReader->StartElement("CameraCenterPosition");
        cameraCenterPosition = Vector2(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}

FieldCutscene::FieldCutscenePause::~FieldCutscenePause()
{

}

void FieldCutscene::FieldCutscenePause::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);

    msElapsedTime = 0;
}

void FieldCutscene::FieldCutscenePause::Update(int delta)
{
    msElapsedTime += delta;

    if (msElapsedTime >= msPauseDuration)
    {
        SetIsFinished(true);
    }
}

void FieldCutscene::FieldCutscenePause::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutscenePause::FieldCutscenePause(XmlReader *pReader)
{
    pReader->StartElement("FieldCutscenePause");
    msPauseDuration = pReader->ReadIntElement("MsPauseDuration");
    pReader->EndElement();
}

FieldCutscene::FieldCutscenePlayBgm::~FieldCutscenePlayBgm()
{

}

void FieldCutscene::FieldCutscenePlayBgm::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);
    Case::GetInstance()->GetAudioManager()->PlayBgmWithId(bgmId);
}

void FieldCutscene::FieldCutscenePlayBgm::Update(int delta)
{
    SetIsFinished(true);
}

void FieldCutscene::FieldCutscenePlayBgm::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutscenePlayBgm::FieldCutscenePlayBgm(XmlReader *pReader)
{
    pReader->StartElement("FieldCutscenePlayBgm");
    bgmId = pReader->ReadTextElement("BgmId");
    pReader->EndElement();
}

FieldCutscene::FieldCutsceneStopBgm::~FieldCutsceneStopBgm()
{

}

void FieldCutscene::FieldCutsceneStopBgm::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);
    Case::GetInstance()->GetAudioManager()->StopCurrentBgm(isInstant);
}

void FieldCutscene::FieldCutsceneStopBgm::Update(int delta)
{
    SetIsFinished(true);
}

void FieldCutscene::FieldCutsceneStopBgm::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutsceneStopBgm::FieldCutsceneStopBgm(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneStopBgm");
    isInstant = pReader->ReadBooleanElement("IsInstant");
    pReader->EndElement();
}

FieldCutscene::FieldCutscenePlayAmbiance::~FieldCutscenePlayAmbiance()
{

}

void FieldCutscene::FieldCutscenePlayAmbiance::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);
    Case::GetInstance()->GetAudioManager()->PlayAmbianceWithId(ambianceSfxId);
}

void FieldCutscene::FieldCutscenePlayAmbiance::Update(int delta)
{
    SetIsFinished(true);
}

void FieldCutscene::FieldCutscenePlayAmbiance::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutscenePlayAmbiance::FieldCutscenePlayAmbiance(XmlReader *pReader)
{
    pReader->StartElement("FieldCutscenePlayAmbiance");
    ambianceSfxId = pReader->ReadTextElement("AmbianceSfxId");
    pReader->EndElement();
}

FieldCutscene::FieldCutsceneStopAmbiance::~FieldCutsceneStopAmbiance()
{

}

void FieldCutscene::FieldCutsceneStopAmbiance::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);
    Case::GetInstance()->GetAudioManager()->StopCurrentAmbiance(isInstant);
}

void FieldCutscene::FieldCutsceneStopAmbiance::Update(int delta)
{
    SetIsFinished(true);
}

void FieldCutscene::FieldCutsceneStopAmbiance::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutsceneStopAmbiance::FieldCutsceneStopAmbiance(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneStopAmbiance");
    isInstant = pReader->ReadBooleanElement("IsInstant");
    pReader->EndElement();
}

FieldCutscene::FieldCutsceneEndCase::~FieldCutsceneEndCase()
{

}

void FieldCutscene::FieldCutsceneEndCase::Begin(FieldCutscene *pParentCutscene)
{
    FieldCutscenePhase::Begin(pParentCutscene);

    SaveDialogsSeenListForCase(Case::GetInstance()->GetUuid());
    Case::GetInstance()->SetIsFinished(true);

    if (completesCase)
    {
        SaveCompletedCase(Case::GetInstance()->GetUuid());
    }
}

void FieldCutscene::FieldCutsceneEndCase::Update(int delta)
{
    SetIsFinished(true);
}

void FieldCutscene::FieldCutsceneEndCase::Draw()
{
    // Nothing to draw - the scene itself is drawn by the cutscene.
}

FieldCutscene::FieldCutsceneEndCase::FieldCutsceneEndCase(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneEndCase");
    completesCase = pReader->ReadBooleanElement("CompletesCase");
    pReader->EndElement();
}
