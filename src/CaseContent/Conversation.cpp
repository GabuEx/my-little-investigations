/**
 * Handles a single conversation in the game.
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

#include "Conversation.h"
#include "../FileFunctions.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../globals.h"
#include "../State.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"
#include "../CaseInformation/Case.h"
#include "../CaseInformation/CommonCaseResources.h"
#include "../CaseInformation/DialogCharacterManager.h"
#include "../UserInterface/ButtonArray.h"
#include <cstdio>

const double MillisecondsBetweenMouthMovement = 100;

const int IconOffscreenOffset = 300;
const double HealthNumberDamageScale = 0.6;
const double ConfrontationEntranceAnimationDuration = 4536; // ms;
const int ConfrontationEntranceClipStartYPosition = 415;
const int ConfrontationEntranceCharacterYPosition = 57;
const int ConfrontationEntranceCharacterHeight = 245;

map<string, vector<Conversation::ShowDialogAction *> > Conversation::showDialogActionListsById;

Image *Confrontation::pBackgroundDarkeningImage = NULL;
Image *Confrontation::pConfrontationHealthCircleImage = NULL;
MLIFont *Confrontation::pConfrontationHealthNumberingFont = NULL;
map<string, Confrontation::Topic *> *Confrontation::pCurrentConfrontationTopicsById = NULL;

Conversation::UnlockCondition * Conversation::UnlockCondition::LoadFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("FlagSetUnlockCondition"))
    {
        return new Conversation::FlagSetUnlockCondition(pReader);
    }
    else if (pReader->ElementExists("PartnerPresentUnlockCondition"))
    {
        return new Conversation::PartnerPresentUnlockCondition(pReader);
    }
    else
    {
        throw MLIException("Unknown unlock condition type.");
    }
}

void Conversation::UnlockCondition::SaveToXmlCore(XmlWriter *pWriter)
{
    pWriter->WriteBooleanElement("HasHandledMetCondition", hasHandledMetCondition);
}

void Conversation::UnlockCondition::LoadFromXmlCore(XmlReader *pReader)
{
    if (pReader->ElementExists("HasHandledMetCondition"))
    {
        hasHandledMetCondition = pReader->ReadBooleanElement("HasHandledMetCondition");
    }
    else
    {
        hasHandledMetCondition = false;
    }
}

bool Conversation::FlagSetUnlockCondition::GetIsConditionMet()
{
    return Case::GetInstance()->GetFlagManager()->IsFlagSet(flagId);
}

bool Conversation::FlagSetUnlockCondition::Equals(UnlockCondition *pOtherCondition)
{
    Conversation::FlagSetUnlockCondition *pOtherFlagSetUnlockCondition = dynamic_cast<Conversation::FlagSetUnlockCondition *>(pOtherCondition);

    if (pOtherFlagSetUnlockCondition == NULL)
    {
        return false;
    }

    return this->flagId == pOtherFlagSetUnlockCondition->flagId;
}

void Conversation::FlagSetUnlockCondition::SaveToXml(XmlWriter *pWriter)
{
    pWriter->StartElement("FlagSetUnlockCondition");

    SaveToXmlCore(pWriter);

    pWriter->WriteTextElement("FlagId", flagId);
    pWriter->EndElement();
}

Conversation::UnlockCondition * Conversation::FlagSetUnlockCondition::Clone()
{
    return new Conversation::FlagSetUnlockCondition(flagId);
}

Conversation::FlagSetUnlockCondition::FlagSetUnlockCondition(const string &flagId)
{
    this->flagId = flagId;
}

Conversation::FlagSetUnlockCondition::FlagSetUnlockCondition(XmlReader *pReader)
{
    pReader->StartElement("FlagSetUnlockCondition");

    LoadFromXmlCore(pReader);

    flagId = pReader->ReadTextElement("FlagId");
    pReader->EndElement();
}

bool Conversation::PartnerPresentUnlockCondition::GetIsConditionMet()
{
    return Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId() == partnerId;
}

bool Conversation::PartnerPresentUnlockCondition::Equals(UnlockCondition *pOtherCondition)
{
    Conversation::PartnerPresentUnlockCondition *pOtherPartnerPresentUnlockCondition = dynamic_cast<Conversation::PartnerPresentUnlockCondition *>(pOtherCondition);

    if (pOtherPartnerPresentUnlockCondition == NULL)
    {
        return false;
    }

    return this->partnerId == pOtherPartnerPresentUnlockCondition->partnerId;
}

void Conversation::PartnerPresentUnlockCondition::SaveToXml(XmlWriter *pWriter)
{
    pWriter->StartElement("PartnerPresentUnlockCondition");

    SaveToXmlCore(pWriter);

    pWriter->WriteTextElement("PartnerId", partnerId);
    pWriter->EndElement();
}

Conversation::UnlockCondition * Conversation::PartnerPresentUnlockCondition::Clone()
{
    return new Conversation::PartnerPresentUnlockCondition(partnerId);
}

Conversation::PartnerPresentUnlockCondition::PartnerPresentUnlockCondition(const string &partnerId)
{
    this->partnerId = partnerId;
}

Conversation::PartnerPresentUnlockCondition::PartnerPresentUnlockCondition(XmlReader *pReader)
{
    pReader->StartElement("PartnerPresentUnlockCondition");

    LoadFromXmlCore(pReader);

    partnerId = pReader->ReadTextElement("PartnerId");
    pReader->EndElement();
}

Conversation::Conversation(XmlReader *pReader)
{
    Initialize();
    pReader->StartElement("Conversation");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Conversation::~Conversation()
{
    for (unsigned int i = 0; i < actionList.size(); i++)
    {
        delete actionList[i];
    }

    for (unsigned int i = 0; i < unlockConditions.size(); i++)
    {
        delete unlockConditions[i];
    }

    delete pSkipTab;
    pSkipTab = NULL;
    delete pSkipArrow;
    pSkipArrow = NULL;
}

int Conversation::GetLockCount() const
{
    int lockCount = 0;

    for (unsigned int i = 0; i < unlockConditions.size(); i++)
    {
        if (!unlockConditions[i]->GetIsConditionMet())
        {
            lockCount++;
        }
    }

    return lockCount;
}

bool Conversation::GetIsLocked() const
{
    return !unlockConditions.empty();
}

bool Conversation::GetHasBeenUnlocked() const
{
    return GetIsLocked() && GetLockCount() == 0;
}

bool Conversation::GetIsNotificationNext()
{
    return
        (int)actionList.size() > pState->GetActionIndex() + 1 &&
        dynamic_cast<NotificationAction *>(actionList[pState->GetActionIndex() + 1]) != NULL;
}

void Conversation::Begin(State *pState)
{
    this->Reset();
    this->pState = pState;
    this->initialBgmId = getPlayingMusic();
    this->initialAmbianceSfxId = getPlayingAmbiance();

    this->SetWrongPartnerUsed(
        this->GetRequiredPartnerId().length() > 0 &&
        this->GetRequiredPartnerId() != Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId());

    pState->SetCurrentConversation(this);
    pState->SetCurrentConfrontation(NULL);

    pSkipTab->Reset();
    pSkipTab->SetText(pgLocalizableContent->GetText("Conversation/FastForwardText"));
    pSkipArrow->Reset();

    Case::GetInstance()->GetAudioManager()->LowerMusicVolumeForDialog();
    Case::GetInstance()->GetAudioManager()->LowerAmbianceVolumeForDialog();
}

void Conversation::Update(int delta)
{
    if (GetIsFinished())
    {
        if (pLastContinuousAction != NULL)
        {
            pLastContinuousAction->Update(delta);
        }

        return;
    }

    if (!GetShouldUpdateActions())
    {
        return;
    }

    if (pState->GetFastForwardEnabled() && pState->GetCanFastForward())
    {
        pSkipTab->Update();

        if (pSkipTab->GetIsClicked())
        {
            pState->SetIsFastForwarding(!pState->GetIsFastForwarding());

            if (pState->GetIsFastForwarding())
            {
                pSkipTab->SetText(pgLocalizableContent->GetText("Conversation/StopText"));
            }
            else
            {
                pSkipTab->SetText(pgLocalizableContent->GetText("Conversation/FastForwardText"));
            }
        }
    }
    else
    {
        pState->SetIsFastForwarding(false);
        pSkipTab->Reset();
        pSkipTab->SetText(pgLocalizableContent->GetText("Conversation/FastForwardText"));
    }

    if (pState->GetIsFastForwarding())
    {
        pSkipArrow->Update(delta);
    }
    else
    {
        pSkipArrow->Reset();
    }

    if (pCurrentContinuousAction == NULL || (pCurrentContinuousAction->GetIsFinished() && pCurrentContinuousAction->GetIsReadyToHide()))
    {
        pState->SetIsFinishingDialog(false);

        if (pCurrentContinuousAction != NULL && pCurrentContinuousAction->GetIsFinished())
        {
            pLastContinuousAction = pCurrentContinuousAction;
            pCurrentContinuousAction = NULL;

            if (!pState->GetActionIndexSet())
            {
                pState->SetActionIndex(pState->GetActionIndex() + 1);
            }

            pState->ClearActionIndexSet();
        }

        while (pState->GetActionIndex() < (int)actionList.size())
        {
            Action *pCurrentAction = actionList[pState->GetActionIndex()];

            if (pCurrentAction->GetIsSingleAction())
            {
                SingleAction *pSingleAction = dynamic_cast<SingleAction *>(pCurrentAction);
                pSingleAction->Execute(pState);
            }
            else
            {
                pCurrentContinuousAction = dynamic_cast<ContinuousAction *>(pCurrentAction);

                if (pCurrentContinuousAction->GetShouldSkip())
                {
                    pCurrentContinuousAction->GoToNext(pState);
                }
                else
                {
                    if (pLastContinuousAction != NULL)
                    {
                        pLastContinuousAction->Reset();
                    }

                    pCurrentContinuousAction->Begin(pState);

                    if (!pCurrentContinuousAction->GetIsFinished())
                    {
                        break;
                    }
                }
            }

            if (pState->GetEndRequested())
            {
                SetIsFinished(true);
                Case::GetInstance()->GetAudioManager()->PlayBgmWithId(initialBgmId);
                Case::GetInstance()->GetAudioManager()->PlayAmbianceWithId(initialAmbianceSfxId);
                return;
            }

            if (!GetShouldUpdateActions())
            {
                return;
            }

            if (!pState->GetActionIndexSet())
            {
                pState->SetActionIndex(pState->GetActionIndex() + 1);
            }

            pState->ClearActionIndexSet();
        }
    }

    if (pState->GetActionIndex() == (int)actionList.size() || pState->GetEndRequested())
    {
        if (pState->GetEndRequested())
        {
            pState->SetEndRequested(false);
        }
        else if (!GetWrongPartnerUsed() && GetLockCount() == 0)
        {
            SetHasBeenCompleted(true);
        }

        if (!pState->GetPreserveBgm())
        {
            Case::GetInstance()->GetAudioManager()->PlayBgmWithId(initialBgmId);
        }

        if (!pState->GetPreserveAmbiance())
        {
            Case::GetInstance()->GetAudioManager()->PlayAmbianceWithId(initialAmbianceSfxId);
        }

        if (pCurrentContinuousAction != NULL)
        {
            pCurrentContinuousAction->Reset();
        }

        SetIsFinished(true);
    }
    else if (pCurrentContinuousAction != NULL)
    {
        pCurrentContinuousAction->Update(delta);
    }
}

void Conversation::Draw(double xOffset, double yOffset)
{
    if (GetIsFinished())
    {
        if (pLastContinuousAction != NULL)
        {
            pLastContinuousAction->Draw(xOffset, yOffset);
        }

        return;
    }

    if (pCurrentContinuousAction != NULL)
    {
        pCurrentContinuousAction->Draw(xOffset, yOffset);
    }

    if (pState->GetFastForwardEnabled() && pState->GetCanFastForward())
    {
        pSkipTab->Draw();
    }

    if (pState->GetIsFastForwarding())
    {
        pSkipArrow->Draw();
    }
}

void Conversation::DrawBackground(double xOffset, double yOffset)
{
    if (GetIsFinished())
    {
        if (pLastContinuousAction != NULL)
        {
            pLastContinuousAction->DrawBackground(xOffset, yOffset);
        }

        return;
    }

    if (pCurrentContinuousAction != NULL)
    {
        pCurrentContinuousAction->DrawBackground(xOffset, yOffset);
    }
}

void Conversation::Reset()
{
    if (pLastContinuousAction != NULL)
    {
        pLastContinuousAction->Reset();
    }

    pCurrentContinuousAction = NULL;
    SetIsFinished(false);
}

void Conversation::ResetState()
{
    if (pState != NULL)
    {
        pState->ResetForConversation();
        pState = NULL;
    }
}

Conversation * Conversation::LoadFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("Conversation"))
    {
        return new Conversation(pReader);
    }
    else if (pReader->ElementExists("Interrogation"))
    {
        return new Interrogation(pReader);
    }
    else if (pReader->ElementExists("Confrontation"))
    {
        return new Confrontation(pReader);
    }
    else
    {
        throw MLIException("Invalid conversation type.");
    }
}

void Conversation::Initialize()
{
    pState = NULL;
    isEnabled = false;
    SetHasBeenCompleted(false);

    isLocked = false;
    wrongPartnerUsed = false;
    isFinished = false;
}

void Conversation::LoadFromXmlCore(XmlReader *pReader)
{
    pCurrentContinuousAction = NULL;
    pLastContinuousAction = NULL;
    pSkipTab = new Tab(gScreenWidth - (TabWidth + 7), true /* isClickable */, pgLocalizableContent->GetText("Conversation/FastForwardText"), false /* useCancelClickSoundEffect */, TabRowTop);
    pSkipArrow = new SkipArrow(789, 8 + TabHeight, 10, false /* isClickable */, true /* isFFwd */);

    if (pReader->ElementExists("Id"))
    {
        id = pReader->ReadTextElement("Id");
    }

    if (pReader->ElementExists("Name"))
    {
        name = pReader->ReadTextElement("Name");
    }

    if (pReader->ElementExists("RequiredPartnerId"))
    {
        requiredPartnerId = pReader->ReadTextElement("RequiredPartnerId");
    }

    isEnabled = pReader->ReadBooleanElement("IsEnabled");
    hasBeenCompleted = pReader->ReadBooleanElement("HasBeenCompleted");

    pReader->StartElement("ActionList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        actionList.push_back(GetActionForNextElement(pReader));
    }

    pReader->EndElement();
}

