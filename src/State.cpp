/**
 * Holds everything we need to know about the current state of an encounter.
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

#include "State.h"
#include "globals.h"
#include "CaseInformation/Case.h"

const int InterjectionDuration = 750; // ms

void State::SetActionIndex(int actionIndex)
{
    this->previousActionIndex = this->actionIndex;
    this->actionIndex = actionIndex;
    this->SetActionIndexSet(true);
}

void State::StartInterjection(Sprite *pInterjectionSprite)
{
    StartInterjection(pInterjectionSprite, false /* isRightSide */);
}

void State::StartInterjection(Sprite *pInterjectionSprite, bool isRightSide)
{
    this->pInterjectionSprite = pInterjectionSprite;
    this->interjectionElapsedTime = 0;
    this->wasInterjectionOngoing = true;

    if (!isRightSide)
    {
        pInterjectionSpriteInEasingFunction = new QuadraticEase(-pInterjectionSprite->GetWidth(), (gScreenWidth - pInterjectionSprite->GetWidth()) / 2, 250);
        pInterjectionSpriteInEasingFunction->Begin();
        pInterjectionSpriteOutEasingFunction = new QuadraticEase((gScreenWidth - pInterjectionSprite->GetWidth()) / 2, gScreenWidth, 250);
    }

    pInterjectionDarkeningOpacityInEasingFunction = new LinearEase(0, 255, 250);
    pInterjectionDarkeningOpacityInEasingFunction->Begin();
    pInterjectionDarkeningOpacityOutEasingFunction = new LinearEase(255, 0, 250);
}

void State::UpdateInterjection(int delta)
{
    if (GetInterjectionSpriteOutEasingFunction()->GetIsStarted() && !GetInterjectionSpriteOutEasingFunction()->GetIsFinished())
    {
        GetInterjectionSpriteOutEasingFunction()->Update(delta);
    }
    else if (GetInterjectionSpriteInEasingFunction()->GetIsStarted() && !GetInterjectionSpriteInEasingFunction()->GetIsFinished())
    {
        GetInterjectionSpriteInEasingFunction()->Update(delta);
    }

    if (GetInterjectionDarkeningOpacityOutEasingFunction()->GetIsStarted() && !GetInterjectionDarkeningOpacityOutEasingFunction()->GetIsFinished())
    {
        GetInterjectionDarkeningOpacityOutEasingFunction()->Update(delta);
    }
    else if (GetInterjectionDarkeningOpacityInEasingFunction()->GetIsStarted() && !GetInterjectionDarkeningOpacityInEasingFunction()->GetIsFinished())
    {
        GetInterjectionDarkeningOpacityInEasingFunction()->Update(delta);
    }

    if (GetInterjectionSpriteInEasingFunction()->GetIsFinished() && !GetInterjectionSpriteOutEasingFunction()->GetIsStarted() &&
        GetInterjectionDarkeningOpacityInEasingFunction()->GetIsFinished() && !GetInterjectionDarkeningOpacityOutEasingFunction()->GetIsStarted())
    {
        if (interjectionElapsedTime < InterjectionDuration)
        {
            interjectionElapsedTime += delta;
        }

        if (interjectionElapsedTime >= InterjectionDuration)
        {
            GetInterjectionSpriteOutEasingFunction()->Begin();
            GetInterjectionDarkeningOpacityOutEasingFunction()->Begin();
        }
    }

    if (GetInterjectionSpriteOutEasingFunction()->GetIsFinished() && GetInterjectionDarkeningOpacityOutEasingFunction()->GetIsFinished())
    {
        EndInterjection();
    }
}

void State::EndInterjection()
{
    pInterjectionSprite = NULL;
    delete pInterjectionSpriteInEasingFunction;
    pInterjectionSpriteInEasingFunction = NULL;
    delete pInterjectionSpriteOutEasingFunction;
    pInterjectionSpriteOutEasingFunction = NULL;
    delete pInterjectionDarkeningOpacityInEasingFunction;
    pInterjectionDarkeningOpacityInEasingFunction = NULL;
    delete pInterjectionDarkeningOpacityOutEasingFunction;
    pInterjectionDarkeningOpacityOutEasingFunction = NULL;
}

