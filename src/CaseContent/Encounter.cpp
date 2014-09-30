/**
 * Handles a single character encounter in the game.
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

#include "Encounter.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../XmlReader.h"
#include "../CaseContent/Dialog.h"
#include "../CaseInformation/Case.h"

const int OpacityAnimationDuration = 250;
const int DialogBackgroundAnimationDuration = 250;
const int CharacterAnimationDuration = 250;

Image *Encounter::pDialogBackgroundImage = NULL;
Image *Encounter::pBackgroundDarkeningImage = NULL;

void Encounter::Initialize(Image *pDialogBackgroundImage, Image *pBackgroundDarkeningImage)
{
    Encounter::pDialogBackgroundImage = pDialogBackgroundImage;
    Encounter::pBackgroundDarkeningImage = pBackgroundDarkeningImage;
}

Encounter::Encounter()
{
    pNextButtonArray = NULL;
    pCurrentButtonArray = NULL;
    pLastButtonArrayBeforeConversation = NULL;
    shouldCloseButtonArray = false;

    pNextConversation = NULL;
    pCurrentConversation = NULL;

    pBackgroundOpacityInEase = NULL;
    pDialogBackgroundInEase = NULL;
    pLeftCharacterInEase = NULL;
    pRightCharacterInEase = NULL;
    pBackgroundOpacityOutEase = NULL;
    pDialogBackgroundOutEase = NULL;
    pLeftCharacterOutEase = NULL;
    pRightCharacterOutEase = NULL;

    backgroundOpacityAlphaValue = 0;
    dialogBackgroundYPosition = 0;

    pInitialConversation = NULL;
    pOneShotConversation = NULL;
    pPresentWrongEvidenceConversation = NULL;
    pPresentWrongProfileConversation = NULL;

    pMainMenuButtonArray = new ButtonArray();
    pCurrentButtonArray = pMainMenuButtonArray;
    pEvidenceSelector = new EvidenceSelector(true /* isCancelable */);

    pCharacterNameTab = new Tab(0, false /* isClickable */);
    pExitTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, pgLocalizableContent->GetText("Encounter/ExitText"));
    pPresentEvidenceTab = new Tab(gScreenWidth - 2 * TabWidth - 7, true /* isClickable */, pgLocalizableContent->GetText("Encounter/PresentEvidenceText"));

    ownsOneShotConversation = false;
    isFinished = false;

    InitializeEasingFunctions();
}