Conversation::Action * Conversation::GetActionForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("MultipleCharacterChangeAction"))
    {
        return new MultipleCharacterChangeAction(pReader);
    }
    else if (pReader->ElementExists("CharacterChangeAction"))
    {
        return new CharacterChangeAction(pReader);
    }
    else if (pReader->ElementExists("SetFlagAction"))
    {
        return new SetFlagAction(pReader);
    }
    else if (pReader->ElementExists("BranchOnConditionAction"))
    {
        return new BranchOnConditionAction(pReader);
    }
    else if (pReader->ElementExists("BranchIfTrueAction"))
    {
        return new BranchIfTrueAction(pReader);
    }
    else if (pReader->ElementExists("BranchIfFalseAction"))
    {
        return new BranchIfFalseAction(pReader);
    }
    else if (pReader->ElementExists("EndBranchOnConditionAction"))
    {
        return new EndBranchOnConditionAction(pReader);
    }
    else if (pReader->ElementExists("ShowDialogAction"))
    {
        return new ShowDialogAction(pReader);
    }
    else if (pReader->ElementExists("ShowDialogAutomaticAction"))
    {
        return new ShowDialogAutomaticAction(pReader);
    }
    else if (pReader->ElementExists("MustPresentEvidenceAction"))
    {
        return new MustPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("SkipWrongEvidencePresentedAction"))
    {
        return new SkipWrongEvidencePresentedAction(pReader);
    }
    else if (pReader->ElementExists("BeginWrongEvidencePresentedAction"))
    {
        return new BeginWrongEvidencePresentedAction(pReader);
    }
    else if (pReader->ElementExists("EndWrongEvidencePresentedAction"))
    {
        return new EndWrongEvidencePresentedAction(pReader);
    }
    else if (pReader->ElementExists("SkipEndRequestedAction"))
    {
        return new SkipEndRequestedAction(pReader);
    }
    else if (pReader->ElementExists("BeginEndRequestedAction"))
    {
        return new BeginEndRequestedAction(pReader);
    }
    else if (pReader->ElementExists("EndEndRequestedAction"))
    {
        return new EndEndRequestedAction(pReader);
    }
    else if (pReader->ElementExists("EndMustPresentEvidenceAction"))
    {
        return new EndMustPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("EnableConversationAction"))
    {
        return new EnableConversationAction(pReader);
    }
    else if (pReader->ElementExists("EnableEvidenceAction"))
    {
        return new EnableEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("UpdateEvidenceAction"))
    {
        return new UpdateEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("DisableEvidenceAction"))
    {
        return new DisableEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("EnableCutsceneAction"))
    {
        return new EnableCutsceneAction(pReader);
    }
    else if (pReader->ElementExists("NotificationAction"))
    {
        return new NotificationAction(pReader);
    }
    else if (pReader->ElementExists("PlayBgmAction"))
    {
        return new PlayBgmAction(pReader);
    }
    else if (pReader->ElementExists("PauseBgmAction"))
    {
        return new PauseBgmAction(pReader);
    }
    else if (pReader->ElementExists("ResumeBgmAction"))
    {
        return new ResumeBgmAction(pReader);
    }
    else if (pReader->ElementExists("StopBgmAction"))
    {
        return new StopBgmAction(pReader);
    }
    else if (pReader->ElementExists("PlayAmbianceAction"))
    {
        return new PlayAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("PauseAmbianceAction"))
    {
        return new PauseAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("ResumeAmbianceAction"))
    {
        return new ResumeAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("StopAmbianceAction"))
    {
        return new StopAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("StartAnimationAction"))
    {
        return new StartAnimationAction(pReader);
    }
    else if (pReader->ElementExists("StopAnimationAction"))
    {
        return new StopAnimationAction(pReader);
    }
    else if (pReader->ElementExists("BeginCheckPartnerAction"))
    {
        return new BeginCheckPartnerAction(pReader);
    }
    else if (pReader->ElementExists("BranchIfRightAction"))
    {
        return new BranchIfRightAction(pReader);
    }
    else if (pReader->ElementExists("BranchIfWrongAction"))
    {
        return new BranchIfWrongAction(pReader);
    }
    else if (pReader->ElementExists("EndCheckPartnerAction"))
    {
        return new EndCheckPartnerAction(pReader);
    }
    else if (pReader->ElementExists("SetPartnerAction"))
    {
        return new SetPartnerAction(pReader);
    }
    else if (pReader->ElementExists("GoToPresentWrongEvidenceAction"))
    {
        return new GoToPresentWrongEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("LockConversationAction"))
    {
        return new LockConversationAction(pReader);
    }
    else if (pReader->ElementExists("ExitEncounterAction"))
    {
        return new ExitEncounterAction(pReader);
    }
    else if (pReader->ElementExists("MoveToLocationAction"))
    {
        return new MoveToLocationAction(pReader);
    }
    else if (pReader->ElementExists("MoveToZoomedViewAction"))
    {
        return new MoveToZoomedViewAction(pReader);
    }
    else if (pReader->ElementExists("EndCaseAction"))
    {
        return new EndCaseAction(pReader);
    }
    else if (pReader->ElementExists("BeginMultipleChoiceAction"))
    {
        return new BeginMultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists("BeginMultipleChoiceOptionAction"))
    {
        return new BeginMultipleChoiceOptionAction(pReader);
    }
    else if (pReader->ElementExists("ExitMultipleChoiceAction"))
    {
        return new ExitMultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists("EndMultipleChoiceOptionAction"))
    {
        return new EndMultipleChoiceOptionAction(pReader);
    }
    else if (pReader->ElementExists("EndMultipleChoiceAction"))
    {
        return new EndMultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists("EnableFastForwardAction"))
    {
        return new EnableFastForwardAction(pReader);
    }
    else if (pReader->ElementExists("DisableFastForwardAction"))
    {
        return new DisableFastForwardAction(pReader);
    }
    else if (pReader->ElementExists("BeginBreakdownAction"))
    {
        return new BeginBreakdownAction(pReader);
    }
    else if (pReader->ElementExists("EndBreakdownAction"))
    {
        return new EndBreakdownAction(pReader);
    }
    else
    {
        throw MLIException("Unknown action type.");
    }
}

void Conversation::ContinuousAction::GoToNext(State *pState)
{
    pState->SetActionIndex(pState->GetActionIndex() + 1);
}

void Conversation::ContinuousAction::Begin(State *pState)
{
    this->pState = pState;
    this->Reset();

    pState->SetCurrentSpeakerName("");
}

void Conversation::ContinuousAction::Reset()
{
    this->SetIsFinished(false);
}

void Conversation::MultipleCharacterChangeAction::Begin(State *pState)
{
    ContinuousAction::Begin(pState);

    for (unsigned int i = 0; i < characterChangeList.size(); i++)
    {
        characterChangeList[i]->Begin(pState);
    }
}

void Conversation::MultipleCharacterChangeAction::Update(int delta)
{
    bool allCharacterChangesFinished = true;

    for (unsigned int i = 0; i < characterChangeList.size(); i++)
    {
        CharacterChangeAction *pCharacterChange = characterChangeList[i];

        pCharacterChange->Update(delta);

        if (!pCharacterChange->GetIsFinished())
        {
            allCharacterChangesFinished = false;
        }
    }

    if (allCharacterChangesFinished)
    {
        SetIsFinished(true);
    }
}

void Conversation::MultipleCharacterChangeAction::Draw(double xOffset, double yOffset)
{
    for (unsigned int i = 0; i < characterChangeList.size(); i++)
    {
        characterChangeList[i]->Draw(xOffset, yOffset);
    }
}

void Conversation::MultipleCharacterChangeAction::Reset()
{
    ContinuousAction::Reset();

    for (unsigned int i = 0; i < characterChangeList.size(); i++)
    {
        characterChangeList[i]->Reset();
    }
}

Conversation::MultipleCharacterChangeAction::MultipleCharacterChangeAction(XmlReader *pReader)
{
    pReader->StartElement("MultipleCharacterChangeAction");

    pReader->StartElement("CharacterChangeList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        characterChangeList.push_back(new CharacterChangeAction(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

Conversation::MultipleCharacterChangeAction::~MultipleCharacterChangeAction()
{
    for (unsigned int i = 0; i < characterChangeList.size(); i++)
    {
        delete characterChangeList[i];
    }
}

void Conversation::CharacterChangeAction::Begin(State *pState)
{
    ContinuousAction::Begin(pState);

    pCharacterOutEase->Begin();
    pCharacterInEase->Begin();

    Case::GetInstance()->GetDialogCharacterManager()->BeginAnimations(characterIdToChangeTo, initialEmotionId);

    // First check to see if the character in question to move in
    // is already there - nothing to do in that case.
    if (((position == CharacterPositionLeft && pState->GetLeftCharacterId() == characterIdToChangeTo) ||
        (position == CharacterPositionRight && pState->GetRightCharacterId() == characterIdToChangeTo)))
    {
        if (position == CharacterPositionLeft)
        {
            pState->SetLeftCharacterId(characterIdToChangeTo);
            pState->SetLeftCharacterEmotionId(initialEmotionId);
        }
        else if (position == CharacterPositionRight)
        {
            pState->SetRightCharacterId(characterIdToChangeTo);
            pState->SetRightCharacterEmotionId(initialEmotionId);
        }

        SetIsFinished(true);
    }
    else
    {
        if (position == CharacterPositionLeft)
        {
            pState->SetLeftCharacterXOffsetEasingFunction(pCharacterOutEase);
            pState->SetLeftReplacementCharacterId(characterIdToChangeTo);
            pState->SetLeftReplacementCharacterEmotionId(initialEmotionId);
            pState->SetLeftReplacementCharacterXOffsetEasingFunction(pCharacterInEase);
        }
        else if (position == CharacterPositionRight)
        {
            pState->SetRightCharacterXOffsetEasingFunction(pCharacterOutEase);
            pState->SetRightReplacementCharacterId(characterIdToChangeTo);
            pState->SetRightReplacementCharacterEmotionId(initialEmotionId);
            pState->SetRightReplacementCharacterXOffsetEasingFunction(pCharacterInEase);
        }

        if (sfxId.length() > 0)
        {
            playSound(sfxId);
        }
    }
}

void Conversation::CharacterChangeAction::Update(int delta)
{
    if (GetIsFinished())
    {
        return;
    }

    if (position == CharacterPositionLeft && pState->GetLeftCharacterId().length() == 0)
    {
        pCharacterOutEase->Finish();
    }
    else if (position == CharacterPositionRight && pState->GetRightCharacterId().length() == 0)
    {
        pCharacterOutEase->Finish();
    }

    if (pCharacterOutEase->GetIsFinished() && pCharacterInEase->GetIsFinished())
    {
        SetIsFinished(true);
        SwapCharacters();
        return;
    }

    pCharacterOutEase->Update(delta);
    pCharacterInEase->Update(delta);
}

void Conversation::CharacterChangeAction::Draw(double xOffset, double yOffset)
{
    // The characters are drawn by the encounter,
    // so there's nothing for us to do here.
}

Conversation::CharacterChangeAction::CharacterChangeAction(XmlReader *pReader)
{
    pReader->StartElement("CharacterChangeAction");
    position = StringToCharacterPosition(pReader->ReadTextElement("Position"));
    characterIdToChangeTo = pReader->ReadTextElement("CharacterIdToChangeTo");

    if (characterIdToChangeTo == "None")
    {
        characterIdToChangeTo = "";
    }

    initialEmotionId = pReader->ReadTextElement("InitialEmotionId");

    if (pReader->ElementExists("SfxId"))
    {
        sfxId = pReader->ReadTextElement("SfxId");
    }

    pReader->EndElement();

    pCharacterOutEase = new QuadraticEase(0, position == CharacterPositionLeft ? -gScreenWidth / 2 : gScreenWidth / 2, 250);
    pCharacterInEase = new QuadraticEase(position == CharacterPositionLeft ? -gScreenWidth / 2 : gScreenWidth / 2, 0, 250);
}

Conversation::CharacterChangeAction::~CharacterChangeAction()
{
    delete pCharacterOutEase;
    pCharacterOutEase = NULL;
    delete pCharacterInEase;
    pCharacterInEase = NULL;
}

void Conversation::CharacterChangeAction::SwapCharacters()
{
    if (position == CharacterPositionLeft)
    {
        pState->SetLeftCharacterId(characterIdToChangeTo);
        pState->SetLeftCharacterEmotionId(initialEmotionId);
        pState->SetLeftCharacterXOffsetEasingFunction(NULL);
        pState->SetLeftReplacementCharacterId("");
        pState->SetLeftReplacementCharacterXOffsetEasingFunction(NULL);

        if (characterIdToChangeTo.length() == 0)
        {
            pCharacterInEase->Finish();
            SetIsFinished(true);
        }
    }
    else if (position == CharacterPositionRight)
    {
        pState->SetRightCharacterId(characterIdToChangeTo);
        pState->SetRightCharacterEmotionId(initialEmotionId);
        pState->SetRightCharacterXOffsetEasingFunction(NULL);
        pState->SetRightReplacementCharacterId("");
        pState->SetRightReplacementCharacterXOffsetEasingFunction(NULL);

        if (characterIdToChangeTo.length() == 0)
        {
            pCharacterInEase->Finish();
            SetIsFinished(true);
        }
    }
}

void Conversation::SetFlagAction::Execute(State *pState)
{
    if (!Case::GetInstance()->GetFlagManager()->IsFlagSet(flagId))
    {
        Case::GetInstance()->GetFlagManager()->SetFlag(flagId);
    }
    else if (pState->GetCurrentConversation()->GetIsNotificationNext())
    {
        // If the flag has already been set, then we want to
        // skip the notification action.
        pState->SetActionIndex(pState->GetActionIndex() + 2);
    }
}

Conversation::SetFlagAction::SetFlagAction(XmlReader *pReader)
{
    pReader->StartElement("SetFlagAction");
    flagId = pReader->ReadTextElement("FlagId");
    pReader->EndElement();
}

void Conversation::BranchOnConditionAction::Execute(State *pState)
{
    if (pCondition->IsTrue())
    {
        pState->SetActionIndex(GetTrueIndex());
    }
    else
    {
        pState->SetActionIndex(GetFalseIndex());
    }
}

Conversation::BranchOnConditionAction::BranchOnConditionAction(XmlReader *pReader)
{
    pReader->StartElement("BranchOnConditionAction");

    pReader->StartElement("Condition");
    pCondition = new Condition(pReader);
    pReader->EndElement();

    trueIndex = pReader->ReadIntElement("TrueIndex");
    falseIndex = pReader->ReadIntElement("FalseIndex");
    pReader->EndElement();
}

Conversation::BranchOnConditionAction::~BranchOnConditionAction()
{
    delete pCondition;
    pCondition = NULL;
}

void Conversation::BranchIfTrueAction::Execute(State *pState)
{
    pState->SetActionIndex(GetEndIndex());
}

Conversation::BranchIfTrueAction::BranchIfTrueAction(XmlReader *pReader)
{
    pReader->StartElement("BranchIfTrueAction");
    endIndex = pReader->ReadIntElement("EndIndex");
    pReader->EndElement();
}

void Conversation::BranchIfFalseAction::Execute(State *pState)
{
    pState->SetActionIndex(GetEndIndex());
}

Conversation::BranchIfFalseAction::BranchIfFalseAction(XmlReader *pReader)
{
    pReader->StartElement("BranchIfFalseAction");
    endIndex = pReader->ReadIntElement("EndIndex");
    pReader->EndElement();
}

void Conversation::EndBranchOnConditionAction::Execute(State *pState)
{

}

Conversation::EndBranchOnConditionAction::EndBranchOnConditionAction(XmlReader *pReader)
{
    pReader->StartElement("EndBranchOnConditionAction");
    pReader->EndElement();
}

void Conversation::ShowDialogAction::Begin(State *pState)
{
    ContinuousAction::Begin(pState);
    pState->SetCanFastForward(hasBeenSeen || gEnableSkippingUnseenDialog);

    pState->SetSpeakerPosition(speakerPosition);

    if (speakerPosition != CharacterPositionLeft)
    {
        pState->SetIsLeftCharacterZoomed(false);
    }

    if (speakerPosition != CharacterPositionRight)
    {
        pState->SetIsRightCharacterZoomed(false);
    }

    string speakerName = "";
    bool allowFrills = false;

    if (speakerPosition == CharacterPositionUnknown)
    {
        speakerName = pgLocalizableContent->GetText("Conversation/UnknownCharacterNameText");
    }
    else if (speakerPosition == CharacterPositionOffscreen)
    {
        speakerName = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterNameFromId(characterId);
    }
    else if (speakerPosition != CharacterPositionNone)
    {
        string positionCharacterId = speakerPosition == CharacterPositionLeft ? pState->GetLeftCharacterId() : pState->GetRightCharacterId();
        speakerName = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterNameFromId(positionCharacterId);
        allowFrills = true;
    }
    else
    {
        speakerName = "";
    }

    if (pDialog == NULL)
    {
        pDialog = Dialog::CreateForString(
            rawDialog,
            filePath,
            leadInTime,
            GetDelayBeforeContinuing(),
            allowFrills && GetIsInterrogation(),
            allowFrills && GetIsPassive(),
            allowFrills && GetIsInConfrontation(),
            allowFrills && GetCanNavigateBack(),
            allowFrills && GetCanNavigateForward(),
            allowFrills && GetShouldPresentEvidenceAutomatically(),
            allowFrills && GetCanStopPresentingEvidence());
    }

    EventProviders::GetDialogEventProvider()->ClearListener(this);
    EventProviders::GetDialogEventProvider()->RegisterListener(this);
    pDialog->Begin(pState);

    pState->SetCurrentSpeakerName(speakerName);
    pState->SetLastShowDialogSpeakerName(speakerName);
    pState->SetLastShowDialogRawDialogText(rawDialog);
}

void Conversation::ShowDialogAction::Update(int delta)
{
    if (GetIsFinished() || pDialog == NULL)
    {
        return;
    }

    if (pState->GetActionIndexAfterInterjection() >= 0 && !pState->GetIsInterjectionOngoing())
    {
        pState->SetActionIndex(pState->GetActionIndexAfterInterjection());
        pState->SetActionIndexAfterInterjection(-1);
        SetIsFinished(true);
        return;
    }

    if (((MouseHelper::ClickedAnywhere() || KeyboardHelper::ClickPressed() || pState->GetIsFastForwarding()) &&
         (pDialog->GetIsReadyToProgress() || hasBeenSeen || gEnableSkippingUnseenDialog) &&
         !pDialog->HandleClick() &&
         !pDialog->GetIsAutomatic() &&
         !GetShouldPresentEvidenceAutomatically()) ||
        (pDialog->GetIsReadyToProgress() && pDialog->GetIsAutomatic()))
    {
        if (pDialog->GetIsReadyToProgress() && !pState->GetIsInterjectionOngoing())
        {
            SetIsFinished(true);
            pState->SetLeftCharacterMouthOffCount(0);
            pState->SetRightCharacterMouthOffCount(0);
            return;
        }
    }
    else if (pDialog->GetIsReadyToProgress() && pState->GetCurrentAnimation() != NULL)
    {
        pState->GetCurrentAnimation()->FinishCurrentFrame();
    }
    else
    {
        pDialog->HandleKeypress();
    }

    bool shouldChangeMouth = false;

    if (isTalking)
    {
        totalMillisecondsSinceMouthChange += delta;

        if (totalMillisecondsSinceMouthChange > MillisecondsBetweenMouthMovement)
        {
            shouldChangeMouth = true;
            totalMillisecondsSinceMouthChange -= MillisecondsBetweenMouthMovement;
        }
    }
    else
    {
        totalMillisecondsSinceMouthChange = 0;
    }

    pDialog->Update(delta);
    bool oldIsTalking = isTalking;
    isTalking =
        !pDialog->GetIsAudioPaused() &&
        !pDialog->GetIsFinished() &&
        (speakerPosition == CharacterPositionLeft ? pState->GetLeftCharacterMouthOffCount() <= 0 : pState->GetRightCharacterMouthOffCount() <= 0);

    if (!isTalking)
    {
        totalMillisecondsSinceMouthChange = 0;
    }
    else if (oldIsTalking == false)
    {
        shouldChangeMouth = true;
    }

    if (speakerPosition == CharacterPositionLeft)
    {
        pState->SetIsLeftCharacterTalking(isTalking);
        pState->SetShouldLeftCharacterChangeMouth(shouldChangeMouth);
    }
    else if (speakerPosition == CharacterPositionRight)
    {
        pState->SetIsRightCharacterTalking(isTalking);
        pState->SetShouldRightCharacterChangeMouth(shouldChangeMouth);
    }
}

void Conversation::ShowDialogAction::Draw(double xOffset, double yOffset)
{
    if (pDialog != NULL)
    {
        pDialog->Draw(xOffset, yOffset);
    }
}

void Conversation::ShowDialogAction::DrawBackground(double xOffset, double yOffset)
{
    if (pDialog != NULL)
    {
        pDialog->DrawBackground(xOffset, yOffset);
    }
}

void Conversation::ShowDialogAction::Reset()
{
    ContinuousAction::Reset();

    if (pDialog != NULL)
    {
        pDialog->Reset();

        delete pDialog;
        pDialog = NULL;
    }

    totalMillisecondsSinceMouthChange = 0;
    isTalking = false;

    pState->SetLeftCharacterXOffsetEasingFunction(NULL);
    pState->SetRightCharacterXOffsetEasingFunction(NULL);

    EventProviders::GetDialogEventProvider()->ClearListener(this);
}

void Conversation::ShowDialogAction::SetIsFinished(bool isFinished)
{
    Conversation::ContinuousAction::SetIsFinished(isFinished);

    if (isFinished)
    {
        if (pDialog != NULL)
        {
            pDialog->Finish();
        }

        if (!hasBeenSeen)
        {
            gDialogsSeenList.push_back(id);

            // Multiple ShowDialogActions can have the same ID if they're effectively the same as the others (same speaker, same dialog, same file path, etc.),
            // so we want to set *all* of those as having been seen, not just this one.
            for (vector<Conversation::ShowDialogAction *>::iterator showDialogActionIterator = showDialogActionListsById[id].begin(); showDialogActionIterator != showDialogActionListsById[id].end(); showDialogActionIterator++)
            {
                (*showDialogActionIterator)->SetHasBeenSeen(true);
            }
        }
    }
}

bool Conversation::ShowDialogAction::GetIsReadyToHide()
{
    return pDialog->GetIsReadyToHide() && pState->GetActionIndexAfterInterjection() == -1;
}

void Conversation::ShowDialogAction::OnDialogSpeakerEmotionChanged(Dialog *pSender, const string &newEmotionId)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (speakerPosition == CharacterPositionLeft)
    {
        pState->SetLeftCharacterEmotionId(newEmotionId);
        Case::GetInstance()->GetDialogCharacterManager()->BeginAnimations(pState->GetLeftCharacterId(), newEmotionId);
    }
    else if (speakerPosition == CharacterPositionRight)
    {
        pState->SetRightCharacterEmotionId(newEmotionId);
        Case::GetInstance()->GetDialogCharacterManager()->BeginAnimations(pState->GetRightCharacterId(), newEmotionId);
    }
}

void Conversation::ShowDialogAction::OnDialogOtherEmotionChanged(Dialog *pSender, const string &newEmotionId)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (speakerPosition == CharacterPositionLeft)
    {
        pState->SetRightCharacterEmotionId(newEmotionId);
    }
    else if (speakerPosition == CharacterPositionRight)
    {
        pState->SetLeftCharacterEmotionId(newEmotionId);
    }
}

