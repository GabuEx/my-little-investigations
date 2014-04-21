/**
 * Basic header/include file for ForegroundElement.cpp.
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

#ifndef FOREGRONDELEMENT_H
#define FOREGRONDELEMENT_H

#include "../Animation.h"
#include "../Collisions.h"
#include "../Condition.h"
#include "../Interfaces.h"
#include "../Polygon.h"
#include "../Sprite.h"
#include "../TransitionRequest.h"
#include "../Vector2.h"

class FieldCharacter;
class ForegroundElementAnimation;
class Encounter;
class XmlReader;

class ForegroundElement : public InteractiveElement, public ZOrderableObject
{
public:
    ForegroundElement()
    {
        pEncounter = NULL;

        anchorPosition = 0;
        pZOrderLine = NULL;

        pSprite = NULL;
        pHitBox = NULL;

        isMouseOver = false;
        isClicked = false;

        pCondition = NULL;
    }

    ForegroundElement(XmlReader *pReader);
    virtual ~ForegroundElement();

    string GetName() const { return this->name; }
    void SetName(const string &name) { this->name = name; }

    HitBox * GetHitBox() const { return this->pHitBox; }

    bool GetIsMouseOver() const { return this->isMouseOver; }
    void SetIsMouseOver(bool isMouseOver) { this->isMouseOver = isMouseOver; }

    bool GetIsClicked() const { return this->isClicked; }
    void SetIsClicked(bool isClicked) { this->isClicked = isClicked; }

    TransitionRequest GetTransitionRequest() const { return this->transitionRequest; }

    bool GetIsInteractive() const;
    bool GetIsInteractionFinished();

    void Begin();

    virtual void Update(int delta);
    virtual void Update(int delta, Vector2 offsetVector);
    void Update(int delta, GeometricPolygon adjustedClickPolygon);

    void UpdateAnimation(int delta);
    void UpdateClickState(Vector2 offsetVector);
    void UpdateClickState(GeometricPolygon adjustedClickPolygon);

    void Draw();
    void Draw(Vector2 offsetVector);

    void BeginInteraction();
    void UpdateInteraction(int delta);
    void DrawInteraction();

    Vector2 GetInexactCenterPoint();
    Vector2 GetCenterPoint();
    RectangleWH GetBoundsForInteraction();
    bool IsInteractionPointExact();
    void BeginInteraction(Location *pLocation);

    int GetZOrder();
    bool IsVisible();
    bool IsPresent();
    Line * GetZOrderLine();
    Vector2 GetZOrderPoint();

protected:
    void LoadFromXmlCore(XmlReader *pReader);

    string name;
    GeometricPolygon clickPolygon;

    Encounter *pEncounter;
    string zoomedViewId;

    Vector2 position;
    int anchorPosition;
    Line *pZOrderLine;

private:
    Sprite * GetSprite();

    Sprite *pSprite;
    string spriteId;

    Vector2 interactionLocation;
    HitBox *pHitBox;

    vector<ForegroundElementAnimation *> foregroundElementAnimationList;

    bool isMouseOver;
    bool isClicked;

    TransitionRequest transitionRequest;
    Condition *pCondition;
};

class ForegroundElementAnimation
{
public:
    ForegroundElementAnimation(XmlReader *pReader);

    void Begin();
    void Update(int delta);
    void Draw(Vector2 offsetVector);

private:
    Animation * GetAnimation();

    string animationId;
    Animation *pAnimation;
    Vector2 position;
};

class HiddenForegroundElement : public ForegroundElement
{
public:
    HiddenForegroundElement(XmlReader *pReader);

    Vector2 GetPosition() const { return this->position; }

    string GetRequiredPartnerId() const { return this->requiredPartnerId; }

    bool GetIsDiscovered() const { return this->isDiscovered; }
    void SetIsDiscovered(bool isDiscovered) { this->isDiscovered = isDiscovered; }

    bool GetIsRequired() const { return this->isRequired; }
    void SetIsRequired(bool isRequired) { this->isRequired = isRequired; }

    void Update(int delta);
    void Update(int delta, Vector2 offsetVector);
    void Draw();
    void Draw(Vector2 offsetVector);

private:
    string requiredPartnerId;
    bool isDiscovered;
    bool isRequired;
};

#endif