Encounter::Encounter(XmlReader *pReader)
{
    pNextButtonArray = NULL;
    pCurrentButtonArray = NULL;
    pLastButtonArrayBeforeConversation = NULL;
    shouldCloseButtonArray = false;

    pNextConversation = NULL;
    pCurrentConversation = NULL;

    pBackgroundOpacityInEase = NULL;
    pDialogBackgroundInEase = NULL;
    pLeftCharacterInEase = NULL;
    pRightCharacterInEase = NULL;
    pBackgroundOpacityOutEase = NULL;
    pDialogBackgroundOutEase = NULL;
    pLeftCharacterOutEase = NULL;
    pRightCharacterOutEase = NULL;

    backgroundOpacityAlphaValue = 0;
    dialogBackgroundYPosition = 0;

    pInitialConversation = NULL;
    pOneShotConversation = NULL;
    pPresentWrongEvidenceConversation = NULL;
    pPresentWrongProfileConversation = NULL;

    pMainMenuButtonArray = new ButtonArray();
    pCurrentButtonArray = pMainMenuButtonArray;
    pEvidenceSelector = new EvidenceSelector(true /* isCancelable */);

    pCharacterNameTab = new Tab(0, false /* isClickable */);
    pExitTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, pgLocalizableContent->GetText("Encounter/ExitText"));
    pPresentEvidenceTab = new Tab(gScreenWidth - 2 * TabWidth - 7, true /* isClickable */, pgLocalizableContent->GetText("Encounter/PresentEvidenceText"));

    ownsOneShotConversation = false;
    isFinished = false;

    InitializeEasingFunctions();

    pReader->StartElement("Encounter");

    if (pReader->ElementExists("Id"))
    {
        id = pReader->ReadTextElement("Id");
    }

    if (pReader->ElementExists("InitialLeftCharacterId"))
    {
        initialLeftCharacterId = pReader->ReadTextElement("InitialLeftCharacterId");
    }

    if (pReader->ElementExists("InitialLeftCharacterEmotionId"))
    {
        initialLeftCharacterEmotionId = pReader->ReadTextElement("InitialLeftCharacterEmotionId");
    }

    if (pReader->ElementExists("InitialRightCharacterId"))
    {
        initialRightCharacterId = pReader->ReadTextElement("InitialRightCharacterId");
    }

    if (pReader->ElementExists("InitialRightCharacterEmotionId"))
    {
        initialRightCharacterEmotionId = pReader->ReadTextElement("InitialRightCharacterEmotionId");
    }

    if (pReader->ElementExists("OneShotConversation"))
    {
        pReader->StartElement("OneShotConversation");
        pOneShotConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();

        ownsOneShotConversation = true;
    }

    if (pReader->ElementExists("PresentWrongEvidenceConversation"))
    {
        pReader->StartElement("PresentWrongEvidenceConversation");
        pPresentWrongEvidenceConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();
    }

    if (pReader->ElementExists("PresentWrongProfileConversation"))
    {
        pReader->StartElement("PresentWrongProfileConversation");
        pPresentWrongProfileConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();
    }

    string initialConversationId;
    string oneShotConversationId;

    if (pReader->ElementExists("InitialConversationId"))
    {
        initialConversationId = pReader->ReadTextElement("InitialConversationId");
    }

    if (pReader->ElementExists("OneShotConversationId") && pOneShotConversation == NULL)
    {
        oneShotConversationId = pReader->ReadTextElement("OneShotConversationId");
    }

    pReader->StartElement("ConversationList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string conversationId = pReader->ReadTextElement("ConversationId");
        Conversation *pConversation = Case::GetInstance()->GetContentManager()->GetConversationFromId(conversationId);
        conversationList.push_back(pConversation);
        allConversationList.push_back(pConversation);

        if (initialConversationId == conversationId)
        {
            pInitialConversation = pConversation;
        }
        else if (oneShotConversationId == conversationId)
        {
            pOneShotConversation = pConversation;
        }
    }

    pReader->EndElement();

    pReader->StartElement("InterrogationList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string interrogationId = pReader->ReadTextElement("InterrogationId");
        Interrogation *pInterrogation = dynamic_cast<Interrogation *>(Case::GetInstance()->GetContentManager()->GetConversationFromId(interrogationId));
        interrogationList.push_back(pInterrogation);
        allConversationList.push_back(pInterrogation);

        if (initialConversationId == interrogationId)
        {
            pInitialConversation = pInterrogation;
        }
        else if (oneShotConversationId == interrogationId)
        {
            pOneShotConversation = pInterrogation;
        }
    }

    pReader->EndElement();

    pReader->StartElement("ConfrontationList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string confrontationId = pReader->ReadTextElement("ConfrontationId");
        Confrontation *pConfrontation = dynamic_cast<Confrontation *>(Case::GetInstance()->GetContentManager()->GetConversationFromId(confrontationId));
        confrontationList.push_back(pConfrontation);
        allConversationList.push_back(pConfrontation);

        if (initialConversationId == confrontationId)
        {
            pInitialConversation = pConfrontation;
        }
        else if (oneShotConversationId == confrontationId)
        {
            pOneShotConversation = pConfrontation;
        }
    }

    pReader->EndElement();

    pReader->StartElement("PresentEvidenceConversationDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string evidenceId = pReader->ReadTextElement("EvidenceId");
        Conversation *pConversation = Conversation::LoadFromXml(pReader);

        presentEvidenceConversationDictionary[evidenceId] = pConversation;
    }

    pReader->EndElement();

    pReader->EndElement();
}