void Conversation::ShowDialogAction::OnDialogNextFrame(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (pState->GetCurrentAnimation() != NULL)
    {
        pState->GetCurrentAnimation()->MoveToNextFrame();
    }
}

void Conversation::ShowDialogAction::OnDialogPlayerDamaged(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (pState->GetCurrentConfrontation() != NULL)
    {
        pState->GetCurrentConfrontation()->DamagePlayer();
    }
}

void Conversation::ShowDialogAction::OnDialogOpponentDamaged(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (pState->GetCurrentConfrontation() != NULL)
    {
        pState->GetCurrentConfrontation()->DamageOpponent();
    }
}

void Conversation::ShowDialogAction::OnDialogSpeakerStartedShaking(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (speakerPosition == CharacterPositionLeft)
    {
        delete pLeftEarthquakeEase;
        pLeftEarthquakeEase = new EarthquakeEase(0, -50, 250);
        pState->SetLeftCharacterXOffsetEasingFunction(pLeftEarthquakeEase);
        pState->GetLeftCharacterXOffsetEasingFunction()->Begin();
    }
    else if (speakerPosition == CharacterPositionRight)
    {
        delete pRightEarthquakeEase;
        pRightEarthquakeEase = new EarthquakeEase(0, -50, 250);
        pState->SetRightCharacterXOffsetEasingFunction(pRightEarthquakeEase);
        pState->GetRightCharacterXOffsetEasingFunction()->Begin();
    }
}

void Conversation::ShowDialogAction::OnDialogScreenStartedShaking(Dialog *pSender, double shakeIntensity)
{
    if (pSender != pDialog)
    {
        return;
    }

    delete pLeftEarthquakeEase;
    pLeftEarthquakeEase = new EarthquakeEase(0, -shakeIntensity, 500);
    pState->SetLeftCharacterXOffsetEasingFunction(pLeftEarthquakeEase);
    pState->GetLeftCharacterXOffsetEasingFunction()->Begin();

    delete pRightEarthquakeEase;
    pRightEarthquakeEase = new EarthquakeEase(0, -shakeIntensity, 500);
    pState->SetRightCharacterXOffsetEasingFunction(pRightEarthquakeEase);
    pState->GetRightCharacterXOffsetEasingFunction()->Begin();

    delete pWindowXEarthquakeEase;
    pWindowXEarthquakeEase = new CenteredEarthquakeEase(0, -shakeIntensity, 500, 15);
    pState->SetWindowMotionXEasingFunction(pWindowXEarthquakeEase);
    pState->GetWindowMotionXEasingFunction()->Begin();

    delete pWindowYEarthquakeEase;
    pWindowYEarthquakeEase = new EarthquakeEase(0, -shakeIntensity, 500, 10);
    pState->SetWindowMotionYEasingFunction(pWindowYEarthquakeEase);
    pState->GetWindowMotionYEasingFunction()->Begin();
}

void Conversation::ShowDialogAction::OnDialogSpeakerMouthStateChanged(Dialog *pSender, bool isMouthOn)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (speakerPosition == CharacterPositionLeft)
    {
        // We'll keep a count instead of just a boolean variable, so that way
        // something of the form MouthOff, MouthOff, MouthOn doesn't result
        // in the mouth being on at that point.
        pState->SetLeftCharacterMouthOffCount(pState->GetLeftCharacterMouthOffCount() + (isMouthOn ? -1 : 1));
    }
    else if (speakerPosition == CharacterPositionRight)
    {
        pState->SetRightCharacterMouthOffCount(pState->GetRightCharacterMouthOffCount() + (isMouthOn ? -1 : 1));
    }
}

void Conversation::ShowDialogAction::OnDialogSpeakerZoomed(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (speakerPosition == CharacterPositionLeft)
    {
        pState->SetIsLeftCharacterZoomed(true);
        pState->SetIsRightCharacterZoomed(false);
    }
    else if (speakerPosition == CharacterPositionRight)
    {
        pState->SetIsRightCharacterZoomed(true);
        pState->SetIsLeftCharacterZoomed(false);
    }
}

void Conversation::ShowDialogAction::OnDialogEndSpeakerZoomed(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (speakerPosition == CharacterPositionLeft)
    {
        pState->SetIsLeftCharacterZoomed(false);
    }
    else if (speakerPosition == CharacterPositionRight)
    {
        pState->SetIsRightCharacterZoomed(false);
    }
}

void Conversation::ShowDialogAction::OnDialogBreakdownBegun(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (speakerPosition == CharacterPositionLeft || speakerPosition == CharacterPositionRight)
    {
        pState->SetBreakdownActivePosition(speakerPosition);
    }
}

void Conversation::ShowDialogAction::OnDialogBreakdownEnded(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    pState->SetBreakdownActivePosition(CharacterPositionNone);
}

void Conversation::ShowDialogAction::LoadFromXmlCore(XmlReader *pReader)
{
    pDialog = NULL;

    id = pReader->ReadTextElement("Id");

    bool dialogHasBeenSeen = false;

    for (unsigned int i = 0; i < gDialogsSeenList.size(); i++)
    {
        if (gDialogsSeenList[i] == id)
        {
            dialogHasBeenSeen = true;
            break;
        }
    }

    hasBeenSeen = dialogHasBeenSeen;

    speakerPosition = StringToCharacterPosition(pReader->ReadTextElement("SpeakerPosition"));
    rawDialog = pReader->ReadTextElement("RawDialog");

    if (pReader->ElementExists("FilePath"))
    {
        filePath = pReader->ReadTextElement("FilePath");
    }

    leadInTime = pReader->ReadIntElement("LeadInTime");

    if (pReader->ElementExists("CharacterId"))
    {
        characterId = pReader->ReadTextElement("CharacterId");
    }

    isInConfrontation = pReader->ReadBooleanElement("IsInConfrontation");

    totalMillisecondsSinceMouthChange = 0;
    isTalking = false;

    pLeftEarthquakeEase = NULL;
    pRightEarthquakeEase = NULL;
    pWindowXEarthquakeEase = NULL;
    pWindowYEarthquakeEase = NULL;

    showDialogActionListsById[id].push_back(this);
}

Conversation::ShowDialogAction::~ShowDialogAction()
{
    delete pDialog;
    pDialog = NULL;

    delete pLeftEarthquakeEase;
    pLeftEarthquakeEase = NULL;
    delete pRightEarthquakeEase;
    pRightEarthquakeEase = NULL;
    delete pWindowXEarthquakeEase;
    pWindowXEarthquakeEase = NULL;
    delete pWindowYEarthquakeEase;
    pWindowYEarthquakeEase = NULL;

    EventProviders::GetDialogEventProvider()->ClearListener(this);
}

Conversation::ShowDialogAction::ShowDialogAction(XmlReader *pReader)
{
    pReader->StartElement("ShowDialogAction");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

void Conversation::ShowDialogAutomaticAction::Begin(State *pState)
{
    Conversation::ShowDialogAction::Begin(pState);
    pState->SetCanFastForward(false);
}

Conversation::ShowDialogAutomaticAction::ShowDialogAutomaticAction(XmlReader *pReader)
{
    pReader->StartElement("ShowDialogAutomaticAction");
    LoadFromXmlCore(pReader);
    delayBeforeContinuing = pReader->ReadIntElement("DelayBeforeContinuing");
    pReader->EndElement();
}

void Conversation::MustPresentEvidenceAction::Begin(State *pState)
{
    ShowDialogAction::Begin(pState);
    pState->SetCanFastForward(false);

    // If we don't have evidence yet, then we have no evidence to present.
    // We'll just early-out and go to the end-requested index.
    if (!Case::GetInstance()->GetEvidenceManager()->GetHasEvidence())
    {
        pState->SetActionIndex(GetEndRequestedIndex());
        SetIsFinished(true);
    }
}

void Conversation::MustPresentEvidenceAction::OnDialogEvidencePresented(Dialog *pSender, const string &evidenceId)
{
    if (pSender != pDialog)
    {
        return;
    }

    DialogCharacter *pPlayerCharacter = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPlayerCharacterId());
    bool isEvidenceCorrect = false;

    for (unsigned int i = 0; i < correctEvidenceIdList.size(); i++)
    {
        if (correctEvidenceIdList[i] == evidenceId)
        {
            isEvidenceCorrect = true;
            break;
        }
    }

    if (isEvidenceCorrect)
    {
        playSound("Interjection");
        pState->SetActionIndexAfterInterjection(GetCorrectEvidencePresentedIndex());
        pState->StartInterjection(Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPlayerCharacterId())->GetPresentCorrectEvidenceSprite());
        playDialog(pPlayerCharacter->GetPresentCorrectEvidencePromptedSoundId());
    }
    else
    {
        pState->SetActionIndexAfterInterjection(GetWrongEvidencePresentedIndex());
        pState->StartInterjection(Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPlayerCharacterId())->GetPresentIncorrectEvidenceSprite());
        playDialog(pPlayerCharacter->GetPresentIncorrectEvidenceSoundId());
    }
}

void Conversation::MustPresentEvidenceAction::OnDialogEndRequested(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    pState->SetActionIndex(GetEndRequestedIndex());
    SetIsFinished(true);
}

void Conversation::MustPresentEvidenceAction::Reset()
{
    ShowDialogAction::Reset();
}

bool Conversation::MustPresentEvidenceAction::GetCanStopPresentingEvidence()
{
    return endRequestedIndex >= 0;
}

void Conversation::MustPresentEvidenceAction::LoadFromXmlCore(XmlReader *pReader)
{
    ShowDialogAction::LoadFromXmlCore(pReader);

    pReader->StartElement("CorrectEvidenceIdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        correctEvidenceIdList.push_back(pReader->ReadTextElement("CorrectEvidenceId"));
    }

    pReader->EndElement();

    correctEvidencePresentedIndex = pReader->ReadIntElement("CorrectEvidencePresentedIndex");
    wrongEvidencePresentedIndex = pReader->ReadIntElement("WrongEvidencePresentedIndex");
    endRequestedIndex = pReader->ReadIntElement("EndRequestedIndex");
}

Conversation::MustPresentEvidenceAction::MustPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("MustPresentEvidenceAction");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

void Conversation::SkipWrongEvidencePresentedAction::Execute(State *pState)
{
    pState->SetActionIndex(GetAfterWrongEvidencePresentedIndex());
}

Conversation::SkipWrongEvidencePresentedAction::SkipWrongEvidencePresentedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipWrongEvidencePresentedAction");
    afterWrongEvidencePresentedIndex = pReader->ReadIntElement("AfterWrongEvidencePresentedIndex");
    pReader->EndElement();
}

void Conversation::BeginWrongEvidencePresentedAction::Execute(State *pState)
{
    pState->CachePreviousActionIndex();
}

Conversation::BeginWrongEvidencePresentedAction::BeginWrongEvidencePresentedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginWrongEvidencePresentedAction");
    pReader->EndElement();
}

void Conversation::EndWrongEvidencePresentedAction::Execute(State *pState)
{
    pState->RestoreActionIndexFromCache();
}

