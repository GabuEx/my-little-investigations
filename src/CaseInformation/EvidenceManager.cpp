/**
 * Manager for all of the evidence in the game; provides their retrieval.
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

#include "EvidenceManager.h"
#include "Case.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"

Evidence::Evidence(XmlReader *pReader)
{
    pReader->StartElement("Evidence");
    smallSpriteId = pReader->ReadTextElement("SmallSpriteId");
    largeSpriteId = pReader->ReadTextElement("LargeSpriteId");
    name = pReader->ReadTextElement("Name");
    description = pReader->ReadTextElement("Description");
    isProfile = pReader->ReadBooleanElement("IsProfile");
    isEnabled = pReader->ReadBooleanElement("IsEnabled");
    isHidden = pReader->ReadBooleanElement("IsHidden");
    pReader->EndElement();

    pSmallSprite = NULL;
    pLargeSprite = NULL;
}

Sprite * Evidence::GetSmallSprite()
{
    if (pSmallSprite == NULL)
    {
        pSmallSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(smallSpriteId);
    }

    return pSmallSprite;
}

Sprite * Evidence::GetLargeSprite()
{
    if (pLargeSprite == NULL)
    {
        pLargeSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(largeSpriteId);
    }

    return pLargeSprite;
}

bool Evidence::operator< (const Evidence &other) const
{
    return name < other.name;
}

EvidenceIdPair::EvidenceIdPair(XmlReader *pReader)
{
    pReader->StartElement("EvidenceIdPair");
    evidenceId1 = pReader->ReadTextElement("EvidenceId1");
    evidenceId2 = pReader->ReadTextElement("EvidenceId2");
    pReader->EndElement();
}

bool EvidenceIdPair::operator==(const EvidenceIdPair &other) const
{
    return
        (evidenceId1 == other.evidenceId1 && evidenceId2 == other.evidenceId2) ||
        (evidenceId1 == other.evidenceId2 && evidenceId2 == other.evidenceId1);
}

bool EvidenceIdPair::operator!=(const EvidenceIdPair &other) const
{
    return !(*this == other);
}

bool EvidenceIdPair::operator< (const EvidenceIdPair &other) const
{
    return (evidenceId1 + evidenceId2) < (other.evidenceId1 + other.evidenceId2);
}

EvidenceManager::~EvidenceManager()
{
    for (map<string, Evidence *>::iterator iter = idToItemMap.begin(); iter != idToItemMap.end(); ++iter)
    {
        delete iter->second;
    }

    for (map<EvidenceIdPair, Conversation *>::iterator iter = idPairToCombinationConversationMap.begin(); iter != idPairToCombinationConversationMap.end(); ++iter)
    {
        delete iter->second;
    }

    delete pWrongCombinationConversation;
}

Sprite * EvidenceManager::GetSmallSpriteForId(const string &id)
{
    return idToItemMap[id]->GetSmallSprite();
}

Sprite * EvidenceManager::GetLargeSpriteForId(const string &id)
{
    return idToItemMap[id]->GetLargeSprite();
}

string EvidenceManager::GetNameForId(const string &id)
{
    return idToItemMap[id]->GetName();
}

string EvidenceManager::GetDescriptionForId(const string &id)
{
    return idToItemMap[id]->GetDescription();
}

bool EvidenceManager::IsEvidenceWithIdProfile(const string &id)
{
    return idToItemMap[id]->GetIsProfile();
}

bool EvidenceManager::IsEvidenceWithIdEnabled(const string &id)
{
    return idToItemMap[id]->GetIsEnabled();
}

bool EvidenceManager::IsEvidenceWithIdVisible(const string &id)
{
    return idToItemMap[id]->GetIsEnabled() && !idToItemMap[id]->GetIsHidden();
}

void EvidenceManager::EnableEvidenceWithId(const string &id)
{
    idToItemMap[id]->SetIsEnabled(true);
    evidenceCount++;
    CheckAreEvidenceCombinations();
}

void EvidenceManager::DisableEvidenceWithId(const string &id)
{
    idToItemMap[id]->SetIsHidden(true);
    evidenceCount--;
    CheckAreEvidenceCombinations();
}

Conversation * EvidenceManager::GetConversationForEvidenceCombination(const string &evidenceId1, const string &evidenceId2)
{
    EvidenceIdPair pair(evidenceId1, evidenceId2);

    for (map<EvidenceIdPair, Conversation *>::iterator iter = idPairToCombinationConversationMap.begin(); iter != idPairToCombinationConversationMap.end(); ++iter)
    {
        if (pair == iter->first)
        {
            return iter->second;
        }
    }

    return GetWrongCombinationConversation();
}

void EvidenceManager::Reset()
{
    for (map<string, Evidence *>::iterator iter = idToItemMap.begin(); iter != idToItemMap.end(); ++iter)
    {
        iter->second->SetIsEnabled(evidenceToOriginalEnabledStateMap[iter->second]);
        iter->second->SetIsHidden(false);
    }
}

void EvidenceManager::CacheState()
{
    evidenceToCachedEnabledStateMap.clear();
    evidenceToCachedHiddenStateMap.clear();

    for (map<string, Evidence *>::iterator iter = idToItemMap.begin(); iter != idToItemMap.end(); ++iter)
    {
        evidenceToCachedEnabledStateMap[iter->second] = iter->second->GetIsEnabled();
        evidenceToCachedHiddenStateMap[iter->second] = iter->second->GetIsHidden();
    }
}

void EvidenceManager::LoadCachedState()
{
    for (map<string, Evidence *>::iterator iter = idToItemMap.begin(); iter != idToItemMap.end(); ++iter)
    {
        iter->second->SetIsEnabled(evidenceToCachedEnabledStateMap[iter->second]);
        iter->second->SetIsHidden(evidenceToCachedHiddenStateMap[iter->second]);
    }
}

void EvidenceManager::SaveToSaveFile(XmlWriter *pWriter)
{
    pWriter->StartElement("EvidenceManager");
    pWriter->StartElement("EvidenceList");

    for (map<string, Evidence *>::iterator iter = idToItemMap.begin(); iter != idToItemMap.end(); ++iter)
    {
        pWriter->StartElement("Evidence");
        pWriter->WriteTextElement("Id", iter->first);
        pWriter->WriteBooleanElement("IsEnabled", iter->second->GetIsEnabled());
        pWriter->WriteBooleanElement("IsHidden", iter->second->GetIsHidden());
        pWriter->EndElement();
    }

    pWriter->EndElement();
    pWriter->EndElement();
}

void EvidenceManager::LoadFromSaveFile(XmlReader *pReader)
{
    pReader->StartElement("EvidenceManager");
    pReader->StartElement("EvidenceList");

    pReader->StartList("Evidence");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");

        if (idToItemMap.count(id) > 0)
        {
            idToItemMap[id]->SetIsEnabled(pReader->ReadBooleanElement("IsEnabled"));
            idToItemMap[id]->SetIsHidden(pReader->ReadBooleanElement("IsHidden"));
        }
    }

    pReader->EndElement();
    pReader->EndElement();

    CheckAreEvidenceCombinations();
}

void EvidenceManager::LoadFromXml(XmlReader *pReader)
{
    pReader->StartElement("EvidenceManager");

    pReader->StartElement("IdToItemDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        idToItemMap[id] = new Evidence(pReader);

        if (idToItemMap[id]->GetIsEnabled())
        {
            evidenceCount++;
        }

        evidenceToOriginalEnabledStateMap[idToItemMap[id]] = idToItemMap[id]->GetIsEnabled();
    }

    CheckAreEvidenceCombinations();

    pReader->EndElement();

    pReader->StartElement("IdPairToCombinationConversationDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        idPairToCombinationConversationMap[EvidenceIdPair(pReader)] = Conversation::LoadFromXml(pReader);
    }

    pReader->EndElement();

    pReader->StartElement("IdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        idList.push_back(pReader->ReadTextElement("Id"));
    }

    pReader->EndElement();

    pReader->StartElement("WrongCombinationConversation");
    pWrongCombinationConversation = Conversation::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

void EvidenceManager::CheckAreEvidenceCombinations()
{
    areEvidenceCombinations = false;

    for (map<EvidenceIdPair, Conversation *>::iterator iter = idPairToCombinationConversationMap.begin(); iter != idPairToCombinationConversationMap.end(); ++iter)
    {
        EvidenceIdPair idPair = iter->first;

        if (IsEvidenceWithIdVisible(idPair.GetEvidenceId1()) && IsEvidenceWithIdVisible(idPair.GetEvidenceId2()))
        {
            areEvidenceCombinations = true;
            break;
        }
    }
}
