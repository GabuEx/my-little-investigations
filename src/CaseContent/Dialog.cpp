/**
 * Handles a single line of dialog in the game.
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

#include "Dialog.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../ResourceLoader.h"
#include "../CaseInformation/Case.h"

#ifdef ENABLE_DEBUG_MODE
#include "../FileFunctions.h"
#include "../CaseInformation/CommonCaseResources.h"
#endif

int Dialog::Height = 180; // px

const double DefaultMillisecondsPerCharacterUpdate = 33;

Color NormalTextColor;
const Color AsideTextColor = Color(1.0, 0.529, 0.809, 0.921); //Color.SkyBlue from C#
const Color EmphasisTextColor = Color(1.0, 1.0, 0.2, 0.2);

const string LetterBlipSoundEffect = "LetterBlip";

RectangleWH Dialog::textAreaRect = RectangleWH(0, 0, 0, 0);
double Dialog::desiredPadding = 0;
MLIFont *Dialog::pDialogFont = NULL;

Dialog::Dialog(const string &filePath, int timeBeforeDialogInitial, int delayBeforeContinuing, bool isInterrogation, bool isPassive, bool isConfrontation, bool canNavigateBack, bool canNavigateForward, bool presentEvidenceAutomatically, bool canStopPresentingEvidence)
{
    this->curTextPosition = 0;

    this->millisecondsPerCharacterUpdate = 0;
    this->millisecondsSinceLastUpdate = 0;
    this->millisecondsUntilPauseCompletes = 0;
    this->millisecondsUntilAudioPauseCompletes = 0;
    this->lastPausePosition = -1;

    this->dialogEventIterator = dialogEventList.end();
    this->pCurrentDialogEvent = NULL;
    this->dialogEventIteratorSet = false;

    this->textColorStack.clear();
    this->textColorStack.push_back(TextColorNormal);
    this->lastTextColorChangeIndex = 0;

    this->timeSinceLetterBlipPlayed = numeric_limits<double>::max();

    this->filePath = filePath;
    this->timeBeforeDialogInitial = timeBeforeDialogInitial;
    this->delayBeforeContinuing = delayBeforeContinuing;
    this->isInterrogation = isInterrogation;
    this->isPassive = isPassive;
    this->isConfrontation = isConfrontation;
    this->isStatic = false;

    this->pPressForInfoTab = new Tab(gScreenWidth - 3 * TabWidth - 14, true /* isClickable */, gpLocalizableContent->GetText("Dialog/PressForInfoText"));
    this->pPresentEvidenceTab = new Tab(gScreenWidth - 2 * TabWidth - 7, true /* isClickable */, gpLocalizableContent->GetText("Dialog/PresentEvidenceText"));
    this->pCurrentPartner = NULL;
    this->pUsePartnerTab = new Tab(gScreenWidth / 2 - TabWidth / 2, true /* isClickable */, "", false /* useCancelClickSoundEffect */, TabRowTop);
    this->pEndInterrogationTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, isConfrontation ? gpLocalizableContent->GetText("Dialog/BackToTopicsText") : gpLocalizableContent->GetText("Dialog/EndInterrogationText"));

    this->pState = NULL;

    this->pConversationDownArrow = new Arrow(927, 505, ArrowDirectionDown, 10 /* bounceDistance */, false /* isClickable */);

    this->canNavigateBack = canNavigateBack;
    this->canNavigateForward = canNavigateForward;

    this->pEvidenceSelector = new EvidenceSelector(canStopPresentingEvidence);
    this->presentEvidenceAutomatically = presentEvidenceAutomatically;
    this->evidencePresented = false;
    this->endRequested = false;
    this->evidenceSelectorShownOnce = false;

    this->pInterrogationUpArrow = new Arrow(927, 375, ArrowDirectionUp, 10 /* bounceDistance */, true /* isClickable */);
    this->pInterrogationDownArrow = new Arrow(927, 505, ArrowDirectionDown, 10 /* bounceDistance */, true /* isClickable */);
}

