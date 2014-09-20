/**
 * Static class exposing all of the event providers in the game.
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

#include "EventProviders.h"
#include "ButtonEventProvider.h"
#include "ButtonArrayEventProvider.h"
#include "CaseParsingEventProvider.h"
#include "DialogEventProvider.h"
#include "EvidenceSelectorEventProvider.h"
#include "ImageButtonEventProvider.h"
#include "LocationEventProvider.h"
#include "PromptOverlayEventProvider.h"
#include "SelectorEventProvider.h"
#include "TextButtonEventProvider.h"

ButtonEventProvider *EventProviders::pButtonEventProvider = NULL;
ButtonArrayEventProvider *EventProviders::pButtonArrayEventProvider = NULL;
CaseParsingEventProvider *EventProviders::pCaseParsingEventProvider = NULL;
DialogEventProvider *EventProviders::pDialogEventProvider = NULL;
EvidenceSelectorEventProvider *EventProviders::pEvidenceSelectorEventProvider = NULL;
ImageButtonEventProvider *EventProviders::pImageButtonEventProvider = NULL;
LocationEventProvider *EventProviders::pLocationEventProvider = NULL;
PromptOverlayEventProvider *EventProviders::pPromptOverlayEventProvider = NULL;
SelectorEventProvider *EventProviders::pSelectorEventProvider = NULL;
TextButtonEventProvider *EventProviders::pTextButtonEventProvider = NULL;

void EventProviders::Init()
{
    pButtonEventProvider = new ButtonEventProvider();
    pButtonArrayEventProvider = new ButtonArrayEventProvider();
    pCaseParsingEventProvider = new CaseParsingEventProvider();
    pDialogEventProvider = new DialogEventProvider();
    pEvidenceSelectorEventProvider = new EvidenceSelectorEventProvider();
    pImageButtonEventProvider = new ImageButtonEventProvider();
    pLocationEventProvider = new LocationEventProvider();
    pPromptOverlayEventProvider = new PromptOverlayEventProvider();
    pSelectorEventProvider = new SelectorEventProvider();
    pTextButtonEventProvider = new TextButtonEventProvider();
}

void EventProviders::Close()
{
    delete pButtonEventProvider;
    pButtonEventProvider = NULL;
    delete pButtonArrayEventProvider;
    pButtonArrayEventProvider = NULL;
    delete pCaseParsingEventProvider;
    pCaseParsingEventProvider = NULL;
    delete pDialogEventProvider;
    pDialogEventProvider = NULL;
    delete pEvidenceSelectorEventProvider;
    pEvidenceSelectorEventProvider = NULL;
    delete pImageButtonEventProvider;
    pImageButtonEventProvider = NULL;
    delete pLocationEventProvider;
    pLocationEventProvider = NULL;
    delete pPromptOverlayEventProvider;
    pPromptOverlayEventProvider = NULL;
    delete pSelectorEventProvider;
    pSelectorEventProvider = NULL;
    delete pTextButtonEventProvider;
    pTextButtonEventProvider = NULL;
}