Conversation::EndWrongEvidencePresentedAction::EndWrongEvidencePresentedAction(XmlReader *pReader)
{
    pReader->StartElement("EndWrongEvidencePresentedAction");
    pReader->EndElement();
}

void Conversation::SkipEndRequestedAction::Execute(State *pState)
{
    pState->SetActionIndex(GetAfterEndRequestedIndex());
}

Conversation::SkipEndRequestedAction::SkipEndRequestedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipEndRequestedAction");
    afterEndRequestedIndex = pReader->ReadIntElement("AfterEndRequestedIndex");
    pReader->EndElement();
}

void Conversation::BeginEndRequestedAction::Execute(State *pState)
{

}

Conversation::BeginEndRequestedAction::BeginEndRequestedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginEndRequestedAction");
    pReader->EndElement();
}

void Conversation::EndEndRequestedAction::Execute(State *pState)
{
    pState->SetEndRequested(true);
}

Conversation::EndEndRequestedAction::EndEndRequestedAction(XmlReader *pReader)
{
    pReader->StartElement("EndEndRequestedAction");
    pReader->EndElement();
}

void Conversation::EndMustPresentEvidenceAction::Execute(State *pState)
{

}

Conversation::EndMustPresentEvidenceAction::EndMustPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("EndMustPresentEvidenceAction");
    pReader->EndElement();
}

void Conversation::EnableConversationAction::Execute(State *pState)
{
    Conversation *pConversation = Case::GetInstance()->GetContentManager()->GetConversationFromId(conversationId);

    if (!pConversation->GetIsEnabled())
    {
        pConversation->SetIsEnabled(true);
    }
    else if (pState->GetCurrentConversation()->GetIsNotificationNext())
    {
        // If the conversation has already been enabled, then we want to
        // skip the notification action.
        pState->SetActionIndex(pState->GetActionIndex() + 2);
    }
}

Conversation::EnableConversationAction::EnableConversationAction(XmlReader *pReader)
{
    pReader->StartElement("EnableConversationAction");
    conversationId = pReader->ReadTextElement("ConversationId");
    pReader->EndElement();
}

void Conversation::EnableEvidenceAction::Execute(State *pState)
{
    if (!Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdEnabled(evidenceId))
    {
        Case::GetInstance()->GetEvidenceManager()->EnableEvidenceWithId(evidenceId);
    }
    else if (pState->GetCurrentConversation()->GetIsNotificationNext())
    {
        // If the evidence has already been enabled, then we want to
        // skip the notification action.
        pState->SetActionIndex(pState->GetActionIndex() + 2);
    }
}

Conversation::EnableEvidenceAction::EnableEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("EnableEvidenceAction");
    evidenceId = pReader->ReadTextElement("EvidenceId");
    pReader->EndElement();
}

void Conversation::UpdateEvidenceAction::Execute(State *pState)
{
    if (Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdVisible(evidenceId) &&
        !Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdEnabled(newEvidenceId))
    {
        Case::GetInstance()->GetEvidenceManager()->DisableEvidenceWithId(evidenceId);
        Case::GetInstance()->GetEvidenceManager()->EnableEvidenceWithId(newEvidenceId);
    }
    else if (pState->GetCurrentConversation()->GetIsNotificationNext())
    {
        // If the evidence has already been updated,
        // or if we don't have the initial evidence in the first place,
        // then we want to skip the notification action.
        pState->SetActionIndex(pState->GetActionIndex() + 2);
    }
}

Conversation::UpdateEvidenceAction::UpdateEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("UpdateEvidenceAction");
    evidenceId = pReader->ReadTextElement("EvidenceId");
    newEvidenceId = pReader->ReadTextElement("NewEvidenceId");
    pReader->EndElement();
}

void Conversation::DisableEvidenceAction::Execute(State *pState)
{
    if (Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdVisible(evidenceId))
    {
        Case::GetInstance()->GetEvidenceManager()->DisableEvidenceWithId(evidenceId);
    }
    else if (pState->GetCurrentConversation()->GetIsNotificationNext())
    {
        // If the evidence has already been disabled, then we want to
        // skip the notification action.
        pState->SetActionIndex(pState->GetActionIndex() + 2);
    }
}

Conversation::DisableEvidenceAction::DisableEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("DisableEvidenceAction");
    evidenceId = pReader->ReadTextElement("EvidenceId");
    pReader->EndElement();
}

void Conversation::EnableCutsceneAction::Execute(State *pState)
{
    if (!Case::GetInstance()->GetFieldCutsceneManager()->GetCutsceneFromId(cutsceneId)->GetIsEnabled())
    {
        Case::GetInstance()->GetFieldCutsceneManager()->GetCutsceneFromId(cutsceneId)->SetIsEnabled(true);
    }
}

Conversation::EnableCutsceneAction::EnableCutsceneAction(XmlReader *pReader)
{
    pReader->StartElement("EnableCutsceneAction");
    cutsceneId = pReader->ReadTextElement("CutsceneId");
    pReader->EndElement();
}

void Conversation::NotificationAction::Begin(State *pState)
{
    ContinuousAction::Begin(pState);
    pNotification->Begin();
}

void Conversation::NotificationAction::Update(int delta)
{
    pNotification->Update(delta, pState->GetIsFastForwarding());

    if (pNotification->GetIsFinished())
    {
        SetIsFinished(true);
    }
}

void Conversation::NotificationAction::Draw(double xOffset, double yOffset)
{
    pNotification->Draw(yOffset);
}

void Conversation::NotificationAction::Reset()
{
    ContinuousAction::Reset();
    pNotification->Reset();
}

Conversation::NotificationAction::NotificationAction(XmlReader *pReader)
{
    pReader->StartElement("NotificationAction");
    rawNotificationText = pReader->ReadTextElement("RawNotificationText");

    if (pReader->ElementExists("PartnerId"))
    {
        partnerId = pReader->ReadTextElement("PartnerId");
    }

    if (pReader->ElementExists("OldEvidenceId"))
    {
        oldEvidenceId = pReader->ReadTextElement("OldEvidenceId");
    }

    if (pReader->ElementExists("NewEvidenceId"))
    {
        newEvidenceId = pReader->ReadTextElement("NewEvidenceId");
    }

    pReader->EndElement();

    pNotification = new Notification(GetRawNotificationText(), GetPartnerId(), GetOldEvidenceId(), GetNewEvidenceId());
}

Conversation::NotificationAction::~NotificationAction()
{
    delete pNotification;
    pNotification = NULL;
}

void Conversation::PlayBgmAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->PlayBgmWithId(bgmId);
    pState->SetPreserveBgm(preserveBgm);
}

Conversation::PlayBgmAction::PlayBgmAction(XmlReader *pReader)
{
    pReader->StartElement("PlayBgmAction");
    bgmId = pReader->ReadTextElement("BgmId");
    preserveBgm = pReader->ReadBooleanElement("PreserveBgm");
    pReader->EndElement();
}

void Conversation::PauseBgmAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->PauseCurrentBgm();
}

Conversation::PauseBgmAction::PauseBgmAction(XmlReader *pReader)
{
    pReader->StartElement("PauseBgmAction");
    pReader->EndElement();
}

void Conversation::ResumeBgmAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->ResumeCurrentBgm();
}

Conversation::ResumeBgmAction::ResumeBgmAction(XmlReader *pReader)
{
    pReader->StartElement("ResumeBgmAction");
    pReader->EndElement();
}

void Conversation::StopBgmAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->StopCurrentBgm(isInstant);
    pState->SetPreserveBgm(preserveBgm);
}

Conversation::StopBgmAction::StopBgmAction(XmlReader *pReader)
{
    pReader->StartElement("StopBgmAction");
    isInstant = pReader->ReadBooleanElement("IsInstant");
    preserveBgm = pReader->ReadBooleanElement("PreserveBgm");
    pReader->EndElement();
}

void Conversation::PlayAmbianceAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->PlayAmbianceWithId(ambianceSfxId);
    pState->SetPreserveAmbiance(preserveAmbiance);
}

Conversation::PlayAmbianceAction::PlayAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("PlayAmbianceAction");
    ambianceSfxId = pReader->ReadTextElement("AmbianceSfxId");
    preserveAmbiance = pReader->ReadBooleanElement("PreserveAmbiance");
    pReader->EndElement();
}

void Conversation::PauseAmbianceAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->PauseCurrentAmbiance();
}

Conversation::PauseAmbianceAction::PauseAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("PauseAmbianceAction");
    pReader->EndElement();
}

void Conversation::ResumeAmbianceAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->ResumeCurrentAmbiance();
}

Conversation::ResumeAmbianceAction::ResumeAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("ResumeAmbianceAction");
    pReader->EndElement();
}

void Conversation::StopAmbianceAction::Execute(State *pState)
{
    Case::GetInstance()->GetAudioManager()->StopCurrentAmbiance(isInstant);
    pState->SetPreserveAmbiance(preserveAmbiance);
}

Conversation::StopAmbianceAction::StopAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("StopAmbianceAction");
    isInstant = pReader->ReadBooleanElement("IsInstant");
    preserveAmbiance = pReader->ReadBooleanElement("PreserveAmbiance");
    pReader->EndElement();
}

void Conversation::StartAnimationAction::Execute(State *pState)
{
    pState->SetCurrentAnimation(Case::GetInstance()->GetDialogCutsceneManager()->GetAnimationFromId(animationId));
    pState->GetCurrentAnimation()->Begin();
}

Conversation::StartAnimationAction::StartAnimationAction(XmlReader *pReader)
{
    pReader->StartElement("StartAnimationAction");
    animationId = pReader->ReadTextElement("AnimationId");
    pReader->EndElement();
}

void Conversation::StopAnimationAction::Execute(State *pState)
{
    pState->GetCurrentAnimation()->Reset();
    pState->SetCurrentAnimation(NULL);
}

Conversation::StopAnimationAction::StopAnimationAction(XmlReader *pReader)
{
    pReader->StartElement("StopAnimationAction");
    pReader->EndElement();
}

void Conversation::BeginCheckPartnerAction::Execute(State *pState)
{
    if (partnerId.length() == 0 || Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId() == partnerId)
    {
        pState->SetActionIndex(GetRightIndex());
    }
    else if (partnerId.length() > 0)
    {
        pState->SetActionIndex(GetWrongIndex());
    }
}

Conversation::BeginCheckPartnerAction::BeginCheckPartnerAction(XmlReader *pReader)
{
    pReader->StartElement("BeginCheckPartnerAction");
    partnerId = pReader->ReadTextElement("PartnerId");
    rightIndex = pReader->ReadIntElement("RightIndex");
    wrongIndex = pReader->ReadIntElement("WrongIndex");
    pReader->EndElement();
}

void Conversation::BranchIfRightAction::Execute(State *pState)
{
    pState->SetActionIndex(GetEndIndex());
}

Conversation::BranchIfRightAction::BranchIfRightAction(XmlReader *pReader)
{
    pReader->StartElement("BranchIfRightAction");
    endIndex = pReader->ReadIntElement("EndIndex");
    pReader->EndElement();
}

void Conversation::BranchIfWrongAction::Execute(State *pState)
{
    pState->SetActionIndex(GetEndIndex());
}

Conversation::BranchIfWrongAction::BranchIfWrongAction(XmlReader *pReader)
{
    pReader->StartElement("BranchIfWrongAction");
    endIndex = pReader->ReadIntElement("EndIndex");
    pReader->EndElement();
}

void Conversation::EndCheckPartnerAction::Execute(State *pState)
{

}

Conversation::EndCheckPartnerAction::EndCheckPartnerAction(XmlReader *pReader)
{
    pReader->StartElement("EndCheckPartnerAction");
    pReader->EndElement();
}

void Conversation::SetPartnerAction::Execute(State *pState)
{
    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId() != partnerId)
    {
        Case::GetInstance()->GetPartnerManager()->SetCurrentPartner(partnerId);
    }
    else if (pState->GetCurrentConversation()->GetIsNotificationNext())
    {
        // If this partner is already the current partner, then we want to
        // skip the notification action.
        pState->SetActionIndex(pState->GetActionIndex() + 2);
    }
}

Conversation::SetPartnerAction::SetPartnerAction(XmlReader *pReader)
{
    pReader->StartElement("SetPartnerAction");
    partnerId = pReader->ReadTextElement("PartnerId");
    pReader->EndElement();
}

void Conversation::GoToPresentWrongEvidenceAction::Execute(State *pState)
{
    pState->SetPresentWrongEvidenceRequested(true);
}

Conversation::GoToPresentWrongEvidenceAction::GoToPresentWrongEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("GoToPresentWrongEvidenceAction");
    pReader->EndElement();
}

Conversation::LockConversationAction::~LockConversationAction()
{
    delete pUnlockCondition;
    pUnlockCondition = NULL;
}

void Conversation::LockConversationAction::Execute(State *pState)
{
    vector<UnlockCondition *> *pUnlockConditions = pState->GetCurrentConversation()->GetUnlockConditions();
    bool unlockConditionExists = false;

    for (unsigned int i = 0; i < pUnlockConditions->size(); i++)
    {
        if ((*pUnlockConditions)[i]->Equals(pUnlockCondition))
        {
            unlockConditionExists = true;
            break;
        }
    }

    if (!unlockConditionExists)
    {
        pState->GetCurrentConversation()->GetUnlockConditions()->push_back(pUnlockCondition->Clone());
    }
    else if (pState->GetCurrentConversation()->GetIsNotificationNext())
    {
        // If this unlock condition has already been set, then we want to
        // skip the notification action.
        pState->SetActionIndex(pState->GetActionIndex() + 2);
    }
}

Conversation::LockConversationAction::LockConversationAction(XmlReader *pReader)
{
    pReader->StartElement("LockConversationAction");

    pReader->StartElement("UnlockCondition");
    pUnlockCondition = Conversation::UnlockCondition::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

void Conversation::ExitEncounterAction::Execute(State *pState)
{
    pState->SetEndRequested(true);
    pState->SetExitRequested(true);
}

Conversation::ExitEncounterAction::ExitEncounterAction(XmlReader *pReader)
{
    pReader->StartElement("ExitEncounterAction");
    pReader->EndElement();
}

void Conversation::MoveToLocationAction::Execute(State *pState)
{
    pState->SetTransitionRequest(TransitionRequest(newAreaId, newAreaId + newLocationId, newAreaId + transitionId));
}

Conversation::MoveToLocationAction::MoveToLocationAction(XmlReader *pReader)
{
    pReader->StartElement("MoveToLocationAction");
    newAreaId = pReader->ReadTextElement("NewAreaId");
    newLocationId = pReader->ReadTextElement("NewLocationId");
    transitionId = pReader->ReadTextElement("TransitionId");
    pReader->EndElement();
}

void Conversation::MoveToZoomedViewAction::Execute(State *pState)
{
    pState->SetTransitionRequest(TransitionRequest(zoomedViewId));
}

Conversation::MoveToZoomedViewAction::MoveToZoomedViewAction(XmlReader *pReader)
{
    pReader->StartElement("MoveToZoomedViewAction");
    zoomedViewId = pReader->ReadTextElement("ZoomedViewId");
    pReader->EndElement();
}

void Conversation::EndCaseAction::Execute(State *pState)
{
    SaveDialogsSeenListForCase(Case::GetInstance()->GetUuid());
    Case::GetInstance()->SetIsFinished(true);

    if (completesCase)
    {
        SaveCompletedCase(Case::GetInstance()->GetUuid());
    }
}

Conversation::EndCaseAction::EndCaseAction(XmlReader *pReader)
{
    pReader->StartElement("EndCaseAction");
    completesCase = pReader->ReadBooleanElement("CompletesCase");
    pReader->EndElement();
}

Conversation::BeginMultipleChoiceAction::~BeginMultipleChoiceAction()
{
    delete pButtonArray;
    pButtonArray = NULL;
    delete pStaticDialog;
    pStaticDialog = NULL;
    delete pDarkeningImageEaseIn;
    pDarkeningImageEaseIn = NULL;
    delete pDarkeningImageEaseOut;
    pDarkeningImageEaseOut = NULL;

    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
}

void Conversation::BeginMultipleChoiceAction::Begin(State *pState)
{
    ContinuousAction::Begin(pState);
    pState->SetCanFastForward(false);

    vector<ButtonArrayLoadParameters> loadParametersList;

    for (unsigned int i = 0; i < optionTexts.size(); i++)
    {
        ButtonArrayLoadParameters loadParameters;
        loadParameters.text = optionTexts[i];
        loadParametersList.push_back(loadParameters);
    }

    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
    EventProviders::GetButtonArrayEventProvider()->RegisterListener(this);

    pButtonArray->Load(loadParametersList);
    pButtonArray->Show();

    pStaticDialog = Dialog::CreateForString(pState->GetLastShowDialogRawDialogText());
    pStaticDialog->Begin(pState);

    pDarkeningImageEaseIn->Reset();
    pDarkeningImageEaseOut->Reset();
    pDarkeningImageEaseIn->Begin();

    pState->SetCurrentSpeakerName(pState->GetLastShowDialogSpeakerName());
}

void Conversation::BeginMultipleChoiceAction::Update(int delta)
{
    if (GetIsFinished())
    {
        return;
    }

    if (!pDarkeningImageEaseIn->GetIsFinished())
    {
        pDarkeningImageEaseIn->Update(delta);
        darkeningImageOpacity = pDarkeningImageEaseIn->GetCurrentValue();
    }
    else if (pDarkeningImageEaseOut->GetIsStarted() && !pDarkeningImageEaseOut->GetIsFinished())
    {
        pDarkeningImageEaseOut->Update(delta);
        darkeningImageOpacity = pDarkeningImageEaseOut->GetCurrentValue();
    }

    pButtonArray->Update(delta);
    pStaticDialog->Update(delta);

    if (shouldCloseButtonArray)
    {
        shouldCloseButtonArray = false;
        pButtonArray->Close();

        if (!pDarkeningImageEaseIn->GetIsFinished())
        {
            pDarkeningImageEaseIn->Finish();
        }

        pDarkeningImageEaseOut->Begin();
    }

    if (pButtonArray->GetIsClosed())
    {
        pState->SetActionIndex(nextIndex);
        SetIsFinished(true);
    }
}

void Conversation::BeginMultipleChoiceAction::Draw(double xOffset, double yOffset)
{
    pButtonArray->Draw(xOffset, yOffset);
    pStaticDialog->Draw(xOffset, yOffset);
}

void Conversation::BeginMultipleChoiceAction::DrawBackground(double xOffset, double yOffset)
{
    pDarkeningImage->Draw(
        Vector2(0, 0),
        Color(darkeningImageOpacity, 1.0, 1.0, 1.0));
}

void Conversation::BeginMultipleChoiceAction::Reset()
{
    ContinuousAction::Reset();

    pButtonArray->Reset();
    shouldCloseButtonArray = false;
    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);

    delete pStaticDialog;
    pStaticDialog = NULL;
}