Dialog::~Dialog()
{
    for (vector<DialogEvent *>::iterator iter = dialogEventListOriginal.begin(); iter != dialogEventListOriginal.end(); ++iter)
    {
        delete *iter;
    }

    for (vector<DialogEvent *>::iterator iter = dialogEventList.begin(); iter != dialogEventList.end(); ++iter)
    {
        delete *iter;
    }

    delete pConversationDownArrow;
    pConversationDownArrow = NULL;
    delete pEvidenceSelector;
    pEvidenceSelector = NULL;
    delete pInterrogationUpArrow;
    pInterrogationUpArrow = NULL;
    delete pInterrogationDownArrow;
    pInterrogationDownArrow = NULL;

    delete pPressForInfoTab;
    pPressForInfoTab = NULL;
    delete pPresentEvidenceTab;
    pPresentEvidenceTab = NULL;
    delete pUsePartnerTab;
    pUsePartnerTab = NULL;
    delete pEndInterrogationTab;
    pEndInterrogationTab = NULL;

    pState = NULL;

    if (filePath.length() > 0)
    {
        ResourceLoader::GetInstance()->UnloadDialog(filePath);
    }

    EventProviders::GetEvidenceSelectorEventProvider()->ClearListener(this);
}

void Dialog::Initialize(
    int textAreaLeft,
    int textAreaTop,
    int textAreaWidth,
    int textAreaHeight,
    double desiredPadding,
    MLIFont *pDialogFont)
{
    NormalTextColor = Color::White;
    Dialog::textAreaRect = RectangleWH(textAreaLeft, textAreaTop, textAreaWidth, textAreaHeight);
    Dialog::desiredPadding = desiredPadding;
    Dialog::pDialogFont = pDialogFont;
}

Dialog * Dialog::CreateForString(const string &dialogText)
{
    Dialog *pDialog =
        CreateForString(
            dialogText,
            "" /* filePath */,
            0 /* timeBeforeDialogInitial */,
            0 /* delayBeforeContinuing */,
            false /* isInterrogation */,
            false /* isPassive */,
            false /* isConfrontation */,
            false /* canNavigateBack */,
            false /* canNavigateForward */,
            false /* presentEvidenceAutomatically */,
            false /* canStopPresentingEvidence */);

    pDialog->isStatic = true;
    return pDialog;
}

Dialog * Dialog::CreateForString(const string &dialogText, const string &filePath, int timeBeforeDialogInitial, int delayBeforeContinuing, bool isInterrogation, bool isPassive, bool isConfrontation, bool canNavigateBack, bool canNavigateForward, bool presentEvidenceAutomatically, bool canStopPresentingEvidence)
{
    Dialog *pDialog = new Dialog(filePath, timeBeforeDialogInitial, delayBeforeContinuing, isInterrogation, isPassive, isConfrontation, canNavigateBack, canNavigateForward, presentEvidenceAutomatically, canStopPresentingEvidence);
    string fullString = ParseRawDialog(pDialog, dialogText, textAreaRect, desiredPadding, pDialogFont);

    pDialog->SetText(fullString);

    if (delayBeforeContinuing >= 0)
    {
        pDialog->AddPausePosition(static_cast<int>(fullString.length()), delayBeforeContinuing);
    }

    if (filePath.length() > 0)
    {
        ResourceLoader::GetInstance()->PreloadDialog(filePath, filePath);
    }

    return pDialog;
}

void Dialog::StartAside(int position)
{
    Interval interval = Interval(textColorStack.back(), lastTextColorChangeIndex, position);

    textIntervalList.push_back(interval);
    AddMouthChangePosition(position, false /* mouthIsOn */);

    textColorStack.push_back(TextColorAside);
    lastTextColorChangeIndex = position;
}

void Dialog::EndAside(int position, int eventEnd, int parsedStringLength, string *pStringToPrependOnNext)
{
    if (textColorStack.back() == TextColorAside)
    {
        Interval interval = Interval(TextColorAside, lastTextColorChangeIndex, position);

        if (eventEnd + 1 == parsedStringLength)
        {
            *pStringToPrependOnNext = "{Mouth:On}";
        }
        else
        {
            AddMouthChangePosition(position, true /* mouthIsOn */);
        }

        textIntervalList.push_back(interval);
        textColorStack.pop_back();
        lastTextColorChangeIndex = position;
    }
}

void Dialog::StartEmphasis(int position)
{
    Interval interval = Interval(textColorStack.back(), lastTextColorChangeIndex, position);

    textIntervalList.push_back(interval);

    textColorStack.push_back(TextColorEmphasis);
    lastTextColorChangeIndex = position;
}

void Dialog::EndEmphasis(int position)
{
    if (textColorStack.back() == TextColorEmphasis)
    {
        Interval interval = Interval(TextColorEmphasis, lastTextColorChangeIndex, position);

        textIntervalList.push_back(interval);
        textColorStack.pop_back();

        lastTextColorChangeIndex = position;
    }
}