Encounter::~Encounter()
{
    for (map<string, Conversation *>::iterator iter = presentEvidenceConversationDictionary.begin(); iter != presentEvidenceConversationDictionary.end(); ++iter)
    {
        delete iter->second;
    }

    presentEvidenceConversationDictionary.clear();

    delete pBackgroundOpacityInEase;
    pBackgroundOpacityInEase = NULL;
    delete pDialogBackgroundInEase;
    pDialogBackgroundInEase = NULL;
    delete pLeftCharacterInEase;
    pLeftCharacterInEase = NULL;
    delete pRightCharacterInEase;
    pRightCharacterInEase = NULL;
    delete pBackgroundOpacityOutEase;
    pBackgroundOpacityOutEase = NULL;
    delete pDialogBackgroundOutEase;
    pDialogBackgroundOutEase = NULL;
    delete pLeftCharacterOutEase;
    pLeftCharacterOutEase = NULL;
    delete pRightCharacterOutEase;
    pRightCharacterOutEase = NULL;

    if (ownsOneShotConversation)
    {
        delete pOneShotConversation;
        pOneShotConversation = NULL;
    }

    delete pPresentWrongEvidenceConversation;
    pPresentWrongEvidenceConversation = NULL;
    delete pPresentWrongProfileConversation;
    pPresentWrongProfileConversation = NULL;

    delete pMainMenuButtonArray;
    pMainMenuButtonArray = NULL;
    delete pEvidenceSelector;
    pEvidenceSelector = NULL;

    delete pCharacterNameTab;
    pCharacterNameTab = NULL;
    delete pExitTab;
    pExitTab = NULL;
    delete pPresentEvidenceTab;
    pPresentEvidenceTab = NULL;

    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
    EventProviders::GetEvidenceSelectorEventProvider()->ClearListener(this);
}

void Encounter::Begin()
{
    Reset();

    state = State();

    backgroundOpacityAlphaValue = 0;
    dialogBackgroundYPosition = 195;

    // We want to unconditionally refresh the button array contents, since otherwise
    // an encounter with an initial conversation where we loaded data from a previous save file
    // might have stale button contents that will be displayed momentarily.
    pMainMenuButtonArray->Reset();
    RefreshButtonArrayContents();

    bool isInitialConversation = GetOneShotConversation() != NULL || (pInitialConversation != NULL && !pInitialConversation->GetHasBeenCompleted() && !pInitialConversation->GetIsLocked());

    // Don't animate in the initial characters if there's an initial conversation to be run.
    // Otherwise, the animation at the start of the encounter will collide with the animation
    // at the start of the conversation.
    if (!isInitialConversation)
    {
        state.SetLeftCharacterId(GetInitialLeftCharacterId());
        state.SetLeftCharacterEmotionId(GetInitialLeftCharacterEmotionId());
        state.SetRightCharacterId(GetInitialRightCharacterId());
        state.SetRightCharacterEmotionId(GetInitialRightCharacterEmotionId());

        state.SetLeftCharacterXOffsetEasingFunction(pLeftCharacterInEase);
        state.SetRightCharacterXOffsetEasingFunction(pRightCharacterInEase);

        pLeftCharacterInEase->Begin();
        pRightCharacterInEase->Begin();

        if (state.GetLeftCharacterId().length() == 0)
        {
            pLeftCharacterInEase->Finish();
        }

        if (state.GetRightCharacterId().length() == 0)
        {
            pRightCharacterInEase->Finish();
        }
    }

    pBackgroundOpacityInEase->Begin();
    pDialogBackgroundInEase->Begin();

    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
    EventProviders::GetEvidenceSelectorEventProvider()->ClearListener(this);
    EventProviders::GetButtonArrayEventProvider()->RegisterListener(this);
    EventProviders::GetEvidenceSelectorEventProvider()->RegisterListener(this);

    if (GetOneShotConversation() != NULL)
    {
        BeginConversation(GetOneShotConversation());
    }
    else if (pInitialConversation != NULL && !pInitialConversation->GetHasBeenCompleted() && !pInitialConversation->GetIsLocked())
    {
        BeginConversation(pInitialConversation);
    }
    else
    {
        RefreshButtonArrays();
    }

    SetIsFinished(false);
    SetTransitionRequest(TransitionRequest());

    pCharacterNameTab->SetIsHidden(true, false);
}

