/**
 * Basic header/include file for Dialog.cpp.
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

#ifndef DIALOG_H
#define DIALOG_H

#include "../Color.h"
#include "../globals.h"
#include "../mli_audio.h"
#include "../CaseInformation/PartnerManager.h"
#include "../Events/DialogEventProvider.h"
#include "../Events/EvidenceSelectorEventProvider.h"
#include "../UserInterface/Arrow.h"
#include "../UserInterface/EvidenceSelector.h"
#include "../UserInterface/Tab.h"
#include <string>

using namespace std;

class State;

class Dialog : public EvidenceSelectorEventListener
{
    class DialogEvent;

public:
    static int Height;

    Dialog(string filePath, int timeBeforeDialogInitial,int delayBeforeContinuing, bool isInterrogation, bool isPassive, bool isConfrontation, bool canNavigateBack, bool canNavigateForward, bool presentEvidenceAutomatically, bool canStopPresentingEvidence);
    virtual ~Dialog();

    static void Initialize(
        int textAreaLeft,
        int textAreaTop,
        int textAreaWidth,
        int textAreaHeight,
        double desiredPadding,
        MLIFont *pDialogFont);

    static Dialog * CreateForString(string dialogText);
    static Dialog * CreateForString(string dialogText, string filePath, int timeBeforeDialogInitial, int delayBeforeContinuing, bool isInterrogation, bool isPassive, bool isConfrontation, bool canNavigateBack, bool canNavigateForward, bool presentEvidenceAutomatically, bool canStopPresentingEvidence);

    string GetText() const { return this->text; }
    void SetText(string text) { this->text = text; }

    bool GetTextSkipped() const { return this->textSkipped; }
    void SetTextSkipped(bool textSkipped) { this->textSkipped = textSkipped; }

    bool GetIsStarted() const
    {
        return this->curTextPosition > 0;
    }

    bool GetIsPaused() const
    {
        return this->millisecondsUntilPauseCompletes > 0;
    }

    bool GetIsAudioPaused() const
    {
        return this->millisecondsUntilAudioPauseCompletes > 0;;
    }

    bool GetIsFinished()
    {
        return this->curTextPosition == (int)this->GetText().length() && this->pCurrentDialogEvent == NULL && !this->GetIsPaused() && (filePath.length() == 0 || getPlayingDialog() != filePath);
    }

    bool GetIsReadyToProgress()
    {
        return this->GetTextSkipped() || this->GetIsFinished();
    }

    bool GetIsReadyToHide()
    {
        return !this->pEvidenceSelector->GetIsShowing() && this->endRequested;
    }

    bool GetIsAutomatic() const
    {
        return this->delayBeforeContinuing >= 0;
    }

    static string StripEvents(string stringToStrip);

    void AddSpeedChangePosition(int position, double newMillisecondsPerCharacterUpdate)
    {
        this->dialogEventListOriginal.push_back(new SpeedChangeEvent(position, this, newMillisecondsPerCharacterUpdate));
    }

    void AddEmotionChangePosition(int position, string newEmotionId)
    {
        this->dialogEventListOriginal.push_back(new SpeakerEmotionChangeEvent(position, this, newEmotionId));
    }

    void AddEmotionOtherChangePosition(int position, string newEmotionId)
    {
        this->dialogEventListOriginal.push_back(new OtherEmotionChangeEvent(position, this, newEmotionId));
    }

    void AddPausePosition(int position, double millisecondDuration)
    {
        this->dialogEventListOriginal.push_back(new PauseEvent(position, this, millisecondDuration));
    }

    void AddAudioPausePosition(int position, double millisecondDuration)
    {
        this->dialogEventListOriginal.push_back(new AudioPauseEvent(position, this, millisecondDuration));
    }

    void AddMouthChangePosition(int position, bool mouthIsOn)
    {
        this->dialogEventListOriginal.push_back(new MouthChangeEvent(position, this, mouthIsOn));
    }

    void AddPlaySoundPosition(int position, string id)
    {
        this->dialogEventListOriginal.push_back(new PlaySoundEvent(position, this, id));
    }

    void AddShakePosition(int position)
    {
        this->dialogEventListOriginal.push_back(new ShakeEvent(position, this));
    }

    void AddScreenShakePosition(int position, double shakeIntensity)
    {
        this->dialogEventListOriginal.push_back(new ScreenShakeEvent(position, this, shakeIntensity));
    }

    void AddNextFramePosition(int position)
    {
        this->dialogEventListOriginal.push_back(new NextFrameEvent(position, this));
    }

    void AddPlayerDamagedPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new PlayerDamagedEvent(position, this));
    }

    void AddOpponentDamagedPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new OpponentDamagedEvent(position, this));
    }

    void AddPlayBgmPosition(int position, string id)
    {
        this->dialogEventListOriginal.push_back(new PlayBgmEvent(position, this, id));
    }

    void AddPlayBgmPermanentlyPosition(int position, string id)
    {
        this->dialogEventListOriginal.push_back(new PlayBgmEvent(position, this, id, true /* isPermanent */));
    }

    void AddStopBgmPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, false /* isInstant */, false /* isPermanent */));
    }

    void AddStopBgmPermanentlyPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, false /* isInstant */, true /* isPermanent */));
    }

    void AddStopBgmInstantlyPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, true /* isInstant */, false /* isPermanent */));
    }

    void AddStopBgmInstantlyPermanentlyPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, true /* isInstant */, true /* isPermanent */));
    }

    void AddZoomPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new ZoomEvent(position, this));
    }

    void AddEndZoomPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new EndZoomEvent(position, this));
    }

    void AddBeginBreakdownPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new BeginBreakdownEvent(position, this));
    }

    void AddEndBreakdownPosition(int position)
    {
        this->dialogEventListOriginal.push_back(new EndBreakdownEvent(position, this));
    }

    string GetString();

    void Begin(State *pState);
    void Update(int delta);
    bool HandleClick();
    void Draw(double xOffset, double yOffset);
    void DrawBackground(double xOffset, double yOffset);
    void Finish(bool shouldPlaySfx = true);
    void Reset();

    void OnEvidenceSelectorEvidencePresented(EvidenceSelector *pSender, string evidenceId);
    void OnEvidenceSelectorClosing(EvidenceSelector *pSender);

