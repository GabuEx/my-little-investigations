/**
 * Manager for all of the partners in the game; provides their retrieval and setting.
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

#include "PartnerManager.h"
#include "Case.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"

PartnerManager::PartnerManager()
{
    pCurrentPartner = NULL;
    pCachedPartner = NULL;
}

PartnerManager::~PartnerManager()
{
    for (map<string, Partner *>::iterator iter = partnerByIdMap.begin(); iter != partnerByIdMap.end(); ++iter)
    {
        delete iter->second;
    }
}

Partner * PartnerManager::GetPartnerFromId(const string &id)
{
    return partnerByIdMap[id];
}

Partner * PartnerManager::GetCurrentPartner()
{
    return pCurrentPartner;
}

string PartnerManager::GetCurrentPartnerId()
{
    return pCurrentPartner != NULL ? pCurrentPartner->GetId() : string("");
}

void PartnerManager::SetCurrentPartner(const string &newPartnerId)
{
    pCurrentPartner = GetPartnerFromId(newPartnerId);
}

void PartnerManager::CacheState()
{
    pCachedPartner = pCurrentPartner;
}

void PartnerManager::LoadCachedState()
{
    pCurrentPartner = pCachedPartner;
}

void PartnerManager::LoadFromXml(XmlReader *pReader)
{
    pReader->StartElement("PartnerManager");

    pReader->StartElement("PartnerByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        partnerByIdMap[id] = new Partner(pReader);
    }

    pReader->EndElement();

    pReader->EndElement();
}

void PartnerManager::SaveToSaveFile(XmlWriter *pWriter)
{
    pWriter->StartElement("PartnerManager");

    string partnerId = GetCurrentPartnerId();
    pWriter->WriteTextElement("CurrentPartnerId", partnerId.length() > 0 ? partnerId : "None");

    pWriter->EndElement();
}

void PartnerManager::LoadFromSaveFile(XmlReader *pReader)
{
    pReader->StartElement("PartnerManager");

    string partnerId = pReader->ReadTextElement("CurrentPartnerId");
    SetCurrentPartner(partnerId != "None" ? partnerId : "");

    pReader->EndElement();
}

void PartnerManager::SetCursor(FieldCustomCursorState state)
{
    if (pCurrentPartner != NULL)
    {
        pCurrentPartner->SetCursor(state);
    }
}

void PartnerManager::UpdateCursor(int delta)
{
    if (pCurrentPartner != NULL)
    {
        pCurrentPartner->UpdateCursor(delta);
    }
}

void PartnerManager::DrawCursor(Vector2 position)
{
    if (pCurrentPartner != NULL)
    {
        pCurrentPartner->DrawCursor(position);
    }
}

Vector2 PartnerManager::GetCursorSize()
{
    if (pCurrentPartner != NULL)
    {
        return pCurrentPartner->GetCursorSize();
    }
    else
    {
        return Vector2(0, 0);
    }
}

Vector2 PartnerManager::GetCursorDrawingOffset()
{
    if (pCurrentPartner != NULL)
    {
        return pCurrentPartner->GetCursorDrawingOffset();
    }
    else
    {
        return Vector2(0, 0);
    }
}

Partner::PartnerConversation::~PartnerConversation()
{
    delete pCondition;
    pCondition = NULL;
    delete pEncounter;
    pEncounter = NULL;
}

Partner::FieldCursorDefinition::FieldCursorDefinition(XmlReader *pReader)
{
    pAnimation = NULL;

    pReader->StartElement("FieldCursorDefinition");
    animationId = pReader->ReadTextElement("AnimationId");

    if (pReader->ElementExists("SfxId"))
    {
        sfxId = pReader->ReadTextElement("SfxId");
    }
    pReader->EndElement();
}

Animation * Partner::FieldCursorDefinition::GetAnimation()
{
    if (pAnimation == NULL)
    {
        pAnimation = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(animationId);
    }

    return pAnimation;
}

Partner::Partner(XmlReader *pReader)
{
    pIconSprite = NULL;
    pProfileImageSprite = NULL;
    usingFieldAbility = false;
    pFieldCursorTransitionOverlayDefinition = NULL;
    currentState = FieldCustomCursorStateOff;
    pCurrentFieldCursorAnimation = NULL;
    isTransitioning = false;

    pReader->StartElement("Partner");
    id = pReader->ReadTextElement("Id");
    name = pReader->ReadTextElement("Name");

    if (pReader->ElementExists("IconSpriteId"))
    {
        iconSpriteId = pReader->ReadTextElement("IconSpriteId");
    }

    if (pReader->ElementExists("ProfileImageSpriteId"))
    {
        profileImageSpriteId = pReader->ReadTextElement("ProfileImageSpriteId");
    }

    if (pReader->ElementExists("PassiveAbilityDescription"))
    {
        passiveAbilityDescription = pReader->ReadTextElement("PassiveAbilityDescription");
    }

    if (pReader->ElementExists("ActiveAbilityDescription"))
    {
        activeAbilityDescription = pReader->ReadTextElement("ActiveAbilityDescription");
    }

    if (pReader->ElementExists("FieldAbilityName"))
    {
        fieldAbilityName = pReader->ReadTextElement("FieldAbilityName");
    }

    if (pReader->ElementExists("ConversationAbilityName"))
    {
        conversationAbilityName = pReader->ReadTextElement("ConversationAbilityName");
    }

    if (pReader->ElementExists("ClickPointOffset"))
    {
        pReader->StartElement("ClickPointOffset");
        clickPointOffset = Vector2(pReader);
        pReader->EndElement();

        if (pReader->ElementExists("TurnOnSoundId"))
        {
            fieldCursorTurnOnSoundId = pReader->ReadTextElement("TurnOnSoundId");
        }

        if (pReader->ElementExists("TurnOffSoundId"))
        {
            fieldCursorTurnOffSoundId = pReader->ReadTextElement("TurnOffSoundId");
        }

        pReader->StartElement("FieldCursorDefinitions");

        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            string stateString = pReader->ReadTextElement("State");

            if (stateString == "Overlay")
            {
                pFieldCursorTransitionOverlayDefinition = new Partner::FieldCursorDefinition(pReader);
            }
            else
            {
                fieldCursorDefinitions[StringToFieldCustomCursorState(stateString)] = new Partner::FieldCursorDefinition(pReader);
            }
        }

        pReader->EndElement();
    }

    pReader->StartElement("PartnerConversations");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        pReader->StartElement("Condition");
        Condition *pCondition = new Condition(pReader);
        pReader->EndElement();

        pReader->StartElement("Conversation");
        Conversation *pConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();

        Encounter *pEncounter = new Encounter();
        pEncounter->SetOneShotConversation(pConversation);
        pEncounter->SetOwnsOneShotConversation(true /* ownsOneShotConversation */);

        conversationList.push_back(new PartnerConversation(pCondition, pEncounter));
    }

    pReader->EndElement();

    pReader->EndElement();
}