void Encounter::Update(int delta)
{
    bool formattingAnimationInProgress = false;

    if (pDialogBackgroundInEase->GetIsStarted() && !pDialogBackgroundInEase->GetIsFinished())
    {
        pDialogBackgroundInEase->Update(delta);
        dialogBackgroundYPosition = (int)pDialogBackgroundInEase->GetCurrentValue();
        formattingAnimationInProgress = true;
    }
    else if (pDialogBackgroundOutEase->GetIsStarted() && !pDialogBackgroundOutEase->GetIsFinished())
    {
        pDialogBackgroundOutEase->Update(delta);
        dialogBackgroundYPosition = (int)pDialogBackgroundOutEase->GetCurrentValue();
        formattingAnimationInProgress = true;
    }
    else if (pDialogBackgroundOutEase->GetIsFinished())
    {
        // If the dialog background out animation has finished,
        // then this encounter as a whole is finished too.
        SetIsFinished(true);
        dialogBackgroundYPosition = (int)pDialogBackgroundOutEase->GetCurrentValue();
        formattingAnimationInProgress = true;
    }
    else
    {
        dialogBackgroundYPosition = 0;
    }

    if (pBackgroundOpacityInEase->GetIsStarted() && !pBackgroundOpacityInEase->GetIsFinished())
    {
        pBackgroundOpacityInEase->Update(delta);
        backgroundOpacityAlphaValue = (int)pBackgroundOpacityInEase->GetCurrentValue();
    }
    else if (pBackgroundOpacityOutEase->GetIsStarted())
    {
        pBackgroundOpacityOutEase->Update(delta);
        backgroundOpacityAlphaValue = (int)pBackgroundOpacityOutEase->GetCurrentValue();
    }
    else
    {
        backgroundOpacityAlphaValue = 255;
    }

    pLeftCharacterInEase->Update(delta);
    pLeftCharacterOutEase->Update(delta);
    pRightCharacterInEase->Update(delta);
    pRightCharacterOutEase->Update(delta);

    // If a formatting animation is in progress, we want to ignore any mouse clicks.
    if (formattingAnimationInProgress)
    {
        MouseHelper::HandleClick();
    }

    if (pEvidenceSelector->GetIsShowing())
    {
        pEvidenceSelector->Update(delta);

        if (!pEvidenceSelector->GetIsShowing())
        {
            if (pLastButtonArrayBeforeConversation->GetCount() > 0)
            {
                pNextButtonArray = pLastButtonArrayBeforeConversation;
            }
            else
            {
                pNextButtonArray = pMainMenuButtonArray;
            }

            pExitTab->SetIsHidden(false);
            pPresentEvidenceTab->SetIsHidden(false);

            SwapButtonArrays();
        }
    }
    else if (pCurrentConversation != NULL && pDialogBackgroundInEase->GetIsFinished())
    {
        if (state.GetCurrentAnimation() != NULL)
        {
            state.GetCurrentAnimation()->Update(delta);
        }

        if (state.GetLeftCharacterXOffsetEasingFunction() != NULL)
        {
            state.GetLeftCharacterXOffsetEasingFunction()->Update(delta);
        }

        if (state.GetRightCharacterXOffsetEasingFunction() != NULL)
        {
            state.GetRightCharacterXOffsetEasingFunction()->Update(delta);
        }

        EasingFunction *pWindowMotionXEase = state.GetWindowMotionXEasingFunction();
        EasingFunction *pWindowMotionYEase = state.GetWindowMotionYEasingFunction();

        if (pWindowMotionXEase != NULL && !pWindowMotionXEase->GetIsFinished())
        {
            pWindowMotionXEase->Update(delta);
        }

        if (pWindowMotionYEase != NULL && !pWindowMotionYEase->GetIsFinished())
        {
            pWindowMotionYEase->Update(delta);
        }

        pCurrentConversation->Update(delta);

        if (pCurrentConversation->GetIsFinished() && !formattingAnimationInProgress)
        {
            stopDialog();

            bool shouldRunPresentWrongEvidenceConversation = pCurrentConversation->GetState()->GetPresentWrongEvidenceRequested();
            bool exitRequested = state.GetExitRequested();

            if (exitRequested)
            {
                SetTransitionRequest(state.GetTransitionRequest());
            }

            pCurrentConversation->ResetState();

            if (pCurrentConversation != GetOneShotConversation() && !exitRequested)
            {
                Conversation *pPresentWrongConversation =
                    shouldRunPresentWrongEvidenceConversation ?
                        (Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdProfile(lastPresentedEvidenceId) ?
                            pPresentWrongProfileConversation :
                            pPresentWrongEvidenceConversation) :
                        NULL;

                if (shouldRunPresentWrongEvidenceConversation && pPresentWrongConversation != NULL)
                {
                    pCurrentConversation = pPresentWrongConversation;
                    pCurrentConversation->Begin(&state);
                }
                else
                {
                    Case::GetInstance()->GetAudioManager()->RestoreMusicVolumeForDialog();
                    Case::GetInstance()->GetAudioManager()->RestoreAmbianceVolumeForDialog();
                    pCurrentConversation = NULL;
                    RefreshButtonArrayContents();

                    if (pLastButtonArrayBeforeConversation != NULL && pLastButtonArrayBeforeConversation->GetCount() > 0)
                    {
                        pNextButtonArray = pLastButtonArrayBeforeConversation;
                    }
                    else
                    {
                        pNextButtonArray = pMainMenuButtonArray;
                    }

                    pExitTab->SetIsHidden(false);
                    pPresentEvidenceTab->SetIsHidden(false);

                    SwapButtonArrays();
                }
            }
            else
            {
                Case::GetInstance()->GetAudioManager()->RestoreMusicVolumeForDialog();
                Case::GetInstance()->GetAudioManager()->RestoreAmbianceVolumeForDialog();
                Close();
            }
        }
    }
    else if (pCurrentConversation == NULL)
    {
        if (pCurrentButtonArray != NULL)
        {
            if (shouldCloseButtonArray)
            {
                pCurrentButtonArray->Close();
                shouldCloseButtonArray = false;
                pExitTab->SetIsHidden(true);
                pPresentEvidenceTab->SetIsHidden(true);
            }

            pCurrentButtonArray->Update(delta);

            if (pCurrentButtonArray->GetIsClosed())
            {
                if (pNextConversation != NULL)
                {
                    BeginNextConversation();
                }
                else
                {
                    SwapButtonArrays();

                    // If the current button array is NULL,
                    // that means we should show the
                    // evidence selector.
                    if (pCurrentButtonArray == NULL)
                    {
                        pEvidenceSelector->Begin();
                        pEvidenceSelector->Show();
                    }
                }
            }

            if (pCurrentButtonArray == pMainMenuButtonArray && !pEvidenceSelector->GetIsShowing())
            {
                pExitTab->Update();
                pExitTab->UpdatePosition(delta);

                if (pExitTab->GetIsClicked())
                {
                    Close();
                }

                pPresentEvidenceTab->Update();
                pPresentEvidenceTab->UpdatePosition(delta);

                if (pPresentEvidenceTab->GetIsClicked())
                {
                    PrepNextButtonArray(NULL, -1);
                }
            }
        }
    }

    Case::GetInstance()->GetDialogCharacterManager()->UpdateForState(&state, delta);

    if (state.GetCurrentSpeakerName().length() > 0)
    {
        pCharacterNameTab->SetText(state.GetCurrentSpeakerName());
        pCharacterNameTab->SetIsHidden(false);
    }
    else
    {
        pCharacterNameTab->SetIsHidden(true);
    }

    pCharacterNameTab->UpdatePosition(delta);
}