string Dialog::GetString()
{
    if (this->GetIsStarted())
    {
        if (this->GetIsReadyToProgress())
        {
            return this->GetText();
        }
        else
        {
            return this->GetText().substr(0, lastPausePosition >= 0 ? this->lastPausePosition : this->curTextPosition);
        }
    }
    else
    {
        return "";
    }
}

void Dialog::Begin(State *pState)
{
    Reset();
    CloneDialogEventList();

    timeBeforeDialog = (double)timeBeforeDialogInitial;
    currentTime = 0;

    stack<double> textDisplaySpeedStack;
    stack<SpeedChangeEvent *> speedChangeEventStack;

    textDisplaySpeedStack.push(DefaultMillisecondsPerCharacterUpdate);

    // First, we want to check for any speaker emotions with a lead-in time.
    // We'll back that emotion up such that the lead-in time will complete
    // as soon as we reach the location where that emotion is actually defined.
    for (unsigned int i = 0; i < dialogEventList.size(); i++)
    {
        DialogEvent *pEvent = dialogEventList[i];

        SpeedChangeEvent *pSpeedChangeEvent = dynamic_cast<SpeedChangeEvent *>(pEvent);

        if (pSpeedChangeEvent != NULL)
        {
            speedChangeEventStack.push(pSpeedChangeEvent);
            textDisplaySpeedStack.push(pSpeedChangeEvent->GetNewMillisecondsPerCharacterUpdate());
            continue;
        }

        SpeakerEmotionChangeEvent *pSpeakerEmotionChange = dynamic_cast<SpeakerEmotionChangeEvent *>(pEvent);

        if (pSpeakerEmotionChange != NULL)
        {
            string speakerCharacterId;

            if (pState->GetSpeakerPosition() == CharacterPositionLeft)
            {
                speakerCharacterId = pState->GetLeftCharacterId();
            }
            else if (pState->GetSpeakerPosition() == CharacterPositionRight)
            {
                speakerCharacterId = pState->GetRightCharacterId();
            }

            if (speakerCharacterId.length() > 0)
            {
                DialogCharacter *pSpeakerCharacter = Case::GetInstance()->GetDialogCharacterManager()->GetCharacterFromId(speakerCharacterId);
                int leadInTime = pSpeakerCharacter->GetLeadInTimeForEmotion(pSpeakerEmotionChange->GetNewEmotionId());

                if (leadInTime > 0)
                {
                    DialogEvent *pEmotionEvent = pSpeakerEmotionChange;
                    dialogEventList.erase(dialogEventList.begin() + i);

                    int lastPosition = pEmotionEvent->GetPosition();

                    for (int j = (int)(i - 1); j >= 0; j--)
                    {
                        DialogEvent *pCurrentEvent = dialogEventList[j];
                        int newPosition = pCurrentEvent->GetPosition();

                        if (newPosition < lastPosition)
                        {
                            double millisecondsElapsed = textDisplaySpeedStack.top() * (lastPosition - newPosition);

                            if (millisecondsElapsed >= leadInTime)
                            {
                                leadInTime = 0;
                                newPosition = (int)(lastPosition - leadInTime / textDisplaySpeedStack.top());
                                pEmotionEvent->SetPosition(newPosition);
                                dialogEventList.insert(dialogEventList.begin() + j + 1, pEmotionEvent);
                                break;
                            }
                            else
                            {
                                leadInTime -= millisecondsElapsed;
                            }

                            lastPosition = newPosition;
                        }

                        PauseEvent *pPauseEvent = dynamic_cast<PauseEvent *>(pCurrentEvent);

                        if (pPauseEvent != NULL)
                        {
                            if (pPauseEvent->GetDuration() > leadInTime)
                            {
                                pPauseEvent->SetDuration(pPauseEvent->GetDuration() - leadInTime);
                                dialogEventList.insert(dialogEventList.begin() + j + 1, pEmotionEvent);
                                dialogEventList.insert(dialogEventList.begin() + j + 2, new PauseEvent(lastPosition, this, leadInTime));

                                i++;
                                leadInTime = 0;
                                break;
                            }
                            else
                            {
                                leadInTime -= pPauseEvent->GetDuration();
                            }
                        }
                    }

                    if (leadInTime > timeBeforeDialog)
                    {
                        dialogEventList.insert(dialogEventList.begin(), pEmotionEvent);
                        dialogEventList.insert(dialogEventList.begin() + 1, new PauseEvent(0, this, leadInTime));
                        timeBeforeDialog = leadInTime;
                    }
                }
            }
        }
    }

    // If we're playing vocals, then we'll add 300 milliseconds of audio pause
    // to the end of every lengthy pause.
    // Otherwise, we'll cap pauses at 500 ms, since there's
    // no audio to sync the text to in that case.
    // The exception is automatic dialogs - these may be timed
    // to background music, so we'll leave them as is.
    for (unsigned int i = 0; i < dialogEventList.size(); i++)
    {
        PauseEvent *pPauseEvent = dynamic_cast<PauseEvent *>(dialogEventList[i]);

        if (pPauseEvent != NULL)
        {
            double millisecondDuration = pPauseEvent->GetDuration();
            dialogEventList.erase(dialogEventList.begin() + i);

            if (gVoiceVolume > 0)
            {
                if (millisecondDuration > 500)
                {
                    pPauseEvent->SetDuration(millisecondDuration - 300);
                    dialogEventList.insert(dialogEventList.begin() + i, pPauseEvent);
                    dialogEventList.insert(dialogEventList.begin() + i + 1, new AudioPauseEvent(pPauseEvent->GetPosition(), this, millisecondDuration - pPauseEvent->GetDuration()));
                    i++;
                }
                else
                {
                    pPauseEvent->SetDuration(millisecondDuration * 2 / 5);
                    dialogEventList.insert(dialogEventList.begin() + i, pPauseEvent);
                    dialogEventList.insert(dialogEventList.begin() + i + 1, new AudioPauseEvent(pPauseEvent->GetPosition(), this, millisecondDuration - pPauseEvent->GetDuration()));
                    i++;
                }
            }
            else
            {
                dialogEventList.insert(dialogEventList.begin() + i, new AudioPauseEvent(pPauseEvent->GetPosition(), this, GetIsAutomatic() ? millisecondDuration : 500));
            }
        }
    }

    pInterrogationUpArrow->SetIsEnabled(canNavigateBack);
    pInterrogationDownArrow->SetIsEnabled(canNavigateForward);

    evidenceSelectorShownOnce = false;
    pEvidenceSelector->Begin();
    EventProviders::GetEvidenceSelectorEventProvider()->ClearListener(this);
    EventProviders::GetEvidenceSelectorEventProvider()->RegisterListener(this);

    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0)
    {
        pCurrentPartner = Case::GetInstance()->GetPartnerManager()->GetCurrentPartner();

        if (pCurrentPartner->GetConversationAbilityName().length() > 0)
        {
            pUsePartnerTab->SetText(pCurrentPartner->GetConversationAbilityName());
            pUsePartnerTab->SetTabImage(pCurrentPartner->GetIconSprite());
        }
    }

    millisecondsPerCharacterUpdate = DefaultMillisecondsPerCharacterUpdate;
    endRequested = false;

    this->pState = pState;

    pState->SetIsLeftCharacterTalking(false);
    pState->SetIsRightCharacterTalking(false);

    // If this dialog is static, then we'll immediately finish.
    if (isStatic)
    {
        Finish();
    }
}