void Conversation::BeginMultipleChoiceAction::OnButtonArrayButtonClicked(ButtonArray *pSender, int id)
{
    if (pSender != pButtonArray)
    {
        return;
    }

    nextIndex = optionIndexes[id];
    pButtonArray->ReorderOutAnimations(id);
    shouldCloseButtonArray = true;
}

void Conversation::BeginMultipleChoiceAction::OnButtonArrayCanceled(ButtonArray *pSender)
{

}

Conversation::BeginMultipleChoiceAction::BeginMultipleChoiceAction(XmlReader *pReader)
{
    pReader->StartElement("BeginMultipleChoiceAction");

    pReader->StartElement("OptionTexts");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        optionTexts.push_back(pReader->ReadTextElement("Text"));
    }

    pReader->EndElement();

    pReader->StartElement("OptionIndexes");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        optionIndexes.push_back(pReader->ReadIntElement("Index"));
    }

    pReader->EndElement();

    pReader->EndElement();

    if (optionTexts.size() != optionIndexes.size())
    {
        throw MLIException("optionTexts and optionIndexes must be the same size.");
    }

    pButtonArray = new ButtonArray((int)optionTexts.size(), 0, 0, 960, 360, 30);
    shouldCloseButtonArray = false;
    nextIndex = -1;
    pStaticDialog = NULL;

    pDarkeningImage = CommonCaseResources::GetInstance()->GetSpriteManager()->GetImageFromId("MultipleChoiceDarkening");
    darkeningImageOpacity = 0;
    pDarkeningImageEaseIn = new LinearEase(0.0, 1.0, 125 * (int)optionTexts.size());
    pDarkeningImageEaseOut = new LinearEase(1.0, 0.0, 125 * (int)optionTexts.size());
}

void Conversation::BeginMultipleChoiceOptionAction::Execute(State *pState)
{
}

Conversation::BeginMultipleChoiceOptionAction::BeginMultipleChoiceOptionAction(XmlReader *pReader)
{
    pReader->StartElement("BeginMultipleChoiceOptionAction");
    pReader->EndElement();
}

void Conversation::ExitMultipleChoiceAction::Execute(State *pState)
{
    pState->SetActionIndex(exitIndex);
}

Conversation::ExitMultipleChoiceAction::ExitMultipleChoiceAction(XmlReader *pReader)
{
    pReader->StartElement("ExitMultipleChoiceAction");
    exitIndex = pReader->ReadIntElement("ExitIndex");
    pReader->EndElement();
}

void Conversation::EndMultipleChoiceOptionAction::Execute(State *pState)
{
    pState->SetActionIndex(startIndex);
}

Conversation::EndMultipleChoiceOptionAction::EndMultipleChoiceOptionAction(XmlReader *pReader)
{
    pReader->StartElement("EndMultipleChoiceOptionAction");
    startIndex = pReader->ReadIntElement("StartIndex");
    pReader->EndElement();
}

void Conversation::EndMultipleChoiceAction::Execute(State *pState)
{
}

Conversation::EndMultipleChoiceAction::EndMultipleChoiceAction(XmlReader *pReader)
{
    pReader->StartElement("EndMultipleChoiceAction");
    pReader->EndElement();
}

void Conversation::EnableFastForwardAction::Execute(State *pState)
{
    pState->SetFastForwardEnabled(true);
}

Conversation::EnableFastForwardAction::EnableFastForwardAction(XmlReader *pReader)
{
    pReader->StartElement("EnableFastForwardAction");
    pReader->EndElement();
}

void Conversation::DisableFastForwardAction::Execute(State *pState)
{
    pState->SetFastForwardEnabled(false);
}

Conversation::DisableFastForwardAction::DisableFastForwardAction(XmlReader *pReader)
{
    pReader->StartElement("DisableFastForwardAction");
    pReader->EndElement();
}

void Conversation::BeginBreakdownAction::Begin(State *pState)
{
    Conversation::ContinuousAction::Begin(pState);
    pState->SetBreakdownActivePosition(characterPosition);
}

void Conversation::BeginBreakdownAction::Update(int delta)
{
    if (pState->GetBreakdownTransitionComplete())
    {
        pState->SetBreakdownTransitionComplete(false);
        SetIsFinished(true);
    }
}

void Conversation::BeginBreakdownAction::Draw(double xOffset, double yOffset)
{
    // Nothing to draw - the breakdown is being drawn by the encounter.
}

Conversation::BeginBreakdownAction::BeginBreakdownAction(XmlReader *pReader)
{
    pReader->StartElement("BeginBreakdownAction");
    characterPosition = StringToCharacterPosition(pReader->ReadTextElement("CharacterPosition"));
    pReader->EndElement();
}

void Conversation::EndBreakdownAction::Begin(State *pState)
{
    Conversation::ContinuousAction::Begin(pState);
    pState->SetBreakdownActivePosition(CharacterPositionNone);
}

void Conversation::EndBreakdownAction::Update(int delta)
{
    if (pState->GetBreakdownTransitionComplete())
    {
        pState->SetBreakdownTransitionComplete(false);
        SetIsFinished(true);
    }
}

void Conversation::EndBreakdownAction::Draw(double xOffset, double yOffset)
{
    // Nothing to draw - the breakdown is being drawn by the encounter.
}

Conversation::EndBreakdownAction::EndBreakdownAction(XmlReader *pReader)
{
    pReader->StartElement("EndBreakdownAction");
    pReader->EndElement();
}

Interrogation::Interrogation(XmlReader *pReader)
{
    Initialize();
    pReader->StartElement("Interrogation");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Conversation::Action * Interrogation::GetActionForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("BeginInterrogationRepeatAction"))
    {
        return new BeginInterrogationRepeatAction(pReader);
    }
    else if (pReader->ElementExists("ShowInterrogationAction"))
    {
        return new ShowInterrogationAction(pReader);
    }
    else if (pReader->ElementExists("BeginPressForInfoAction"))
    {
        return new BeginPressForInfoAction(pReader);
    }
    else if (pReader->ElementExists("EndPressForInfoAction"))
    {
        return new EndPressForInfoAction(pReader);
    }
    else if (pReader->ElementExists("BeginPresentEvidenceAction"))
    {
        return new BeginPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("EndPresentEvidenceAction"))
    {
        return new EndPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("BeginUsePartnerAction"))
    {
        return new BeginUsePartnerAction(pReader);
    }
    else if (pReader->ElementExists("EndUsePartnerAction"))
    {
        return new EndUsePartnerAction(pReader);
    }
    else if (pReader->ElementExists("EndShowInterrogationBranchesAction"))
    {
        return new EndShowInterrogationBranchesAction(pReader);
    }
    else if (pReader->ElementExists("ExitInterrogationRepeatAction"))
    {
        return new ExitInterrogationRepeatAction(pReader);
    }
    else if (pReader->ElementExists("SkipWrongPartnerUsedAction"))
    {
        return new SkipWrongPartnerUsedAction(pReader);
    }
    else if (pReader->ElementExists("BeginWrongPartnerUsedAction"))
    {
        return new BeginWrongPartnerUsedAction(pReader);
    }
    else if (pReader->ElementExists("EndWrongPartnerUsedAction"))
    {
        return new EndWrongPartnerUsedAction(pReader);
    }
    else if (pReader->ElementExists("EndInterrogationRepeatAction"))
    {
        return new EndInterrogationRepeatAction(pReader);
    }
    else
    {
        return Conversation::GetActionForNextElement(pReader);
    }
}

void Interrogation::BeginInterrogationRepeatAction::Execute(State *pState)
{
    if (pCachedState == NULL)
    {
        pCachedState = pState->CreateCachedState();
    }
    else
    {
        pState->RestoreFromState(pCachedState);
    }
}

Interrogation::BeginInterrogationRepeatAction::BeginInterrogationRepeatAction(XmlReader *pReader)
{
    pCachedState = NULL;

    pReader->StartElement("BeginInterrogationRepeatAction");
    pReader->EndElement();
}

Interrogation::BeginInterrogationRepeatAction::~BeginInterrogationRepeatAction()
{
    delete pCachedState;
    pCachedState = NULL;
}

bool Interrogation::ShowInterrogationAction::GetShouldSkip()
{
    return this->conditionFlag.length() > 0 && !Case::GetInstance()->GetFlagManager()->IsFlagSet(this->conditionFlag);
}

void Interrogation::ShowInterrogationAction::GoToNext(State *pState)
{
    if (pState->GetLastNavigationDirection() == DirectNavigationDirectionBack)
    {
        pState->SetActionIndex(GetPreviousInterrogationIndex());
    }
    else
    {
        pState->SetActionIndex(GetNextInterrogationIndex() >= 0 ? GetNextInterrogationIndex() : GetInterrogationFinishIndex());
    }
}

void Interrogation::ShowInterrogationAction::Begin(State *pState)
{
    Conversation::ShowDialogAction::Begin(pState);
    pState->SetCanFastForward(false);

    nextIndexSet = false;

    pState->RestoreCachedState();

    if (pCachedState == NULL)
    {
        pCachedState = pState->CreateCachedState();
    }
    else
    {
        pState->RestoreFromState(pCachedState);
    }

    // If we directly navigated here in an interrogation,
    // we should just immediately display all of the text.
    if (pState->GetLastNavigationDirection() != DirectNavigationDirectionNone)
    {
        pDialog->Finish(false /* shouldPlaySfx */ );
        pState->SetLastNavigationDirection(DirectNavigationDirectionNone);
    }
}

void Interrogation::ShowInterrogationAction::Update(int delta)
{
    Conversation::ShowDialogAction::Update(delta);

    if (GetIsFinished() && !nextIndexSet)
    {
        int nextIndex = -1;

        if (GetNextInterrogationIndex() >= 0)
        {
            nextIndex = GetNextInterrogationIndex();
        }
        else if (GetInterrogationFinishIndex() >= 0)
        {
            nextIndex = GetInterrogationFinishIndex();
        }
        else
        {
            throw MLIException("At least one of NextInterrogationIndex and InterrogationFinishIndex must be set.");
        }

        // If we're done and the next index hasn't been set yet,
        // set it to the next dialog index.
        pState->SetActionIndex(nextIndex);
        nextIndexSet = true;
    }
}

void Interrogation::ShowInterrogationAction::Draw(double xOffset, double yOffset)
{
    Conversation::ShowDialogAction::Draw(xOffset, yOffset);
}

void Interrogation::ShowInterrogationAction::Reset()
{
    Conversation::ShowDialogAction::Reset();

    nextIndexSet = false;

    pState->CacheState();
}

void Interrogation::ShowInterrogationAction::OnDialogDirectlyNavigated(Dialog *pSender, DirectNavigationDirection direction)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (direction == DirectNavigationDirectionBack)
    {
        if (GetPreviousInterrogationIndex() < 0)
        {
            throw MLIException("Cannot navigate back when there is nowhere to navigate back to.");
        }

        pState->SetActionIndex(GetPreviousInterrogationIndex());
        nextIndexSet = true;
        SetIsFinished(true);
    }
    else if (direction == DirectNavigationDirectionForward)
    {
        pState->SetActionIndex(GetNextInterrogationIndex());
        nextIndexSet = true;
        SetIsFinished(true);
    }

    pState->SetLastNavigationDirection(direction);
}

void Interrogation::ShowInterrogationAction::OnDialogPressForInfoClicked(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    DialogCharacter *pPlayerCharacter = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPlayerCharacterId());

    pState->SetActionIndexAfterInterjection(GetPressForInfoIndex());
    pState->StartInterjection(pPlayerCharacter->GetPressStatementSprite());
    playSound("Interjection");
    playDialog(pPlayerCharacter->GetPressStatementSoundId());

    nextIndexSet = true;
}

void Interrogation::ShowInterrogationAction::OnDialogEvidencePresented(Dialog *pSender, const string &evidenceId)
{
    if (pSender != pDialog)
    {
        return;
    }

    DialogCharacter *pPlayerCharacter = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPlayerCharacterId());

    if (GetEvidencePresentedIndexes().count(evidenceId) > 0)
    {
        playSound("Interjection");

        pState->SetActionIndexAfterInterjection(GetEvidencePresentedIndexes()[evidenceId]);
        pState->StartInterjection(Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPlayerCharacterId())->GetPresentCorrectEvidenceSprite());
        playDialog(pPlayerCharacter->GetPresentCorrectEvidenceSoundId());

        nextIndexSet = true;
    }
    else
    {
        pState->SetActionIndexAfterInterjection(GetWrongEvidencePresentedIndex());
        pState->StartInterjection(Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(Case::GetInstance()->GetPlayerCharacterId())->GetPresentIncorrectEvidenceSprite());
        playDialog(pPlayerCharacter->GetPresentIncorrectEvidenceSoundId());

        nextIndexSet = true;
    }
}

void Interrogation::ShowInterrogationAction::OnDialogUsePartner(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    if (GetPartnerUsedIndexes().count(Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId()) > 0)
    {
        pState->SetActionIndex(GetPartnerUsedIndexes()[Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId()]);
    }
    else
    {
        pState->SetActionIndex(GetWrongPartnerUsedIndex());
    }

    nextIndexSet = true;
    SetIsFinished(true);
}

void Interrogation::ShowInterrogationAction::OnDialogEndRequested(Dialog *pSender)
{
    if (pSender != pDialog)
    {
        return;
    }

    pState->SetActionIndex(GetEndRequestedIndex());

    nextIndexSet = true;
    SetIsFinished(true);
}