void Encounter::Draw()
{
    pBackgroundDarkeningImage->Draw(Vector2(0, 0), Color(backgroundOpacityAlphaValue / 255.0, 1.0, 1.0, 1.0));
    Case::GetInstance()->GetDialogCharacterManager()->DrawForState(&state);

    Vector2 windowPosition(0, 0);

    if (state.GetWindowMotionXEasingFunction() != NULL && state.GetWindowMotionYEasingFunction() != NULL)
    {
        windowPosition = Vector2(state.GetWindowMotionXEasingFunction()->GetCurrentValue(), state.GetWindowMotionYEasingFunction()->GetCurrentValue());
    }

    if (state.GetCurrentAnimation() != NULL)
    {
        state.GetCurrentAnimation()->Draw();
    }

    if (pEvidenceSelector->GetIsShowing())
    {
        pEvidenceSelector->Draw(dialogBackgroundYPosition);
    }
    else if (pCurrentConversation != NULL && pDialogBackgroundInEase->GetIsFinished())
    {
        pCurrentConversation->DrawBackground(windowPosition.GetX(), windowPosition.GetY() + dialogBackgroundYPosition);
    }

    if (state.GetIsInterjectionOngoing())
    {
        double interjectionBackgroundOpacity = 0;

        if (state.GetInterjectionDarkeningOpacityInEasingFunction()->GetIsStarted() && !state.GetInterjectionDarkeningOpacityOutEasingFunction()->GetIsStarted())
        {
            interjectionBackgroundOpacity = state.GetInterjectionDarkeningOpacityInEasingFunction()->GetCurrentValue();
        }
        else if (state.GetInterjectionDarkeningOpacityOutEasingFunction()->GetIsStarted() && !state.GetInterjectionDarkeningOpacityOutEasingFunction()->GetIsFinished())
        {
            interjectionBackgroundOpacity = state.GetInterjectionDarkeningOpacityOutEasingFunction()->GetCurrentValue();
        }

        pBackgroundDarkeningImage->Draw(Vector2(0, 0), Color(interjectionBackgroundOpacity / 255.0, 1.0, 1.0, 1.0));
        Case::GetInstance()->GetDialogCharacterManager()->DrawInterjectionForState(&state);
    }

    pCharacterNameTab->Draw(windowPosition.GetX(), windowPosition.GetY() + dialogBackgroundYPosition);

    if (pCurrentButtonArray == pMainMenuButtonArray && !pEvidenceSelector->GetIsShowing() && pCurrentConversation == NULL)
    {
        pExitTab->Draw(windowPosition.GetX(), windowPosition.GetY() + dialogBackgroundYPosition);
        pPresentEvidenceTab->Draw(windowPosition.GetX(), windowPosition.GetY() + dialogBackgroundYPosition);
    }

    pDialogBackgroundImage->Draw(Vector2(windowPosition.GetX(), windowPosition.GetY() + dialogBackgroundYPosition));

    if (!pEvidenceSelector->GetIsShowing())
    {
        if (pCurrentConversation != NULL && pDialogBackgroundInEase->GetIsFinished())
        {
            pCurrentConversation->Draw(windowPosition.GetX(), windowPosition.GetY() + dialogBackgroundYPosition);
        }
        else if (pCurrentButtonArray != NULL)
        {
            pCurrentButtonArray->Draw(windowPosition.GetX(), windowPosition.GetY() + dialogBackgroundYPosition);
        }
    }
}

