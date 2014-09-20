/**
 * Basic header/include file for EventProviders.cpp.
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

#ifndef EVENTPROVIDERS_H
#define EVENTPROVIDERS_H

class ButtonEventProvider;
class ButtonArrayEventProvider;
class CaseParsingEventProvider;
class DialogEventProvider;
class EvidenceSelectorEventProvider;
class ImageButtonEventProvider;
class LocationEventProvider;
class PromptOverlayEventProvider;
class SelectorEventProvider;
class TextButtonEventProvider;

class EventProviders
{
public:
    static void Init();
    static void Close();

    static ButtonEventProvider * GetButtonEventProvider() { return pButtonEventProvider; }
    static ButtonArrayEventProvider * GetButtonArrayEventProvider() { return pButtonArrayEventProvider; }
    static CaseParsingEventProvider * GetCaseParsingEventProvider() { return pCaseParsingEventProvider; }
    static DialogEventProvider * GetDialogEventProvider() { return pDialogEventProvider; }
    static EvidenceSelectorEventProvider * GetEvidenceSelectorEventProvider() { return pEvidenceSelectorEventProvider; }
    static ImageButtonEventProvider * GetImageButtonEventProvider() { return pImageButtonEventProvider; }
    static LocationEventProvider * GetLocationEventProvider() { return pLocationEventProvider; }
    static PromptOverlayEventProvider * GetPromptOverlayEventProvider() { return pPromptOverlayEventProvider; }
    static SelectorEventProvider * GetSelectorEventProvider() { return pSelectorEventProvider; }
    static TextButtonEventProvider * GetTextButtonEventProvider() { return pTextButtonEventProvider; }

private:
    static ButtonEventProvider *pButtonEventProvider;
    static ButtonArrayEventProvider *pButtonArrayEventProvider;
    static CaseParsingEventProvider *pCaseParsingEventProvider;
    static DialogEventProvider *pDialogEventProvider;
    static EvidenceSelectorEventProvider *pEvidenceSelectorEventProvider;
    static ImageButtonEventProvider *pImageButtonEventProvider;
    static LocationEventProvider *pLocationEventProvider;
    static PromptOverlayEventProvider *pPromptOverlayEventProvider;
    static SelectorEventProvider *pSelectorEventProvider;
    static TextButtonEventProvider *pTextButtonEventProvider;
};

#endif
