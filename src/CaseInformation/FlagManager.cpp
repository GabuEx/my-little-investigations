/**
 * Manager for all of the flags in the game; provides their retrieval and setting.
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

#include "FlagManager.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"

bool FlagManager::IsFlagSet(string flagName)
{
    if (namedFlagMap.count(flagName) == 0)
    {
        namedFlagMap[flagName] = false;
    }

    return namedFlagMap[flagName];
}

void FlagManager::SetFlag(string flagName)
{
    namedFlagMap[flagName] = true;
}

void FlagManager::ClearFlag(string flagName)
{
    namedFlagMap[flagName] = false;
}

void FlagManager::Reset()
{
    namedFlagMap.clear();
}

void FlagManager::CacheState()
{
    cachedFlagStateMap.clear();

    for (map<string, bool>::iterator iter = namedFlagMap.begin(); iter != namedFlagMap.end(); ++iter)
    {
        cachedFlagStateMap[iter->first] = iter->second;
    }
}

void FlagManager::LoadCachedState()
{
    for (map<string, bool>::iterator iter = cachedFlagStateMap.begin(); iter != cachedFlagStateMap.end(); ++iter)
    {
        namedFlagMap[iter->first] = iter->second;
    }
}

void FlagManager::SaveToSaveFile(XmlWriter *pWriter)
{
    pWriter->StartElement("FlagManager");
    pWriter->StartElement("FlagList");

    for (map<string, bool>::iterator iter = namedFlagMap.begin(); iter != namedFlagMap.end(); ++iter)
    {
        pWriter->StartElement("Flag");
        pWriter->WriteTextElement("Id", iter->first);
        pWriter->WriteBooleanElement("IsSet", iter->second);
        pWriter->EndElement();
    }

    pWriter->EndElement();
    pWriter->EndElement();
}

void FlagManager::LoadFromSaveFile(XmlReader *pReader)
{
    Reset();

    pReader->StartElement("FlagManager");
    pReader->StartElement("FlagList");

    pReader->StartList("Flag");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        namedFlagMap[id] = pReader->ReadBooleanElement("IsSet");
    }

    pReader->EndElement();
    pReader->EndElement();
}

void FlagManager::LoadFromXml(XmlReader *pReader)
{
    pReader->StartElement("FlagManager");
    pReader->EndElement();
}