void Encounter::Reset()
{
    pExitTab->Reset();
    pPresentEvidenceTab->Reset();

    pBackgroundOpacityInEase->Reset();
    pDialogBackgroundInEase->Reset();
    pLeftCharacterInEase->Reset();
    pRightCharacterInEase->Reset();
    pBackgroundOpacityOutEase->Reset();
    pDialogBackgroundOutEase->Reset();
    pLeftCharacterOutEase->Reset();
    pRightCharacterOutEase->Reset();

    EventProviders::GetButtonArrayEventProvider()->ClearListener(this);
    EventProviders::GetEvidenceSelectorEventProvider()->ClearListener(this);

    shouldCloseButtonArray = false;
}

void Encounter::OnButtonArrayButtonClicked(ButtonArray *pSender, int id)
{
    if (pSender == pMainMenuButtonArray)
    {
        OnMainMenuButtonClicked(id);
    }
}

void Encounter::OnButtonArrayCanceled(ButtonArray *pSender)
{
    // Nothing to do here - no button array is cancelable.
}

void Encounter::OnEvidenceSelectorEvidencePresented(EvidenceSelector *pSender, const string &evidenceId)
{
    Conversation *pConversationToUse = NULL;

    if (pSender != pEvidenceSelector)
    {
        return;
    }

    lastPresentedEvidenceId = evidenceId;

    if (presentEvidenceConversationDictionary.count(evidenceId) > 0)
    {
        pConversationToUse = presentEvidenceConversationDictionary[evidenceId];
    }
    else
    {
        pConversationToUse =
            Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdProfile(evidenceId) ?
            GetPresentWrongProfileConversation() :
            GetPresentWrongEvidenceConversation();
    }

    if (pCurrentButtonArray != NULL)
    {
        pLastButtonArrayBeforeConversation = pCurrentButtonArray;
    }

    BeginConversation(pConversationToUse);
}

