/**
 * An event provider for evidence selectors.
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

#ifndef EVIDENCESELECTOREVENTPROVIDER_H
#define EVIDENCESELECTOREVENTPROVIDER_H

#include "EventProviders.h"
#include "../UserInterface/EvidenceSelector.h"
#include <list>

using namespace std;

class EvidenceSelectorEventListener
{
public:
    virtual void OnEvidenceSelectorEvidencePresented(EvidenceSelector *pSender, const string &evidenceId) = 0;
    virtual void OnEvidenceSelectorClosing(EvidenceSelector *pSender) = 0;
};

class EvidenceSelectorEventProvider
{
public:
    void RegisterListener(EvidenceSelectorEventListener *pListener)
    {
        listenerList.push_back(pListener);
        listenerList.unique();
    }

    void ClearListener(EvidenceSelectorEventListener *pListener)
    {
        listenerList.remove(pListener);
    }

    void RaiseEvidenceSelectorEvidencePresented(EvidenceSelector *pSender, const string &evidenceId)
    {
        for (list<EvidenceSelectorEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnEvidenceSelectorEvidencePresented(pSender, evidenceId);
        }
    }

    void RaiseEvidenceSelectorClosing(EvidenceSelector *pSender)
    {
        for (list<EvidenceSelectorEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnEvidenceSelectorClosing(pSender);
        }
    }

protected:
    list<EvidenceSelectorEventListener *> listenerList;
};

#endif