void Dialog::Update(int delta)
{
    bool newCharacterDrawn = false;

    double lastCurrentTime = currentTime;
    currentTime += delta;

    // If we're now ready to play the dialog, do so.
    if (timeBeforeDialog >= lastCurrentTime && timeBeforeDialog < currentTime)
    {
        if (filePath.length() > 0 && gVoiceVolume > 0 && !GetIsFinished())
        {
            playDialog(filePath);
        }
    }

    if (!dialogEventIteratorSet && !dialogEventList.empty())
    {
        dialogEventIterator = dialogEventList.begin();
        pCurrentDialogEvent = *dialogEventIterator;
        ++dialogEventIterator;
        dialogEventIteratorSet = true;
    }

    if (!GetIsPaused())
    {
        lastPausePosition = -1;

        if (!GetIsFinished())
        {
            millisecondsSinceLastUpdate += delta;

            if (!GetIsStarted() || millisecondsSinceLastUpdate > millisecondsPerCharacterUpdate)
            {
                int positionsToAdvance = (int)(millisecondsSinceLastUpdate / millisecondsPerCharacterUpdate);

                string::const_iterator begin = GetText().begin() + curTextPosition;
                string::const_iterator end = begin;

                if (AdvanceStringIterator(end, positionsToAdvance, GetText().end()))
                {
                    curTextPosition = static_cast<int>(curTextPosition + distance(begin, end));
                }
                else
                {
                    curTextPosition = (int)GetText().length();
                }

                millisecondsSinceLastUpdate = max(millisecondsSinceLastUpdate - positionsToAdvance * millisecondsPerCharacterUpdate, 0.0);
                newCharacterDrawn = true;
            }
        }
    }
    else
    {
        millisecondsSinceLastUpdate = 0;
        millisecondsUntilPauseCompletes -= delta;
        millisecondsUntilAudioPauseCompletes -= delta;

        if (millisecondsUntilPauseCompletes < 0)
        {
            // Account for going over by adding the amount we went over
            // to the milliseconds since we updated,
            // such that we'll update sooner if we went over.
            millisecondsSinceLastUpdate += -millisecondsUntilPauseCompletes;
            millisecondsUntilPauseCompletes = 0;
        }

        if (millisecondsUntilAudioPauseCompletes < 0)
        {
            millisecondsUntilAudioPauseCompletes = 0;
        }
    }

    if (!GetIsPaused() && pCurrentDialogEvent != NULL)
    {
        while (curTextPosition >= pCurrentDialogEvent->GetPosition())
        {
            int eventPosition = pCurrentDialogEvent->GetPosition();
            pCurrentDialogEvent->RaiseEvent();

            if (dialogEventIterator != dialogEventList.end())
            {
                pCurrentDialogEvent = *dialogEventIterator;
                ++dialogEventIterator;
            }
            else
            {
                pCurrentDialogEvent = NULL;
            }

            if (GetIsPaused() || pCurrentDialogEvent == NULL)
            {
                if (GetIsPaused())
                {
                    lastPausePosition = eventPosition;
                }

                break;
            }
        }
    }

    // We'll only play the letter blips if voice acting isn't enabled.
    if (gVoiceVolume <= 0 && !GetIsReadyToProgress())
    {
        timeSinceLetterBlipPlayed += delta;

        if (newCharacterDrawn && timeSinceLetterBlipPlayed > 50)// TODO letterBlipSoundEffect.Duration.TotalMilliseconds)
        {
            playSound(LetterBlipSoundEffect);
            timeSinceLetterBlipPlayed = 0;
        }
    }

    if (GetIsReadyToProgress())
    {
        if (presentEvidenceAutomatically && !pEvidenceSelector->GetIsShowing())
        {
            if (!evidenceSelectorShownOnce)
            {
                pEvidenceSelector->Show();
                evidenceSelectorShownOnce = true;

                Confrontation *pConfrontation = pState->GetCurrentConfrontation();

                if (pConfrontation != NULL)
                {
                    pConfrontation->HideHealthIcons();
                }
            }
            else
            {
                if (!evidencePresented)
                {
                    // If the evidence selector isn't showing and it was shown once,
                    // and if no evidence has been shown, then the player must've
                    // clicked Cancel, so we'll end the conversation here
                    // since progress depends on presenting the right evidence.
                    OnEndRequested();
                }

                endRequested = true;
            }
        }
        else if (!presentEvidenceAutomatically)
        {
            endRequested = true;
        }

        if (isInterrogation)
        {
            if (pEvidenceSelector->GetIsShowing())
            {
                pEvidenceSelector->Update(delta);
            }
            else if (!pState->WasInterjectionOngoing())
            {
                if (!isPassive)
                {
                    pPressForInfoTab->Update();
                    pPresentEvidenceTab->Update();

                    if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && pCurrentPartner->GetConversationAbilityName().length() > 0)
                    {
                        pUsePartnerTab->Update();
                    }
                }

                if (!isConfrontation || Confrontation::GetEnabledConfrontationTopicCount() > 1)
                {
                    pEndInterrogationTab->Update();
                }

                pInterrogationUpArrow->Update(delta);
                pInterrogationDownArrow->Update(delta);
            }
        }
        else
        {
            if (pEvidenceSelector->GetIsShowing())
            {
                pEvidenceSelector->Update(delta);
            }
            else if (!GetIsAutomatic() && !isStatic)
            {
                pConversationDownArrow->Update(delta);
            }
        }
    }

    if (isInterrogation && !pState->WasInterjectionOngoing())
    {
        if (!isPassive)
        {
            pPressForInfoTab->SetIsEnabled(GetIsReadyToProgress());
            pPresentEvidenceTab->SetIsEnabled(GetIsReadyToProgress() && Case::GetInstance()->GetEvidenceManager()->GetHasEvidence());

            if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && pCurrentPartner->GetConversationAbilityName().length() > 0)
            {
                pUsePartnerTab->SetIsEnabled(GetIsReadyToProgress());
            }
        }

        if (!isConfrontation || Confrontation::GetEnabledConfrontationTopicCount() > 1)
        {
            pEndInterrogationTab->SetIsEnabled(GetIsReadyToProgress());
        }
    }
}

