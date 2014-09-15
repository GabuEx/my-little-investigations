/**
 * An event provider for text buttons.
 *
 * @author mad-mix
 * @since 1.0.7
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

#ifndef TEXTBUTTONEVENTPROVIDER_H
#define TEXTBUTTONEVENTPROVIDER_H

#include "EventProviders.h"
#include "../UserInterface/TextButton.h"
#include <list>

class TextButtonEventListener
{
public:
    virtual void OnButtonClicked(TextButton *pSender) = 0;
};

class TextButtonEventProvider
{
public:
    void RegisterListener(TextButtonEventListener *pListener)
    {
        listenerList.push_back(pListener);
        listenerList.unique();
    }

    void ClearListener(TextButtonEventListener *pListener)
    {
        listenerList.remove(pListener);
    }

    void RaiseClicked(TextButton *pSender)
    {
        for (list<TextButtonEventListener *>::iterator iter = listenerList.begin(); iter != listenerList.end(); ++iter)
        {
            (*iter)->OnButtonClicked(pSender);
        }
    }

protected:
    list<TextButtonEventListener *> listenerList;
};

#endif // TEXTBUTTONEVENTPROVIDER_H
