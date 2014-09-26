/**
 * Basic header/include file for EvidenceManager.cpp.
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

#ifndef EVIDENCEMANAGER_H
#define EVIDENCEMANAGER_H

#include "../Sprite.h"
#include "../CaseContent/Conversation.h"
#include <map>
#include <vector>

class XmlReader;
class XmlWriter;

class Evidence
{
public:
    Evidence()
        : smallSpriteId("")
        , largeSpriteId("")
        , pSmallSprite(NULL)
        , pLargeSprite(NULL)
        , name("")
        , description("")
        , isProfile(false)
        , isEnabled(false)
        , isHidden(false)
    {
    }

    Evidence(XmlReader *pReader);

    Sprite * GetSmallSprite();
    Sprite * GetLargeSprite();

    string GetName() const { return this->name; }

    string GetDescription() const { return this->description; }

    bool GetIsProfile() const { return this->isProfile; }
    void SetIsProfile(bool isProfile) { this->isProfile = isProfile; }

    bool GetIsEnabled() const { return this->isEnabled; }
    void SetIsEnabled(bool isEnabled) { this->isEnabled = isEnabled; }

    bool GetIsHidden() const { return this->isHidden; }
    void SetIsHidden(bool isHidden) { this->isHidden = isHidden; }

    bool operator< (const Evidence &other) const;

private:
    string smallSpriteId;
    string largeSpriteId;

    Sprite *pSmallSprite;
    Sprite *pLargeSprite;

    string name;
    string description;
    bool isProfile;
    bool isEnabled;
    bool isHidden;
};

class EvidenceIdPair
{
public:
    EvidenceIdPair()
        : evidenceId1("")
        , evidenceId2("")
    {
    }

    EvidenceIdPair(const string &evidenceId1, const string &evidenceId2)
        : evidenceId1(evidenceId1)
        , evidenceId2(evidenceId2)
    {
    }

    EvidenceIdPair(XmlReader *pReader);

    string GetEvidenceId1() const { return this->evidenceId1; }
    void SetEvidenceId1(const string &evidenceId1) { this->evidenceId1 = evidenceId1; }

    string GetEvidenceId2() const { return this->evidenceId2; }
    void SetEvidenceId2(const string &evidenceId2) { this->evidenceId2 = evidenceId2; }

    bool operator==(const EvidenceIdPair &other) const;
    bool operator!=(const EvidenceIdPair &other) const;

    bool operator< (const EvidenceIdPair &other) const;

private:
    string evidenceId1;
    string evidenceId2;
};

class EvidenceManager
{
public:
    EvidenceManager()
    {
        evidenceCount = 0;
        areEvidenceCombinations = false;
        pWrongCombinationEncounter = NULL;
    }

    ~EvidenceManager();

    Encounter * GetWrongCombinationEncounter() { return this->pWrongCombinationEncounter; }
    void SetWrongCombinationEncounter(Encounter *pWrongCombinationEncounter) { this->pWrongCombinationEncounter = pWrongCombinationEncounter; }

    bool GetHasEvidence() const { return this->evidenceCount > 0; }
    bool GetCanCombineEvidence() const { return this->evidenceCount > 1; }
    bool GetAreEvidenceCombinations() const { return this->areEvidenceCombinations; }
    vector<string> * GetIds() { return &this->idList; }

    Sprite * GetSmallSpriteForId(const string &id);
    Sprite * GetLargeSpriteForId(const string &id);
    string GetNameForId(const string &id);
    string GetDescriptionForId(const string &id);

    bool IsEvidenceWithIdProfile(const string &id);
    bool IsEvidenceWithIdEnabled(const string &id);
    bool IsEvidenceWithIdVisible(const string &id);

    void EnableEvidenceWithId(const string &id);
    void DisableEvidenceWithId(const string &id);

    Encounter * GetEncounterForEvidenceCombination(const string &evidenceId1, const string &evidenceId2);
    void Reset();

    void CacheState();
    void LoadCachedState();

    void SaveToSaveFile(XmlWriter *pWriter);
    void LoadFromSaveFile(XmlReader *pReader);

    void LoadFromXml(XmlReader *pReader);

private:
    void CheckAreEvidenceCombinations();

    map<string, Evidence *> idToItemMap;
    map<EvidenceIdPair, Encounter *> idPairToCombinationEncounterMap;
    vector<string> idList;

    int evidenceCount;
    bool areEvidenceCombinations;

    map<Evidence *, bool> evidenceToOriginalEnabledStateMap;

    map<Evidence *, bool> evidenceToCachedEnabledStateMap;
    map<Evidence *, bool> evidenceToCachedHiddenStateMap;

    Encounter *pWrongCombinationEncounter;
};

#endif