void Interrogation::ShowInterrogationAction::LoadFromXmlCore(XmlReader *pReader)
{
    pCachedState = NULL;

    Conversation::ShowDialogAction::LoadFromXmlCore(pReader);
    pReader->StartElement("EvidencePresentedIndexes");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string evidenceId = pReader->ReadTextElement("EvidenceId");
        int index = pReader->ReadIntElement("Index");
        evidencePresentedIndexes[evidenceId] = index;
    }

    pReader->EndElement();

    pReader->StartElement("PartnerUsedIndexes");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string partnerId = pReader->ReadTextElement("PartnerId");
        int index = pReader->ReadIntElement("Index");
        partnerUsedIndexes[partnerId] = index;
    }

    pReader->EndElement();

    if (pReader->ElementExists("ConditionFlag"))
    {
        conditionFlag = pReader->ReadTextElement("ConditionFlag");
    }

    previousInterrogationIndex = pReader->ReadIntElement("PreviousInterrogationIndex");
    nextInterrogationIndex = pReader->ReadIntElement("NextInterrogationIndex");
    interrogationFinishIndex = pReader->ReadIntElement("InterrogationFinishIndex");
    pressForInfoIndex = pReader->ReadIntElement("PressForInfoIndex");
    wrongEvidencePresentedIndex = pReader->ReadIntElement("WrongEvidencePresentedIndex");
    wrongPartnerUsedIndex = pReader->ReadIntElement("WrongPartnerUsedIndex");
    endRequestedIndex = pReader->ReadIntElement("EndRequestedIndex");
}

Interrogation::ShowInterrogationAction::ShowInterrogationAction(XmlReader *pReader)
{
    pReader->StartElement("ShowInterrogationAction");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Interrogation::ShowInterrogationAction::~ShowInterrogationAction()
{
    delete pCachedState;
    pCachedState = NULL;
}

void Interrogation::BeginPressForInfoAction::Execute(State *pState)
{

}

Interrogation::BeginPressForInfoAction::BeginPressForInfoAction(XmlReader *pReader)
{
    pReader->StartElement("BeginPressForInfoAction");
    pReader->EndElement();
}

void Interrogation::EndPressForInfoAction::Execute(State *pState)
{
    pState->SetActionIndex(GetNextDialogIndex());
}

Interrogation::EndPressForInfoAction::EndPressForInfoAction(XmlReader *pReader)
{
    pReader->StartElement("EndPressForInfoAction");
    nextDialogIndex = pReader->ReadIntElement("NextDialogIndex");
    pReader->EndElement();
}

void Interrogation::BeginPresentEvidenceAction::Execute(State *pState)
{

}

Interrogation::BeginPresentEvidenceAction::BeginPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("BeginPresentEvidenceAction");
    pReader->EndElement();
}

void Interrogation::EndPresentEvidenceAction::Execute(State *pState)
{
    pState->SetActionIndex(GetNextDialogIndex());
}

Interrogation::EndPresentEvidenceAction::EndPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("EndPresentEvidenceAction");
    nextDialogIndex = pReader->ReadIntElement("NextDialogIndex");
    pReader->EndElement();
}

void Interrogation::BeginUsePartnerAction::Execute(State *pState)
{

}

Interrogation::BeginUsePartnerAction::BeginUsePartnerAction(XmlReader *pReader)
{
    pReader->StartElement("BeginUsePartnerAction");
    pReader->EndElement();
}

void Interrogation::EndUsePartnerAction::Execute(State *pState)
{
    pState->SetActionIndex(GetNextDialogIndex());
}

Interrogation::EndUsePartnerAction::EndUsePartnerAction(XmlReader *pReader)
{
    pReader->StartElement("EndUsePartnerAction");
    nextDialogIndex = pReader->ReadIntElement("NextDialogIndex");
    pReader->EndElement();
}

void Interrogation::EndShowInterrogationBranchesAction::Execute(State *pState)
{

}

Interrogation::EndShowInterrogationBranchesAction::EndShowInterrogationBranchesAction(XmlReader *pReader)
{
    pReader->StartElement("EndShowInterrogationBranchesAction");
    pReader->EndElement();
}

void Interrogation::ExitInterrogationRepeatAction::Execute(State *pState)
{
    pState->SetActionIndex(GetAfterEndInterrogationRepeatIndex());
}

Interrogation::ExitInterrogationRepeatAction::ExitInterrogationRepeatAction(XmlReader *pReader)
{
    pReader->StartElement("ExitInterrogationRepeatAction");
    afterEndInterrogationRepeatIndex = pReader->ReadIntElement("AfterEndInterrogationRepeatIndex");
    pReader->EndElement();
}

void Interrogation::SkipWrongPartnerUsedAction::Execute(State *pState)
{
    pState->SetActionIndex(GetAfterWrongPartnerUsedIndex());
}

Interrogation::SkipWrongPartnerUsedAction::SkipWrongPartnerUsedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipWrongPartnerUsedAction");
    afterWrongPartnerUsedIndex = pReader->ReadIntElement("AfterWrongPartnerUsedIndex");
    pReader->EndElement();
}

void Interrogation::BeginWrongPartnerUsedAction::Execute(State *pState)
{
    pState->CachePreviousActionIndex();
}

Interrogation::BeginWrongPartnerUsedAction::BeginWrongPartnerUsedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginWrongPartnerUsedAction");
    pReader->EndElement();
}

void Interrogation::EndWrongPartnerUsedAction::Execute(State *pState)
{
    pState->RestoreActionIndexFromCache();
}

Interrogation::EndWrongPartnerUsedAction::EndWrongPartnerUsedAction(XmlReader *pReader)
{
    pReader->StartElement("EndWrongPartnerUsedAction");
    pReader->EndElement();
}

void Interrogation::EndInterrogationRepeatAction::Execute(State *pState)
{
    pState->SetActionIndex(GetBeginInterrogationRepeatIndex());
}

Interrogation::EndInterrogationRepeatAction::EndInterrogationRepeatAction(XmlReader *pReader)
{
    pReader->StartElement("EndInterrogationRepeatAction");
    beginInterrogationRepeatIndex = pReader->ReadIntElement("BeginInterrogationRepeatIndex");
    pReader->EndElement();
}

Confrontation::Topic::Topic(const string &id, const string &name, int actionIndex)
    : id(id)
    , name(name)
    , actionIndex(actionIndex)
    , isEnabled(false)
    , isCompleted(false)
{
}

Confrontation::Topic::Topic(XmlReader *pReader)
{
    pReader->StartElement("ConfrontationTopic");

    if (pReader->ElementExists("Id"))
    {
        id = pReader->ReadTextElement("Id");
    }

    if (pReader->ElementExists("Name"))
    {
        name = pReader->ReadTextElement("Name");
    }

    actionIndex = pReader->ReadIntElement("ActionIndex");
    isEnabled = pReader->ReadBooleanElement("IsEnabled");
    isCompleted = false;
    pReader->EndElement();
}

Confrontation::Confrontation()
    : pCurrentBeginConfrontationTopicSelectionAction(NULL)
    , currentBeginConfrontationTopicSelectionActionIndex(-1)
    , playerIconOffset(Vector2(0, 0))
    , opponentIconOffset(Vector2(0, 0))
{
    Conversation::Initialize();

    pIconOffsetEasingFunction = NULL;
    pPlayerHealthNumberScaleEasingFunction = NULL;
    pPlayerHealthNumberNotRedRgbValueEasingFunction = NULL;
    pOpponentHealthNumberScaleEasingFunction = NULL;
    pOpponentHealthNumberNotRedRgbValueEasingFunction = NULL;

    pPlayerCharacter = NULL;
    pOpponentCharacter = NULL;

    pCurrentBeginConfrontationTopicSelectionAction = NULL;
    pCurrentSkipPlayerDefeatedAction = NULL;
    pPlayerIcon = NULL;
    pOpponentIcon = NULL;

    initialPlayerHealth = 0;
    initialOpponentHealth = 0;

    backgroundDarkeningOpacity = 0;
    pBackgroundDarkeningOpacityEaseIn = NULL;
    pBackgroundDarkeningOpacityEaseOut = NULL;
}

Confrontation::Confrontation(XmlReader *pReader)
    : pCurrentBeginConfrontationTopicSelectionAction(NULL)
    , currentBeginConfrontationTopicSelectionActionIndex(-1)
    , playerIconOffset(Vector2(0, 0))
    , opponentIconOffset(Vector2(0, 0))
{
    Conversation::Initialize();

    backgroundDarkeningOpacity = 0;
    pBackgroundDarkeningOpacityEaseIn = new LinearEase(0, 1, 500);
    pBackgroundDarkeningOpacityEaseOut = new LinearEase(1, 0, 500);

    pReader->StartElement("Confrontation");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Confrontation::~Confrontation()
{
    delete pIconOffsetEasingFunction;
    pIconOffsetEasingFunction = NULL;
    delete pPlayerHealthNumberScaleEasingFunction;
    pPlayerHealthNumberScaleEasingFunction = NULL;
    delete pPlayerHealthNumberNotRedRgbValueEasingFunction;
    pPlayerHealthNumberNotRedRgbValueEasingFunction = NULL;
    delete pOpponentHealthNumberScaleEasingFunction;
    pOpponentHealthNumberScaleEasingFunction = NULL;
    delete pOpponentHealthNumberNotRedRgbValueEasingFunction;
    pOpponentHealthNumberNotRedRgbValueEasingFunction = NULL;

    pPlayerCharacter = NULL;
    pOpponentCharacter = NULL;

    delete pBackgroundDarkeningOpacityEaseIn;
    pBackgroundDarkeningOpacityEaseIn = NULL;
    delete pBackgroundDarkeningOpacityEaseOut;
    pBackgroundDarkeningOpacityEaseOut = NULL;
}

DialogCharacter * Confrontation::GetPlayerCharacter()
{
    if (pPlayerCharacter == NULL)
    {
        pPlayerCharacter = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(playerCharacterId);
    }

    return pPlayerCharacter;
}

DialogCharacter * Confrontation::GetOpponentCharacter()
{
    if (pOpponentCharacter == NULL)
    {
        pOpponentCharacter = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(opponentCharacterId);
    }

    return pOpponentCharacter;
}

void Confrontation::Initialize(Image *pBackgroundDarkeningImage, Image *pConfrontationHealthCircleImage, MLIFont *pConfrontationHealthNumberingFont)
{
    Confrontation::pBackgroundDarkeningImage = pBackgroundDarkeningImage;
    Confrontation::pConfrontationHealthCircleImage = pConfrontationHealthCircleImage;
    Confrontation::pConfrontationHealthNumberingFont = pConfrontationHealthNumberingFont;
}

Confrontation::Topic * Confrontation::GetConfrontationTopicFromId(const string &id)
{
    Confrontation::Topic *pTopic = NULL;

    if (pCurrentConfrontationTopicsById != NULL)
    {
        pTopic = (*pCurrentConfrontationTopicsById)[id];
    }

    return pTopic;
}

int Confrontation::GetEnabledConfrontationTopicCount()
{
    int topicCount = 0;

    if (pCurrentConfrontationTopicsById != NULL)
    {
        for (map<string, Confrontation::Topic *>::iterator iter = pCurrentConfrontationTopicsById->begin(); iter != pCurrentConfrontationTopicsById->end(); ++iter)
        {
            if (iter->second->GetIsEnabled() && !iter->second->GetIsCompleted())
            {
                topicCount++;
            }
        }
    }

    return topicCount;
}

bool Confrontation::GetShouldUpdateActions()
{
    return
        (!pBackgroundDarkeningOpacityEaseIn->GetIsStarted() || pBackgroundDarkeningOpacityEaseIn->GetIsFinished()) &&
        (!pBackgroundDarkeningOpacityEaseOut->GetIsStarted() || pBackgroundDarkeningOpacityEaseOut->GetIsFinished());
}

void Confrontation::Begin(State *pState)
{
    Interrogation::Begin(pState);

    SetPlayerHealth(GetInitialPlayerHealth());
    SetOpponentHealth(GetInitialOpponentHealth());
    iconOffset = IconOffscreenOffset;
    healthIconsShowing = false;

    playerHealthNumberScale = 1;
    playerHealthNumberNotRedRgbValue = 255;
    opponentHealthNumberScale = 1;
    opponentHealthNumberNotRedRgbValue = 255;

    pState->SetCurrentConfrontation(this);
}

void Confrontation::Update(int delta)
{
    Interrogation::Update(delta);

    if (pIconOffsetEasingFunction != NULL)
    {
        pIconOffsetEasingFunction->Update(delta);
        iconOffset = (int)pIconOffsetEasingFunction->GetCurrentValue();

        if (pIconOffsetEasingFunction->GetIsFinished())
        {
            delete pIconOffsetEasingFunction;
            pIconOffsetEasingFunction = NULL;
        }
    }

    if (pPlayerHealthNumberScaleEasingFunction != NULL)
    {
        pPlayerHealthNumberScaleEasingFunction->Update(delta);
        playerHealthNumberScale = pPlayerHealthNumberScaleEasingFunction->GetCurrentValue();

        if (pPlayerHealthNumberScaleEasingFunction->GetIsFinished())
        {
            delete pPlayerHealthNumberScaleEasingFunction;
            pPlayerHealthNumberScaleEasingFunction = NULL;
        }
    }

    if (pPlayerHealthNumberNotRedRgbValueEasingFunction != NULL)
    {
        pPlayerHealthNumberNotRedRgbValueEasingFunction->Update(delta);
        playerHealthNumberNotRedRgbValue = (int)pPlayerHealthNumberNotRedRgbValueEasingFunction->GetCurrentValue();

        if (pPlayerHealthNumberNotRedRgbValueEasingFunction->GetIsFinished())
        {
            delete pPlayerHealthNumberNotRedRgbValueEasingFunction;
            pPlayerHealthNumberNotRedRgbValueEasingFunction = NULL;
        }
    }

    if (pOpponentHealthNumberScaleEasingFunction != NULL)
    {
        pOpponentHealthNumberScaleEasingFunction->Update(delta);
        opponentHealthNumberScale = pOpponentHealthNumberScaleEasingFunction->GetCurrentValue();

        if (pOpponentHealthNumberScaleEasingFunction->GetIsFinished())
        {
            delete pOpponentHealthNumberScaleEasingFunction;
            pOpponentHealthNumberScaleEasingFunction = NULL;
        }
    }

    if (pOpponentHealthNumberNotRedRgbValueEasingFunction != NULL)
    {
        pOpponentHealthNumberNotRedRgbValueEasingFunction->Update(delta);
        opponentHealthNumberNotRedRgbValue = (int)pOpponentHealthNumberNotRedRgbValueEasingFunction->GetCurrentValue();

        if (pOpponentHealthNumberNotRedRgbValueEasingFunction->GetIsFinished())
        {
            delete pOpponentHealthNumberNotRedRgbValueEasingFunction;
            pOpponentHealthNumberNotRedRgbValueEasingFunction = NULL;
        }
    }

    if (pBackgroundDarkeningOpacityEaseIn->GetIsStarted() && !pBackgroundDarkeningOpacityEaseIn->GetIsFinished())
    {
        pBackgroundDarkeningOpacityEaseIn->Update(delta);
        backgroundDarkeningOpacity = pBackgroundDarkeningOpacityEaseIn->GetCurrentValue();

        if (pBackgroundDarkeningOpacityEaseIn->GetIsFinished())
        {
            Case::GetInstance()->LoadCachedState();
            pState->RestoreCachedStateForConfrontationRestart();
            pState->SetActionIndex(confrontationBeginIndex);

            pPlayerIcon = pCachedPlayerIcon;
            pOpponentIcon = pCachedOpponentIcon;
            playerIconOffset = cachedPlayerIconOffset;
            initialPlayerHealth = cachedInitialPlayerHealth;
            playerHealth = initialPlayerHealth;
            opponentIconOffset = cachedOpponentIconOffset;
            initialOpponentHealth = cachedInitialOpponentHealth;
            opponentHealth = initialOpponentHealth;
            Case::GetInstance()->GetAudioManager()->PlayBgmWithId(cachedInitialBgm);
            Case::GetInstance()->GetAudioManager()->PlayAmbianceWithId(cachedInitialAmbiance);

            ResetTopics();
            pBackgroundDarkeningOpacityEaseOut->Begin();
        }
    }

    if (pBackgroundDarkeningOpacityEaseOut->GetIsStarted() && !pBackgroundDarkeningOpacityEaseOut->GetIsFinished())
    {
        pBackgroundDarkeningOpacityEaseOut->Update(delta);
        backgroundDarkeningOpacity = pBackgroundDarkeningOpacityEaseOut->GetCurrentValue();

        if (pBackgroundDarkeningOpacityEaseOut->GetIsFinished())
        {
            pBackgroundDarkeningOpacityEaseIn->Reset();
            pBackgroundDarkeningOpacityEaseOut->Reset();
        }
    }
}

void Confrontation::Draw(double xOffset, double yOffset)
{
    if (GetPlayerIcon() != NULL && GetOpponentIcon() != NULL)
    {
        Vector2 healthIconLeftCenter = Vector2(90, 90 + TabHeight);
        Vector2 healthIconRightCenter = Vector2(gScreenWidth - 90, 90 + TabHeight);

        Vector2 circleOffset = Vector2(pConfrontationHealthCircleImage->width / 2, pConfrontationHealthCircleImage->height / 2);
        Vector2 leftIconStandardOffset = Vector2(GetPlayerIcon()->GetWidth() / 2, GetPlayerIcon()->GetHeight() / 2);
        Vector2 rightIconStandardOffset = Vector2(GetOpponentIcon()->GetWidth() / 2, GetOpponentIcon()->GetHeight() / 2);
        Vector2 healthNumberOffset = Vector2(0, 3);
        Vector2 horizontalOffset = Vector2(iconOffset, 0);

        char playerHealthCStr[16] = { '\0' };
        char opponentHealthCStr[16] = { '\0' };

        sprintf(&playerHealthCStr[0], "%d", GetPlayerHealth());
        sprintf(&opponentHealthCStr[0], "%d", GetOpponentHealth());

        string playerHealthString = playerHealthCStr;
        string opponentHealthString = opponentHealthCStr;

        pConfrontationHealthCircleImage->Draw(healthIconLeftCenter - circleOffset - horizontalOffset);
        GetPlayerIcon()->Draw((healthIconLeftCenter - leftIconStandardOffset + GetOpponentIconOffset()) - horizontalOffset);

        pConfrontationHealthNumberingFont->Draw(
            playerHealthString,
            healthIconLeftCenter - (Vector2(pConfrontationHealthNumberingFont->GetWidth(playerHealthString),
                                            pConfrontationHealthNumberingFont->GetLineHeight()) * (playerHealthNumberScale / 2)) + healthNumberOffset - horizontalOffset,
            Color(1.0, 1.0, playerHealthNumberNotRedRgbValue / 255.0, playerHealthNumberNotRedRgbValue / 255.0),
            playerHealthNumberScale);

        pConfrontationHealthCircleImage->Draw(healthIconRightCenter - circleOffset + horizontalOffset);
        GetOpponentIcon()->Draw(
            healthIconRightCenter - rightIconStandardOffset + Vector2(-GetPlayerIconOffset().GetX(), GetPlayerIconOffset().GetY()) + horizontalOffset,
            Color::White,
            1.0 /* scale */,
            true /* flipHorizontally */);

        pConfrontationHealthNumberingFont->Draw(
            opponentHealthString,
            healthIconRightCenter - (Vector2(pConfrontationHealthNumberingFont->GetWidth(opponentHealthString),
                                             pConfrontationHealthNumberingFont->GetLineHeight()) * (opponentHealthNumberScale / 2)) + healthNumberOffset + horizontalOffset,
            Color(1.0, 1.0, opponentHealthNumberNotRedRgbValue / 255.0, opponentHealthNumberNotRedRgbValue / 255.0),
            opponentHealthNumberScale);
    }

    Interrogation::Draw(xOffset, yOffset);

    pBackgroundDarkeningImage->Draw(Vector2(0, 0), Color(backgroundDarkeningOpacity, 1.0, 1.0, 1.0));
}

void Confrontation::ResetTopics()
{
    for (unsigned int i = 0; i < actionList.size(); i++)
    {
        actionList[i]->ResetTopics();
    }
}

void Confrontation::ShowHealthIcons()
{
    if (!healthIconsShowing)
    {
        delete pIconOffsetEasingFunction;
        pIconOffsetEasingFunction = new QuadraticEase(IconOffscreenOffset, 0, 250);
        pIconOffsetEasingFunction->Begin();

        healthIconsShowing = true;
    }
}

void Confrontation::HideHealthIcons()
{
    if (healthIconsShowing)
    {
        delete pIconOffsetEasingFunction;
        pIconOffsetEasingFunction = new QuadraticEase(0, IconOffscreenOffset, 250);
        pIconOffsetEasingFunction->Begin();

        healthIconsShowing = false;
    }
}

void Confrontation::DamagePlayer()
{
    if (GetPlayerHealth() > 0)
    {
        SetPlayerHealth(GetPlayerHealth() - 1);

        delete pPlayerHealthNumberScaleEasingFunction;
        pPlayerHealthNumberScaleEasingFunction = new QuadraticEase(HealthNumberDamageScale, 1, 250);
        delete pPlayerHealthNumberNotRedRgbValueEasingFunction;
        pPlayerHealthNumberNotRedRgbValueEasingFunction = new LinearEase(0, 255, 250);
        pPlayerHealthNumberScaleEasingFunction->Begin();
        pPlayerHealthNumberNotRedRgbValueEasingFunction->Begin();

        if (GetPlayerHealth() == 0)
        {
            GetState()->SetActionIndex(GetPlayerDefeatedIndex());
        }
    }
}

void Confrontation::DamageOpponent()
{
    if (GetOpponentHealth() > 0)
    {
        SetOpponentHealth(GetOpponentHealth() - 1);

        delete pOpponentHealthNumberScaleEasingFunction;
        pOpponentHealthNumberScaleEasingFunction = new QuadraticEase(HealthNumberDamageScale, 1, 250);
        delete pOpponentHealthNumberNotRedRgbValueEasingFunction;
        pOpponentHealthNumberNotRedRgbValueEasingFunction = new LinearEase(0, 255, 250);
        pOpponentHealthNumberScaleEasingFunction->Begin();
        pOpponentHealthNumberNotRedRgbValueEasingFunction->Begin();
    }
}

void Confrontation::CacheState()
{
    pCachedPlayerIcon = pPlayerIcon;
    pCachedOpponentIcon = pOpponentIcon;
    cachedPlayerIconOffset = playerIconOffset;
    cachedInitialPlayerHealth = initialPlayerHealth;
    cachedOpponentIconOffset = opponentIconOffset;
    cachedInitialOpponentHealth = initialOpponentHealth;
    cachedInitialBgm = getPlayingMusic();
    cachedInitialAmbiance = getPlayingAmbiance();
}

void Confrontation::Restart()
{
    pBackgroundDarkeningOpacityEaseIn->Begin();
    pBackgroundDarkeningOpacityEaseOut->Reset();
}

string Confrontation::GetTopicNameById(const string &topicId)
{
    return confrontationTopicNamesById[topicId];
}

void Confrontation::LoadFromXmlCore(XmlReader *pReader)
{
    Interrogation::LoadFromXmlCore(pReader);

    pIconOffsetEasingFunction = NULL;
    pPlayerHealthNumberScaleEasingFunction = NULL;
    pPlayerHealthNumberNotRedRgbValueEasingFunction = NULL;
    pOpponentHealthNumberScaleEasingFunction = NULL;
    pOpponentHealthNumberNotRedRgbValueEasingFunction = NULL;

    pPlayerCharacter = NULL;
    pOpponentCharacter = NULL;

    pCurrentBeginConfrontationTopicSelectionAction = NULL;
    pCurrentSkipPlayerDefeatedAction = NULL;
    pPlayerIcon = NULL;
    pOpponentIcon = NULL;

    pReader->StartElement("ConfrontationTopicNamesById");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        string confrontationTopicName = pReader->ReadTextElement("ConfrontationTopicName");
        confrontationTopicNamesById[id] = confrontationTopicName;
    }

    initialPlayerHealth = 0;
    initialOpponentHealth = 0;

    pReader->EndElement();
}

