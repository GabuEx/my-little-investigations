/**
 * Manager for all of the content in the game; provides their retrieval.
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

#include "ContentManager.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"

ContentManager::~ContentManager()
{
    for (map<string, Area *>::iterator iter = areaByIdMap.begin(); iter != areaByIdMap.end(); ++iter)
    {
        delete iter->second;
    }

    for (map<string, Location *>::iterator iter = locationByIdMap.begin(); iter != locationByIdMap.end(); ++iter)
    {
        delete iter->second;
    }

    for (map<string, Encounter *>::iterator iter = encounterByIdMap.begin(); iter != encounterByIdMap.end(); ++iter)
    {
        delete iter->second;
    }

    for (map<string, Conversation *>::iterator iter = conversationByIdMap.begin(); iter != conversationByIdMap.end(); ++iter)
    {
        delete iter->second;
    }

    Conversation::showDialogActionListsById.clear();
}

Area * ContentManager::GetAreaFromId(string areaId)
{
    return areaByIdMap[areaId];
}

Location * ContentManager::GetLocationFromId(string locationId)
{
    return locationByIdMap[locationId];
}

Encounter * ContentManager::GetEncounterFromId(string encounterId)
{
    return encounterByIdMap[encounterId];
}

Conversation * ContentManager::GetConversationFromId(string conversationId)
{
    return conversationByIdMap[conversationId];
}

void ContentManager::Reset()
{
    for (map<string, Conversation *>::iterator iter = conversationByIdMap.begin(); iter != conversationByIdMap.end(); ++iter)
    {
        iter->second->SetIsEnabled(conversationToOriginalEnabledStateMap[iter->second]);
        iter->second->GetUnlockConditions()->clear();
        iter->second->SetHasBeenCompleted(conversationToOriginalHasBeenCompletedStateMap[iter->second]);
        iter->second->SetWrongPartnerUsed(false);
        iter->second->ResetTopics();
    }

    for (map<string, Location *>::iterator iter = locationByIdMap.begin(); iter != locationByIdMap.end(); ++iter)
    {
        iter->second->Reset();
    }

    for (map<string, Area *>::iterator iter = areaByIdMap.begin(); iter != areaByIdMap.end(); ++iter)
    {
        iter->second->Reset();
    }
}

void ContentManager::SaveToSaveFile(XmlWriter *pWriter)
{
    pWriter->StartElement("ContentManager");
    pWriter->StartElement("Conversations");

    for (map<string, Conversation *>::iterator iter = conversationByIdMap.begin(); iter != conversationByIdMap.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            pWriter->StartElement("Conversation");

            pWriter->WriteTextElement("Id", iter->first);

            pWriter->WriteBooleanElement("IsEnabled", iter->second->GetIsEnabled());

            pWriter->StartElement("UnlockConditions");
            vector<Conversation::UnlockCondition *> *pUnlockConditions = iter->second->GetUnlockConditions();

            for (unsigned int i = 0; i < pUnlockConditions->size(); i++)
            {
                pWriter->StartElement("Entry");
                (*pUnlockConditions)[i]->SaveToXml(pWriter);
                pWriter->EndElement();
            }

            pWriter->EndElement();

            pWriter->WriteBooleanElement("HasBeenCompleted", iter->second->GetHasBeenCompleted());
            pWriter->WriteBooleanElement("WrongPartnerUsed", iter->second->GetWrongPartnerUsed());

            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->StartElement("Locations");

    for (map<string, Location *>::iterator iter = locationByIdMap.begin(); iter != locationByIdMap.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            pWriter->StartElement("Location");

            pWriter->WriteTextElement("Id", iter->first);

            pWriter->StartElement("HiddenForegroundElementList");

            vector<HiddenForegroundElement *> *pHiddenForegroundElementList = iter->second->GetHiddenForegroundElementList();

            for (unsigned int i = 0; i < pHiddenForegroundElementList->size(); i++)
            {
                pWriter->StartElement("Entry");

                HiddenForegroundElement *pHiddenForegroundElement = (*pHiddenForegroundElementList)[i];

                pWriter->WriteTextElement("Name", pHiddenForegroundElement->GetName());
                pWriter->WriteBooleanElement("IsDiscovered", pHiddenForegroundElement->GetIsDiscovered());

                pWriter->EndElement();
            }

            pWriter->EndElement();
            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->EndElement();
}

void ContentManager::LoadFromSaveFile(XmlReader *pReader)
{
    pReader->StartElement("ContentManager");
    pReader->StartElement("Conversations");

    pReader->StartList("Conversation");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");

        if (conversationByIdMap.count(id) > 0)
        {
            conversationByIdMap[id]->SetIsEnabled(pReader->ReadBooleanElement("IsEnabled"));

            conversationByIdMap[id]->GetUnlockConditions()->clear();

            pReader->StartElement("UnlockConditions");
            pReader->StartList("Entry");

            while (pReader->MoveToNextListItem())
            {
                conversationByIdMap[id]->GetUnlockConditions()->push_back(Conversation::UnlockCondition::LoadFromXml(pReader));
            }

            pReader->EndElement();

            conversationByIdMap[id]->SetHasBeenCompleted(pReader->ReadBooleanElement("HasBeenCompleted"));
            conversationByIdMap[id]->SetWrongPartnerUsed(pReader->ReadBooleanElement("WrongPartnerUsed"));
            conversationByIdMap[id]->ResetTopics();
        }
    }

    pReader->EndElement();

    for (map<string, Location *>::iterator iter = locationByIdMap.begin(); iter != locationByIdMap.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            iter->second->Reset();
        }
    }

    for (map<string, Area *>::iterator iter = areaByIdMap.begin(); iter != areaByIdMap.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            iter->second->Reset();
        }
    }

    pReader->StartElement("Locations");

    pReader->StartList("Location");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        Location *pLocation = locationByIdMap[id];

        pReader->StartElement("HiddenForegroundElementList");

        pReader->StartList("Entry");
        vector<HiddenForegroundElement *> *pHiddenForegroundElementList = pLocation->GetHiddenForegroundElementList();

        while (pReader->MoveToNextListItem())
        {
            string name = pReader->ReadTextElement("Name");
            HiddenForegroundElement *pHiddenForegroundElement = NULL;

            for (unsigned int i = 0; i < pHiddenForegroundElementList->size(); i++)
            {
                HiddenForegroundElement *pCurHiddenForegroundElement = (*pHiddenForegroundElementList)[i];

                if (pCurHiddenForegroundElement->GetName() == name)
                {
                    pHiddenForegroundElement = pCurHiddenForegroundElement;
                }
            }

            if (pHiddenForegroundElement != NULL)
            {
                pHiddenForegroundElement->SetIsDiscovered(pReader->ReadBooleanElement("IsDiscovered"));
            }
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->EndElement();
}

void ContentManager::LoadFromXml(XmlReader *pReader)
{
    Conversation::showDialogActionListsById.clear();

    pReader->StartElement("Content");

    pReader->StartElement("ConversationByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        Conversation *pConversation = Conversation::LoadFromXml(pReader);
        conversationByIdMap[id] = pConversation;
        conversationToOriginalEnabledStateMap[pConversation] = pConversation->GetIsEnabled();
        conversationToOriginalHasBeenCompletedStateMap[pConversation] = pConversation->GetHasBeenCompleted();
    }

    pReader->EndElement();

    pReader->StartElement("EncounterByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        encounterByIdMap[id] = new Encounter(pReader);
    }

    pReader->EndElement();

    pReader->StartElement("LocationByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        locationByIdMap[id] = new Location(pReader);
    }

    pReader->EndElement();

    pReader->StartElement("AreaByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        areaByIdMap[id] = new Area(pReader);
    }

    pReader->EndElement();

    initialAreaId = pReader->ReadTextElement("InitialAreaId");
    initialLocationId = pReader->ReadTextElement("InitialLocationId");

    pReader->EndElement();
}