void Encounter::RefreshButtonArrays()
{
    pNextButtonArray = pMainMenuButtonArray;
    SwapButtonArrays();
}

void Encounter::RefreshButtonArrayContents()
{
    vector<ButtonArrayLoadParameters> loadParametersList;

    for (unsigned int i = 0; i < conversationList.size(); i++)
    {
        Conversation *pConversation = conversationList[i];

        if (pConversation->GetIsEnabled() && (pConversation->GetRequiredPartnerId().length() == 0 || !pConversation->GetHasBeenCompleted()))
        {
            ButtonArrayLoadParameters loadParameters;

            loadParameters.text = pConversation->GetName();

            if (pConversation->GetIsLocked() || (pConversation->GetRequiredPartnerId().length() > 0 && pConversation->GetWrongPartnerUsed() && Case::GetInstance()->GetPartnerManager()->GetPartnerFromId(pConversation->GetRequiredPartnerId()) != NULL))
            {
                loadParameters.lockCount = pConversation->GetIsLocked() ? pConversation->GetLockCount() : 1;
            }

            loadParameters.unlockedLockCount = 0;

            vector<Conversation::UnlockCondition *> *pUnlockConditions = pConversation->GetUnlockConditions();

            for (unsigned int i = 0; i < pUnlockConditions->size(); i++)
            {
                Conversation::UnlockCondition *pUnlockCondition = (*pUnlockConditions)[i];

                if (pUnlockCondition->GetIsConditionMet() && !pUnlockCondition->GetHasHandledMetCondition())
                {
                    loadParameters.unlockedLockCount++;
                    pUnlockCondition->SetHasHandledMetCondition(true);
                }
            }

            loadParameters.showCheckBox = pConversation->GetHasBeenCompleted();

            loadParametersList.push_back(loadParameters);
        }
    }

    for (unsigned int i = 0; i < interrogationList.size(); i++)
    {
        Interrogation *pInterrogation = interrogationList[i];

        if (!pInterrogation->GetHasBeenCompleted())
        {
            if (pInterrogation->GetIsEnabled())
            {
                ButtonArrayLoadParameters loadParameters;

                char text[256];
                sprintf(text, pgLocalizableContent->GetText("Encounter/InterrogationDesignationFormatText").c_str(), pInterrogation->GetName().c_str());

                loadParameters.text = string(text);

                if (pInterrogation->GetRequiredPartnerId().length() > 0 && pInterrogation->GetWrongPartnerUsed() && Case::GetInstance()->GetPartnerManager()->GetPartnerFromId(pInterrogation->GetRequiredPartnerId()) != NULL)
                {
                    loadParameters.lockCount = 1;
                }

                loadParametersList.push_back(loadParameters);
            }
        }
    }

    for (unsigned int i = 0; i < confrontationList.size(); i++)
    {
        Confrontation *pConfrontation = confrontationList[i];

        if (!pConfrontation->GetHasBeenCompleted())
        {
            if (pConfrontation->GetIsEnabled())
            {
                ButtonArrayLoadParameters loadParameters;

                char text[256];
                sprintf(text, pgLocalizableContent->GetText("Encounter/ConfrontationDesignationFormatText").c_str(), pConfrontation->GetName().c_str());

                loadParameters.text = string(text);

                if (pConfrontation->GetRequiredPartnerId().length() > 0 && pConfrontation->GetWrongPartnerUsed() && Case::GetInstance()->GetPartnerManager()->GetPartnerFromId(pConfrontation->GetRequiredPartnerId()) != NULL)
                {
                    loadParameters.lockCount = 1;
                }

                loadParametersList.push_back(loadParameters);
            }
        }
    }

    pMainMenuButtonArray->Load(loadParametersList);
    pPresentEvidenceTab->SetIsEnabled(Case::GetInstance()->GetEvidenceManager()->GetHasEvidence());
}

