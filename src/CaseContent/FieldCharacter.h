/**
 * Basic header/include file for FieldCharacter.cpp.
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

#ifndef FIELDCHARACTER_H
#define FIELDCHARACTER_H

#include "../Animation.h"
#include "../Collisions.h"
#include "../Condition.h"
#include "../enums.h"
#include "../HeightMap.h"
#include "../Interfaces.h"
#include "../Line.h"

using namespace std;

class Location;
class Encounter;
class FieldCutscene;
class ForegroundElement;
class Crowd;
class XmlReader;

class FieldCharacter : public InteractiveElement, public ZOrderableObject
{
public:
    FieldCharacter();
    FieldCharacter(const string &characterId, const string &characterName, string characterStandingAnimationIds[], string characterWalkingAnimationIds[], string characterRunningAnimationIds[]);
    FieldCharacter(XmlReader *pReader);
    virtual ~FieldCharacter();

    string GetId() const { return this->id; }
    void SetId(const string &id) { this->id = id; }

    string GetName() const { return this->name; }
    void SetName(const string &name) { this->id = name; }

    Vector2 GetPosition() const { return this->position; }
    void SetPosition(Vector2 position) { this->position = position; }

    HitBox * GetHitBox() { return this->pHitBox; }
    void SetHitBox(HitBox *pHitBox) { this->pHitBox = pHitBox; }

    CharacterDirection GetDirection() const { return this->direction; }
    void SetDirection(CharacterDirection direction) { this->direction = direction; }

    FieldCharacterDirection GetSpriteDirection() const { return this->spriteDirection; }
    void SetSpriteDirection(FieldCharacterDirection spriteDirection);

    int GetAnchorPosition() const { return this->anchorPosition; }
    void SetAnchorPosition(int anchorPosition) { this->anchorPosition = anchorPosition; }

    RectangleWH GetClickRect() const { return this->clickRect; }
    void SetClickRect(RectangleWH clickRect) { this->clickRect = clickRect; }

    string GetClickEncounterId() const { return this->clickEncounterId; }
    void SetClickEncounterId(const string &clickEncounterId) { this->clickEncounterId = clickEncounterId; }

    Encounter * GetClickEncounter();

    string GetClickCutsceneId() const { return this->clickCutsceneId; }
    void SetClickCutsceneId(const string &clickCutsceneId) { this->clickCutsceneId = clickCutsceneId; }

    FieldCutscene * GetClickCutscene();

    bool GetIsMouseOver() const { return this->isMouseOver; }
    void SetIsMouseOver(bool isMouseOver) { this->isMouseOver = isMouseOver; }

    bool GetIsClicked() const { return this->isClicked; }
    void SetIsClicked(bool isClicked) { this->isClicked = isClicked; }

    FieldCharacterState GetState() const { return this->state; }

    void SetState(FieldCharacterState state);

    Condition * GetCondition() { return this->pCondition; }
    void SetCondition(Condition *pCondition) { this->pCondition = pCondition; }

    bool GetIsPresent();

    void Begin();

    void Update(int delta);
    void Update(int delta, Vector2 offsetVector);
    void Update(int delta, RectangleWH adjustedClickRect);

    void UpdateDirection(Vector2 directionVector);
    void UpdateAnimation(int delta);
    void UpdateClickState(Vector2 offsetVector);
    void UpdateClickState(RectangleWH adjustedClickRect);

    void Draw();
    void Draw(Vector2 offsetVector);

    void Reset();
    void Reset(CharacterDirection direction, FieldCharacterDirection spriteDirection, Vector2 position, FieldCharacterState state);

    Vector2 GetVectorAnchorPosition();
    Vector2 GetMidPoint();

    Vector2 GetCenterPoint()
    {
        return IsInteractionPointExact() ? interactionLocation : GetVectorAnchorPosition();
    }

    int GetExtraHeight() const
    {
        return extraHeight;
    }

    void SetExtraHeightFromHeightMaps(vector<HeightMap *> *pHeightMapList);

    RectangleWH GetBoundsForInteraction();
    bool IsInteractionPointExact();
    void BeginInteraction(Location *pLocation);

    void SetPositionByAnchorPosition(Vector2 targetPosition);

    bool IsCollision(FieldCharacter *pCharacter, CollisionParameter *pParam);
    bool IsCollision(ForegroundElement *pElement, CollisionParameter *pParam);
    bool IsCollision(Crowd *pCrowd, CollisionParameter *pParam);
    bool IsCollision(HitBox *pHitBox, CollisionParameter *pParam);
    bool TestCollisionAtPosition(Vector2 position, FieldCharacter *pCharacter, CollisionParameter *pParam);
    bool TestCollisionAtPosition(Vector2 position, ForegroundElement *pElement, CollisionParameter *pParam);
    bool TestCollisionAtPosition(Vector2 position, Crowd *pCrowd, CollisionParameter *pParam);
    bool TestCollisionAtPosition(Vector2 position, HitBox *pHitBox, CollisionParameter *pParam);

    FieldCharacter * Clone();

    void InitializeAnimations(FieldCharacterState newState);
    void UpdateAnimation();
    void ResetAnimations();

    AnimationSound * GetSoundToPlay();

    int GetZOrder();

    bool IsVisible()
    {
        return true;
    }

    Line * GetZOrderLine()
    {
        return NULL;
    }

    Vector2 GetZOrderPoint()
    {
        return GetVectorAnchorPosition();
    }

private:
    Animation * GetCharacterStandingAnimationForDirection(FieldCharacterDirection spriteDirection);
    Animation * GetCharacterWalkingAnimationForDirection(FieldCharacterDirection spriteDirection);
    Animation * GetCharacterRunningAnimationForDirection(FieldCharacterDirection spriteDirection);

    string characterStandingAnimationIds[FieldCharacterDirectionCount];
    string characterWalkingAnimationIds[FieldCharacterDirectionCount];
    string characterRunningAnimationIds[FieldCharacterDirectionCount];

    Animation *pCharacterStandingAnimations[FieldCharacterDirectionCount];
    Animation *pCharacterWalkingAnimations[FieldCharacterDirectionCount];
    Animation *pCharacterRunningAnimations[FieldCharacterDirectionCount];

    bool isBegun;
    FieldCharacterState state;
    Animation *pCurrentAnimation;

    Encounter *pClickEncounter;
    FieldCutscene *pClickCutscene;

    int extraHeight;

    string id;
    string name;
    Vector2 position;
    HitBox *pHitBox;
    CharacterDirection direction;
    FieldCharacterDirection spriteDirection;
    int anchorPosition;
    RectangleWH clickRect;
    string clickEncounterId;
    string clickCutsceneId;
    bool isMouseOver;
    bool isClicked;
    Vector2 interactionLocation;
    Condition *pCondition;
};

#endif
