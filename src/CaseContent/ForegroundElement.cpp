/**
 * Represents an object in the field.
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

#include "ForegroundElement.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../XmlReader.h"
#include "../CaseInformation/Case.h"

ForegroundElement::ForegroundElement(XmlReader *pReader)
{
    pReader->StartElement("ForegroundElement");

    LoadFromXmlCore(pReader);

    pReader->EndElement();
}

ForegroundElement::~ForegroundElement()
{
    for (unsigned int i = 0; i < foregroundElementAnimationList.size(); i++)
    {
        delete foregroundElementAnimationList[i];
    }

    delete pHitBox;
    pHitBox = NULL;
    delete pEncounter;
    pEncounter = NULL;
    delete pZOrderLine;
    pZOrderLine = NULL;
    delete pCondition;
    pCondition = NULL;
}

bool ForegroundElement::GetIsInteractive() const
{
    return pEncounter != NULL || zoomedViewId.length() > 0;
}

bool ForegroundElement::GetIsInteractionFinished()
{
    return
        (pEncounter == NULL || pEncounter->GetIsFinished());
}

void ForegroundElement::Begin()
{
    for (unsigned int i = 0; i < foregroundElementAnimationList.size(); i++)
    {
        foregroundElementAnimationList[i]->Begin();
    }

    transitionRequest = TransitionRequest();
}

void ForegroundElement::Update(int delta)
{
    Update(delta, Vector2(0, 0));
}

void ForegroundElement::Update(int delta, Vector2 offsetVector)
{
    GeometricPolygon adjustedClickPolygon = clickPolygon - offsetVector;
    Update(delta, adjustedClickPolygon);
}

void ForegroundElement::Update(int delta, GeometricPolygon adjustedClickPolygon)
{
    UpdateAnimation(delta);
    UpdateClickState(adjustedClickPolygon);
}

void ForegroundElement::UpdateAnimation(int delta)
{
    for (unsigned int i = 0; i < foregroundElementAnimationList.size(); i++)
    {
        foregroundElementAnimationList[i]->Update(delta);
    }
}

void ForegroundElement::UpdateClickState(Vector2 offsetVector)
{
    GeometricPolygon adjustedClickPolygon = clickPolygon - offsetVector;
    UpdateClickState(adjustedClickPolygon);
}

void ForegroundElement::UpdateClickState(GeometricPolygon adjustedClickPolygon)
{
    if ((MouseHelper::MouseOverPolygon(adjustedClickPolygon) || MouseHelper::MouseDownOnPolygon(adjustedClickPolygon)) &&
        !MouseHelper::PressedAndHeldAnywhere() && !MouseHelper::DoublePressedAndHeldAnywhere())
    {
        MouseHelper::SetCursorType(CursorTypeLook);
        MouseHelper::SetMouseOverText(GetName());
        SetIsMouseOver(true);
    }
    else
    {
        SetIsMouseOver(false);
    }

    SetIsClicked(MouseHelper::ClickedOnPolygon(adjustedClickPolygon) || MouseHelper::DoubleClickedOnPolygon(adjustedClickPolygon));
}

void ForegroundElement::Draw()
{
    Draw(Vector2(0, 0));
}

void ForegroundElement::Draw(Vector2 offsetVector)
{
    Vector2 screenPosition = position - offsetVector;

    if (spriteId.length() > 0)
    {
        GetSprite()->Draw(screenPosition);
    }

    for (unsigned int i = 0; i < foregroundElementAnimationList.size(); i++)
    {
        foregroundElementAnimationList[i]->Draw(screenPosition);
    }
}

void ForegroundElement::BeginInteraction()
{
    if (pEncounter != NULL)
    {
        pEncounter->Begin();
    }
}

void ForegroundElement::UpdateInteraction(int delta)
{
    if (pEncounter != NULL && !pEncounter->GetIsFinished())
    {
        pEncounter->Update(delta);

        if (pEncounter->GetIsFinished())
        {
            transitionRequest = pEncounter->GetTransitionRequest();
        }
    }

    if ((pEncounter == NULL || pEncounter->GetIsFinished()) && zoomedViewId.length() > 0 && transitionRequest.GetTransitionDestination() == TransitionDestinationNone)
    {
        transitionRequest = TransitionRequest(zoomedViewId);
    }
}

void ForegroundElement::DrawInteraction()
{
    if (pEncounter != NULL && !pEncounter->GetIsFinished())
    {
        pEncounter->Draw();
    }
}

Vector2 ForegroundElement::GetInexactCenterPoint()
{
    RectangleWH clickPolygonBoundingBox = clickPolygon.GetBoundingBox();

    return Vector2(
        clickPolygonBoundingBox.GetX() + clickPolygonBoundingBox.GetWidth() / 2,
        clickPolygonBoundingBox.GetY() + clickPolygonBoundingBox.GetHeight() / 2);
}

Vector2 ForegroundElement::GetCenterPoint()
{
    if (IsInteractionPointExact())
    {
        return interactionLocation;
    }

    return GetInexactCenterPoint();
}

RectangleWH ForegroundElement::GetBoundsForInteraction()
{
    if (IsInteractionPointExact())
    {
        return RectangleWH(interactionLocation.GetX(), interactionLocation.GetY(), 1, 1);
    }

    return clickPolygon.GetBoundingBox();
}

bool ForegroundElement::IsInteractionPointExact()
{
    return interactionLocation.GetX() >= 0 && interactionLocation.GetY() >= 0;
}

void ForegroundElement::BeginInteraction(Location *pLocation)
{
    pLocation->BeginForegroundElementInteraction(this);
}

int ForegroundElement::GetZOrder()
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

bool ForegroundElement::IsVisible()
{
    return spriteId.length() > 0 && IsPresent();
}

bool ForegroundElement::IsPresent()
{
    return pCondition != NULL ? pCondition->IsTrue() : true;
}

Line * ForegroundElement::GetZOrderLine()
{
    return pZOrderLine;
}

Vector2 ForegroundElement::GetZOrderPoint()
{
    if (anchorPosition >= 0)
    {
        return Vector2(position.GetX() + GetSprite()->GetWidth() / 2, position.GetY() + anchorPosition);
    }
    else
    {
        return Vector2(0, -1);
    }
}

void ForegroundElement::LoadFromXmlCore(XmlReader *pReader)
{
    pEncounter = NULL;
    zoomedViewId = "";
    pZOrderLine = NULL;
    pSprite = NULL;
    pHitBox = NULL;
    isMouseOver = false;
    isClicked = false;
    pCondition = NULL;

    if (pReader->ElementExists("Name"))
    {
        name = pReader->ReadTextElement("Name");
    }

    if (pReader->ElementExists("SpriteId"))
    {
        spriteId = pReader->ReadTextElement("SpriteId");
    }

    pReader->StartElement("Position");
    position = Vector2(pReader);
    pReader->EndElement();

    anchorPosition = pReader->ReadIntElement("AnchorPosition");

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

    if (pReader->ElementExists("HitBox"))
    {
        pReader->StartElement("HitBox");
        pHitBox = new HitBox(pReader);
        pReader->EndElement();
    }

    pReader->StartElement("ClickPolygon");
    clickPolygon = GeometricPolygon(pReader);
    pReader->EndElement();

    if (pReader->ElementExists("Encounter"))
    {
        pReader->StartElement("Encounter");
        pEncounter = new Encounter(pReader);
        pReader->EndElement();
    }

    if (pReader->ElementExists("ZoomedViewId"))
    {
        zoomedViewId = pReader->ReadTextElement("ZoomedViewId");
    }

    pReader->StartElement("Animations");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        foregroundElementAnimationList.push_back(new ForegroundElementAnimation(pReader));
    }

    pReader->EndElement();

    if (pReader->ElementExists("ZOrderLine"))
    {
        pReader->StartElement("ZOrderLine");
        pZOrderLine = new Line(pReader);
        pReader->EndElement();
    }

    if (pReader->ElementExists("Condition"))
    {
        pReader->StartElement("Condition");
        pCondition = new Condition(pReader);
        pReader->EndElement();
    }
}

Sprite * ForegroundElement::GetSprite()
{
    if (pSprite == NULL && spriteId.length() > 0)
    {
        pSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(spriteId);
    }

    return pSprite;
}

ForegroundElementAnimation::ForegroundElementAnimation(XmlReader *pReader)
{
    pAnimation = NULL;

    pReader->StartElement("ForegroundElementAnimation");
    animationId = pReader->ReadTextElement("AnimationId");
    pReader->StartElement("Position");
    position = Vector2(pReader);
    pReader->EndElement();
    pReader->EndElement();
}

void ForegroundElementAnimation::Begin()
{
    GetAnimation()->Begin();
}

void ForegroundElementAnimation::Update(int delta)
{
    GetAnimation()->Update(delta);
}

void ForegroundElementAnimation::Draw(Vector2 offsetVector)
{
    GetAnimation()->Draw(position + offsetVector, false /* flipHorizontally */, 1.0);
}