bool Dialog::HandleClick()
{
    // If this dialog is automatic or static, then we'll ignore every click.
    if (GetIsAutomatic() || isStatic)
    {
        return false;
    }

    if (isInterrogation && !pEvidenceSelector->GetIsShowing() && !pState->WasInterjectionOngoing())
    {
        if (!isPassive)
        {
            pPressForInfoTab->Update();
            pPresentEvidenceTab->Update();

            if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && pCurrentPartner->GetConversationAbilityName().length() > 0)
            {
                pUsePartnerTab->Update();
            }
        }

        if (!isConfrontation || Confrontation::GetEnabledConfrontationTopicCount() > 1)
        {
            pEndInterrogationTab->Update();
        }

        pInterrogationUpArrow->UpdateState();
        pInterrogationDownArrow->UpdateState();
    }
    else if (pEvidenceSelector->GetIsShowing())
    {
        pEvidenceSelector->UpdateState();
        return true;
    }
    else if (presentEvidenceAutomatically && !pEvidenceSelector->GetIsShowing() && GetIsReadyToProgress())
    {
        // If the evidence selector is being shown automatically, is not presently showing,
        // and if the dialog is ready to progress, then the user must've canceled.
        // We'll ignore clicks in this case, since we're about to exit.
        return true;
    }

    if (!GetIsReadyToProgress())
    {
        //SetTextSkipped(true);
        Finish();
        return true;
    }
    else if (
        pPressForInfoTab->GetIsClicked() ||
        pPresentEvidenceTab->GetIsClicked() ||
        pUsePartnerTab->GetIsClicked() ||
        pEndInterrogationTab->GetIsClicked() ||
        pInterrogationUpArrow->GetIsClicked() ||
        pInterrogationDownArrow->GetIsClicked())
    {
        if (pPressForInfoTab->GetIsClicked())
        {
            if (filePath.length() > 0 && gVoiceVolume > 0)
            {
                stopDialog();
            }

            Finish();
            OnPressForInfoClicked();
            pPressForInfoTab->SetIsClicked(false);
        }
        else if (pPresentEvidenceTab->GetIsClicked())
        {
            pEvidenceSelector->Begin();
            pEvidenceSelector->Show();

            Confrontation *pConfrontation = pState->GetCurrentConfrontation();

            if (pConfrontation != NULL)
            {
                pConfrontation->HideHealthIcons();
            }

            pPresentEvidenceTab->SetIsClicked(false);
        }
        else if (pUsePartnerTab->GetIsClicked())
        {
            if (filePath.length() > 0 && gVoiceVolume > 0)
            {
                stopDialog();
            }

            Finish();
            OnUsePartnerClicked();
            pUsePartnerTab->SetIsClicked(false);
        }
        else if (pEndInterrogationTab->GetIsClicked())
        {
            if (filePath.length() > 0 && gVoiceVolume > 0)
            {
                stopDialog();
            }

            Finish();
            OnEndRequested();
            pEndInterrogationTab->SetIsClicked(false);
        }
        else if (pInterrogationUpArrow->GetIsClicked())
        {
            if (filePath.length() > 0 && gVoiceVolume > 0)
            {
                stopDialog();
            }

            Finish();
            OnDirectlyNavigated(DirectNavigationDirectionBack);
            pInterrogationUpArrow->SetIsClicked(false);
        }
        else if (pInterrogationDownArrow->GetIsClicked())
        {
            if (filePath.length() > 0 && gVoiceVolume > 0)
            {
                stopDialog();
            }

            Finish();
            OnDirectlyNavigated(DirectNavigationDirectionForward);
            pInterrogationDownArrow->SetIsClicked(false);
        }

        return true;
    }

    return false;
}

