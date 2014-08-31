/**
 * Basic header/include file for PartnerManager.cpp.
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

#ifndef PARTNERMANAGER_H
#define PARTNERMANAGER_H

#include "AnimationManager.h"
#include "SpriteManager.h"
#include "../enums.h"
#include "../Vector2.h"
#include <map>

class Partner;

class Condition;
class Encounter;
class XmlReader;
class XmlWriter;

class PartnerManager
{
public:
    PartnerManager();
    ~PartnerManager();
    Partner * GetPartnerFromId(const string &id);
    Partner * GetCurrentPartner();
    string GetCurrentPartnerId();
    void SetCurrentPartner(const string &newPartnerId);

    void CacheState();
    void LoadCachedState();

    void LoadFromXml(XmlReader *pReader);

    void SaveToSaveFile(XmlWriter *pWriter);
    void LoadFromSaveFile(XmlReader *pReader);

    void SetCursor(FieldCustomCursorState state);
    void UpdateCursor(int delta);
    void DrawCursor(Vector2 position);
    Vector2 GetCursorSize();
    Vector2 GetCursorDrawingOffset();

private:
    map<string, Partner *> partnerByIdMap;
    Partner *pCurrentPartner;
    Partner *pCachedPartner;
};

class Partner
{
private:
    class PartnerConversation
    {
    public:
        PartnerConversation(Condition *pCondition, Encounter *pEncounter)
        {
            this->pCondition = pCondition;
            this->pEncounter = pEncounter;
        }

        ~PartnerConversation();

        Condition * GetCondition() { return this->pCondition; }
        Encounter * GetEncounter() { return this->pEncounter; }

    private:
        Condition *pCondition;
        Encounter *pEncounter;
    };

    class FieldCursorDefinition
    {
    public:
        FieldCursorDefinition(XmlReader *pReader);

        Animation * GetAnimation();
        string GetSfxId() { return this->sfxId; }

    private:
        string animationId;
        string sfxId;

        Animation *pAnimation;
    };

public:
    Partner()
    {
        pIconSprite = NULL;
        pProfileImageSprite = NULL;
        usingFieldAbility = false;
        pFieldCursorTransitionOverlayDefinition = NULL;

        currentState = FieldCustomCursorStateOff;
        pCurrentFieldCursorAnimation = NULL;
        isTransitioning = false;
    }

    Partner(XmlReader *pReader);
    ~Partner();

    string GetId() const { return this->id; }
    void SetId(const string &id) { this->id = id; }

    string GetName() const { return this->name; }
    void SetName(const string &name) { this->name = name; }

    string GetIconSpriteId() const { return this->iconSpriteId; }
    void SetIconSpriteId(const string &iconSpriteId) { this->iconSpriteId = iconSpriteId; }

    Sprite * GetIconSprite();

    string GetProfileImageSpriteId() const { return this->profileImageSpriteId; }
    void SetProfileImageSpriteId(const string &profileImageSpriteId) { this->profileImageSpriteId = profileImageSpriteId; }

    Sprite * GetProfileImageSprite();

    string GetPassiveAbilityDescription() const { return this->passiveAbilityDescription; }
    void SetPassiveAbilityDescription(const string &passiveAbilityDescription) { this->passiveAbilityDescription = passiveAbilityDescription; }

    string GetActiveAbilityDescription() const { return this->activeAbilityDescription; }
    void SetActiveAbilityDescription(const string &activeAbilityDescription) { this->activeAbilityDescription = activeAbilityDescription; }

    string GetFieldAbilityName() const { return this->fieldAbilityName; }
    void SetFieldAbilityName(const string &fieldAbilityName) { this->fieldAbilityName = fieldAbilityName; }

    string GetConversationAbilityName() const { return this->conversationAbilityName; }
    void SetConversationAbilityName(const string &conversationAbilityName) { this->conversationAbilityName = conversationAbilityName; }

    bool GetIsUsingFieldAbility() const;
    void ToggleIsUsingFieldAbility();

    Encounter * GetCurrentEncounter();

    void SetCursor(FieldCustomCursorState state);
    void UpdateCursor(int delta);
    void DrawCursor(Vector2 position);
    Vector2 GetCursorSize();
    Vector2 GetCursorDrawingOffset() const;

private:
    Animation * GetAnimationForCursorState(FieldCustomCursorState state);
    Animation * GetTransitionAnimation();

    Sprite *pIconSprite;
    Sprite *pProfileImageSprite;

    vector<PartnerConversation *> conversationList;

    string id;
    string name;
    string iconSpriteId;
    string profileImageSpriteId;
    string passiveAbilityDescription;
    string activeAbilityDescription;
    string fieldAbilityName;
    string conversationAbilityName;
    bool usingFieldAbility;

    string fieldCursorTurnOnSoundId;
    string fieldCursorTurnOffSoundId;

    map<FieldCustomCursorState, FieldCursorDefinition *> fieldCursorDefinitions;
    FieldCursorDefinition *pFieldCursorTransitionOverlayDefinition;

    FieldCustomCursorState currentState;
    Animation *pCurrentFieldCursorAnimation;
    bool isTransitioning;

    Vector2 clickPointOffset;
};

#endif
