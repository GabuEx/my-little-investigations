/**
 * Basic header/include file for FieldCharacterManager.cpp.
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

#ifndef FIELDCHARACTERMANAGER_H
#define FIELDCHARACTERMANAGER_H

#include "../CaseContent/FieldCharacter.h"
#include <map>

using namespace std;

class XmlReader;

class FieldCharacterManager
{
    friend class Case;

public:
    FieldCharacterManager();
    ~FieldCharacterManager();

    // Everyone calling this needs their own version of the character,
    // since they'll be mucking with its values,
    // so we create a copy for the caller.
    // As such, the caller needs to delete it.
    FieldCharacter * GetCharacterFromId(string characterId);

    // That is not the case for GetPlayerCharacter.
    FieldCharacter * GetPlayerCharacter();

    void LoadFromXml(XmlReader *pReader);

private:
    map<string, FieldCharacter *> characterByIdMap;
    string playerCharacterId;
    FieldCharacter *pPlayerCharacter;
};

#endif