Conversation::Action * Confrontation::GetActionForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("SetParticipantsAction"))
    {
        return new SetParticipantsAction(pReader);
    }
    else if (pReader->ElementExists("SetIconOffsetAction"))
    {
        return new SetIconOffsetAction(pReader);
    }
    else if (pReader->ElementExists("SetHealthAction"))
    {
        return new SetHealthAction(pReader);
    }
    else if (pReader->ElementExists("InitializeBeginConfrontationTopicSelectionAction"))
    {
        return new InitializeBeginConfrontationTopicSelectionAction(pReader);
    }
    else if (pReader->ElementExists("BeginConfrontationTopicSelectionAction"))
    {
        return new BeginConfrontationTopicSelectionAction(pReader);
    }
    else if (pReader->ElementExists("BeginConfrontationTopicAction"))
    {
        return new BeginConfrontationTopicAction(pReader);
    }
    else if (pReader->ElementExists("EndConfrontationTopicAction"))
    {
        return new EndConfrontationTopicAction(pReader);
    }
    else if (pReader->ElementExists("SkipPlayerDefeatedAction"))
    {
        return new SkipPlayerDefeatedAction(pReader);
    }
    else if (pReader->ElementExists("BeginPlayerDefeatedAction"))
    {
        return new BeginPlayerDefeatedAction(pReader);
    }
    else if (pReader->ElementExists("EndPlayerDefeatedAction"))
    {
        return new EndPlayerDefeatedAction(pReader);
    }
    else if (pReader->ElementExists("EndConfrontationTopicSelectionAction"))
    {
        return new EndConfrontationTopicSelectionAction(pReader);
    }
    else if (pReader->ElementExists("EnableTopicAction"))
    {
        return new EnableTopicAction(pReader);
    }
    else if (pReader->ElementExists("RestartConfrontationAction"))
    {
        return new RestartConfrontationAction(pReader);
    }
    else
    {
        return Interrogation::GetActionForNextElement(pReader);
    }
}

Sprite * Confrontation::GetPlayerIcon()
{
    if (pPlayerIcon == NULL)
    {
        pPlayerIcon = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(GetPlayerIconSpriteId());
    }

    return pPlayerIcon;
}

Sprite * Confrontation::GetOpponentIcon()
{
    if (pOpponentIcon == NULL)
    {
        pOpponentIcon = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(GetOpponentIconSpriteId());
    }

    return pOpponentIcon;
}

void Confrontation::SetParticipantsAction::Execute(State *pState)
{
    pState->GetCurrentConfrontation()->SetPlayerCharacterId(playerCharacterId);
    pState->GetCurrentConfrontation()->SetOpponentCharacterId(opponentCharacterId);

    pState->GetCurrentConfrontation()->SetPlayerIconSpriteId(pState->GetCurrentConfrontation()->GetPlayerCharacter()->GetHealthBackgroundSpriteId());
    pState->GetCurrentConfrontation()->SetOpponentIconSpriteId(pState->GetCurrentConfrontation()->GetOpponentCharacter()->GetHealthBackgroundSpriteId());
}

Confrontation::SetParticipantsAction::SetParticipantsAction(XmlReader *pReader)
{
    pReader->StartElement("SetParticipantsAction");
    playerCharacterId = pReader->ReadTextElement("PlayerCharacterId");
    opponentCharacterId = pReader->ReadTextElement("OpponentCharacterId");
    pReader->EndElement();
}

void Confrontation::SetIconOffsetAction::Execute(State *pState)
{
    if (isPlayer)
    {
        pState->GetCurrentConfrontation()->SetPlayerIconOffset(offset);
    }
    else
    {
        pState->GetCurrentConfrontation()->SetOpponentIconOffset(offset);
    }
}

Confrontation::SetIconOffsetAction::SetIconOffsetAction(XmlReader *pReader)
{
    pReader->StartElement("SetIconOffsetAction");

    pReader->StartElement("Offset");
    offset = Vector2(pReader);
    pReader->EndElement();

    isPlayer = pReader->ReadBooleanElement("IsPlayer");
    pReader->EndElement();
}

Confrontation::SetHealthAction::SetHealthAction()
{
    initialHealth = 0;
    isPlayer = false;
}

void Confrontation::SetHealthAction::Execute(State *pState)
{
    if (isPlayer)
    {
        pState->GetCurrentConfrontation()->SetInitialPlayerHealth(initialHealth);
    }
    else
    {
        pState->GetCurrentConfrontation()->SetInitialOpponentHealth(initialHealth);
    }
}

Confrontation::SetHealthAction::SetHealthAction(XmlReader *pReader)
{
    pReader->StartElement("SetHealthAction");
    initialHealth = pReader->ReadIntElement("InitialHealth");
    isPlayer = pReader->ReadBooleanElement("IsPlayer");
    pReader->EndElement();
}

Confrontation::InitializeBeginConfrontationTopicSelectionAction::InitializeBeginConfrontationTopicSelectionAction()
{
    pConfrontationEntranceBackgroundVideo = NULL;
    pConfrontationEntranceBlockSprite = NULL;
    pConfrontationEntranceVfxVideo = NULL;

    pCharacterEntranceEase = new ConfrontationEntranceCharacterEase(-gScreenWidth / 2, 0, ConfrontationEntranceAnimationDuration);
    pClipStartEase = new ConfrontationEntranceBlockEase(-gScreenWidth / 2, 0, ConfrontationEntranceAnimationDuration);
    pBlockPositionEase = new ConfrontationEntranceBlockEase(-gScreenWidth / 2, 0, ConfrontationEntranceAnimationDuration);
    pFlashOpacityEase = new ConfrontationEntranceFlashEase(0, 1, ConfrontationEntranceAnimationDuration);
}

Confrontation::InitializeBeginConfrontationTopicSelectionAction::~InitializeBeginConfrontationTopicSelectionAction()
{
    pConfrontationEntranceBackgroundVideo = NULL;
    pConfrontationEntranceBlockSprite = NULL;
    pConfrontationEntranceVfxVideo = NULL;

    delete pCharacterEntranceEase;
    pCharacterEntranceEase = NULL;
    delete pClipStartEase;
    pClipStartEase = NULL;
    delete pBlockPositionEase;
    pBlockPositionEase = NULL;
    delete pFlashOpacityEase;
    pFlashOpacityEase = NULL;
}

void Confrontation::InitializeBeginConfrontationTopicSelectionAction::Begin(State *pState)
{
    ContinuousAction::Begin(pState);
    pState->SetCanFastForward(false);

    Case::GetInstance()->CacheState();
    pState->CacheStateForConfrontationRestart();
    pState->GetCurrentConfrontation()->CacheState();

    if (pConfrontationEntranceBackgroundVideo == NULL)
    {
        pConfrontationEntranceBackgroundVideo = CommonCaseResources::GetInstance()->GetAnimationManager()->GetVideoFromId("ConfrontationEntranceBackground");
    }

    if (pConfrontationEntranceBlockSprite == NULL)
    {
        pConfrontationEntranceBlockSprite = CommonCaseResources::GetInstance()->GetSpriteManager()->GetImageFromId("ConfrontationEntranceBlock");
    }

    if (pConfrontationEntranceVfxVideo == NULL)
    {
        pConfrontationEntranceVfxVideo = CommonCaseResources::GetInstance()->GetAnimationManager()->GetVideoFromId("ConfrontationEntranceVfx");
    }

    pConfrontationEntranceBackgroundVideo->Begin();
    pConfrontationEntranceVfxVideo->Begin();

    pCharacterEntranceEase->Begin();
    pClipStartEase->Begin();
    pBlockPositionEase->Begin();
    pFlashOpacityEase->Begin();
    characterEntrancePosition = -gScreenWidth / 2;
    clipStartPosition = -gScreenWidth / 2;
    blockPosition = -gScreenWidth / 2;
    flashOpacity = 0;
}

void Confrontation::InitializeBeginConfrontationTopicSelectionAction::Update(int delta)
{
    pConfrontationEntranceBackgroundVideo->Update(delta);
    pConfrontationEntranceVfxVideo->Update(delta);

    if (pConfrontationEntranceVfxVideo->IsFinished())
    {
        pState->GetCurrentConfrontation()->ShowHealthIcons();
        SetIsFinished(true);
    }

    pCharacterEntranceEase->Update(delta);

    if (!pCharacterEntranceEase->GetIsFinished())
    {
        characterEntrancePosition = pCharacterEntranceEase->GetCurrentValue();
    }

    pClipStartEase->Update(delta);

    if (!pClipStartEase->GetIsFinished())
    {
        clipStartPosition = pClipStartEase->GetCurrentValue();
    }

    pBlockPositionEase->Update(delta);

    if (!pBlockPositionEase->GetIsFinished())
    {
        blockPosition = pBlockPositionEase->GetCurrentValue();
    }

    pFlashOpacityEase->Update(delta);

    if (!pFlashOpacityEase->GetIsFinished())
    {
        flashOpacity = pFlashOpacityEase->GetCurrentValue();
    }
}

