/**
 * Represents a character as they appear out in the field.
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

#include "FieldCharacter.h"
#include "ForegroundElement.h"
#include "Crowd.h"
#include "../MouseHelper.h"
#include "../XmlReader.h"
#include "../CaseInformation/Case.h"

FieldCharacter::FieldCharacter()
{
    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        characterStandingAnimationIds[i] = "";
        characterWalkingAnimationIds[i] = "";
        characterRunningAnimationIds[i] = "";

        pCharacterStandingAnimations[i] = NULL;
        pCharacterWalkingAnimations[i] = NULL;
        pCharacterRunningAnimations[i] = NULL;
    }

    isBegun = false;
    state = FieldCharacterStateNone;
    pCurrentAnimation = NULL;

    pClickEncounter = NULL;
    pClickCutscene = NULL;

    extraHeight = 0;

    id = "";
    name = "";
    position = Vector2(0, 0);
    pHitBox = NULL;
    direction = CharacterDirectionLeft;
    spriteDirection = FieldCharacterDirectionSide;
    anchorPosition = 0;
    clickEncounterId = "";
    clickCutsceneId = "";
    isMouseOver = false;
    isClicked = false;
    pCondition = NULL;
}

FieldCharacter::FieldCharacter(const string &characterId, const string &characterName, string characterStandingAnimationIds[], string characterWalkingAnimationIds[], string characterRunningAnimationIds[])
{
    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        this->characterStandingAnimationIds[i] = characterStandingAnimationIds[i];
        this->characterWalkingAnimationIds[i] = characterWalkingAnimationIds[i];
        this->characterRunningAnimationIds[i] = characterRunningAnimationIds[i];

        pCharacterStandingAnimations[i] = NULL;
        pCharacterWalkingAnimations[i] = NULL;
        pCharacterRunningAnimations[i] = NULL;
    }

    isBegun = false;
    state = FieldCharacterStateNone;
    pCurrentAnimation = NULL;

    pClickEncounter = NULL;
    pClickCutscene = NULL;

    extraHeight = 0;

    id = characterId;
    name = characterName;
    position = Vector2(0, 0);
    pHitBox = NULL;
    direction = CharacterDirectionLeft;
    spriteDirection = FieldCharacterDirectionSide;
    anchorPosition = 0;
    clickEncounterId = "";
    clickCutsceneId = "";
    isMouseOver = false;
    isClicked = false;
    pCondition = NULL;
}

FieldCharacter::FieldCharacter(XmlReader *pReader)
{
    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        characterStandingAnimationIds[i] = "";
        characterWalkingAnimationIds[i] = "";
        characterRunningAnimationIds[i] = "";

        pCharacterStandingAnimations[i] = NULL;
        pCharacterWalkingAnimations[i] = NULL;
        pCharacterRunningAnimations[i] = NULL;
    }

    isBegun = false;
    state = FieldCharacterStateNone;
    pCurrentAnimation = NULL;

    pClickEncounter = NULL;
    pClickCutscene = NULL;

    extraHeight = 0;

    id = "";
    name = "";
    position = Vector2(0, 0);
    pHitBox = NULL;
    direction = CharacterDirectionLeft;
    spriteDirection = FieldCharacterDirectionSide;
    anchorPosition = 0;
    clickEncounterId = "";
    clickCutsceneId = "";
    isMouseOver = false;
    isClicked = false;
    pCondition = NULL;

    pReader->StartElement("FieldCharacter");
    id = pReader->ReadTextElement("Id");
    name = pReader->ReadTextElement("Name");

    pReader->StartElement("CharacterStandingAnimationIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));
        characterStandingAnimationIds[(int)direction] = pReader->ReadTextElement("Id");
    }

    pReader->EndElement();

    pReader->StartElement("CharacterWalkingAnimationIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));
        characterWalkingAnimationIds[(int)direction] = pReader->ReadTextElement("Id");
    }

    pReader->EndElement();

    pReader->StartElement("CharacterRunningAnimationIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));
        characterRunningAnimationIds[(int)direction] = pReader->ReadTextElement("Id");
    }

    pReader->EndElement();

    pReader->StartElement("Position");
    position = Vector2(pReader);
    pReader->EndElement();

    pHitBox = new HitBox(pReader);
    direction = StringToCharacterDirection(pReader->ReadTextElement("Direction"));
    spriteDirection = StringToFieldCharacterDirection(pReader->ReadTextElement("SpriteDirection"));
    anchorPosition = pReader->ReadIntElement("AnchorPosition");

    pReader->StartElement("ClickRect");
    clickRect = RectangleWH(pReader);
    pReader->EndElement();

    if (pReader->ElementExists("ClickEncounterId"))
    {
        clickEncounterId = pReader->ReadTextElement("ClickEncounterId");
    }

    if (pReader->ElementExists("ClickCutsceneId"))
    {
        clickCutsceneId = pReader->ReadTextElement("ClickCutsceneId");
    }

    if (pReader->ElementExists("InteractionLocation"))
    {
        pReader->StartElement("InteractionLocation");
        interactionLocation = Vector2(pReader);
        pReader->EndElement();
    }
    else
    {
        interactionLocation = Vector2(-1, -1);
    }

    interactFromAnywhere = pReader->ReadBooleanElement("InteractFromAnywhere");

    if (pReader->ElementExists("Condition"))
    {
        pReader->StartElement("Condition");
        pCondition = new Condition(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}

FieldCharacter::~FieldCharacter()
{
    delete pHitBox;
    pHitBox = NULL;
}

void FieldCharacter::SetSpriteDirection(FieldCharacterDirection spriteDirection)
{
    if (spriteDirection != this->spriteDirection)
    {
        this->spriteDirection = spriteDirection;
        UpdateAnimation();
    }
}

Encounter * FieldCharacter::GetClickEncounter()
{
    if (id == Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId())
    {
        return Case::GetInstance()->GetPartnerManager()->GetCurrentPartner()->GetCurrentEncounter();
    }

    if (pClickEncounter == NULL)
    {
        pClickEncounter = Case::GetInstance()->GetContentManager()->GetEncounterFromId(GetClickEncounterId());
    }

    return pClickEncounter;
}

FieldCutscene * FieldCharacter::GetClickCutscene()
{
    if (pClickCutscene == NULL)
    {
        pClickCutscene = Case::GetInstance()->GetFieldCutsceneManager()->GetCutsceneFromId(GetClickCutsceneId());
    }

    return pClickCutscene;
}

void FieldCharacter::SetState(FieldCharacterState state)
{
    if (state != this->state)
    {
        InitializeAnimations(state);
        this->state = state;
        UpdateAnimation();
    }
}

bool FieldCharacter::GetIsPresent()
{
    return pCondition != NULL ? pCondition->IsTrue() : true;
}

void FieldCharacter::Begin()
{
    InitializeAnimations(FieldCharacterStateStanding);
    state = FieldCharacterStateStanding;

    UpdateAnimation();
    isBegun = true;
}

void FieldCharacter::Update(int delta)
{
    RectangleWH adjustedClickRect = RectangleWH((int)(GetPosition().GetX() + GetClickRect().GetX()), (int)(GetPosition().GetY() + GetClickRect().GetY() - extraHeight), GetClickRect().GetWidth(), GetClickRect().GetHeight());
    Update(delta, adjustedClickRect);
}

void FieldCharacter::Update(int delta, Vector2 offsetVector)
{
    RectangleWH adjustedClickRect = RectangleWH((int)(GetPosition().GetX() - offsetVector.GetX() + GetClickRect().GetX()), (int)(GetPosition().GetY() - offsetVector.GetY() + GetClickRect().GetY() - extraHeight), GetClickRect().GetWidth(), GetClickRect().GetHeight());
    Update(delta, adjustedClickRect);
}

void FieldCharacter::Update(int delta, RectangleWH adjustedClickRect)
{
    UpdateAnimation(delta);
    UpdateClickState(adjustedClickRect);
}

void FieldCharacter::UpdateDirection(Vector2 directionVector)
{
    directionVector = directionVector.Normalize();

    // We'll snap the player's direction vector according to
    // the nearest direction for which we have an animation.
    double angleToHorizontal = acos(directionVector.GetX());

    // acos() only returns values from 0 to pi,
    // so to get the full circle we need to check
    // whether we're in the bottom two quandrants,
    // and change the angle to account for this if so.
    if (directionVector.GetY() > 0)
    {
        angleToHorizontal = 2 * M_PI - angleToHorizontal;
    }

    if (angleToHorizontal <= M_PI / 8 || angleToHorizontal > M_PI * 15 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionSide);
        SetDirection(CharacterDirectionRight);
    }
    else if (angleToHorizontal > M_PI / 8 && angleToHorizontal <= M_PI * 3 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionDiagonalUp);
        SetDirection(CharacterDirectionRight);
    }
    else if (angleToHorizontal > 3 * M_PI / 8 && angleToHorizontal <= M_PI * 5 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionUp);
    }
    else if (angleToHorizontal > 5 * M_PI / 8 && angleToHorizontal <= M_PI * 7 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionDiagonalUp);
        SetDirection(CharacterDirectionLeft);
    }
    else if (angleToHorizontal > 7 * M_PI / 8 && angleToHorizontal <= M_PI * 9 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionSide);
        SetDirection(CharacterDirectionLeft);
    }
    else if (angleToHorizontal > 9 * M_PI / 8 && angleToHorizontal <= M_PI * 11 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionDiagonalDown);
        SetDirection(CharacterDirectionLeft);
    }
    else if (angleToHorizontal > 11 * M_PI / 8 && angleToHorizontal <= M_PI * 13 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionDown);
    }
    else if (angleToHorizontal > 13 * M_PI / 8 && angleToHorizontal <= M_PI * 15 / 8)
    {
        SetSpriteDirection(FieldCharacterDirectionDiagonalDown);
        SetDirection(CharacterDirectionRight);
    }
}

void FieldCharacter::UpdateAnimation(int delta)
{
    if (!isBegun)
    {
        Begin();
    }

    if (state == FieldCharacterStateStanding)
    {
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionUp)->Update(delta);
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Update(delta);
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionSide)->Update(delta);
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Update(delta);
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDown)->Update(delta);
    }
    else if (state == FieldCharacterStateWalking)
    {
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionUp)->Update(delta);
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Update(delta);
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionSide)->Update(delta);
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Update(delta);
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDown)->Update(delta);
    }
    else if (state == FieldCharacterStateRunning)
    {
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionUp)->Update(delta);
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Update(delta);
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionSide)->Update(delta);
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Update(delta);
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDown)->Update(delta);
    }
}

void FieldCharacter::UpdateClickState(Vector2 offsetVector)
{
    RectangleWH adjustedClickRect = RectangleWH((int)(GetPosition().GetX() - offsetVector.GetX() + GetClickRect().GetX()), (int)(GetPosition().GetY() - offsetVector.GetY() + GetClickRect().GetY() - extraHeight), GetClickRect().GetWidth(), GetClickRect().GetHeight());
    UpdateClickState(adjustedClickRect);
}

void FieldCharacter::UpdateClickState(RectangleWH adjustedClickRect)
{
    if ((GetClickEncounter() != NULL || GetClickCutscene() != NULL) &&
        (MouseHelper::MouseOverRect(adjustedClickRect) || MouseHelper::MouseDownOnRect(adjustedClickRect)) &&
        !MouseHelper::PressedAndHeldAnywhere() &&
        !MouseHelper::DoublePressedAndHeldAnywhere())
    {
        MouseHelper::SetCursorType(CursorTypeTalk);
        MouseHelper::SetMouseOverText(GetName());
        SetIsMouseOver(true);
    }
    else
    {
        SetIsMouseOver(false);
    }

    SetIsClicked(MouseHelper::ClickedOnRect(adjustedClickRect) || MouseHelper::DoubleClickedOnRect(adjustedClickRect));
}

void FieldCharacter::Draw()
{
    pCurrentAnimation->Draw(GetPosition() - Vector2(0, extraHeight), GetDirection() == CharacterDirectionRight, 1.0);
}

void FieldCharacter::Draw(Vector2 offsetVector)
{
    pCurrentAnimation->Draw(GetPosition() - offsetVector - Vector2(0, extraHeight), GetDirection() == CharacterDirectionRight, 1.0);
}

void FieldCharacter::Reset()
{
    ResetAnimations();
    pCurrentAnimation = NULL;
    SetIsMouseOver(false);
    SetIsClicked(false);
    isBegun = false;
}

void FieldCharacter::Reset(CharacterDirection direction, FieldCharacterDirection spriteDirection, Vector2 position, FieldCharacterState state)
{
    this->direction = direction;
    this->spriteDirection = spriteDirection;
    this->position = position;
    this->state = state;
}

Vector2 FieldCharacter::GetVectorAnchorPosition()
{
    return Vector2(
        GetPosition().GetX() + GetClickRect().GetX() + GetClickRect().GetWidth() / 2,
        GetPosition().GetY() + GetAnchorPosition());
}

Vector2 FieldCharacter::GetMidPoint()
{
    return Vector2(
        GetPosition().GetX() + GetClickRect().GetX() + GetClickRect().GetWidth() / 2,
        GetPosition().GetY() + GetClickRect().GetY() + GetClickRect().GetHeight() / 2 - extraHeight);
}

void FieldCharacter::SetExtraHeightFromHeightMaps(vector<HeightMap *> *pHeightMapList)
{
    extraHeight = 0;
    Vector2 characterPosition = GetVectorAnchorPosition();

    for (unsigned int i = 0; i < pHeightMapList->size(); i++)
    {
        HeightMap *pHeightMap = (*pHeightMapList)[i];

        if (pHeightMap->IsPointInBoundingPolygon(characterPosition))
        {
            extraHeight += pHeightMap->GetHeightAtPoint(characterPosition);
        }
    }
}

RectangleWH FieldCharacter::GetBoundsForInteraction()
{
    if (IsInteractionPointExact())
    {
        return RectangleWH(interactionLocation.GetX(), interactionLocation.GetY(), 1, 1);
    }

    RectangleWH hitboxBoundingBox = GetHitBox()->GetBoundingBox();

    return RectangleWH(
        GetVectorAnchorPosition().GetX() + hitboxBoundingBox.GetX(),
        GetVectorAnchorPosition().GetY() + hitboxBoundingBox.GetY(),
        hitboxBoundingBox.GetWidth(),
        hitboxBoundingBox.GetHeight());
}

bool FieldCharacter::IsInteractionPointExact()
{
    return interactionLocation.GetX() >= 0 && interactionLocation.GetY() >= 0;
}

void FieldCharacter::BeginInteraction(Location *pLocation)
{
    pLocation->BeginCharacterInteraction(this);
}

void FieldCharacter::SetPositionByAnchorPosition(Vector2 targetPosition)
{
    SetPosition(GetPosition() + (targetPosition - GetVectorAnchorPosition()));
}

bool FieldCharacter::IsCollision(FieldCharacter *pCharacter, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(GetPosition(), pCharacter->GetHitBox(), pCharacter->GetPosition(), pParam);
}

bool FieldCharacter::IsCollision(ForegroundElement *pElement, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(GetPosition(), pElement->GetHitBox(), Vector2(0, 0), pParam);
}

bool FieldCharacter::IsCollision(Crowd *pCrowd, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(GetPosition(), pCrowd->GetHitBox(), Vector2(0, 0), pParam);
}

bool FieldCharacter::IsCollision(HitBox *pHitBox, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(GetPosition(), pHitBox, Vector2(0, 0), pParam);
}

bool FieldCharacter::TestCollisionAtPosition(Vector2 position, FieldCharacter *pCharacter, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(position, pCharacter->GetHitBox(), pCharacter->GetPosition(), pParam);
}

bool FieldCharacter::TestCollisionAtPosition(Vector2 position, ForegroundElement *pElement, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(position, pElement->GetHitBox(), Vector2(0, 0), pParam);
}

bool FieldCharacter::TestCollisionAtPosition(Vector2 position, Crowd *pCrowd, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(position, pCrowd->GetHitBox(), Vector2(0, 0), pParam);
}

bool FieldCharacter::TestCollisionAtPosition(Vector2 position, HitBox *pHitBox, CollisionParameter *pParam)
{
    return GetHitBox()->IsCollision(position, pHitBox, Vector2(0, 0), pParam);
}

FieldCharacter * FieldCharacter::Clone()
{
    FieldCharacter *pCloneCharacter = new FieldCharacter(GetId(), GetName(), characterStandingAnimationIds, characterWalkingAnimationIds, characterRunningAnimationIds);
    pCloneCharacter->SetHitBox(GetHitBox()->Clone());
    pCloneCharacter->SetAnchorPosition(GetAnchorPosition());
    pCloneCharacter->SetClickRect(GetClickRect());
    pCloneCharacter->interactionLocation = interactionLocation;
    pCloneCharacter->SetCondition(pCondition != NULL ? pCondition->Clone() : NULL);
    return pCloneCharacter;
}

void FieldCharacter::InitializeAnimations(FieldCharacterState newState)
{
    if (state != newState)
    {
        if (state == FieldCharacterStateStanding)
        {
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionUp)->Reset();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Reset();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionSide)->Reset();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Reset();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDown)->Reset();
        }
        else if (state == FieldCharacterStateWalking)
        {
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionUp)->Reset();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Reset();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionSide)->Reset();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Reset();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDown)->Reset();
        }
        else if (state == FieldCharacterStateRunning)
        {
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionUp)->Reset();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Reset();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionSide)->Reset();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Reset();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDown)->Reset();
        }

        if (newState == FieldCharacterStateStanding)
        {
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionUp)->Begin();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Begin();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionSide)->Begin();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Begin();
            GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDown)->Begin();
        }
        else if (newState == FieldCharacterStateWalking)
        {
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionUp)->Begin();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Begin();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionSide)->Begin();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Begin();
            GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDown)->Begin();
        }
        else if (newState == FieldCharacterStateRunning)
        {
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionUp)->Begin();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Begin();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionSide)->Begin();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Begin();
            GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDown)->Begin();
        }
    }
}

void FieldCharacter::UpdateAnimation()
{
    if (state == FieldCharacterStateStanding)
    {
        pCurrentAnimation = GetCharacterStandingAnimationForDirection(spriteDirection);
    }
    else if (state == FieldCharacterStateWalking)
    {
        pCurrentAnimation = GetCharacterWalkingAnimationForDirection(spriteDirection);
    }
    else if (state == FieldCharacterStateRunning)
    {
        pCurrentAnimation = GetCharacterRunningAnimationForDirection(spriteDirection);
    }
}

void FieldCharacter::ResetAnimations()
{
    if (state == FieldCharacterStateStanding)
    {
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionUp)->Reset();
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Reset();
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionSide)->Reset();
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Reset();
        GetCharacterStandingAnimationForDirection(FieldCharacterDirectionDown)->Reset();
    }
    else if (state == FieldCharacterStateWalking)
    {
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionUp)->Reset();
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Reset();
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionSide)->Reset();
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Reset();
        GetCharacterWalkingAnimationForDirection(FieldCharacterDirectionDown)->Reset();
    }
    else if (state == FieldCharacterStateRunning)
    {
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionUp)->Reset();
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalUp)->Reset();
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionSide)->Reset();
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDiagonalDown)->Reset();
        GetCharacterRunningAnimationForDirection(FieldCharacterDirectionDown)->Reset();
    }
}

AnimationSound * FieldCharacter::GetSoundToPlay()
{
    return pCurrentAnimation != NULL ? pCurrentAnimation->GetSoundToPlay() : NULL;
}

int FieldCharacter::GetZOrder()
{
    if (anchorPosition >= 0)
    {
        return (int)(GetPosition().GetY() + anchorPosition);
    }
    else
    {
        return GetCanInteractFromAnywhere() ? -2 : -1;
    }
}

Animation * FieldCharacter::GetCharacterStandingAnimationForDirection(FieldCharacterDirection spriteDirection)
{
    if (spriteDirection == FieldCharacterDirectionNone)
    {
        return NULL;
    }

    if (pCharacterStandingAnimations[spriteDirection] == NULL)
    {
        pCharacterStandingAnimations[FieldCharacterDirectionUp] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterStandingAnimationIds[FieldCharacterDirectionUp]);
        pCharacterStandingAnimations[FieldCharacterDirectionDiagonalUp] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterStandingAnimationIds[FieldCharacterDirectionDiagonalUp]);
        pCharacterStandingAnimations[FieldCharacterDirectionSide] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterStandingAnimationIds[FieldCharacterDirectionSide]);
        pCharacterStandingAnimations[FieldCharacterDirectionDiagonalDown] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterStandingAnimationIds[FieldCharacterDirectionDiagonalDown]);
        pCharacterStandingAnimations[FieldCharacterDirectionDown] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterStandingAnimationIds[FieldCharacterDirectionDown]);
    }

    return pCharacterStandingAnimations[spriteDirection];
}

Animation * FieldCharacter::GetCharacterWalkingAnimationForDirection(FieldCharacterDirection spriteDirection)
{
    if (spriteDirection == FieldCharacterDirectionNone)
    {
        return NULL;
    }

    if (pCharacterWalkingAnimations[spriteDirection] == NULL)
    {
        pCharacterWalkingAnimations[FieldCharacterDirectionUp] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterWalkingAnimationIds[FieldCharacterDirectionUp]);
        pCharacterWalkingAnimations[FieldCharacterDirectionDiagonalUp] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterWalkingAnimationIds[FieldCharacterDirectionDiagonalUp]);
        pCharacterWalkingAnimations[FieldCharacterDirectionSide] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterWalkingAnimationIds[FieldCharacterDirectionSide]);
        pCharacterWalkingAnimations[FieldCharacterDirectionDiagonalDown] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterWalkingAnimationIds[FieldCharacterDirectionDiagonalDown]);
        pCharacterWalkingAnimations[FieldCharacterDirectionDown] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterWalkingAnimationIds[FieldCharacterDirectionDown]);
    }

    return pCharacterWalkingAnimations[spriteDirection];
}

Animation * FieldCharacter::GetCharacterRunningAnimationForDirection(FieldCharacterDirection spriteDirection)
{
    if (spriteDirection == FieldCharacterDirectionNone)
    {
        return NULL;
    }

    if (pCharacterRunningAnimations[spriteDirection] == NULL)
    {
        pCharacterRunningAnimations[FieldCharacterDirectionUp] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterRunningAnimationIds[FieldCharacterDirectionUp]);
        pCharacterRunningAnimations[FieldCharacterDirectionDiagonalUp] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterRunningAnimationIds[FieldCharacterDirectionDiagonalUp]);
        pCharacterRunningAnimations[FieldCharacterDirectionSide] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterRunningAnimationIds[FieldCharacterDirectionSide]);
        pCharacterRunningAnimations[FieldCharacterDirectionDiagonalDown] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterRunningAnimationIds[FieldCharacterDirectionDiagonalDown]);
        pCharacterRunningAnimations[FieldCharacterDirectionDown] = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(characterRunningAnimationIds[FieldCharacterDirectionDown]);
    }

    return pCharacterRunningAnimations[spriteDirection];
}
