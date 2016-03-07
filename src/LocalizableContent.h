/**
 * Basic header/include file for LocalizableContent.cpp.
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

#ifndef LOCALIZABLECONTENT_H
#define LOCALIZABLECONTENT_H

#include "XmlReader.h"

#include <SDL2/SDL.h>

#include <map>
#include <list>
#include <string>
#include <algorithm>

using namespace std;

class ILocalizableFont
{
public:
    virtual void ReloadFontInfo() = 0;
};

class ILocalizableTextOwner
{
public:
    virtual void ReloadLocalizableText() = 0;
};

class LocalizableContent
{
public:
    struct FontInfo
    {
        FontInfo()
            : Filename("")
            , PointSize(0)
        {
        }

        FontInfo(string filename, int pointSize)
            : Filename(filename)
            , PointSize(pointSize)
        {
        }

        string Filename;
        int PointSize;
    };

    struct Setting
    {
        Setting()
            : ValueType(Type::None)
        {
        }

        Setting(string type, string value);

        enum class Type
        {
            None,
            Boolean,
        };

        union
        {
            bool BooleanValue;
        };

        Type ValueType;
    };

    LocalizableContent();
    LocalizableContent(XmlReader *pReader);
    ~LocalizableContent();

    LocalizableContent::FontInfo GetFontInfo(const string &fontId);
    string GetText(const string &textId);
    bool GetBooleanSetting(const string &settingId);

    void LoadNewLanguage(XmlReader *pReader);

    void AddLocalizableFont(ILocalizableFont *pFont)
    {
        if (find(localizableFonts.begin(), localizableFonts.end(), pFont) == localizableFonts.end())
        {
            localizableFonts.push_back(pFont);
        }
    }

    void RemoveLocalizableFont(ILocalizableFont *pFont)
    {
        localizableFonts.remove(pFont);
    }

    void AddLocalizableTextOwner(ILocalizableTextOwner *pTextOwner)
    {
        if (find(localizableTextOwners.begin(), localizableTextOwners.end(), pTextOwner) == localizableTextOwners.end())
        {
            localizableTextOwners.push_back(pTextOwner);
        }
    }

    void RemoveLocalizableTextOwner(ILocalizableTextOwner *pTextOwner)
    {
        localizableTextOwners.remove(pTextOwner);
    }

private:
    SDL_sem *pAccessSemaphore;

    map<string, LocalizableContent::FontInfo> fontIdToFontInfoMap;
    map<string, string> textIdToTextMap;
    map<string, LocalizableContent::Setting> settingIdToSettingMap;

    list<ILocalizableFont *> localizableFonts;
    list<ILocalizableTextOwner *> localizableTextOwners;
};

#endif
