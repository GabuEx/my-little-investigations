/**
 * Represents a crowd of characters out in the field.
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

#include "Crowd.h"
#include "Encounter.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../XmlReader.h"
#include "../CaseInformation/Case.h"

Crowd::Crowd()
{
    name = "";
    pEncounter = NULL;
    videoId = "";
    pVideo = NULL;
    position = Vector2(0, 0);
    anchorPosition = -1;
    pZOrderLine = NULL;
    pHitBox = NULL;
    isMouseOver = false;
    isClicked = false;
}

Crowd::Crowd(XmlReader *pReader)
{
    name = "";
    pEncounter = NULL;
    videoId = "";
    pVideo = NULL;
    position = Vector2(0, 0);
    anchorPosition = -1;
    pZOrderLine = NULL;
    pHitBox = NULL;
    isMouseOver = false;
    isClicked = false;

    pReader->StartElement("Crowd");

    name = pReader->ReadTextElement("Name");
    anchorPosition = pReader->ReadIntElement("AnchorPosition");

    pReader->StartElement("Position");
    position = Vector2(pReader);
    pReader->EndElement();

    pReader->StartElement("HitBox");
    pHitBox = new HitBox(pReader);
    pReader->EndElement();

    pReader->StartElement("ClickPolygon");
    clickPolygon = GeometricPolygon(pReader);
    pReader->EndElement();

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

    pReader->StartElement("Encounter");
    pEncounter = new Encounter(pReader);
    pReader->EndElement();

    videoId = pReader->ReadTextElement("VideoId");

    pReader->EndElement();
}

Crowd::~Crowd()
{
    delete pEncounter;
    pEncounter = NULL;
    delete pZOrderLine;
    pZOrderLine = NULL;
    delete pHitBox;
    pHitBox = NULL;
}

bool Crowd::GetIsInteractionFinished()
{
    return
        (pEncounter == NULL || pEncounter->GetIsFinished());
}

void Crowd::Begin()
{
    GetVideo()->Begin();
}

void Crowd::Update(int delta)
{
    GeometricPolygon adjustedClickPolygon = clickPolygon;
    Update(delta, adjustedClickPolygon);
}

void Crowd::Update(int delta, Vector2 offsetVector)
{
    GeometricPolygon adjustedClickPolygon = clickPolygon - offsetVector;
    Update(delta, adjustedClickPolygon);
}

void Crowd::Update(int delta, GeometricPolygon adjustedClickPolygon)
{
    UpdateAnimation(delta);
    UpdateClickState(adjustedClickPolygon);
}

void Crowd::UpdateAnimation(int delta)
{
    if (!GetVideo()->IsReady())
    {
        Begin();
    }

    GetVideo()->Update(delta);
}

void Crowd::UpdateClickState(Vector2 offsetVector)
{
    GeometricPolygon adjustedClickPolygon = clickPolygon - offsetVector;
    UpdateClickState(adjustedClickPolygon);
}

void Crowd::UpdateClickState(GeometricPolygon adjustedClickPolygon)
{
    if (pEncounter != NULL &&
        (MouseHelper::MouseOverPolygon(adjustedClickPolygon) || MouseHelper::MouseDownOnPolygon(adjustedClickPolygon)) &&
        !MouseHelper::PressedAndHeldAnywhere() && !MouseHelper::DoublePressedAndHeldAnywhere())
    {
        MouseHelper::SetCursorType(CursorTypeTalk);
        MouseHelper::SetMouseOverText(GetName());
        SetIsMouseOver(true);
    }
    else
    {
        SetIsMouseOver(false);
    }

    SetIsClicked(MouseHelper::ClickedOnPolygon(adjustedClickPolygon) || MouseHelper::DoubleClickedOnPolygon(adjustedClickPolygon));
}

void Crowd::Draw()
{
    Draw(Vector2(0, 0));
}

void Crowd::Draw(Vector2 offsetVector)
{
    GetVideo()->Draw(GetPosition() - offsetVector);
}

void Crowd::Reset()
{
    GetVideo()->Reset();
    SetIsClicked(false);
}

Vector2 Crowd::GetCenterPoint()
{
    if (IsInteractionPointExact())
    {
        return interactionLocation;
    }

    RectangleWH clickPolygonBoundingBox = clickPolygon.GetBoundingBox();

    return Vector2(
        clickPolygonBoundingBox.GetX() + clickPolygonBoundingBox.GetWidth() / 2,
        clickPolygonBoundingBox.GetY() + clickPolygonBoundingBox.GetHeight() / 2);
}

RectangleWH Crowd::GetBoundsForInteraction()
{
    if (IsInteractionPointExact())
    {
        return RectangleWH(interactionLocation.GetX(), interactionLocation.GetY(), 1, 1);
    }

    return clickPolygon.GetBoundingBox();
}

bool Crowd::IsInteractionPointExact()
{
    return interactionLocation.GetX() >= 0 && interactionLocation.GetY() >= 0;
}

void Crowd::BeginInteraction(Location *pLocation)
{
    pLocation->BeginCrowdInteraction(this);
}

void Crowd::BeginInteraction()
{
    if (pEncounter != NULL)
    {
        pEncounter->Begin();
    }
}

void Crowd::UpdateInteraction(int delta)
{
    if (pEncounter != NULL && !pEncounter->GetIsFinished())
    {
        pEncounter->Update(delta);

        if (pEncounter->GetIsFinished())
        {
            transitionRequest = pEncounter->GetTransitionRequest();
        }
    }
}

void Crowd::DrawInteraction()
{
    if (pEncounter != NULL && !pEncounter->GetIsFinished())
    {
        pEncounter->Draw();
    }
}

int Crowd::GetZOrder()
{
    if (anchorPosition >= 0)
    {
        return (int)position.GetY() + anchorPosition;
    }
    else
    {
        return GetCanInteractFromAnywhere() ? -2 : -1;
    }
}

Line * Crowd::GetZOrderLine()
{
    return pZOrderLine;
}

Vector2 Crowd::GetZOrderPoint()
{
    if (anchorPosition >= 0)
    {
        return Vector2(position.GetX() + GetVideo()->GetWidth() / 2, position.GetY() + anchorPosition);
    }
    else
    {
        return Vector2(0, -1);
    }
}

Video * Crowd::GetVideo()
{
    if (pVideo == NULL)
    {
        pVideo = Case::GetInstance()->GetAnimationManager()->GetVideoFromId(videoId);
    }

    return pVideo;
}