void DrawClippedCharacterSprite(Sprite *pCharacterSprite, double yOffset, double characterEntrancePosition, int actualClipStartPosition, bool flipHorizontally, double opacity)
{
    pCharacterSprite->DrawClipped(
        Vector2(
            flipHorizontally ? gScreenWidth - actualClipStartPosition - characterEntrancePosition : characterEntrancePosition,
            yOffset + ConfrontationEntranceCharacterYPosition),
        RectangleWH(0, ConfrontationEntranceCharacterYPosition, actualClipStartPosition, ConfrontationEntranceCharacterHeight),
        flipHorizontally,
        Color(opacity, 1.0, 1.0, 1.0));

    for (int i = 0; i < 16; i++)
    {
        pCharacterSprite->DrawClipped(
            Vector2(
                flipHorizontally ? gScreenWidth - actualClipStartPosition - characterEntrancePosition - 4 * (i + 1) : characterEntrancePosition + actualClipStartPosition + 4 * i,
                yOffset + ConfrontationEntranceCharacterYPosition + 3 * i),
            RectangleWH(actualClipStartPosition + 4 * i, ConfrontationEntranceCharacterYPosition + 3 * i, 4, ConfrontationEntranceCharacterHeight - 3 * i * 2),
            flipHorizontally,
            Color(opacity, 1.0, 1.0, 1.0));
    }
}

void Confrontation::InitializeBeginConfrontationTopicSelectionAction::Draw(double xOffset, double yOffset)
{
    pConfrontationEntranceBackgroundVideo->Draw(
        Vector2(blockPosition, yOffset),
        false,
        pState->GetCurrentConfrontation()->GetPlayerCharacter()->GetBackgroundColor(),
        RectangleWH(0, 0, gScreenWidth / 2, pConfrontationEntranceBackgroundVideo->GetHeight()));

    pConfrontationEntranceBackgroundVideo->Draw(
        Vector2(gScreenWidth / 2 - blockPosition, yOffset),
        false,
        pState->GetCurrentConfrontation()->GetOpponentCharacter()->GetBackgroundColor(),
        RectangleWH(gScreenWidth / 2, 0, gScreenWidth / 2, pConfrontationEntranceBackgroundVideo->GetHeight()));

    int actualClipStartPositionLeft = (int)(ConfrontationEntranceClipStartYPosition + clipStartPosition - characterEntrancePosition);
    int actualClipStartPositionRight = (int)(ConfrontationEntranceClipStartYPosition + clipStartPosition - characterEntrancePosition + 0.5);

    DrawClippedCharacterSprite(
        pState->GetCurrentConfrontation()->GetPlayerCharacter()->GetEntranceForegroundSprite(),
        yOffset,
        characterEntrancePosition,
        actualClipStartPositionLeft,
        false /* flipHorizontally */,
        1.0 /* opacity */);
    DrawClippedCharacterSprite(
        pState->GetCurrentConfrontation()->GetPlayerCharacter()->GetEntranceForegroundWhiteSprite(),
        yOffset,
        characterEntrancePosition,
        actualClipStartPositionLeft,
        false /* flipHorizontally */,
        flashOpacity);

    DrawClippedCharacterSprite(
        pState->GetCurrentConfrontation()->GetOpponentCharacter()->GetEntranceForegroundSprite(),
        yOffset,
        characterEntrancePosition,
        actualClipStartPositionRight,
        true /* flipHorizontally */,
        1.0 /* opacity */);
    DrawClippedCharacterSprite(
        pState->GetCurrentConfrontation()->GetOpponentCharacter()->GetEntranceForegroundWhiteSprite(),
        yOffset,
        characterEntrancePosition,
        actualClipStartPositionRight,
        true /* flipHorizontally */,
        flashOpacity);

    pConfrontationEntranceBlockSprite->Draw(
        Vector2(blockPosition, yOffset), RectangleWH(0, 0, pConfrontationEntranceBlockSprite->width, pConfrontationEntranceBlockSprite->height),
        false /* flipHorizontally */,
        false /* flipVertically */,
        1.0,
        Color::White);
    pConfrontationEntranceBlockSprite->Draw(
        Vector2(gScreenWidth / 2 - blockPosition, yOffset), RectangleWH(0, 0, pConfrontationEntranceBlockSprite->width, pConfrontationEntranceBlockSprite->height),
        true /* flipHorizontally */,
        false /* flipVertically */,
        1.0,
        Color::White);

    pConfrontationEntranceVfxVideo->Draw(Vector2(0, yOffset));
}

Confrontation::InitializeBeginConfrontationTopicSelectionAction::InitializeBeginConfrontationTopicSelectionAction(XmlReader *pReader)
{
    pConfrontationEntranceBackgroundVideo = NULL;
    pConfrontationEntranceBlockSprite = NULL;
    pConfrontationEntranceVfxVideo = NULL;

    pCharacterEntranceEase = new ConfrontationEntranceCharacterEase(-gScreenWidth / 2, 0, ConfrontationEntranceAnimationDuration);
    pClipStartEase = new ConfrontationEntranceBlockEase(-gScreenWidth / 2, 0, ConfrontationEntranceAnimationDuration);
    pBlockPositionEase = new ConfrontationEntranceBlockEase(-gScreenWidth / 2, 0, ConfrontationEntranceAnimationDuration);
    pFlashOpacityEase = new ConfrontationEntranceFlashEase(0, 1, ConfrontationEntranceAnimationDuration);

    pReader->StartElement("InitializeBeginConfrontationTopicSelectionAction");
    pReader->EndElement();
}

Confrontation::BeginConfrontationTopicSelectionAction::BeginConfrontationTopicSelectionAction()
{
    Initialize();
    pInitialTopic = NULL;
}

Confrontation::BeginConfrontationTopicSelectionAction::~BeginConfrontationTopicSelectionAction()
{
    for (unsigned int i = 0; i < topicList.size(); i++)
    {
        delete topicList[i];
    }

    topicList.clear();

    delete pInitialTopic;
    pInitialTopic = NULL;

    delete pTopicSelectionButtonArray;
    pTopicSelectionButtonArray = NULL;

    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
}

void Confrontation::BeginConfrontationTopicSelectionAction::Initialize()
{
    pTopicSelectionButtonArray = new ButtonArray();
    pTopicSelectionButtonArray->SetIsCancelable(false);
}

void Confrontation::BeginConfrontationTopicSelectionAction::Begin(State *pState)
{
    Conversation::ContinuousAction::Begin(pState);
    pState->SetCanFastForward(false);
    pState->GetCurrentConfrontation()->SetConfrontationBeginIndex(pState->GetActionIndex());

    bool allConversationsFinished = true;
    vector<ButtonArrayLoadParameters> loadParametersList;

    delete pCurrentConfrontationTopicsById;
    pCurrentConfrontationTopicsById = new map<string, Confrontation::Topic *>();

    Confrontation::Topic *pLastTopic = NULL;

    for (unsigned int i = 0; i < topicList.size(); i++)
    {
        Confrontation::Topic *pTopic = topicList[i];
        (*pCurrentConfrontationTopicsById)[pTopic->GetId()] = pTopic;

        if (!pTopic->GetIsCompleted())
        {
            allConversationsFinished = false;

            if (pTopic->GetIsEnabled())
            {
                ButtonArrayLoadParameters loadParameters;
                loadParameters.text = pTopic->GetName();
                loadParametersList.push_back(loadParameters);
                pLastTopic = pTopic;
            }
        }
    }

    if (!allConversationsFinished)
    {
        pTopicSelectionButtonArray->Load(loadParametersList);
        pState->GetCurrentConfrontation()->SetPlayerDefeatedIndex(playerDefeatedIndex);
    }

    if (GetInitialTopic() != NULL && !GetInitialTopic()->GetIsCompleted())
    {
        pState->SetActionIndex(GetInitialTopic()->GetActionIndex());
        pState->SetCurrentConfrontationTopic(GetInitialTopic());
        SetIsFinished(true);
    }
    else if (allConversationsFinished)
    {
        pState->SetActionIndex(GetEndActionIndex());
        SetIsFinished(true);
    }
    else if (loadParametersList.size() == 1)
    {
        pState->SetActionIndex(pLastTopic->GetActionIndex());
        pState->SetCurrentConfrontationTopic(pLastTopic);
        SetIsFinished(true);
    }
    else
    {
        shouldCloseButtonArray = false;
        EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
        EventProviders::GetButtonArrayEventProvider()->RegisterListener(this);
        pTopicSelectionButtonArray->Show();
        SetIsFinished(false);
    }
}

void Confrontation::BeginConfrontationTopicSelectionAction::Update(int delta)
{
    if (GetIsFinished())
    {
        return;
    }

    pTopicSelectionButtonArray->Update(delta);

    if (shouldCloseButtonArray)
    {
        shouldCloseButtonArray = false;
        pTopicSelectionButtonArray->Close();
    }

    if (pTopicSelectionButtonArray->GetIsClosed())
    {
        SetIsFinished(true);
    }
}

void Confrontation::BeginConfrontationTopicSelectionAction::Draw(double xOffset, double yOffset)
{
    if (GetIsFinished())
    {
        return;
    }

    pTopicSelectionButtonArray->Draw(xOffset, yOffset);
}

void Confrontation::BeginConfrontationTopicSelectionAction::Reset()
{
    ContinuousAction::Reset();
    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
}

void Confrontation::BeginConfrontationTopicSelectionAction::ResetTopics()
{
    if (pInitialTopic != NULL)
    {
        pInitialTopic->SetIsEnabled(initialTopicIsEnabled);
        pInitialTopic->SetIsCompleted(false);
    }

    for (unsigned int i = 0; i < topicList.size(); i++)
    {
        topicList[i]->SetIsEnabled(topicToInitialEnabledStateMap[topicList[i]]);
        topicList[i]->SetIsCompleted(false);
    }
}

void Confrontation::BeginConfrontationTopicSelectionAction::OnButtonArrayButtonClicked(ButtonArray *pSender, int id)
{
    if (pSender != pTopicSelectionButtonArray)
    {
        return;
    }

    // First we find the topic that was clicked on,
    // and then we move to that topic.
    for (unsigned int i = 0; i < topicList.size(); i++)
    {
        Confrontation::Topic *pTopic = topicList[i];

        if (pTopic->GetIsEnabled() && !pTopic->GetIsCompleted())
        {
            if (id == 0)
            {
                // This is the topic we're looking for.
                // Move to its action index.
                pState->SetCurrentConfrontationTopic(pTopic);
                pState->SetActionIndex(pTopic->GetActionIndex());
                pTopicSelectionButtonArray->ReorderOutAnimations(id);
                shouldCloseButtonArray = true;
                break;
            }
            else
            {
                // This isn't the topic we're looking for,
                // but it IS a topic in the button array.
                // Decrement id by 1 to bring us closer to
                // the topic we're looking for.
                id--;
            }
        }
    }
}

void Confrontation::BeginConfrontationTopicSelectionAction::OnButtonArrayCanceled(ButtonArray *pSender)
{

}

Confrontation::BeginConfrontationTopicSelectionAction::BeginConfrontationTopicSelectionAction(XmlReader *pReader)
{
    Initialize();
    pInitialTopic = NULL;

    pReader->StartElement("BeginConfrontationTopicSelectionAction");
    playerDefeatedIndex = pReader->ReadIntElement("PlayerDefeatedIndex");
    endActionIndex = pReader->ReadIntElement("EndActionIndex");

    if (pReader->ElementExists("InitialTopic"))
    {
        pReader->StartElement("InitialTopic");
        pInitialTopic = new Confrontation::Topic(pReader);
        initialTopicIsEnabled = pInitialTopic->GetIsEnabled();
        pReader->EndElement();
    }

    pReader->StartElement("TopicList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        Confrontation::Topic *pTopic = new Confrontation::Topic(pReader);
        topicList.push_back(pTopic);
        topicToInitialEnabledStateMap[pTopic] = pTopic->GetIsEnabled();
    }

    pReader->EndElement();

    pReader->EndElement();
}

Confrontation::BeginConfrontationTopicAction::BeginConfrontationTopicAction()
{

}

void Confrontation::BeginConfrontationTopicAction::Execute(State *pState)
{

}

Confrontation::BeginConfrontationTopicAction::BeginConfrontationTopicAction(XmlReader *pReader)
{
    pReader->StartElement("BeginConfrontationTopicAction");
    pReader->EndElement();
}

Confrontation::EndConfrontationTopicAction::EndConfrontationTopicAction()
{
    beginConfrontationTopicSelectionActionIndex = -1;
}

void Confrontation::EndConfrontationTopicAction::Execute(State *pState)
{
    pState->GetCurrentConfrontationTopic()->SetIsCompleted(true);
    pState->SetCurrentConfrontationTopic(NULL);
    pState->SetActionIndex(GetBeginConfrontationTopicSelectionActionIndex());
}

Confrontation::EndConfrontationTopicAction::EndConfrontationTopicAction(XmlReader *pReader)
{
    pReader->StartElement("EndConfrontationTopicAction");
    beginConfrontationTopicSelectionActionIndex = pReader->ReadIntElement("BeginConfrontationTopicSelectionActionIndex");
    pReader->EndElement();
}

Confrontation::SkipPlayerDefeatedAction::SkipPlayerDefeatedAction()
{
    skipToIndex = -1;
}

void Confrontation::SkipPlayerDefeatedAction::Execute(State *pState)
{
    pState->SetActionIndex(GetSkipToIndex());
}

Confrontation::SkipPlayerDefeatedAction::SkipPlayerDefeatedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipPlayerDefeatedAction");
    skipToIndex = pReader->ReadIntElement("SkipToIndex");
    pReader->EndElement();
}

Confrontation::BeginPlayerDefeatedAction::BeginPlayerDefeatedAction()
{

}

void Confrontation::BeginPlayerDefeatedAction::Execute(State *pState)
{
    pState->ClearCachedActionIndex();
}

Confrontation::BeginPlayerDefeatedAction::BeginPlayerDefeatedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginPlayerDefeatedAction");
    pReader->EndElement();
}

Confrontation::EndPlayerDefeatedAction::EndPlayerDefeatedAction()
{

}

void Confrontation::EndPlayerDefeatedAction::Execute(State *pState)
{
    pState->SetEndRequested(true);
}

Confrontation::EndPlayerDefeatedAction::EndPlayerDefeatedAction(XmlReader *pReader)
{
    pReader->StartElement("EndPlayerDefeatedAction");
    pReader->EndElement();
}

Confrontation::EndConfrontationTopicSelectionAction::EndConfrontationTopicSelectionAction()
{

}

void Confrontation::EndConfrontationTopicSelectionAction::Execute(State *pState)
{
    delete pCurrentConfrontationTopicsById;
    pCurrentConfrontationTopicsById = NULL;
    pState->GetCurrentConfrontation()->HideHealthIcons();
}

Confrontation::EndConfrontationTopicSelectionAction::EndConfrontationTopicSelectionAction(XmlReader *pReader)
{
    pReader->StartElement("EndConfrontationTopicSelectionAction");
    pReader->EndElement();
}

Confrontation::EnableTopicAction::EnableTopicAction(const string &topicId)
    : topicId(topicId)
{
}

void Confrontation::EnableTopicAction::Execute(State *pState)
{
    if (Confrontation::pCurrentConfrontationTopicsById->count(topicId) > 0)
    {
        Confrontation::Topic *pTopic = (*Confrontation::pCurrentConfrontationTopicsById)[topicId];
        pTopic->SetIsEnabled(true);
    }
}

Confrontation::EnableTopicAction::EnableTopicAction(XmlReader *pReader)
{
    pReader->StartElement("EnableTopicAction");
    topicId = pReader->ReadTextElement("TopicId");
    pReader->EndElement();
}

Confrontation::RestartConfrontationAction::RestartConfrontationAction()
{
}

void Confrontation::RestartConfrontationAction::Execute(State *pState)
{
    pState->GetCurrentConfrontation()->Restart();
}

Confrontation::RestartConfrontationAction::RestartConfrontationAction(XmlReader *pReader)
{
    pReader->StartElement("RestartConfrontationAction");
    pReader->EndElement();
}
