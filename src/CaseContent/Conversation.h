/**
 * Basic header/include file for Conversation.cpp.
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

#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "Dialog.h"
#include "Notification.h"
#include "../Condition.h"
#include "../enums.h"
#include "../Events/DialogEventProvider.h"
#include "../Events/ButtonArrayEventProvider.h"
#include "../UserInterface/SkipArrow.h"
#include <map>
#include <vector>

using namespace std;

class State;
class ButtonArray;
class DialogCharacter;
class XmlReader;
class XmlWriter;

class Conversation
{
friend class ContentManager;

protected:
    class Action;
    class ContinuousAction;
    class ShowDialogAction;

public:
    class UnlockCondition
    {
    public:
        static UnlockCondition * LoadFromXml(XmlReader *pReader);

        UnlockCondition() { this->hasHandledMetCondition = false; }
        virtual ~UnlockCondition() { }

        virtual bool GetIsConditionMet() = 0;

        virtual bool GetHasHandledMetCondition() { return this->hasHandledMetCondition; }
        virtual void SetHasHandledMetCondition(bool hasHandledMetCondition) { this->hasHandledMetCondition = hasHandledMetCondition; };

        virtual bool Equals(UnlockCondition *pOtherCondition) = 0;
        virtual void SaveToXml(XmlWriter *pWriter) = 0;

        void SaveToXmlCore(XmlWriter *pWriter);
        void LoadFromXmlCore(XmlReader *pReader);

        virtual UnlockCondition * Clone() = 0;

    private:
        bool hasHandledMetCondition;
    };

    class FlagSetUnlockCondition : public UnlockCondition
    {
        friend class UnlockCondition;

    public:
        virtual ~FlagSetUnlockCondition() { }
        bool GetIsConditionMet();
        bool Equals(UnlockCondition *pOtherCondition);
        void SaveToXml(XmlWriter *pWriter);
        UnlockCondition * Clone();

    private:
        FlagSetUnlockCondition(string flagId);
        FlagSetUnlockCondition(XmlReader *pReader);

        string flagId;
    };

    class PartnerPresentUnlockCondition : public UnlockCondition
    {
        friend class UnlockCondition;

    public:
        virtual ~PartnerPresentUnlockCondition() { }
        bool GetIsConditionMet();
        bool Equals(UnlockCondition *pOtherCondition);
        void SaveToXml(XmlWriter *pWriter);
        UnlockCondition * Clone();

    private:
        PartnerPresentUnlockCondition(string partnerId);
        PartnerPresentUnlockCondition(XmlReader *pReader);

        string partnerId;
    };

    Conversation()
    {
        pCurrentContinuousAction = NULL;
        isEnabled = false;
        pLastContinuousAction = NULL;
        isLocked = false;
        hasBeenCompleted = false;
        wrongPartnerUsed = false;
        isFinished = false;
        pState = NULL;
        pSkipTab = NULL;
        pSkipArrow = NULL;
    }

    Conversation(XmlReader *pReader);
    virtual ~Conversation();

    string GetId() const { return this->id; }
    void SetId(string id) { this->id = id; }

    string GetName() const { return this->name; }
    void SetName(string name) { this->name = name; }

    vector<UnlockCondition *> * GetUnlockConditions() { return &this->unlockConditions; }

    string GetRequiredPartnerId() const { return this->requiredPartnerId; }
    void SetRequiredPartnerId(string requiredPartnerId) { this->requiredPartnerId = requiredPartnerId; }

    bool GetIsEnabled() const { return this->isEnabled; }
    void SetIsEnabled(bool isEnabled) { this->isEnabled = isEnabled; }

    int GetLockCount() const;
    bool GetIsLocked() const;
    bool GetHasBeenUnlocked() const;

    bool GetHasBeenCompleted() const { return this->hasBeenCompleted; }
    void SetHasBeenCompleted(bool hasBeenCompleted) { this->hasBeenCompleted = hasBeenCompleted; }

    bool GetWrongPartnerUsed() const { return this->wrongPartnerUsed; }
    void SetWrongPartnerUsed(bool wrongPartnerUsed) { this->wrongPartnerUsed = wrongPartnerUsed; }

    bool GetIsFinished() const { return this->isFinished; }
    void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

    virtual bool GetDisappearsAfterCompleted() { return false; }
    virtual bool GetShouldUpdateActions() { return true; }

    State * GetState() { return this->pState; }
    void SetState(State *pState) { this->pState = pState; }

    bool GetIsNotificationNext();

    virtual void Begin(State *pState);
    virtual void Update(int delta);
    virtual void Draw(double xOffset, double yOffset);
    virtual void DrawBackground(double xOffset, double yOffset);
    virtual void Reset();
    virtual void ResetTopics() { }

    static Conversation * LoadFromXml(XmlReader *pReader);

protected:
    void Initialize();
    void LoadFromXmlCore(XmlReader *pReader);
    virtual Action * GetActionForNextElement(XmlReader *pReader);

    vector<Action *> actionList;
    ContinuousAction *pCurrentContinuousAction;

    string id;
    string name;
    vector<UnlockCondition *> unlockConditions;
    string requiredPartnerId;
    bool isEnabled;
    State *pState;

private:
    static map<string, vector<Conversation::ShowDialogAction *> > showDialogActionListsById;

    ContinuousAction *pLastContinuousAction;
    string initialBgmId;
    string initialAmbianceSfxId;

    bool isLocked;
    bool hasBeenCompleted;
    bool wrongPartnerUsed;
    bool isFinished;

    Tab *pSkipTab;
    SkipArrow *pSkipArrow;

protected:
    class Action
    {
    public:
        Action() {}
        virtual ~Action() {}

        virtual bool GetIsSingleAction() = 0;
        virtual void ResetTopics() { }
    };

    class SingleAction : public Action
    {
    public:
        SingleAction() {}

        virtual bool GetIsSingleAction()
        {
            return true;
        }

        virtual void Execute(State *pState) = 0;
    };

    class ContinuousAction : public Action
    {
    public:
        ContinuousAction()
        {
            pState = NULL;
            isFinished = false;
        }

        virtual bool GetIsSingleAction()
        {
            return false;
        }

        bool GetIsFinished() const { return this->isFinished; }
        virtual void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

        virtual bool GetIsReadyToHide()
        {
            return true;
        }

        virtual bool GetShouldSkip()
        {
            return false;
        }

        virtual void GoToNext(State *pState);
        virtual void Begin(State *pState);
        virtual void Update(int delta) = 0;
        virtual void Draw(double xOffset, double yOffset) = 0;
        virtual void DrawBackground(double xOffset, double yOffset) {}
        virtual void Reset();

    protected:
        State *pState;
        bool isFinished;
    };

    class CharacterChangeAction;

    class MultipleCharacterChangeAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);
        virtual void Reset();

    private:
        MultipleCharacterChangeAction(XmlReader *pReader);
        ~MultipleCharacterChangeAction();

        vector<CharacterChangeAction *> characterChangeList;
    };

    class CharacterChangeAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);

    private:
        CharacterChangeAction(XmlReader *pReader);
        virtual ~CharacterChangeAction();
        void SwapCharacters();

        CharacterPosition position;
        string characterIdToChangeTo;
        string initialEmotionId;
        string sfxId;

        EasingFunction *pCharacterOutEase;
        EasingFunction *pCharacterInEase;
    };

    class SetFlagAction : public SingleAction
    {
        friend class Conversation;

    public:
        void Execute(State *pState);

    private:
        SetFlagAction(XmlReader *pReader);

        string flagId;
    };

    class BranchOnConditionAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetTrueIndex() const { return this->trueIndex; }
        void SetTrueIndex(int trueIndex) { this->trueIndex = trueIndex; }

        int GetFalseIndex() const { return this->falseIndex; }
        void SetFalseIndex(int falseIndex) { this->falseIndex = falseIndex; }

        virtual void Execute(State *pState);

    private:
        BranchOnConditionAction(XmlReader *pReader);
        ~BranchOnConditionAction();

        Condition *pCondition;

        // Action index to branch to if the condition is true.
        int trueIndex;

        // Action index to branch to if the condition is false.
        int falseIndex;
    };

    // Dummy placeholder to jump to when the tested condition is true.
    class BranchIfTrueAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetEndIndex() { return this->endIndex; }
        void SetEndIndex(int endIndex) { this->endIndex = endIndex; }

        virtual void Execute(State *pState);

    private:
        BranchIfTrueAction(XmlReader *pReader);

        // Action index to branch to representing the end of this branch statement.
        int endIndex;
    };

    // Dummy placeholder to jump to when the tested condition is false.
    class BranchIfFalseAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetEndIndex() { return this->endIndex; }
        void SetEndIndex(int endIndex) { this->endIndex = endIndex; }

        virtual void Execute(State *pState);

    private:
        BranchIfFalseAction(XmlReader *pReader);

        // Action index to branch to representing the end of this branch statement.
        int endIndex;
    };

    // Dummy placeholder to jump to when finished with a branch on condition action.
    class EndBranchOnConditionAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndBranchOnConditionAction(XmlReader *pReader);
    };

    class ShowDialogAction : public ContinuousAction, DialogEventListener
    {
        friend class Conversation;

    public:
        ShowDialogAction()
        {
            pDialog = NULL;

            id = "";
            hasBeenSeen = false;

            speakerPosition = CharacterPositionNone;

            totalMillisecondsSinceMouthChange = 0;
            isTalking = false;

            isInConfrontation = false;

            pLeftEarthquakeEase = NULL;
            pRightEarthquakeEase = NULL;
            pWindowXEarthquakeEase = NULL;
            pWindowYEarthquakeEase = NULL;
        }

        bool GetHasBeenSeen() { return this->hasBeenSeen; }
        void SetHasBeenSeen(bool hasBeenSeen) { this->hasBeenSeen = hasBeenSeen; }

        bool GetIsInConfrontation() { return this->isInConfrontation; }
        void SetIsInConfrontation(bool isInConfrontation) { this->isInConfrontation = isInConfrontation; }

        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);
        virtual void DrawBackground(double xOffset, double yOffset);
        virtual void Reset();

        virtual void OnDialogSpeakerEmotionChanged(Dialog *pSender, string newEmotionId);
        virtual void OnDialogOtherEmotionChanged(Dialog *pSender, string newEmotionId);
        virtual void OnDialogNextFrame(Dialog *pSender);
        virtual void OnDialogPlayerDamaged(Dialog *pSender);
        virtual void OnDialogOpponentDamaged(Dialog *pSender);
        virtual void OnDialogSpeakerStartedShaking(Dialog *pSender);
        virtual void OnDialogScreenStartedShaking(Dialog *pSender, double shakeIntensity);
        virtual void OnDialogSpeakerMouthStateChanged(Dialog *pSender, bool isMouthOn);
        virtual void OnDialogSpeakerZoomed(Dialog *pSender);
        virtual void OnDialogEndSpeakerZoomed(Dialog *pSender);
        virtual void OnDialogBreakdownBegun(Dialog *pSender);
        virtual void OnDialogBreakdownEnded(Dialog *pSender);

        virtual void OnDialogDirectlyNavigated(Dialog *pSender, DirectNavigationDirection direction) { }
        virtual void OnDialogPressForInfoClicked(Dialog *pSender) { }
        virtual void OnDialogUsePartner(Dialog *pSender) { }
        virtual void OnDialogEvidencePresented(Dialog *pSender, string evidenceId) { }
        virtual void OnDialogEndRequested(Dialog *pSender) { }

        virtual void SetIsFinished(bool isFinished);
        virtual bool GetIsReadyToHide();

    protected:
        virtual bool GetIsInterrogation()
        {
            return false;
        }

        virtual bool GetIsPassive()
        {
            return false;
        }

        virtual int GetDelayBeforeContinuing()
        {
            return -1;
        }

        virtual bool GetCanNavigateBack()
        {
            return false;
        }

        virtual bool GetCanNavigateForward()
        {
            return false;
        }

        virtual bool GetShouldPresentEvidenceAutomatically()
        {
            return false;
        }

        virtual bool GetCanStopPresentingEvidence()
        {
            return true;
        }

        virtual void LoadFromXmlCore(XmlReader *pReader);

        Dialog *pDialog;

    protected:
        virtual ~ShowDialogAction();

    private:
        ShowDialogAction(XmlReader *pReader);

        string id;
        bool hasBeenSeen;

        CharacterPosition speakerPosition;
        string characterId;
        string rawDialog;
        string filePath;
        int leadInTime;

        double totalMillisecondsSinceMouthChange;
        bool isTalking;

        bool isInConfrontation;

        EasingFunction *pLeftEarthquakeEase;
        EasingFunction *pRightEarthquakeEase;
        EasingFunction *pWindowXEarthquakeEase;
        EasingFunction *pWindowYEarthquakeEase;
    };

    class ShowDialogAutomaticAction : public ShowDialogAction
    {
        friend class Conversation;

    protected:
        virtual void Begin(State *pState);

        virtual int GetDelayBeforeContinuing()
        {
            return this->delayBeforeContinuing;
        }

    private:
        ShowDialogAutomaticAction(XmlReader *pReader);

        int delayBeforeContinuing;
    };

    class MustPresentEvidenceAction : public ShowDialogAction
    {
        friend class Conversation;

    public:
        int GetCorrectEvidencePresentedIndex() { return this->correctEvidencePresentedIndex; }
        void SetCorrectEvidencePresentedIndex(int correctEvidencePresentedIndex) { this->correctEvidencePresentedIndex = correctEvidencePresentedIndex; }

        int GetWrongEvidencePresentedIndex() { return this->wrongEvidencePresentedIndex; }
        void SetWrongEvidencePresentedIndex(int wrongEvidencePresentedIndex) { this->wrongEvidencePresentedIndex = wrongEvidencePresentedIndex; }

        int GetEndRequestedIndex() { return this->endRequestedIndex; }
        void SetEndRequestedIndex(int endRequestedIndex) { this->endRequestedIndex = endRequestedIndex; }

        virtual void Begin(State *pState);
        virtual void OnDialogEvidencePresented(Dialog *pSender, string evidenceId);
        virtual void OnDialogEndRequested(Dialog *pSender);
        virtual void Reset();

    protected:
        virtual bool GetShouldPresentEvidenceAutomatically()
        {
            return true;
        }

        virtual bool GetCanStopPresentingEvidence();
        virtual void LoadFromXmlCore(XmlReader *pReader);

    private:
        MustPresentEvidenceAction(XmlReader *pReader);

        vector<string> correctEvidenceIdList;

        int correctEvidencePresentedIndex;
        int wrongEvidencePresentedIndex;
        int endRequestedIndex;
    };

    class SkipWrongEvidencePresentedAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetAfterWrongEvidencePresentedIndex() { return this->afterWrongEvidencePresentedIndex; }
        void SetAfterWrongEvidencePresentedIndex(int afterWrongEvidencePresentedIndex) { this->afterWrongEvidencePresentedIndex = afterWrongEvidencePresentedIndex; }

        virtual void Execute(State *pState);

    private:
        SkipWrongEvidencePresentedAction(XmlReader *pReader);

        int afterWrongEvidencePresentedIndex;
    };

    class BeginWrongEvidencePresentedAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginWrongEvidencePresentedAction(XmlReader *pReader);
    };

    class EndWrongEvidencePresentedAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndWrongEvidencePresentedAction(XmlReader *pReader);
    };

    class SkipEndRequestedAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetAfterEndRequestedIndex() { return this->afterEndRequestedIndex; }
        void SetAfterEndRequestedIndex(int afterEndRequestedIndex) { this->afterEndRequestedIndex = afterEndRequestedIndex; }

        virtual void Execute(State *pState);

    private:
        SkipEndRequestedAction(XmlReader *pReader);

        int afterEndRequestedIndex;
    };

    class BeginEndRequestedAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginEndRequestedAction(XmlReader *pReader);
    };

    class EndEndRequestedAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndEndRequestedAction(XmlReader *pReader);
    };

    class EndMustPresentEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndMustPresentEvidenceAction(XmlReader *pReader);
    };

    class EnableConversationAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EnableConversationAction(XmlReader *pReader);

        string conversationId;
    };

    class EnableEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EnableEvidenceAction(XmlReader *pReader);

        string evidenceId;
    };

    class UpdateEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        UpdateEvidenceAction(XmlReader *pReader);

        string evidenceId;
        string newEvidenceId;
    };

    class DisableEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        DisableEvidenceAction(XmlReader *pReader);

        string evidenceId;
    };

    class EnableCutsceneAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EnableCutsceneAction(XmlReader *pReader);

        string cutsceneId;
    };

    class NotificationAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        string GetRawNotificationText() { return this->rawNotificationText; }
        void SetRawNotificationText(string rawNotificationText) { this->rawNotificationText = rawNotificationText; }

        string GetPartnerId() { return this->partnerId; }
        void SetPartnerId(string partnerId) { this->partnerId = partnerId; }

        string GetOldEvidenceId() { return this->oldEvidenceId; }
        void SetOldEvidenceId(string oldEvidenceId) { this->oldEvidenceId = oldEvidenceId; }

        string GetNewEvidenceId() { return this->newEvidenceId; }
        void SetNewEvidenceId(string newEvidenceId) { this->newEvidenceId = newEvidenceId; }

        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);
        virtual void Reset();
        NotificationAction(XmlReader *pReader);
        ~NotificationAction();

    private:
        Notification *pNotification;

        string rawNotificationText;
        string partnerId;
        string oldEvidenceId;
        string newEvidenceId;
    };

    class PlayBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        PlayBgmAction(XmlReader *pReader);

        string bgmId;
        bool preserveBgm;
    };

    class PauseBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        PauseBgmAction(XmlReader *pReader);
    };

    class ResumeBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        ResumeBgmAction(XmlReader *pReader);
    };

    class StopBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        StopBgmAction(XmlReader *pReader);

        bool isInstant;
        bool preserveBgm;
    };

    class PlayAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        PlayAmbianceAction(XmlReader *pReader);

        string ambianceSfxId;
        bool preserveAmbiance;
    };

    class PauseAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        PauseAmbianceAction(XmlReader *pReader);
    };

    class ResumeAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        ResumeAmbianceAction(XmlReader *pReader);
    };

    class StopAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        StopAmbianceAction(XmlReader *pReader);

        bool isInstant;
        bool preserveAmbiance;
    };

    class StartAnimationAction : public SingleAction
    {
        friend class Conversation;

    public:
        void Execute(State *pState);

    private:
        StartAnimationAction(XmlReader *pReader);

        string animationId;
    };

    class StopAnimationAction : public SingleAction
    {
        friend class Conversation;

    public:
        void Execute(State *pState);

    private:
        StopAnimationAction(XmlReader *pReader);
    };

    class BeginCheckPartnerAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetRightIndex() { return this->rightIndex; }
        void SetRightIndex(int rightIndex) { this->rightIndex = rightIndex; }

        int GetWrongIndex() { return this->wrongIndex; }
        void SetWrongIndex(int wrongIndex) { this->wrongIndex = wrongIndex; }

        virtual void Execute(State *pState);

    private:
        BeginCheckPartnerAction(XmlReader *pReader);

        string partnerId;

        // Action index to branch to if the right partner is present.
        int rightIndex;

        // Action index to branch to if the right partner is not present.
        int wrongIndex;
    };

    // Dummy placeholder to jump to when the right partner is present.
    class BranchIfRightAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetEndIndex() { return this->endIndex; }
        void SetEndIndex(int endIndex) { this->endIndex = endIndex; }

        virtual void Execute(State *pState);

    private:
        BranchIfRightAction(XmlReader *pReader);

        // Action index to branch to representing the end of this branch statement.
        int endIndex;
    };

    // Dummy placeholder to jump to when the right partner is not present.
    class BranchIfWrongAction : public SingleAction
    {
        friend class Conversation;

    public:
        int GetEndIndex() { return this->endIndex; }
        void SetEndIndex(int endIndex) { this->endIndex = endIndex; }

        virtual void Execute(State *pState);

    private:
        BranchIfWrongAction(XmlReader *pReader);

        // Action index to branch to representing the end of this branch statement.
        int endIndex;
    };

    // Dummy placeholder to jump to when finished with a check partner action.
    class EndCheckPartnerAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndCheckPartnerAction(XmlReader *pReader);
    };

    // Sets the current partner to the partner designated by the given ID.
    class SetPartnerAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        SetPartnerAction(XmlReader *pReader);

        string partnerId;
    };

    class GoToPresentWrongEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        GoToPresentWrongEvidenceAction(XmlReader *pReader);
    };

    class LockConversationAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual ~LockConversationAction();
        virtual void Execute(State *pState);

    private:
        LockConversationAction(XmlReader *pReader);

        UnlockCondition *pUnlockCondition;
    };

    class ExitEncounterAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        ExitEncounterAction(XmlReader *pReader);
    };

    class MoveToLocationAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        MoveToLocationAction(XmlReader *pReader);

        string newAreaId;
        string newLocationId;
        string transitionId;
    };

    class MoveToZoomedViewAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        MoveToZoomedViewAction(XmlReader *pReader);

        string zoomedViewId;
    };

    class EndCaseAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndCaseAction(XmlReader *pReader);

        bool completesCase;
    };

    class BeginMultipleChoiceAction : public ContinuousAction, public ButtonArrayEventListener
    {
        friend class Conversation;

    public:
        ~BeginMultipleChoiceAction();

        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);
        virtual void DrawBackground(double xOffset, double yOffset);
        virtual void Reset();

        void OnButtonArrayButtonClicked(ButtonArray *pSender, int id);
        void OnButtonArrayCanceled(ButtonArray *pSender);

        BeginMultipleChoiceAction(XmlReader *pReader);

    private:
        vector<string> optionTexts;
        vector<int> optionIndexes;

        ButtonArray *pButtonArray;
        bool shouldCloseButtonArray;

        Dialog *pStaticDialog;

        Image *pDarkeningImage;
        double darkeningImageOpacity;
        EasingFunction *pDarkeningImageEaseIn;
        EasingFunction *pDarkeningImageEaseOut;

        int nextIndex;
    };

    class BeginMultipleChoiceOptionAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginMultipleChoiceOptionAction(XmlReader *pReader);
    };

    class ExitMultipleChoiceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        ExitMultipleChoiceAction(XmlReader *pReader);

        int exitIndex;
    };

    class EndMultipleChoiceOptionAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndMultipleChoiceOptionAction(XmlReader *pReader);

        int startIndex;
    };

    class EndMultipleChoiceAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EndMultipleChoiceAction(XmlReader *pReader);
    };

    class EnableFastForwardAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        EnableFastForwardAction(XmlReader *pReader);
    };

    class DisableFastForwardAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual void Execute(State *pState);

    private:
        DisableFastForwardAction(XmlReader *pReader);
    };

    class BeginBreakdownAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);

    private:
        BeginBreakdownAction(XmlReader *pReader);

        CharacterPosition characterPosition;
    };

    class EndBreakdownAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);

    private:
        EndBreakdownAction(XmlReader *pReader);
    };
};

class Interrogation : public Conversation
{
    friend class Conversation;

public:
    Interrogation() {}
    Interrogation(XmlReader *pReader);

    virtual bool GetDisappearsAfterCompleted() { return true; }

protected:
    bool GetCanExit()
    {
        return true;
    }

    virtual Action * GetActionForNextElement(XmlReader *pReader);

    class BeginInterrogationRepeatAction : public SingleAction
    {
        friend class Interrogation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginInterrogationRepeatAction(XmlReader *pReader);
        ~BeginInterrogationRepeatAction();

        State *pCachedState;
    };

    class ShowInterrogationAction : public ShowDialogAction
    {
        friend class Interrogation;

    public:
        virtual bool GetShouldSkip();

        int GetPreviousInterrogationIndex() { return this->previousInterrogationIndex; }
        void SetPreviousInterrogationIndex(int previousInterrogationIndex) { this->previousInterrogationIndex = previousInterrogationIndex; }

        int GetNextInterrogationIndex() { return this->nextInterrogationIndex; }
        void SetNextInterrogationIndex(int nextInterrogationIndex) { this->nextInterrogationIndex = nextInterrogationIndex; }

        int GetInterrogationFinishIndex() { return this->interrogationFinishIndex; }
        void SetInterrogationFinishIndex(int interrogationFinishIndex) { this->interrogationFinishIndex = interrogationFinishIndex; }

        int GetPressForInfoIndex() { return this->pressForInfoIndex; }
        void SetPressForInfoIndex(int pressForInfoIndex) { this->pressForInfoIndex = pressForInfoIndex; }

        map<string, int> GetEvidencePresentedIndexes() { return this->evidencePresentedIndexes; }
        void SetEvidencePresentedIndexes(map<string, int> evidencePresentedIndexes) { this->evidencePresentedIndexes = evidencePresentedIndexes; }

        map<string, int> GetPartnerUsedIndexes() { return this->partnerUsedIndexes; }
        void SetPartnerUsedIndexes(map<string, int> partnerUsedIndexes) { this->partnerUsedIndexes = partnerUsedIndexes; }

        int GetWrongEvidencePresentedIndex() { return this->wrongEvidencePresentedIndex; }
        void SetWrongEvidencePresentedIndex(int wrongEvidencePresentedIndex) { this->wrongEvidencePresentedIndex = wrongEvidencePresentedIndex; }

        int GetWrongPartnerUsedIndex() { return this->wrongPartnerUsedIndex; }
        void SetWrongPartnerUsedIndex(int wrongPartnerUsedIndex) { this->wrongPartnerUsedIndex = wrongPartnerUsedIndex; }

        int GetEndRequestedIndex() { return this->endRequestedIndex; }
        void SetEndRequestedIndex(int endRequestedIndex) { this->endRequestedIndex = endRequestedIndex; }

        virtual void GoToNext(State *pState);
        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);
        virtual void Reset();

        virtual void OnDialogDirectlyNavigated(Dialog *pSender, DirectNavigationDirection direction);
        virtual void OnDialogPressForInfoClicked(Dialog *pSender);
        virtual void OnDialogEvidencePresented(Dialog *pSender, string evidenceId);
        virtual void OnDialogUsePartner(Dialog *pSender);
        virtual void OnDialogEndRequested(Dialog *pSender);

    protected:
        ShowInterrogationAction()
            : ShowDialogAction()
        {
            nextIndexSet = false;
            pCachedState = NULL;

            previousInterrogationIndex = 0;
            nextInterrogationIndex = 0;
            interrogationFinishIndex = 0;
            pressForInfoIndex = 0;
            wrongEvidencePresentedIndex = 0;
            wrongPartnerUsedIndex = 0;
            endRequestedIndex = 0;
        }

        virtual bool GetIsInterrogation()
        {
            return true;
        }

        virtual bool GetCanNavigateBack()
        {
            return this->GetPreviousInterrogationIndex() >= 0;
        }

        virtual bool GetCanNavigateForward()
        {
            return this->GetNextInterrogationIndex() >= 0;
        }

        virtual void LoadFromXmlCore(XmlReader *pReader);

        map<string, int> evidencePresentedIndexes;
        map<string, int> partnerUsedIndexes;

    private:
        ShowInterrogationAction(XmlReader *pReader);
        virtual ~ShowInterrogationAction();

        bool nextIndexSet;
        State *pCachedState;
        string conditionFlag;

        int previousInterrogationIndex;
        int nextInterrogationIndex;
        int interrogationFinishIndex;
        int pressForInfoIndex;
        int wrongEvidencePresentedIndex;
        int wrongPartnerUsedIndex;
        int endRequestedIndex;
    };

    class BeginPressForInfoAction : public SingleAction
    {
        friend class Interrogation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginPressForInfoAction(XmlReader *pReader);
    };

    class EndPressForInfoAction : public SingleAction
    {
        friend class Interrogation;

    public:
        int GetNextDialogIndex() { return this->nextDialogIndex; }
        void SetNextDialogIndex(int nextDialogIndex) { this->nextDialogIndex = nextDialogIndex; }

        virtual void Execute(State *pState);

    private:
        EndPressForInfoAction(XmlReader *pReader);

        int nextDialogIndex;
    };

    class BeginPresentEvidenceAction : public SingleAction
    {
        friend class Interrogation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginPresentEvidenceAction(XmlReader *pReader);
    };

    class EndPresentEvidenceAction : public SingleAction
    {
        friend class Interrogation;

    public:
        int GetNextDialogIndex() { return this->nextDialogIndex; }
        void SetNextDialogIndex(int nextDialogIndex) { this->nextDialogIndex = nextDialogIndex; }

        virtual void Execute(State *pState);

    private:
        EndPresentEvidenceAction(XmlReader *pReader);

        int nextDialogIndex;
    };

    class BeginUsePartnerAction : public SingleAction
    {
        friend class Interrogation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginUsePartnerAction(XmlReader *pReader);
    };

    class EndUsePartnerAction : public SingleAction
    {
        friend class Interrogation;

    public:
        int GetNextDialogIndex() { return this->nextDialogIndex; }
        void SetNextDialogIndex(int nextDialogIndex) { this->nextDialogIndex = nextDialogIndex; }

        virtual void Execute(State *pState);

    private:
        EndUsePartnerAction(XmlReader *pReader);

        int nextDialogIndex;
    };

    class EndShowInterrogationBranchesAction : public SingleAction
    {
        friend class Interrogation;

    public:
        virtual void Execute(State *pState);

    private:
        EndShowInterrogationBranchesAction(XmlReader *pReader);
    };

    class ExitInterrogationRepeatAction : public SingleAction
    {
        friend class Interrogation;

    public:
        int GetAfterEndInterrogationRepeatIndex() { return this->afterEndInterrogationRepeatIndex; }
        void SetAfterEndInterrogationRepeatIndex(int afterEndInterrogationRepeatIndex) { this->afterEndInterrogationRepeatIndex = afterEndInterrogationRepeatIndex; }

        virtual void Execute(State *pState);

    private:
        ExitInterrogationRepeatAction(XmlReader *pReader);

        int afterEndInterrogationRepeatIndex;
    };

    class SkipWrongPartnerUsedAction : public SingleAction
    {
        friend class Interrogation;

    public:
        int GetAfterWrongPartnerUsedIndex() { return this->afterWrongPartnerUsedIndex; }
        void SetAfterWrongPartnerUsedIndex(int afterWrongPartnerUsedIndex) { this->afterWrongPartnerUsedIndex = afterWrongPartnerUsedIndex; }

        virtual void Execute(State *pState);

    private:
        SkipWrongPartnerUsedAction(XmlReader *pReader);

        int afterWrongPartnerUsedIndex;
    };

    class BeginWrongPartnerUsedAction : public SingleAction
    {
        friend class Interrogation;

    public:
        virtual void Execute(State *pState);

    private:
        BeginWrongPartnerUsedAction(XmlReader *pReader);
    };

    class EndWrongPartnerUsedAction : public SingleAction
    {
        friend class Interrogation;

    public:
        void Execute(State *pState);

    private:
        EndWrongPartnerUsedAction(XmlReader *pReader);
    };

    class EndInterrogationRepeatAction : public SingleAction
    {
        friend class Interrogation;

    public:
        int GetBeginInterrogationRepeatIndex() { return this->beginInterrogationRepeatIndex; }
        void SetBeginInterrogationRepeatIndex(int beginInterrogationRepeatIndex) { this->beginInterrogationRepeatIndex = beginInterrogationRepeatIndex; }

        virtual void Execute(State *pState);

    private:
        EndInterrogationRepeatAction(XmlReader *pReader);

        int beginInterrogationRepeatIndex;
    };
};

class Confrontation : public Interrogation
{
    friend class Conversation;

protected:
    class BeginConfrontationTopicSelectionAction;
    class SkipPlayerDefeatedAction;

public:
    class Topic
    {
    public:
        Topic(string id, string name, int actionIndex);
        Topic(XmlReader *pReader);

        string GetId() { return this->id; }
        string GetName() { return this->name; }
        int GetActionIndex() { return this->actionIndex; }

        bool GetIsEnabled() { return this->isEnabled; }
        void SetIsEnabled(bool isEnabled) { this->isEnabled = isEnabled; }

        bool GetIsCompleted() { return this->isCompleted; }
        void SetIsCompleted(bool isCompleted) { this->isCompleted = isCompleted; }

    private:
        string id;
        string name;
        int actionIndex;
        bool isEnabled;
        bool isCompleted;
    };

    Confrontation();
    Confrontation(XmlReader *pReader);
    virtual ~Confrontation();

    string GetPlayerCharacterId() { return this->playerCharacterId; }
    void SetPlayerCharacterId(string playerCharacterId) { this->playerCharacterId = playerCharacterId; this->pPlayerCharacter = NULL; }

    DialogCharacter * GetPlayerCharacter();

    string GetOpponentCharacterId() { return this->opponentCharacterId; }
    void SetOpponentCharacterId(string opponentCharacterId) { this->opponentCharacterId = opponentCharacterId; this->pOpponentCharacter = NULL; }

    DialogCharacter * GetOpponentCharacter();

    string GetPlayerIconSpriteId() { return this->playerIconSpriteId; }
    void SetPlayerIconSpriteId(string playerIconSpriteId) { this->playerIconSpriteId = playerIconSpriteId; this->pPlayerIcon = NULL; }

    Vector2 GetPlayerIconOffset() { return this->playerIconOffset; }
    void SetPlayerIconOffset(Vector2 playerIconOffset) { this->playerIconOffset = playerIconOffset; }

    int GetInitialPlayerHealth() { return this->initialPlayerHealth; }
    void SetInitialPlayerHealth(int initialPlayerHealth) { this->initialPlayerHealth = initialPlayerHealth; this->playerHealth = initialPlayerHealth; }

    int GetPlayerHealth() { return this->playerHealth; }
    void SetPlayerHealth(int playerHealth) { this->playerHealth = playerHealth; }

    string GetOpponentIconSpriteId() { return this->opponentIconSpriteId; }
    void SetOpponentIconSpriteId(string opponentIconSpriteId) { this->opponentIconSpriteId = opponentIconSpriteId; this->pOpponentIcon = NULL; }

    Vector2 GetOpponentIconOffset() { return this->opponentIconOffset; }
    void SetOpponentIconOffset(Vector2 opponentIconOffset) { this->opponentIconOffset = opponentIconOffset; }

    int GetInitialOpponentHealth() { return this->initialOpponentHealth; }
    void SetInitialOpponentHealth(int initialOpponentHealth) { this->initialOpponentHealth = initialOpponentHealth; this->opponentHealth = initialOpponentHealth; }

    int GetOpponentHealth() { return this->opponentHealth; }
    void SetOpponentHealth(int opponentHealth) { this->opponentHealth = opponentHealth; }

    int GetPlayerDefeatedIndex() { return this->playerDefeatedIndex; }
    void SetPlayerDefeatedIndex(int playerDefeatedIndex) { this->playerDefeatedIndex = playerDefeatedIndex; }

    int GetConfrontationBeginIndex() { return this->confrontationBeginIndex; }
    void SetConfrontationBeginIndex(int confrontationBeginIndex) { this->confrontationBeginIndex = confrontationBeginIndex; }

    static void Initialize(Image *pBackgroundDarkeningImage, Image *pConfrontationHealthCircleImage, MLIFont *pConfrontationHealthNumberingFont);
    static Topic * GetConfrontationTopicFromId(string id);
    static int GetEnabledConfrontationTopicCount();

    virtual bool GetShouldUpdateActions();

    virtual void Begin(State *pState);
    virtual void Update(int delta);
    virtual void Draw(double xOffset, double yOffset);
    virtual void ResetTopics();

    void ShowHealthIcons();
    void HideHealthIcons();
    void DamagePlayer();
    void DamageOpponent();

    void CacheState();
    void Restart();

    string GetTopicNameById(string topicId);

protected:
    virtual bool GetCanExit()
    {
        return false;
    }

    virtual void LoadFromXmlCore(XmlReader *pReader);
    virtual Conversation::Action * GetActionForNextElement(XmlReader *pReader);

private:
    Sprite * GetPlayerIcon();
    Sprite * GetOpponentIcon();

    static Image *pBackgroundDarkeningImage;
    static Image *pConfrontationHealthCircleImage;
    static MLIFont *pConfrontationHealthNumberingFont;
    static map<string, Topic *> *pCurrentConfrontationTopicsById;

    map<string, string> confrontationTopicNamesById;
    BeginConfrontationTopicSelectionAction *pCurrentBeginConfrontationTopicSelectionAction;
    int currentBeginConfrontationTopicSelectionActionIndex;
    SkipPlayerDefeatedAction *pCurrentSkipPlayerDefeatedAction;

    Sprite *pPlayerIcon;
    Sprite *pOpponentIcon;

    EasingFunction *pIconOffsetEasingFunction;
    bool healthIconsShowing;
    int iconOffset;
    EasingFunction *pPlayerHealthNumberScaleEasingFunction;
    double playerHealthNumberScale;
    EasingFunction *pPlayerHealthNumberNotRedRgbValueEasingFunction;
    int playerHealthNumberNotRedRgbValue;
    EasingFunction *pOpponentHealthNumberScaleEasingFunction;
    double opponentHealthNumberScale;
    EasingFunction *pOpponentHealthNumberNotRedRgbValueEasingFunction;
    int opponentHealthNumberNotRedRgbValue;

    string playerCharacterId;
    string opponentCharacterId;

    DialogCharacter *pPlayerCharacter;
    DialogCharacter *pOpponentCharacter;

    string playerIconSpriteId;
    Vector2 playerIconOffset;
    int initialPlayerHealth;
    int playerHealth;
    string opponentIconSpriteId;
    Vector2 opponentIconOffset;
    int initialOpponentHealth;
    int opponentHealth;

    int playerDefeatedIndex;
    int confrontationBeginIndex;

    Sprite *pCachedPlayerIcon;
    Sprite *pCachedOpponentIcon;
    Vector2 cachedPlayerIconOffset;
    int cachedInitialPlayerHealth;
    Vector2 cachedOpponentIconOffset;
    int cachedInitialOpponentHealth;
    string cachedInitialBgm;
    string cachedInitialAmbiance;

    double backgroundDarkeningOpacity;
    EasingFunction *pBackgroundDarkeningOpacityEaseIn;
    EasingFunction *pBackgroundDarkeningOpacityEaseOut;

protected:
    class SetParticipantsAction : public SingleAction
    {
        friend class Confrontation;

    public:
        void Execute(State *pState);

    private:
        SetParticipantsAction(XmlReader *pReader);

        string playerCharacterId;
        string opponentCharacterId;
    };

    class SetIconOffsetAction : public SingleAction
    {
        friend class Confrontation;

    public:
        SetIconOffsetAction();
        void Execute(State *pState);

    private:
        SetIconOffsetAction(XmlReader *pReader);

        Vector2 offset;
        bool isPlayer;
    };

    class SetHealthAction : public SingleAction
    {
        friend class Confrontation;

    public:
        SetHealthAction();
        void Execute(State *pState);

    private:
        SetHealthAction(XmlReader *pReader);

        int initialHealth;
        bool isPlayer;
    };

    class InitializeBeginConfrontationTopicSelectionAction : public ContinuousAction
    {
        friend class Confrontation;

    public:
        InitializeBeginConfrontationTopicSelectionAction();
        virtual ~InitializeBeginConfrontationTopicSelectionAction();
        void Begin(State *pState);
        void Update(int delta);
        void Draw(double xOffset, double yOffset);

    private:
        InitializeBeginConfrontationTopicSelectionAction(XmlReader *pReader);

        Video *pConfrontationEntranceBackgroundVideo;
        Image *pConfrontationEntranceBlockSprite;
        Video *pConfrontationEntranceVfxVideo;

        EasingFunction *pCharacterEntranceEase;
        EasingFunction *pClipStartEase;
        EasingFunction *pBlockPositionEase;
        EasingFunction *pFlashOpacityEase;
        double characterEntrancePosition;
        double clipStartPosition;
        double blockPosition;
        double flashOpacity;
    };

    class BeginConfrontationTopicSelectionAction : public ContinuousAction, public ButtonArrayEventListener
    {
        friend class Confrontation;

    public:
        BeginConfrontationTopicSelectionAction();
        ~BeginConfrontationTopicSelectionAction();

        Topic * GetInitialTopic() { return this->pInitialTopic; }
        void SetInitialTopic(Topic *pInitialTopic) { this->pInitialTopic = pInitialTopic; }

        int GetPlayerDefeatedIndex() { return this->playerDefeatedIndex; }
        void SetPlayerDefeatedIndex(int playerDefeatedIndex) { this->playerDefeatedIndex = playerDefeatedIndex; }

        int GetEndActionIndex() { return this->endActionIndex; }
        void SetEndActionIndex(int endActionIndex) { this->endActionIndex = endActionIndex; }

        void Initialize();

        virtual void Begin(State *pState);
        virtual void Update(int delta);
        virtual void Draw(double xOffset, double yOffset);
        virtual void Reset();
        virtual void ResetTopics();

        void OnButtonArrayButtonClicked(ButtonArray *pSender, int id);
        void OnButtonArrayCanceled(ButtonArray *pSender);

    private:
        BeginConfrontationTopicSelectionAction(XmlReader *pReader);

        vector<Topic *> topicList;
        map<Topic *, bool> topicToInitialEnabledStateMap;
        Topic *pInitialTopic;
        bool initialTopicIsEnabled;

        ButtonArray *pTopicSelectionButtonArray;

        int playerDefeatedIndex;
        int endActionIndex;
        bool shouldCloseButtonArray;
    };

    class BeginConfrontationTopicAction : public SingleAction
    {
        friend class Confrontation;

    public:
        BeginConfrontationTopicAction();

        virtual void Execute(State *pState);

    private:
        BeginConfrontationTopicAction(XmlReader *pReader);
    };

    class EndConfrontationTopicAction : public SingleAction
    {
        friend class Confrontation;

    public:
        EndConfrontationTopicAction();

        int GetBeginConfrontationTopicSelectionActionIndex() { return this->beginConfrontationTopicSelectionActionIndex; }
        void SetBeginConfrontationTopicSelectionActionIndex(int beginConfrontationTopicSelectionActionIndex) { this->beginConfrontationTopicSelectionActionIndex = beginConfrontationTopicSelectionActionIndex; }

        virtual void Execute(State *pState);

    private:
        EndConfrontationTopicAction(XmlReader *pReader);

        int beginConfrontationTopicSelectionActionIndex;
    };

    class SkipPlayerDefeatedAction : public SingleAction
    {
        friend class Confrontation;

    public:
        SkipPlayerDefeatedAction();

        int GetSkipToIndex() { return this->skipToIndex; }
        void SetSkipToIndex(int skipToIndex) { this->skipToIndex = skipToIndex; }

        virtual void Execute(State *pState);

    private:
        SkipPlayerDefeatedAction(XmlReader *pReader);

        int skipToIndex;
    };

    class BeginPlayerDefeatedAction : public SingleAction
    {
        friend class Confrontation;

    public:
        BeginPlayerDefeatedAction();

        virtual void Execute(State *pState);

    private:
        BeginPlayerDefeatedAction(XmlReader *pReader);
    };

    class EndPlayerDefeatedAction : public SingleAction
    {
        friend class Confrontation;

    public:
        EndPlayerDefeatedAction();

        virtual void Execute(State *pState);

    private:
        EndPlayerDefeatedAction(XmlReader *pReader);
    };

    class EndConfrontationTopicSelectionAction : public SingleAction
    {
        friend class Confrontation;

    public:
        EndConfrontationTopicSelectionAction();

        virtual void Execute(State *pState);

    private:
        EndConfrontationTopicSelectionAction(XmlReader *pReader);
    };

    class EnableTopicAction : public SingleAction
    {
        friend class Confrontation;

    public:
        EnableTopicAction(string topicId);

        virtual void Execute(State *pState);

    private:
        EnableTopicAction(XmlReader *pReader);

        string topicId;
    };

    class RestartConfrontationAction : public SingleAction
    {
        friend class Confrontation;

    public:
        RestartConfrontationAction();

        virtual void Execute(State *pState);

    private:
        RestartConfrontationAction(XmlReader *pReader);
    };
};

#endif