bool Dialog::HandleKeypress()
{
    if(KeyboardHelper::UpPressed() && canNavigateBack)
    {
        if (filePath.length() > 0 && gVoiceVolume > 0)
        {
            stopDialog();
        }

        Finish();
        OnDirectlyNavigated(DirectNavigationDirectionBack);
        return true;
    }
    if(KeyboardHelper::DownPressed() && canNavigateForward)
    {
        if (filePath.length() > 0 && gVoiceVolume > 0)
        {
            stopDialog();
        }

        Finish();
        OnDirectlyNavigated(DirectNavigationDirectionForward);
        return true;
    }
    return false;
}

Color Dialog::GetColorFromTextColor(TextColor textColor)
{
    switch (textColor)
    {
    case TextColorNormal:
        return NormalTextColor;

    case TextColorAside:
        return AsideTextColor;

    case TextColorEmphasis:
        return EmphasisTextColor;

    default:
        return NormalTextColor;
    }
}

void Dialog::Draw(double xOffset, double yOffset)
{
#ifdef ENABLE_DEBUG_MODE
    if (gEnableDebugMode && filePath.length() > 0)
    {
        MLIFont *pFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("MouseOverFont");
        pFont->Draw(GetFileNameFromFilePath(ConvertSeparatorsInPath(filePath + ".ogg")), Vector2(xOffset, yOffset + gScreenHeight - Dialog::Height - TabHeight + 3 - pFont->GetLineHeight()));
    }
#endif

    if (isInterrogation)
    {
        if (!pEvidenceSelector->GetIsShowing() && !evidencePresented && !pState->WasInterjectionOngoing())
        {
            if (!isPassive)
            {
                pPressForInfoTab->Draw(xOffset, yOffset);
                pPresentEvidenceTab->Draw(xOffset, yOffset);

                if (Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId().length() > 0 && pCurrentPartner->GetConversationAbilityName().length() > 0)
                {
                    pUsePartnerTab->Draw(0, yOffset);
                }
            }

            if (!isConfrontation || Confrontation::GetEnabledConfrontationTopicCount() > 1)
            {
                pEndInterrogationTab->Draw(xOffset, yOffset);
            }
        }
    }
    else if (pEvidenceSelector->GetIsShowing())
    {
        pEvidenceSelector->Draw(yOffset);
    }

    if (GetIsStarted())
    {
        Vector2 lineScreenPosition = Vector2(textAreaRect.GetX() + desiredPadding, textAreaRect.GetY() + desiredPadding);
        int curTextPosition = 0;
        TextColor curTextColor = TextColorNormal;
        deque<string> lines = split(GetString(), '\n');

        list<Interval>::iterator textIntervalEnumerator = textIntervalList.begin();
        Interval *pCurrentTextInterval = NULL;

        if (textIntervalEnumerator != textIntervalList.end())
        {
            pCurrentTextInterval = &(*textIntervalEnumerator);
            curTextColor = pCurrentTextInterval->Color;
            ++textIntervalEnumerator;
        }

        for (unsigned int i = 0; i < lines.size(); i++)
        {
            string line = lines[i];
            int curLineTextPosition = 0;
            Vector2 curScreenPosition = Vector2(lineScreenPosition.GetX(), lineScreenPosition.GetY());
            string lineRemainder = line;

            while (pCurrentTextInterval != NULL && pCurrentTextInterval->EndIndex - curTextPosition - curLineTextPosition <= (int)lineRemainder.length())
            {
                string linePortionToDraw = lineRemainder.substr(0, pCurrentTextInterval->EndIndex - curTextPosition - curLineTextPosition);
                lineRemainder = lineRemainder.substr(pCurrentTextInterval->EndIndex - curTextPosition - curLineTextPosition);
                pDialogFont->Draw(linePortionToDraw, Vector2(curScreenPosition.GetX() + xOffset, curScreenPosition.GetY() + yOffset), GetColorFromTextColor(curTextColor));
                curScreenPosition.SetX(curScreenPosition.GetX() + pDialogFont->GetWidth(linePortionToDraw));
                curLineTextPosition += linePortionToDraw.length();

                if (textIntervalEnumerator != textIntervalList.end())
                {
                    pCurrentTextInterval = &(*textIntervalEnumerator);
                    curTextColor = pCurrentTextInterval->Color;
                    ++textIntervalEnumerator;
                }
                else
                {
                    curTextColor = TextColorNormal;
                    pCurrentTextInterval = NULL;
                }
            }

            pDialogFont->Draw(lineRemainder, Vector2(curScreenPosition.GetX() + xOffset, curScreenPosition.GetY() + yOffset), GetColorFromTextColor(curTextColor));
            lineScreenPosition.SetY(lineScreenPosition.GetY() + pDialogFont->GetLineHeight());

            // Add one for the carriage return character.
            curTextPosition += line.length() + 1;
        }
    }

    if (GetIsReadyToProgress() && !evidencePresented)
    {
        if (isInterrogation)
        {
            if (!pEvidenceSelector->GetIsShowing())
            {
                pInterrogationUpArrow->Draw(xOffset, yOffset);
                pInterrogationDownArrow->Draw(xOffset, yOffset);
            }
        }
        else if (!pEvidenceSelector->GetIsShowing() && !GetIsAutomatic() && !isStatic)
        {
            pConversationDownArrow->Draw(xOffset, yOffset);
        }
    }
}

