/**
 * An event provider for dialogs.
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

#ifndef DIALOGEVENTPROVIDER_H
#define DIALOGEVENTPROVIDER_H

#include "EventProviders.h"
#include "../enums.h"
#include <list>

using namespace std;

class Dialog;

class DialogEventListener
{
public:
    virtual void OnDialogSpeakerEmotionChanged(Dialog *pSender, const string &newEmotionId) = 0;
    virtual void OnDialogOtherEmotionChanged(Dialog *pSender, const string &newEmotionId) = 0;
    virtual void OnDialogNextFrame(Dialog *pSender) = 0;
    virtual void OnDialogPlayerDamaged(Dialog *pSender) = 0;
    virtual void OnDialogOpponentDamaged(Dialog *pSender) = 0;
    virtual void OnDialogSpeakerStartedShaking(Dialog *pSender) = 0;
    virtual void OnDialogScreenStartedShaking(Dialog *pSender, double shakeIntensity) = 0;
    virtual void OnDialogSpeakerMouthStateChanged(Dialog *pSender, bool isMouthOn) = 0;
    virtual void OnDialogSpeakerZoomed(Dialog *pSender) = 0;
    virtual void OnDialogEndSpeakerZoomed(Dialog *pSender) = 0;
    virtual void OnDialogBreakdownBegun(Dialog *pSender) = 0;
    virtual void OnDialogBreakdownEnded(Dialog *pSender) = 0;
    virtual void OnDialogDirectlyNavigated(Dialog *pSender, DirectNavigationDirection direction) = 0;
    virtual void OnDialogPressForInfoClicked(Dialog *pSender) = 0;
    virtual void OnDialogUsePartner(Dialog *pSender) = 0;
    virtual void OnDialogEvidencePresented(Dialog *pSender, const string &evidenceId) = 0;
    virtual void OnDialogEndRequested(Dialog *pSender) = 0;
};

class DialogEventProvider
{
public:
    void RegisterListener(DialogEventListener *pListener)
    {
        listenerList.push_back(pListener);
        listenerList.unique();
    }

    void ClearListener(DialogEventListener *pListener)
    {
        listenerList.remove(pListener);
    }

    void RaiseSpeakerEmotionChanged(Dialog *pSender, const string &newEmotionId)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogSpeakerEmotionChanged(pSender, newEmotionId);
        }
    }

    void RaiseOtherEmotionChanged(Dialog *pSender, const string &newEmotionId)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogOtherEmotionChanged(pSender, newEmotionId);
        }
    }

    void RaiseNextFrame(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogNextFrame(pSender);
        }
    }

    void RaisePlayerDamaged(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogPlayerDamaged(pSender);
        }
    }

    void RaiseOpponentDamaged(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogOpponentDamaged(pSender);
        }
    }

    void RaiseSpeakerStartedShaking(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogSpeakerStartedShaking(pSender);
        }
    }

    void RaiseScreenStartedShaking(Dialog *pSender, double shakeIntensity)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogScreenStartedShaking(pSender, shakeIntensity);
        }
    }

    void RaiseSpeakerMouthStateChanged(Dialog *pSender, bool isMouthOn)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogSpeakerMouthStateChanged(pSender, isMouthOn);
        }
    }

    void RaiseSpeakerZoomed(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogSpeakerZoomed(pSender);
        }
    }

    void RaiseEndSpeakerZoomed(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogEndSpeakerZoomed(pSender);
        }
    }

    void RaiseBreakdownBegun(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogBreakdownBegun(pSender);
        }
    }

    void RaiseBreakdownEnded(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogBreakdownEnded(pSender);
        }
    }

    void RaiseDirectlyNavigated(Dialog *pSender, DirectNavigationDirection direction)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogDirectlyNavigated(pSender, direction);
        }
    }

    void RaisePressForInfoClicked(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogPressForInfoClicked(pSender);
        }
    }

    void RaiseUsePartner(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogUsePartner(pSender);
        }
    }

    void RaiseEvidencePresented(Dialog *pSender, const string &evidenceId)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogEvidencePresented(pSender, evidenceId);
        }
    }

    void RaiseEndRequested(Dialog *pSender)
    {
        for (list<DialogEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnDialogEndRequested(pSender);
        }
    }

protected:
    list<DialogEventListener *> listenerList;
};

#endif
