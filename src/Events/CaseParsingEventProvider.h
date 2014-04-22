/**
 * An event provider for case parsing.
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

#ifndef CASEPARSINGEVENTPROVIDER_H
#define CASEPARSINGEVENTPROVIDER_H

#include "EventProviders.h"
#include <string>
#include <list>

using namespace std;

class CaseParsingEventListener
{
public:
    virtual void OnCaseParsingComplete(const string &caseFileName) = 0;
};

class CaseParsingEventProvider
{
public:
    void RegisterListener(CaseParsingEventListener *pListener)
    {
        listenerList.push_back(pListener);
        listenerList.unique();
    }

    void ClearListener(CaseParsingEventListener *pListener)
    {
        listenerList.remove(pListener);
    }

    void RaiseCaseParsingComplete(const string &caseFileName)
    {
        for (list<CaseParsingEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnCaseParsingComplete(caseFileName);
        }
    }

protected:
    list<CaseParsingEventListener *> listenerList;
};

#endif