void Dialog::DrawBackground(double xOffset, double yOffset)
{
    if (isInterrogation)
    {
        if (pEvidenceSelector->GetIsShowing())
        {
            pEvidenceSelector->Draw(yOffset);
        }
    }
}

void Dialog::Finish(bool shouldPlaySfx)
{
    pState->SetIsFinishingDialog(true);
    shouldPlaySfx = shouldPlaySfx && !pState->GetIsFastForwarding();

    if (!dialogEventIteratorSet && !dialogEventList.empty())
    {
        dialogEventIterator = dialogEventList.begin();
        pCurrentDialogEvent = *dialogEventIterator;
        ++dialogEventIterator;
        dialogEventIteratorSet = true;
    }

    curTextPosition = static_cast<int>(GetText().length());

    if (pCurrentDialogEvent != NULL)
    {
        while (curTextPosition >= pCurrentDialogEvent->GetPosition())
        {
            if (pCurrentDialogEvent->GetShouldBeRaisedWhenFinishing() && (!pCurrentDialogEvent->PlaysSfx() || shouldPlaySfx))
            {
                pCurrentDialogEvent->RaiseEvent();
            }

            if (dialogEventIterator != dialogEventList.end())
            {
                pCurrentDialogEvent = *dialogEventIterator;
                ++dialogEventIterator;
            }
            else
            {
                pCurrentDialogEvent = NULL;
            }

            if (pCurrentDialogEvent == NULL)
            {
                break;
            }
        }
    }

    stopDialog();
    millisecondsUntilPauseCompletes = 0;
    millisecondsUntilAudioPauseCompletes = 0;
    lastPausePosition = -1;
    endRequested = true;
}