Animation * ForegroundElementAnimation::GetAnimation()
{
    if (pAnimation == NULL)
    {
        pAnimation = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId(animationId);
    }

    return pAnimation;
}

HiddenForegroundElement::HiddenForegroundElement(XmlReader *pReader)
{
    isDiscovered = false;

    pReader->StartElement("HiddenForegroundElement");

    LoadFromXmlCore(pReader);
    requiredPartnerId = pReader->ReadTextElement("RequiredPartnerId");
    isRequired = pReader->ReadBooleanElement("IsRequired");

    pReader->EndElement();
}

void HiddenForegroundElement::Update(int delta)
{
    Update(delta, Vector2(0, 0));
}

void HiddenForegroundElement::Update(int delta, Vector2 offsetVector)
{
    if (GetIsDiscovered())
    {
        ForegroundElement::Update(delta, offsetVector);
    }
    else
    {
        MouseHelper::SetMouseOverText("?");
        SetIsClicked(MouseHelper::ClickedAnywhere() || MouseHelper::DoubleClickedAnywhere() || KeyboardHelper::ClickPressed());

        if (GetIsClicked())
        {
            SetIsDiscovered(true);
        }
    }
}

void HiddenForegroundElement::Draw()
{
    if (GetIsDiscovered())
    {
        ForegroundElement::Draw();
    }
}

void HiddenForegroundElement::Draw(Vector2 offsetVector)
{
    if (GetIsDiscovered())
    {
        ForegroundElement::Draw(offsetVector);
    }
}