void Encounter::PrepNextButtonArray(ButtonArray *pNextButtonArray, int buttonClickedId)
{
    this->pNextButtonArray = pNextButtonArray;

    if (buttonClickedId != -1)
    {
        pCurrentButtonArray->ReorderOutAnimations(buttonClickedId);
    }

    shouldCloseButtonArray = true;
}

void Encounter::SwapButtonArrays()
{
    if (pNextButtonArray == NULL)
    {
        // Save away the current button array here if the next button array is null -
        // we want to return to the current button array, not the temporary null array.
        pLastButtonArrayBeforeConversation = pCurrentButtonArray;
    }

    pCurrentButtonArray = pNextButtonArray;
    pNextButtonArray = NULL;

    if (pCurrentButtonArray != NULL)
    {
        pCurrentButtonArray->Show();
    }
}

void Encounter::BeginConversation(Conversation *pConversation)
{
    pCurrentConversation = pConversation;
    pCurrentConversation->Begin(&state);
}

void Encounter::PrepNextConversation(Conversation *pNextConversation, int buttonClickedId)
{
    this->pNextConversation = pNextConversation;
    pCurrentButtonArray->ReorderOutAnimations(buttonClickedId);

    if (pCurrentButtonArray != NULL)
    {
        pLastButtonArrayBeforeConversation = pCurrentButtonArray;
    }

    shouldCloseButtonArray = true;
}

void Encounter::BeginNextConversation()
{
    pCurrentConversation = pNextConversation;
    pNextConversation = NULL;
    pCurrentConversation->Begin(&state);
}

void Encounter::OnMainMenuButtonClicked(int id)
{
    int originalId = id;

    // No buttons are in the list for conversations that aren't available,
    // so we want to bump up the ID to account for these.
    for (int i = 0; i <= id; i++)
    {
        Conversation *pConversation = allConversationList[i];

        if (!pConversation->GetIsEnabled() ||
                (pConversation->GetRequiredPartnerId().length() > 0 && pConversation->GetHasBeenCompleted()) ||
                (pConversation->GetDisappearsAfterCompleted() && pConversation->GetHasBeenCompleted()))
        {
            id++;
        }
    }

    PrepNextConversation(allConversationList[id], originalId);
}

void Encounter::InitializeEasingFunctions()
{
    pBackgroundOpacityInEase = new LinearEase(0, 255, OpacityAnimationDuration);
    pDialogBackgroundInEase = new QuadraticEase(Dialog::Height + TabHeight - 3, 0, DialogBackgroundAnimationDuration);
    pLeftCharacterInEase = new QuadraticEase(-gScreenWidth / 2, 0, CharacterAnimationDuration);
    pLeftCharacterInEase->SetAnimationStartDelay(max(OpacityAnimationDuration, DialogBackgroundAnimationDuration));
    pRightCharacterInEase = new QuadraticEase(gScreenWidth / 2, 0, CharacterAnimationDuration);
    pRightCharacterInEase->SetAnimationStartDelay(max(OpacityAnimationDuration, DialogBackgroundAnimationDuration));
    pBackgroundOpacityOutEase = new LinearEase(255, 0, OpacityAnimationDuration);
    pBackgroundOpacityOutEase->SetAnimationStartDelay(CharacterAnimationDuration);
    pDialogBackgroundOutEase = new QuadraticEase(0, Dialog::Height + TabHeight - 3, DialogBackgroundAnimationDuration);
    pDialogBackgroundOutEase->SetAnimationStartDelay(CharacterAnimationDuration);
    pLeftCharacterOutEase = new QuadraticEase(0, -gScreenWidth / 2, CharacterAnimationDuration);
    pRightCharacterOutEase = new QuadraticEase(0, gScreenWidth / 2, CharacterAnimationDuration);
}

void Encounter::Close()
{
    pBackgroundOpacityOutEase->Begin();
    pDialogBackgroundOutEase->Begin();
    pLeftCharacterOutEase->Begin();
    pRightCharacterOutEase->Begin();

    state.SetLeftCharacterXOffsetEasingFunction(pLeftCharacterOutEase);
    state.SetRightCharacterXOffsetEasingFunction(pRightCharacterOutEase);
}
