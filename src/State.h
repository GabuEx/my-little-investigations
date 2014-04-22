/**
 * Basic header/include file for State.cpp.
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

#ifndef STATE_H
#define STATE_H

#include "enums.h"
#include "TransitionRequest.h"
#include "CaseInformation/DialogCutsceneManager.h"
#include "CaseContent/Conversation.h"

class State
{
public:
    State()
        : pCachedState(NULL)
        , pCachedStateForConfrontationRestart(NULL)
        , cachedActionIndex(-1)
        , actionIndex(0)
        , previousActionIndex(0)
        , actionIndexSet(false)
        , speakerPosition(CharacterPositionNone)
        , leftCharacterId("")
        , leftCharacterEmotionId("")
        , isLeftCharacterTalking(false)
        , shouldLeftCharacterChangeMouth(false)
        , leftCharacterMouthOffCount(0)
        , pLeftCharacterXOffsetEasingFunction(NULL)
        , isLeftCharacterZoomed(false)
        , leftReplacementCharacterId("")
        , leftReplacementCharacterEmotionId("")
        , pLeftReplacementCharacterXOffsetEasingFunction(NULL)
        , rightCharacterId("")
        , rightCharacterEmotionId("")
        , isRightCharacterTalking(false)
        , shouldRightCharacterChangeMouth(false)
        , rightCharacterMouthOffCount(0)
        , pRightCharacterXOffsetEasingFunction(NULL)
        , isRightCharacterZoomed(false)
        , rightReplacementCharacterId("")
        , rightReplacementCharacterEmotionId("")
        , pRightReplacementCharacterXOffsetEasingFunction(NULL)
        , lastNavigationDirection(DirectNavigationDirectionNone)
        , endRequested(false)
        , exitRequested(false)
        , presentWrongEvidenceRequested(false)
        , preserveBgm(false)
        , preserveAmbiance(false)
        , pCurrentAnimation(NULL)
        , pCurrentConversation(NULL)
        , pCurrentConfrontation(NULL)
        , pCurrentConfrontationTopic(NULL)
        , pInterjectionSprite(NULL)
        , interjectionElapsedTime(0)
        , wasInterjectionOngoing(false)
        , pInterjectionSpriteInEasingFunction(NULL)
        , pInterjectionSpriteOutEasingFunction(NULL)
        , pInterjectionDarkeningOpacityInEasingFunction(NULL)
        , pInterjectionDarkeningOpacityOutEasingFunction(NULL)
        , actionIndexAfterInterjection(-1)
        , lastShowDialogSpeakerName("")
        , lastShowDialogRawDialogText("")
        , fastForwardEnabled(true)
        , canFastForward(false)
        , isFastForwarding(false)
        , isFinishingDialog(false)
        , currentSpeakerName("")
        , breakdownActivePosition(CharacterPositionNone)
        , breakdownTransitionStarted(false)
        , breakdownTransitionComplete(false)
        , pWindowMotionXEasingFunction(NULL)
        , pWindowMotionYEasingFunction(NULL)
    {
    }

    ~State()
    {
        delete pInterjectionSpriteInEasingFunction;
        delete pInterjectionSpriteOutEasingFunction;
        delete pInterjectionDarkeningOpacityInEasingFunction;
        delete pInterjectionDarkeningOpacityOutEasingFunction;

        delete pCachedState;
        delete pCachedStateForConfrontationRestart;
    }

    int GetActionIndex() const { return this->actionIndex; }
    void SetActionIndex(int actionIndex);

    int GetPreviousActionIndex() const { return this->previousActionIndex; }

    bool GetActionIndexSet() const { return this->actionIndexSet; }

    CharacterPosition GetSpeakerPosition() const { return this->speakerPosition; }
    void SetSpeakerPosition(CharacterPosition speakerPosition) { this->speakerPosition = speakerPosition; }

    string GetLeftCharacterId() const { return this->leftCharacterId; }
    void SetLeftCharacterId(const string &leftCharacterId) { this->leftCharacterId = leftCharacterId; }

    string GetLeftCharacterEmotionId() const { return this->leftCharacterEmotionId; }
    void SetLeftCharacterEmotionId(const string &leftCharacterEmotionId) { this->leftCharacterEmotionId = leftCharacterEmotionId; }

    bool GetIsLeftCharacterTalking() const { return this->isLeftCharacterTalking; }
    void SetIsLeftCharacterTalking(bool isLeftCharacterTalking) { this->isLeftCharacterTalking = isLeftCharacterTalking; }

    bool GetShouldLeftCharacterChangeMouth() const { return this->shouldLeftCharacterChangeMouth; }
    void SetShouldLeftCharacterChangeMouth(bool shouldLeftCharacterChangeMouth) { this->shouldLeftCharacterChangeMouth = shouldLeftCharacterChangeMouth; }

    int GetLeftCharacterMouthOffCount() const { return this->leftCharacterMouthOffCount; }
    void SetLeftCharacterMouthOffCount(int leftCharacterMouthOffCount) { this->leftCharacterMouthOffCount = leftCharacterMouthOffCount; }

    EasingFunction * GetLeftCharacterXOffsetEasingFunction() { return this->pLeftCharacterXOffsetEasingFunction; }
    void SetLeftCharacterXOffsetEasingFunction(EasingFunction *pLeftCharacterXOffsetEasingFunction) { this->pLeftCharacterXOffsetEasingFunction = pLeftCharacterXOffsetEasingFunction; }

    bool GetIsLeftCharacterZoomed() { return this->isLeftCharacterZoomed; }
    void SetIsLeftCharacterZoomed(bool isLeftCharacterZoomed) { this->isLeftCharacterZoomed = isLeftCharacterZoomed; }

    string GetLeftReplacementCharacterId() const { return this->leftReplacementCharacterId; }
    void SetLeftReplacementCharacterId(const string &leftReplacementCharacterId) { this->leftReplacementCharacterId = leftReplacementCharacterId; }

    string GetLeftReplacementCharacterEmotionId() const { return this->leftReplacementCharacterEmotionId; }
    void SetLeftReplacementCharacterEmotionId(const string &leftReplacementCharacterEmotionId) { this->leftReplacementCharacterEmotionId = leftReplacementCharacterEmotionId; }

    EasingFunction * GetLeftReplacementCharacterXOffsetEasingFunction() { return this->pLeftReplacementCharacterXOffsetEasingFunction; }
    void SetLeftReplacementCharacterXOffsetEasingFunction(EasingFunction *pLeftReplacementCharacterXOffsetEasingFunction) { this->pLeftReplacementCharacterXOffsetEasingFunction = pLeftReplacementCharacterXOffsetEasingFunction; }

    string GetRightCharacterId() const { return this->rightCharacterId; }
    void SetRightCharacterId(const string &rightCharacterId) { this->rightCharacterId = rightCharacterId; }

    string GetRightCharacterEmotionId() const { return this->rightCharacterEmotionId; }
    void SetRightCharacterEmotionId(const string &rightCharacterEmotionId) { this->rightCharacterEmotionId = rightCharacterEmotionId; }

    bool GetIsRightCharacterTalking() const { return this->isRightCharacterTalking; }
    void SetIsRightCharacterTalking(bool isRightCharacterTalking) { this->isRightCharacterTalking = isRightCharacterTalking; }

    bool GetShouldRightCharacterChangeMouth() const { return this->shouldRightCharacterChangeMouth; }
    void SetShouldRightCharacterChangeMouth(bool shouldRightCharacterChangeMouth) { this->shouldRightCharacterChangeMouth = shouldRightCharacterChangeMouth; }

    int GetRightCharacterMouthOffCount() const { return this->rightCharacterMouthOffCount; }
    void SetRightCharacterMouthOffCount(int rightCharacterMouthOffCount) { this->rightCharacterMouthOffCount = rightCharacterMouthOffCount; }

    EasingFunction * GetRightCharacterXOffsetEasingFunction() { return this->pRightCharacterXOffsetEasingFunction; }
    void SetRightCharacterXOffsetEasingFunction(EasingFunction *pRightCharacterXOffsetEasingFunction) { this->pRightCharacterXOffsetEasingFunction = pRightCharacterXOffsetEasingFunction; }

    bool GetIsRightCharacterZoomed() { return this->isRightCharacterZoomed; }
    void SetIsRightCharacterZoomed(bool isRightCharacterZoomed) { this->isRightCharacterZoomed = isRightCharacterZoomed; }

    string GetRightReplacementCharacterId() const { return this->rightReplacementCharacterId; }
    void SetRightReplacementCharacterId(const string &rightReplacementCharacterId) { this->rightReplacementCharacterId = rightReplacementCharacterId; }

    string GetRightReplacementCharacterEmotionId() const { return this->rightReplacementCharacterEmotionId; }
    void SetRightReplacementCharacterEmotionId(const string &rightReplacementCharacterEmotionId) { this->rightReplacementCharacterEmotionId = rightReplacementCharacterEmotionId; }

    EasingFunction * GetRightReplacementCharacterXOffsetEasingFunction() { return this->pRightReplacementCharacterXOffsetEasingFunction; }
    void SetRightReplacementCharacterXOffsetEasingFunction(EasingFunction *pRightReplacementCharacterXOffsetEasingFunction) { this->pRightReplacementCharacterXOffsetEasingFunction = pRightReplacementCharacterXOffsetEasingFunction; }

    DirectNavigationDirection GetLastNavigationDirection() const { return this->lastNavigationDirection; }
    void SetLastNavigationDirection(DirectNavigationDirection lastNavigationDirection) { this->lastNavigationDirection = lastNavigationDirection; }

    bool GetEndRequested() const { return this->endRequested; }
    void SetEndRequested(bool endRequested) { this->endRequested = endRequested; }

    bool GetExitRequested() const { return this->exitRequested; }
    void SetExitRequested(bool exitRequested) { this->exitRequested = exitRequested; }

    bool GetPresentWrongEvidenceRequested() const { return this->presentWrongEvidenceRequested; }
    void SetPresentWrongEvidenceRequested(bool presentWrongEvidenceRequested) { this->presentWrongEvidenceRequested = presentWrongEvidenceRequested; }

    bool GetPreserveBgm() const { return this->preserveBgm; }
    void SetPreserveBgm(bool preserveBgm) { this->preserveBgm = preserveBgm; }

    bool GetPreserveAmbiance() const { return this->preserveAmbiance; }
    void SetPreserveAmbiance(bool preserveAmbiance) { this->preserveAmbiance = preserveAmbiance; }

    DialogCutsceneAnimation * GetCurrentAnimation() { return this->pCurrentAnimation; }
    void SetCurrentAnimation(DialogCutsceneAnimation *pCurrentAnimation) { this->pCurrentAnimation = pCurrentAnimation; }

    Conversation * GetCurrentConversation() { return this->pCurrentConversation; }
    void SetCurrentConversation(Conversation *pCurrentConversation) { this->pCurrentConversation = pCurrentConversation; }

    Confrontation * GetCurrentConfrontation() { return this->pCurrentConfrontation; }
    void SetCurrentConfrontation(Confrontation *pCurrentConfrontation) { this->pCurrentConfrontation = pCurrentConfrontation; }

    Confrontation::Topic * GetCurrentConfrontationTopic() { return this->pCurrentConfrontationTopic; }
    void SetCurrentConfrontationTopic(Confrontation::Topic *pCurrentConfrontationTopic) { this->pCurrentConfrontationTopic = pCurrentConfrontationTopic; }

    TransitionRequest GetTransitionRequest() const { return this->transitionRequest; }
    void SetTransitionRequest(TransitionRequest transitionRequest) { this->transitionRequest = transitionRequest; }

    Sprite * GetInterjectionSprite() { return this->pInterjectionSprite; }
    void SetInterjectionSprite(Sprite *pInterjectionSprite) { this->pInterjectionSprite = pInterjectionSprite; }

    EasingFunction * GetInterjectionSpriteInEasingFunction() { return this->pInterjectionSpriteInEasingFunction; }
    void SetInterjectionSpriteInEasingFunction(EasingFunction *pInterjectionSpriteInEasingFunction) { this->pInterjectionSpriteInEasingFunction = pInterjectionSpriteInEasingFunction; }

    EasingFunction * GetInterjectionSpriteOutEasingFunction() { return this->pInterjectionSpriteOutEasingFunction; }
    void SetInterjectionSpriteOutEasingFunction(EasingFunction *pInterjectionSpriteOutEasingFunction) { this->pInterjectionSpriteOutEasingFunction = pInterjectionSpriteOutEasingFunction; }

    EasingFunction * GetInterjectionDarkeningOpacityInEasingFunction() { return this->pInterjectionDarkeningOpacityInEasingFunction; }
    void SetInterjectionDarkeningOpacityInEasingFunction(EasingFunction *pInterjectionDarkeningOpacityInEasingFunction) { this->pInterjectionDarkeningOpacityInEasingFunction = pInterjectionDarkeningOpacityInEasingFunction; }

    EasingFunction * GetInterjectionDarkeningOpacityOutEasingFunction() { return this->pInterjectionDarkeningOpacityOutEasingFunction; }
    void SetInterjectionDarkeningOpacityOutEasingFunction(EasingFunction *pInterjectionDarkeningOpacityOutEasingFunction) { this->pInterjectionDarkeningOpacityOutEasingFunction = pInterjectionDarkeningOpacityOutEasingFunction; }

    bool GetIsInterjectionOngoing() const { return this->pInterjectionSprite != NULL; }
    bool WasInterjectionOngoing() const { return this->wasInterjectionOngoing; }

    int GetActionIndexAfterInterjection() const { return this->actionIndexAfterInterjection; }
    void SetActionIndexAfterInterjection(int actionIndexAfterInterjection) { this->actionIndexAfterInterjection = actionIndexAfterInterjection; }

    string GetLastShowDialogSpeakerName() const { return this->lastShowDialogSpeakerName; }
    void SetLastShowDialogSpeakerName(const string &lastShowDialogSpeakerName) { this->lastShowDialogSpeakerName = lastShowDialogSpeakerName; }

    string GetLastShowDialogRawDialogText() const { return this->lastShowDialogRawDialogText; }
    void SetLastShowDialogRawDialogText(const string &lastShowDialogRawDialogText) { this->lastShowDialogRawDialogText = lastShowDialogRawDialogText; }

    bool GetFastForwardEnabled() { return this->fastForwardEnabled; }
    void SetFastForwardEnabled(bool fastForwardEnabled) { this->fastForwardEnabled = fastForwardEnabled; }

    bool GetCanFastForward() const { return this->canFastForward; }
    void SetCanFastForward(bool canFastForward) { this->canFastForward = canFastForward; }

    bool GetIsFastForwarding() const { return this->isFastForwarding; }
    void SetIsFastForwarding(bool isFastForwarding) { this->isFastForwarding = isFastForwarding; }

    bool GetIsFinishingDialog() const { return this->isFinishingDialog; }
    void SetIsFinishingDialog(bool isFinishingDialog) { this->isFinishingDialog = isFinishingDialog; }

    string GetCurrentSpeakerName() const { return this->currentSpeakerName; }
    void SetCurrentSpeakerName(const string &currentSpeakerName) { this->currentSpeakerName = currentSpeakerName; }

    CharacterPosition GetBreakdownActivePosition() { return this->breakdownActivePosition; }
    void SetBreakdownActivePosition(CharacterPosition breakdownActivePosition)
    {
        this->breakdownActivePosition = breakdownActivePosition;
        this->breakdownTransitionStarted = true;
        this->breakdownTransitionComplete = false;
    }

    bool GetBreakdownTransitionStarted() { return this->breakdownTransitionStarted; }
    void SetBreakdownTransitionStarted(bool breakdownTransitionStarted) { this->breakdownTransitionStarted = breakdownTransitionStarted; }

    bool GetBreakdownTransitionComplete() { return this->breakdownTransitionComplete; }
    void SetBreakdownTransitionComplete(bool breakdownTransitionComplete) { this->breakdownTransitionComplete = breakdownTransitionComplete; }

    EasingFunction * GetWindowMotionXEasingFunction() { return this->pWindowMotionXEasingFunction; }
    void SetWindowMotionXEasingFunction(EasingFunction *pWindowMotionXEasingFunction) { this->pWindowMotionXEasingFunction = pWindowMotionXEasingFunction; }

    EasingFunction * GetWindowMotionYEasingFunction() { return this->pWindowMotionYEasingFunction; }
    void SetWindowMotionYEasingFunction(EasingFunction *pWindowMotionYEasingFunction) { this->pWindowMotionYEasingFunction = pWindowMotionYEasingFunction; }

    void StartInterjection(Sprite *pInterjectionSprite);
    void StartInterjection(Sprite *pInterjectionSprite, bool isRightSide);
    void UpdateInterjection(int delta);
    void EndInterjection();

    void ClearActionIndexSet();
    State * CreateCachedState();
    void CacheState();
    void CacheStateForConfrontationRestart();
    void CachePreviousActionIndex();
    void RestoreFromState(State *pState);
    void RestoreCachedState();
    void DeleteCachedState();
    void RestoreCachedStateForConfrontationRestart();
    void RestoreActionIndexFromCache();
    void ClearCachedActionIndex();
    void ResetForConversation();

private:
    void SetActionIndexSet(bool actionIndexSet) { this->actionIndexSet = actionIndexSet; }

    State *pCachedState;
    State *pCachedStateForConfrontationRestart;
    int cachedActionIndex;
    int actionIndex;
    int previousActionIndex;

    bool actionIndexSet;
    CharacterPosition speakerPosition;
    string leftCharacterId;
    string leftCharacterEmotionId;
    bool isLeftCharacterTalking;
    bool shouldLeftCharacterChangeMouth;
    int leftCharacterMouthOffCount;
    EasingFunction *pLeftCharacterXOffsetEasingFunction;
    bool isLeftCharacterZoomed;
    string leftReplacementCharacterId;
    string leftReplacementCharacterEmotionId;
    EasingFunction *pLeftReplacementCharacterXOffsetEasingFunction;
    string rightCharacterId;
    string rightCharacterEmotionId;
    bool isRightCharacterTalking;
    bool shouldRightCharacterChangeMouth;
    int rightCharacterMouthOffCount;
    EasingFunction *pRightCharacterXOffsetEasingFunction;
    bool isRightCharacterZoomed;
    string rightReplacementCharacterId;
    string rightReplacementCharacterEmotionId;
    EasingFunction *pRightReplacementCharacterXOffsetEasingFunction;
    DirectNavigationDirection lastNavigationDirection;
    bool endRequested;
    bool exitRequested;
    bool presentWrongEvidenceRequested;
    bool preserveBgm;
    bool preserveAmbiance;
    DialogCutsceneAnimation *pCurrentAnimation;
    Conversation *pCurrentConversation;
    Confrontation *pCurrentConfrontation;
    Confrontation::Topic *pCurrentConfrontationTopic;
    TransitionRequest transitionRequest;
    Sprite *pInterjectionSprite;
    int interjectionElapsedTime;
    bool wasInterjectionOngoing;
    EasingFunction *pInterjectionSpriteInEasingFunction;
    EasingFunction *pInterjectionSpriteOutEasingFunction;
    EasingFunction *pInterjectionDarkeningOpacityInEasingFunction;
    EasingFunction *pInterjectionDarkeningOpacityOutEasingFunction;
    int actionIndexAfterInterjection;
    string lastShowDialogSpeakerName;
    string lastShowDialogRawDialogText;
    bool fastForwardEnabled;
    bool canFastForward;
    bool isFastForwarding;
    bool isFinishingDialog;
    string currentSpeakerName;
    CharacterPosition breakdownActivePosition;
    bool breakdownTransitionStarted;
    bool breakdownTransitionComplete;
    EasingFunction *pWindowMotionXEasingFunction;
    EasingFunction *pWindowMotionYEasingFunction;
};

#endif