private:
    string ParseEvents(int lineOffset, string stringToParse, string *pStringToPrependOnNext);

    void OnDirectlyNavigated(DirectNavigationDirection direction)
    {
        EventProviders::GetDialogEventProvider()->RaiseDirectlyNavigated(this, direction);
    }

    void OnPressForInfoClicked()
    {
        EventProviders::GetDialogEventProvider()->RaisePressForInfoClicked(this);
    }

    void OnEvidencePresented(string evidenceId)
    {
        EventProviders::GetDialogEventProvider()->RaiseEvidencePresented(this, evidenceId);
    }

    void OnUsePartnerClicked()
    {
        EventProviders::GetDialogEventProvider()->RaiseUsePartner(this);
    }

    void OnEndRequested()
    {
        EventProviders::GetDialogEventProvider()->RaiseEndRequested(this);
    }

    enum TextColor
    {
        TextColorNormal,
        TextColorAside,
        TextColorEmphasis,
    };

    Color GetColorFromTextColor(TextColor textColor);

    class Interval
    {
    public:
        TextColor Color;
        int StartIndex;
        int EndIndex;

        Interval(TextColor color, int startIndex, int endIndex)
        {
            this->Color = color;
            this->StartIndex = startIndex;
            this->EndIndex = endIndex;
        }
    };

    class DialogEvent
    {
    public:
        DialogEvent(int position, Dialog *pOwningDialog)
        {
            this->position = position;
            this->pOwningDialog = pOwningDialog;
        }

        virtual ~DialogEvent() {}

        int GetPosition() const { return this->position; }
        void SetPosition(int position) { this->position = position; }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return false;
        }

        virtual bool PlaysSfx()
        {
            return false;
        }

        virtual void RaiseEvent() = 0;
        virtual DialogEvent * Clone() = 0;

    protected:
        Dialog *pOwningDialog;
        int position;
    };

    static bool CompareEventsByPosition(DialogEvent *pDialogEvent1, DialogEvent *pDialogEvent2)
    {
        return pDialogEvent1->GetPosition() < pDialogEvent2->GetPosition();
    }

    class SpeedChangeEvent : public DialogEvent
    {
    public:
        SpeedChangeEvent(int position, Dialog *pOwningDialog, double newMillisecondsPerCharacterUpdate)
            : DialogEvent(position, pOwningDialog)
        {
            this->newMillisecondsPerCharacterUpdate = newMillisecondsPerCharacterUpdate;
        }

        double GetNewMillisecondsPerCharacterUpdate()
        {
            return newMillisecondsPerCharacterUpdate;
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            this->pOwningDialog->millisecondsPerCharacterUpdate = this->newMillisecondsPerCharacterUpdate;
        }

        virtual DialogEvent * Clone()
        {
            return new SpeedChangeEvent(this->position, this->pOwningDialog, this->newMillisecondsPerCharacterUpdate);
        }

    private:
        double newMillisecondsPerCharacterUpdate;
    };

    class SpeakerEmotionChangeEvent : public DialogEvent
    {
    public:
        SpeakerEmotionChangeEvent(int position, Dialog *pOwningDialog, string newEmotionId)
            : DialogEvent(position, pOwningDialog)
        {
            this->newEmotionId = newEmotionId;
        }

        string GetNewEmotionId()
        {
            return newEmotionId;
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseSpeakerEmotionChanged(this->pOwningDialog, this->newEmotionId);
        }

        virtual DialogEvent * Clone()
        {
            return new SpeakerEmotionChangeEvent(this->position, this->pOwningDialog, this->newEmotionId);
        }

    private:
        string newEmotionId;
    };

    class OtherEmotionChangeEvent : public DialogEvent
    {
    public:
        OtherEmotionChangeEvent(int position, Dialog *pOwningDialog, string newEmotionId)
            : DialogEvent(position, pOwningDialog)
        {
            this->newEmotionId = newEmotionId;
        }

        string GetNewEmotionId()
        {
            return newEmotionId;
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseOtherEmotionChanged(this->pOwningDialog, this->newEmotionId);
        }

        virtual DialogEvent * Clone()
        {
            return new OtherEmotionChangeEvent(this->position, this->pOwningDialog, this->newEmotionId);
        }

    private:
        string newEmotionId;
    };

    class PauseEvent : public DialogEvent
    {
    public:
        PauseEvent(int position, Dialog *pOwningDialog, double msDuration)
            : DialogEvent(position, pOwningDialog)
        {
            this->msDuration = msDuration;
        }

        double GetDuration() { return this->msDuration; }
        void SetDuration(double msDuration) { this->msDuration = msDuration; }

        virtual void RaiseEvent()
        {
            this->pOwningDialog->millisecondsUntilPauseCompletes += this->msDuration;
        }

        virtual DialogEvent * Clone()
        {
            return new PauseEvent(this->position, this->pOwningDialog, this->msDuration);
        }

    private:
        double msDuration;
    };

    class AudioPauseEvent : public DialogEvent
    {
    public:
        AudioPauseEvent(int position, Dialog *pOwningDialog, double msDuration)
            : DialogEvent(position, pOwningDialog)
        {
            this->msDuration = msDuration;
        }

        virtual void RaiseEvent()
        {
            this->pOwningDialog->millisecondsUntilPauseCompletes += this->msDuration;
            this->pOwningDialog->millisecondsUntilAudioPauseCompletes += this->msDuration;
        }

        virtual DialogEvent * Clone()
        {
            return new AudioPauseEvent(this->position, this->pOwningDialog, this->msDuration);
        }

    private:
        double msDuration;
    };

    class MouthChangeEvent : public DialogEvent
    {
    public:
        MouthChangeEvent(int position, Dialog *pOwningDialog, bool isMouthOpen)
            : DialogEvent(position, pOwningDialog)
        {
            this->isMouthOpen = isMouthOpen;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseSpeakerMouthStateChanged(this->pOwningDialog, this->isMouthOpen);
        }

        virtual DialogEvent * Clone()
        {
            return new MouthChangeEvent(this->position, this->pOwningDialog, this->isMouthOpen);
        }

    private:
        bool isMouthOpen;
    };

    class PlaySoundEvent : public DialogEvent
    {
    public:
        PlaySoundEvent(int position, Dialog *pOwningDialog, string soundId)
            : DialogEvent(position, pOwningDialog)
        {
            this->soundId = soundId;
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual bool PlaysSfx()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            playSound(this->soundId);
        }

        virtual DialogEvent * Clone()
        {
            return new PlaySoundEvent(this->position, this->pOwningDialog, this->soundId);
        }

    private:
        string soundId;
    };

    class ShakeEvent : public DialogEvent
    {
    public:
        ShakeEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseSpeakerStartedShaking(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new ShakeEvent(this->position, this->pOwningDialog);
        }
    };

    class ScreenShakeEvent : public DialogEvent
    {
    public:
        ScreenShakeEvent(int position, Dialog *pOwningDialog, double shakeIntensity)
            : DialogEvent(position, pOwningDialog)
        {
            this->shakeIntensity = shakeIntensity;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseScreenStartedShaking(this->pOwningDialog, this->shakeIntensity);
        }

        virtual DialogEvent * Clone()
        {
            return new ScreenShakeEvent(this->position, this->pOwningDialog, this->shakeIntensity);
        }

    private:
        double shakeIntensity;
    };

    class NextFrameEvent : public DialogEvent
    {
    public:
        NextFrameEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseNextFrame(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new NextFrameEvent(this->position, this->pOwningDialog);
        }
    };

    class PlayerDamagedEvent : public DialogEvent
    {
    public:
        PlayerDamagedEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaisePlayerDamaged(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new PlayerDamagedEvent(this->position, this->pOwningDialog);
        }
    };

    class OpponentDamagedEvent : public DialogEvent
    {
    public:
        OpponentDamagedEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseOpponentDamaged(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new OpponentDamagedEvent(this->position, this->pOwningDialog);
        }
    };

    class PlayBgmEvent : public DialogEvent
    {
    public:
        PlayBgmEvent(int position, Dialog *pOwningDialog, string bgmId, bool isPermanent = false)
            : DialogEvent(position, pOwningDialog)
        {
            this->bgmId = bgmId;
            this->isPermanent = isPermanent;
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent();

        virtual DialogEvent * Clone()
        {
            return new PlayBgmEvent(this->position, this->pOwningDialog, this->bgmId, this->isPermanent);
        }

    private:
        string bgmId;
        bool isPermanent;
    };

    class StopBgmEvent : public DialogEvent
    {
    public:
        StopBgmEvent(int position, Dialog *pOwningDialog, bool isInstant, bool isPermanent)
            : DialogEvent(position, pOwningDialog)
        {
            this->isInstant = isInstant;
            this->isPermanent = isPermanent;
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent();

        virtual DialogEvent * Clone()
        {
            return new StopBgmEvent(this->position, this->pOwningDialog, this->isInstant, this->isPermanent);
        }

    private:
        bool isInstant;
        bool isPermanent;
    };

    class ZoomEvent : public DialogEvent
    {
    public:
        ZoomEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseSpeakerZoomed(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new ZoomEvent(this->position, this->pOwningDialog);
        }
    };

    class EndZoomEvent : public DialogEvent
    {
    public:
        EndZoomEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        virtual void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseEndSpeakerZoomed(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new EndZoomEvent(this->position, this->pOwningDialog);
        }
    };

    class BeginBreakdownEvent : public DialogEvent
    {
    public:
        BeginBreakdownEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseBreakdownBegun(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new BeginBreakdownEvent(this->position, this->pOwningDialog);
        }
    };

    class EndBreakdownEvent : public DialogEvent
    {
    public:
        EndBreakdownEvent(int position, Dialog *pOwningDialog)
            : DialogEvent(position, pOwningDialog)
        {
        }

        virtual bool GetShouldBeRaisedWhenFinishing()
        {
            return true;
        }

        void RaiseEvent()
        {
            EventProviders::GetDialogEventProvider()->RaiseBreakdownEnded(this->pOwningDialog);
        }

        virtual DialogEvent * Clone()
        {
            return new EndBreakdownEvent(this->position, this->pOwningDialog);
        }
    };

    void CloneDialogEventList()
    {
        for (vector<DialogEvent *>::iterator iter = dialogEventList.begin(); iter != dialogEventList.end(); ++iter)
        {
            delete *iter;
        }

        dialogEventList.clear();

        for (vector<DialogEvent *>::iterator iter = dialogEventListOriginal.begin(); iter != dialogEventListOriginal.end(); ++iter)
        {
            dialogEventList.push_back((*iter)->Clone());
        }
    }

    static RectangleWH textAreaRect;
    static double desiredPadding;
    static MLIFont *pDialogFont;

    Arrow *pInterrogationUpArrow;
    Arrow *pInterrogationDownArrow;

    int curTextPosition;

    double millisecondsPerCharacterUpdate;
    double millisecondsSinceLastUpdate;
    double millisecondsUntilPauseCompletes;
    double millisecondsUntilAudioPauseCompletes;
    int lastPausePosition;

    vector<DialogEvent *> dialogEventListOriginal;
    vector<DialogEvent *> dialogEventList;
    vector<DialogEvent *>::iterator dialogEventIterator;
    DialogEvent *pCurrentDialogEvent;
    bool dialogEventIteratorSet;

    deque<TextColor> textColorStack;
    int lastTextColorChangeIndex;
    list<Interval> textIntervalList;

    double timeSinceLetterBlipPlayed;

    string filePath;
    int timeBeforeDialogInitial;
    int delayBeforeContinuing;
    bool isInterrogation;
    bool isPassive;
    bool isConfrontation;
    bool isStatic;

    Tab *pPressForInfoTab;
    Tab *pPresentEvidenceTab;
    Partner *pCurrentPartner;
    Tab *pUsePartnerTab;
    Tab *pEndInterrogationTab;

    State *pState;

    Arrow *pConversationDownArrow;

    bool canNavigateBack;
    bool canNavigateForward;

    EvidenceSelector *pEvidenceSelector;
    bool presentEvidenceAutomatically;
    bool evidencePresented;
    bool endRequested;
    bool evidenceSelectorShownOnce;

    string text;
    bool textSkipped;

    double timeBeforeDialog;
    double currentTime;
};

#endif