Partner::~Partner()
{
    for (unsigned int i = 0; i < conversationList.size(); i++)
    {
        delete conversationList[i];
    }

    conversationList.clear();

    for (map<FieldCustomCursorState, FieldCursorDefinition *>::iterator iter = fieldCursorDefinitions.begin(); iter != fieldCursorDefinitions.end(); ++iter)
    {
        delete iter->second;
    }

    fieldCursorDefinitions.clear();

    delete pFieldCursorTransitionOverlayDefinition;
    pFieldCursorTransitionOverlayDefinition = NULL;
}

Sprite * Partner::GetIconSprite()
{
    if (pIconSprite == NULL)
    {
        pIconSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(GetIconSpriteId());
    }

    return pIconSprite;
}

Sprite * Partner::GetProfileImageSprite()
{
    if (pProfileImageSprite == NULL)
    {
        pProfileImageSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(GetProfileImageSpriteId());
    }

    return pProfileImageSprite;
}

bool Partner::GetIsUsingFieldAbility() const
{
    return usingFieldAbility;
}

void Partner::ToggleIsUsingFieldAbility()
{
    usingFieldAbility = !usingFieldAbility;

    if (!usingFieldAbility)
    {
        SetCursor(FieldCustomCursorStateOff);
    }
}

Encounter * Partner::GetCurrentEncounter()
{
    for (unsigned int i = 0; i < conversationList.size(); i++)
    {
        PartnerConversation *pPartnerConversation = conversationList[i];

        if (pPartnerConversation->GetCondition()->IsTrue())
        {
            return pPartnerConversation->GetEncounter();
        }
    }

    return NULL;
}

void Partner::SetCursor(FieldCustomCursorState state)
{
    setPartnerAbilityLoopVolume();

    if (currentState != state)
    {
        FieldCustomCursorState previousState = currentState;
        currentState = state;

        if (currentState != FieldCustomCursorStateOff)
        {
            pCurrentFieldCursorAnimation = GetAnimationForCursorState(currentState);
            pCurrentFieldCursorAnimation->Begin();

            isTransitioning = true;
            GetTransitionAnimation()->Begin();

            if (fieldCursorDefinitions[state]->GetSfxId().length() > 0)
            {
                playPartnerAbilityLoop(fieldCursorDefinitions[state]->GetSfxId());
            }

            if (previousState == FieldCustomCursorStateOff)
            {
                if (fieldCursorTurnOnSoundId.length() > 0)
                {
                    playSound(fieldCursorTurnOnSoundId);
                }
            }
            else
            {
                if (pFieldCursorTransitionOverlayDefinition->GetSfxId().length() > 0)
                {
                    playSound(pFieldCursorTransitionOverlayDefinition->GetSfxId());
                }
            }
        }
        else
        {
            stopPartnerAbilityLoop();

            if (fieldCursorTurnOffSoundId.length() > 0)
            {
                playSound(fieldCursorTurnOffSoundId);
            }

            pCurrentFieldCursorAnimation = NULL;
        }
    }

    if (currentState != FieldCustomCursorStateOff)
    {
        MouseHelper::SetCursorType(CursorTypeCustom);
    }
}

void Partner::UpdateCursor(int delta)
{
    if (pCurrentFieldCursorAnimation != NULL)
    {
        pCurrentFieldCursorAnimation->Update(delta);
    }

    if (isTransitioning && GetTransitionAnimation() != NULL)
    {
        GetTransitionAnimation()->Update(delta);

        if (GetTransitionAnimation()->IsFinished())
        {
            isTransitioning = false;
        }
    }
}

void Partner::DrawCursor(Vector2 position)
{
    if (pCurrentFieldCursorAnimation != NULL)
    {
        pCurrentFieldCursorAnimation->Draw(position);
    }

    if (isTransitioning)
    {
        GetTransitionAnimation()->Draw(position);
    }
}

Vector2 Partner::GetCursorSize()
{
    if (pCurrentFieldCursorAnimation != NULL)
    {
        return pCurrentFieldCursorAnimation->GetSize();
    }
    else
    {
        return Vector2(0, 0);
    }
}

Vector2 Partner::GetCursorDrawingOffset() const
{
    return clickPointOffset * -1;
}

Animation * Partner::GetAnimationForCursorState(FieldCustomCursorState state)
{
    return fieldCursorDefinitions[state]->GetAnimation();
}

Animation * Partner::GetTransitionAnimation()
{
    return pFieldCursorTransitionOverlayDefinition->GetAnimation();
}