void Dialog::Reset()
{
    curTextPosition = 0;
    millisecondsUntilPauseCompletes = 0;
    millisecondsUntilAudioPauseCompletes = 0;
    dialogEventIterator = dialogEventList.end();
    pCurrentDialogEvent = NULL;
    dialogEventIteratorSet = false;
    timeSinceLetterBlipPlayed = numeric_limits<double>::max();

    textColorStack.clear();
    textColorStack.push_back(TextColorNormal);
    lastTextColorChangeIndex = 0;

    pPressForInfoTab->Reset();
    pPresentEvidenceTab->Reset();
    pCurrentPartner = NULL;
    pUsePartnerTab->Reset();
    pEndInterrogationTab->Reset();

    pConversationDownArrow->Reset();
    pInterrogationUpArrow->Reset();
    pInterrogationDownArrow->Reset();

    pEvidenceSelector->Reset();
    EventProviders::GetEvidenceSelectorEventProvider()->ClearListener(this);
    evidencePresented = false;
    lastPausePosition = -1;
    textSkipped = false;

    if (pState != NULL)
    {
        pState->SetIsLeftCharacterTalking(false);
        pState->SetIsRightCharacterTalking(false);
    }

    pState = NULL;

    if (filePath.length() > 0 && gVoiceVolume > 0)
    {
        stopDialog();
    }
}

void Dialog::OnEvidenceSelectorEvidencePresented(EvidenceSelector *pSender, const string &evidenceId)
{
    if (pSender == pEvidenceSelector)
    {
        stopDialog();

        evidencePresented = true;

        Finish();
        OnEvidencePresented(evidenceId);
    }
}

void Dialog::OnEvidenceSelectorClosing(EvidenceSelector *pSender)
{
    Confrontation *pConfrontation = pState->GetCurrentConfrontation();

    if (pConfrontation != NULL)
    {
        pConfrontation->ShowHealthIcons();
    }
}

void Dialog::PlayBgmEvent::RaiseEvent()
{
    Case::GetInstance()->GetAudioManager()->PlayBgmWithId(bgmId);
    pOwningDialog->pState->SetPreserveBgm(isPermanent);
}

void Dialog::StopBgmEvent::RaiseEvent()
{
    Case::GetInstance()->GetAudioManager()->StopCurrentBgm(isInstant);
    pOwningDialog->pState->SetPreserveBgm(isPermanent);
}
