/**
 * Manager for all of the field cutscenes in the game; provides their retrieval.
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

#include "FieldCutsceneManager.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"

FieldCutsceneManager::~FieldCutsceneManager()
{
    for (map<string, FieldCutscene *>::iterator iter = idToCutsceneMap.begin(); iter != idToCutsceneMap.end(); ++iter)
    {
        delete iter->second;
    }
}

FieldCutscene * FieldCutsceneManager::GetCutsceneFromId(const string &id)
{
    return idToCutsceneMap[id];
}

void FieldCutsceneManager::Reset()
{
    for (map<string, FieldCutscene *>::iterator iter = idToCutsceneMap.begin(); iter != idToCutsceneMap.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            iter->second->SetIsEnabled(cutsceneToOriginalEnabledStateMap[iter->second]);
            iter->second->SetIsFinished(false);
            iter->second->SetHasBegun(false);
            iter->second->SetHasCompleted(false);
            iter->second->Reset();
        }
    }
}

void FieldCutsceneManager::SaveToSaveFile(XmlWriter *pWriter)
{
    pWriter->StartElement("FieldCutsceneManager");
    pWriter->StartElement("FieldCutsceneList");

    for (map<string, FieldCutscene *>::iterator iter = idToCutsceneMap.begin(); iter != idToCutsceneMap.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            pWriter->StartElement("FieldCutscene");
            pWriter->WriteTextElement("Id", iter->first);
            pWriter->WriteBooleanElement("IsEnabled", iter->second->GetIsEnabled());
            pWriter->WriteBooleanElement("IsFinished", iter->second->GetIsFinished());
            pWriter->WriteBooleanElement("HasCompleted", iter->second->GetHasCompleted());
            pWriter->EndElement();
        }
    }

    pWriter->EndElement();
    pWriter->EndElement();
}

void FieldCutsceneManager::LoadFromSaveFile(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneManager");
    pReader->StartElement("FieldCutsceneList");

    pReader->StartList("FieldCutscene");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");

        if (idToCutsceneMap.count(id) > 0)
        {
            idToCutsceneMap[id]->SetIsEnabled(pReader->ReadBooleanElement("IsEnabled"));
            idToCutsceneMap[id]->SetIsFinished(pReader->ReadBooleanElement("IsFinished"));
            idToCutsceneMap[id]->SetHasCompleted(pReader->ReadBooleanElement("HasCompleted"));
            idToCutsceneMap[id]->Reset();
        }
    }

    pReader->EndElement();
    pReader->EndElement();
}

void FieldCutsceneManager::LoadFromXml(XmlReader *pReader)
{
    pReader->StartElement("FieldCutsceneManager");

    pReader->StartElement("IdToCutsceneHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        idToCutsceneMap[id] = new FieldCutscene(pReader);
        cutsceneToOriginalEnabledStateMap[idToCutsceneMap[id]] = idToCutsceneMap[id]->GetIsEnabled();
    }

    pReader->EndElement();

    pReader->EndElement();
}