void State::ClearActionIndexSet()
{
    SetActionIndexSet(false);
    wasInterjectionOngoing = false;
}

State * State::CreateCachedState()
{
    State *pState = new State();

    pState->SetLeftCharacterId(GetLeftCharacterId());
    pState->SetLeftCharacterEmotionId(GetLeftCharacterEmotionId());
    pState->SetIsLeftCharacterZoomed(GetIsLeftCharacterZoomed());
    pState->SetRightCharacterId(GetRightCharacterId());
    pState->SetRightCharacterEmotionId(GetRightCharacterEmotionId());
    pState->SetIsRightCharacterZoomed(GetIsRightCharacterZoomed());

    return pState;
}

void State::CacheState()
{
    delete pCachedState;
    pCachedState = CreateCachedState();
}

void State::CacheStateForConfrontationRestart()
{
    delete pCachedStateForConfrontationRestart;
    pCachedStateForConfrontationRestart = CreateCachedState();
    pCachedStateForConfrontationRestart->SetLeftCharacterEmotionId(Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(pCachedStateForConfrontationRestart->GetLeftCharacterId())->GetDefaultEmotionId());
    pCachedStateForConfrontationRestart->SetRightCharacterEmotionId(Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(pCachedStateForConfrontationRestart->GetRightCharacterId())->GetDefaultEmotionId());
}

void State::CachePreviousActionIndex()
{
    cachedActionIndex = GetPreviousActionIndex();
}

void State::RestoreFromState(State *pState)
{
    if (pState != NULL)
    {
        SetLeftCharacterId(pState->GetLeftCharacterId());
        SetLeftCharacterEmotionId(pState->GetLeftCharacterEmotionId());
        SetIsLeftCharacterZoomed(pState->GetIsLeftCharacterZoomed());
        SetRightCharacterId(pState->GetRightCharacterId());
        SetRightCharacterEmotionId(pState->GetRightCharacterEmotionId());
        SetIsRightCharacterZoomed(pState->GetIsRightCharacterZoomed());
    }
}

void State::RestoreCachedState()
{
    RestoreFromState(pCachedState);
    DeleteCachedState();
}

void State::DeleteCachedState()
{
    delete pCachedState;
    pCachedState = NULL;
}

void State::RestoreCachedStateForConfrontationRestart()
{
    RestoreFromState(pCachedStateForConfrontationRestart);
}

void State::RestoreActionIndexFromCache()
{
    if (cachedActionIndex >= 0)
    {
        SetActionIndex(cachedActionIndex);
    }

    cachedActionIndex = -1;
}

void State::ClearCachedActionIndex()
{
    cachedActionIndex = -1;
}

void State::ResetForConversation()
{
    SetActionIndex(0);
    SetSpeakerPosition(CharacterPositionNone);
    SetIsLeftCharacterTalking(false);
    SetShouldLeftCharacterChangeMouth(false);
    SetLeftCharacterMouthOffCount(0);
    SetIsLeftCharacterZoomed(false);
    SetIsRightCharacterTalking(false);
    SetShouldRightCharacterChangeMouth(false);
    SetRightCharacterMouthOffCount(0);
    SetIsRightCharacterZoomed(false);
    SetEndRequested(false);
    SetExitRequested(false);
    SetPresentWrongEvidenceRequested(false);
    SetPreserveBgm(false);
    SetTransitionRequest(TransitionRequest());
    SetFastForwardEnabled(true);
    SetCanFastForward(false);
    SetIsFastForwarding(false);
    SetIsFinishingDialog(false);
    SetCurrentSpeakerName("");
    SetBreakdownActivePosition(CharacterPositionNone);
    SetBreakdownTransitionStarted(false);
    SetBreakdownTransitionComplete(false);
    DeleteCachedState();
}
