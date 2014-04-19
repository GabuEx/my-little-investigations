/**
 * Basic header/include file for Crowd.cpp.
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

#ifndef CROWD_H
#define CROWD_H

#include "../Collisions.h"
#include "../Interfaces.h"
#include "../Line.h"
#include "../Polygon.h"
#include "../TransitionRequest.h"
#include "../Video.h"

class Encounter;
class XmlReader;

class Crowd : public InteractiveElement, public ZOrderableObject
{
public:
    Crowd();
    Crowd(XmlReader *pReader);
    virtual ~Crowd();

    string GetName() const { return this->name; }
    void SetName(string name) { this->name = name; }

    Vector2 GetPosition() const { return this->position; }
    void SetPosition(Vector2 position) { this->position = position; }

    HitBox * GetHitBox() { return this->pHitBox; }
    void SetHitBox(HitBox *pHitBox) { this->pHitBox = pHitBox; }

    bool GetIsMouseOver() const { return this->isMouseOver; }
    void SetIsMouseOver(bool isMouseOver) { this->isMouseOver = isMouseOver; }

    bool GetIsClicked() const { return this->isClicked; }
    void SetIsClicked(bool isClicked) { this->isClicked = isClicked; }

    TransitionRequest GetTransitionRequest() const { return this->transitionRequest; }
    bool GetIsInteractionFinished();

    void Begin();

    void Update(int delta);
    void Update(int delta, Vector2 offsetVector);
    void Update(int delta, GeometricPolygon adjustedClickPolygon);

    void UpdateAnimation(int delta);
    void UpdateClickState(Vector2 offsetVector);
    void UpdateClickState(GeometricPolygon adjustedClickPolygon);

    void Draw();
    void Draw(Vector2 offsetVector);

    void Reset();

    Vector2 GetCenterPoint();
    RectangleWH GetBoundsForInteraction();
    bool IsInteractionPointExact();
    void BeginInteraction(Location *pLocation);

    void BeginInteraction();
    void UpdateInteraction(int delta);
    void DrawInteraction();

    int GetZOrder();

    bool IsVisible()
    {
        return true;
    }

    Line * GetZOrderLine();
    Vector2 GetZOrderPoint();

private:
    Video * GetVideo();

    string name;
    GeometricPolygon clickPolygon;

    Encounter *pEncounter;
    string videoId;
    Video *pVideo;

    Vector2 position;
    int anchorPosition;
    Line *pZOrderLine;

    HitBox *pHitBox;

    bool isMouseOver;
    bool isClicked;
    Vector2 interactionLocation;
    TransitionRequest transitionRequest;
};

#endif
