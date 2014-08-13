/**
 * Manager for all of the field characters in the game; provides their retrieval.
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

#include "FieldCharacterManager.h"
#include "../XmlReader.h"

FieldCharacterManager::FieldCharacterManager()
{
    pPlayerCharacter = NULL;
}

FieldCharacterManager::~FieldCharacterManager()
{
    // The field characters are stored on the heap since we need
    // their hitbox to be a pointer rather than a straight object,
    // so we need to free that memory when the field character manager
    // is going away.
    for (map<string, FieldCharacter *>::iterator iter = characterByIdMap.begin(); iter != characterByIdMap.end(); ++iter)
    {
        delete iter->second;
    }

    delete pPlayerCharacter;
    pPlayerCharacter = NULL;
}

FieldCharacter * FieldCharacterManager::GetCharacterFromId(const string &characterId)
{
    return characterByIdMap[characterId]->Clone();
}

FieldCharacter * FieldCharacterManager::GetPlayerCharacter()
{
    if (pPlayerCharacter == NULL)
    {
        pPlayerCharacter = GetCharacterFromId(playerCharacterId);
    }

    return pPlayerCharacter;
}

void FieldCharacterManager::LoadFromXml(XmlReader *pReader)
{
    pPlayerCharacter = NULL;

    pReader->StartElement("FieldCharacterManager");

    pReader->StartElement("CharacterByIdDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        characterByIdMap[id] = new FieldCharacter(pReader);
    }

    pReader->EndElement();

    playerCharacterId = pReader->ReadTextElement("PlayerCharacterId");
    pReader->EndElement();
}
