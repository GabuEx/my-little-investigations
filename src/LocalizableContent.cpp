/**
 * Holds all of the game-wide text and fonts to aid localization.
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

#include "LocalizableContent.h"

LocalizableContent::LocalizableContent()
{
    pAccessSemaphore = SDL_CreateSemaphore(1);
}

LocalizableContent::LocalizableContent(XmlReader *pReader)
{
    pAccessSemaphore = SDL_CreateSemaphore(1);

    pReader->StartElement("LocalizableContent");

    pReader->StartElement("Fonts");
    pReader->StartList("Font");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextAttribute("Id");
        string filename = pReader->ReadTextAttribute("Filename");
        int size = pReader->ReadIntAttribute("Size");

        if (fontIdToFontMap.count(id) > 0)
        {
            throw MLIException(string("Duplicate font ID found: ") + id);
        }

        fontIdToFontMap[id] = LocalizableContent::Font(filename, size);
    }

    pReader->EndElement();

    pReader->StartElement("TextContent");
    pReader->StartList("Text");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextAttribute("Id");
        string text = pReader->ReadText();

        if (textIdToTextMap.count(id) > 0)
        {
            throw MLIException(string("Duplicate text ID found: ") + id);
        }

        textIdToTextMap[id] = text;
    }

    pReader->EndElement();

    pReader->EndElement();
}

LocalizableContent::~LocalizableContent()
{
    SDL_DestroySemaphore(pAccessSemaphore);
    pAccessSemaphore = NULL;
}

LocalizableContent::Font LocalizableContent::GetFont(const string &fontId)
{
    LocalizableContent::Font returnValue;

    SDL_SemWait(pAccessSemaphore);
    if (fontIdToFontMap.count(fontId) == 0)
    {
        throw MLIException(string("Font ID not found: ") + fontId);
    }

    returnValue = fontIdToFontMap[fontId];
    SDL_SemPost(pAccessSemaphore);

    return returnValue;
}

string LocalizableContent::GetText(const string &textId)
{
    string returnValue;

    SDL_SemWait(pAccessSemaphore);
    if (textIdToTextMap.count(textId) == 0)
    {
        throw MLIException(string("Text ID not found: ") + textId);
    }

    returnValue = textIdToTextMap[textId];
    SDL_SemPost(pAccessSemaphore);

    return returnValue;
}
