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

LocalizableContent::Setting::Setting(string type, string value)
{
    if (type == "Boolean")
    {
        ValueType = Type::Boolean;
        BooleanValue = (value == "True");
    }
    else
    {
        ThrowException(string("Unknown setting type: ") + type);
    }
}

LocalizableContent::LocalizableContent()
{
    pAccessSemaphore = SDL_CreateSemaphore(1);
}

LocalizableContent::LocalizableContent(XmlReader *pReader)
{
    pAccessSemaphore = SDL_CreateSemaphore(1);
    LoadNewLanguage(pReader);
}

LocalizableContent::~LocalizableContent()
{
    SDL_DestroySemaphore(pAccessSemaphore);
    pAccessSemaphore = NULL;
}

LocalizableContent::FontInfo LocalizableContent::GetFontInfo(const string &fontId)
{
    LocalizableContent::FontInfo returnValue;

    SDL_SemWait(pAccessSemaphore);
    if (fontIdToFontInfoMap.count(fontId) == 0)
    {
        ThrowException(string("Font info ID not found: ") + fontId);
    }

    returnValue = fontIdToFontInfoMap[fontId];
    SDL_SemPost(pAccessSemaphore);

    return returnValue;
}

string LocalizableContent::GetText(const string &textId)
{
    string returnValue;

    SDL_SemWait(pAccessSemaphore);
    if (textIdToTextMap.count(textId) == 0)
    {
        ThrowException(string("Text ID not found: ") + textId);
    }

    returnValue = textIdToTextMap[textId];
    SDL_SemPost(pAccessSemaphore);

    return returnValue;
}

bool LocalizableContent::GetBooleanSetting(const string &settingId)
{
    bool returnValue;

    SDL_SemWait(pAccessSemaphore);
    if (settingIdToSettingMap.count(settingId) == 0)
    {
        ThrowException(string("Setting ID not found: ") + settingId);
    }

    if (settingIdToSettingMap[settingId].ValueType != LocalizableContent::Setting::Type::Boolean)
    {
        ThrowException(string("Setting ID ") + settingId + string(" is not a Boolean value."));
    }

    returnValue = settingIdToSettingMap[settingId].BooleanValue;
    SDL_SemPost(pAccessSemaphore);

    return returnValue;
}

void LocalizableContent::LoadNewLanguage(XmlReader *pReader)
{
    fontIdToFontInfoMap.clear();
    textIdToTextMap.clear();
    settingIdToSettingMap.clear();

    pReader->StartElement("LocalizableContent");

    pReader->StartElement("Fonts");
    pReader->StartList("Font");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextAttribute("Id");
        string filename = pReader->ReadTextAttribute("Filename");
        int size = pReader->ReadIntAttribute("Size");

        if (fontIdToFontInfoMap.count(id) > 0)
        {
            ThrowException(string("Duplicate font ID found: ") + id);
        }

        fontIdToFontInfoMap[id] = LocalizableContent::FontInfo(filename, size);
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
            ThrowException(string("Duplicate text ID found: ") + id);
        }

        textIdToTextMap[id] = text;
    }

    pReader->EndElement();

    pReader->StartElement("Settings");
    pReader->StartList("Setting");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextAttribute("Id");
        string type = pReader->ReadTextAttribute("Type");
        string value = pReader->ReadTextAttribute("Value");

        if (settingIdToSettingMap.count(id) > 0)
        {
            ThrowException(string("Duplicate setting ID found: ") + id);
        }

        settingIdToSettingMap[id] = LocalizableContent::Setting(type, value);
    }

    pReader->EndElement();

    pReader->EndElement();

    for (ILocalizableFont *pLocalizableFont : localizableFonts)
    {
        pLocalizableFont->ReloadFontInfo();
    }

    for (ILocalizableTextOwner *pLocalizableTextOwner : localizableTextOwners)
    {
        pLocalizableTextOwner->ReloadLocalizableText();
    }
}
