/**
 * Handles a single clickable arrow.
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

#include "Arrow.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"

const Color NormalColor = Color(1.0, 1.0, 1.0, 1.0);
const Color MouseOverColor = Color(1.0, 1.0, 1.0, 0.0);
const Color MouseDownColor = Color(1.0, 1.0, 0.0, 0.0);
const Color DisabledColor = Color(1.0, 0.5, 0.5, 0.5);

Image *Arrow::pHorizontalImage = NULL;
Image *Arrow::pVerticalImage = NULL;

void Arrow::Initialize(Image *pHorizontalImage, Image *pVerticalImage)
{
    Arrow::pHorizontalImage = pHorizontalImage;
    Arrow::pVerticalImage = pVerticalImage;
}

Arrow::Arrow(int xPosition, int yPosition, ArrowDirection direction, int bounceDistance, bool isClickable)
{
    this->xPosition = xPosition;
    this->yPosition = yPosition;
    this->direction = direction;

    if (direction == ArrowDirectionUp || direction == ArrowDirectionDown)
    {
        width = pVerticalImage->width;
        height = pVerticalImage->height;
    }
    else
    {
        width = pHorizontalImage->width;
        height = pHorizontalImage->height;
    }

    this->bounceDistance = bounceDistance;

    hitboxRect = RectangleWH(xPosition, yPosition, width, height);

    if (direction == ArrowDirectionUp)
    {
        hitboxRect.SetX(hitboxRect.GetX() - 5);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + 10);
        hitboxRect.SetY(hitboxRect.GetY() - bounceDistance);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + bounceDistance);
    }
    else if (direction == ArrowDirectionDown)
    {
        hitboxRect.SetX(hitboxRect.GetX() - 5);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + 10);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + bounceDistance);
    }
    else if (direction == ArrowDirectionLeft)
    {
        hitboxRect.SetY(hitboxRect.GetY() - 5);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + 10);
        hitboxRect.SetX(hitboxRect.GetY() - bounceDistance);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + bounceDistance);
    }
    else if (direction == ArrowDirectionRight)
    {
        hitboxRect.SetY(hitboxRect.GetY() - 5);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + 10);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + bounceDistance);
    }

    this->isClickable = isClickable;
    isEnabled = true;

    arrowOffset = 0;
    pArrowOffsetEase = new GravityBounceEase(
            0,
            direction == ArrowDirectionUp || direction == ArrowDirectionLeft ? -bounceDistance : bounceDistance,
            300);

    isMouseDown = false;
    isMouseOver = false;
    isClicked = false;
}

Arrow::Arrow(const Arrow &other)
{
    this->xPosition = other.xPosition;
    this->yPosition = other.yPosition;
    this->direction = other.direction;

    if (direction == ArrowDirectionUp || direction == ArrowDirectionDown)
    {
        width = pVerticalImage->width;
        height = pVerticalImage->height;
    }
    else
    {
        width = pHorizontalImage->width;
        height = pHorizontalImage->height;
    }

    this->bounceDistance = other.bounceDistance;

    hitboxRect = RectangleWH(xPosition, yPosition, width, height);

    if (direction == ArrowDirectionUp)
    {
        hitboxRect.SetX(hitboxRect.GetX() - 5);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + 10);
        hitboxRect.SetY(hitboxRect.GetY() - bounceDistance);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + bounceDistance);
    }
    else if (direction == ArrowDirectionDown)
    {
        hitboxRect.SetX(hitboxRect.GetX() - 5);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + 10);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + bounceDistance);
    }
    else if (direction == ArrowDirectionLeft)
    {
        hitboxRect.SetY(hitboxRect.GetY() - 5);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + 10);
        hitboxRect.SetX(hitboxRect.GetY() - bounceDistance);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + bounceDistance);
    }
    else if (direction == ArrowDirectionRight)
    {
        hitboxRect.SetY(hitboxRect.GetY() - 5);
        hitboxRect.SetHeight(hitboxRect.GetHeight() + 10);
        hitboxRect.SetWidth(hitboxRect.GetWidth() + bounceDistance);
    }

    this->isClickable = other.isClickable;
    isEnabled = true;

    arrowOffset = 0;
    pArrowOffsetEase = new GravityBounceEase(
            0,
            direction == ArrowDirectionUp || direction == ArrowDirectionLeft ? -bounceDistance : bounceDistance,
            300);

    isMouseDown = false;
    isMouseOver = false;
    isClicked = false;
}

void Arrow::Update(int delta)
{
    UpdateAnimation(delta);
    UpdateState();
}

void Arrow::UpdateAnimation(int delta)
{
    if (!pArrowOffsetEase->GetIsStarted() || pArrowOffsetEase->GetIsFinished())
    {
        pArrowOffsetEase->Begin();
    }

    pArrowOffsetEase->Update(delta);
    arrowOffset = pArrowOffsetEase->GetCurrentValue();
}

void Arrow::UpdateState()
{
    if (isClickable && GetIsEnabled())
    {
        bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

        isMouseOver = MouseHelper::MouseOverRect(hitboxRect) && !isPressed;
        isMouseDown = MouseHelper::MouseDownOnRect(hitboxRect) && !isPressed;
        SetIsClicked(MouseHelper::ClickedOnRect(hitboxRect));

        if (GetIsClicked())
        {
            playSound(GetClickSoundEffect());
        }
    }
    else
    {
        Reset();
    }
}

void Arrow::Draw()
{
    Draw(0);
}

void Arrow::Draw(double xOffset)
{
    Draw(xOffset, 0);
}

void Arrow::Draw(double xOffset, double yOffset)
{
    Color color;

    if (!GetIsEnabled() && isClickable)
    {
        color = DisabledColor;
    }
    else if (isMouseDown)
    {
        color = MouseDownColor;
    }
    else if (isMouseOver)
    {
        color = MouseOverColor;
    }
    else
    {
        color = NormalColor;
    }

    Vector2 position = Vector2(xPosition + xOffset, yPosition + yOffset);

    if (GetIsEnabled())
    {
        if (direction == ArrowDirectionUp || direction == ArrowDirectionDown)
        {
            position.SetY(position.GetY() + arrowOffset);
        }
        else if (direction == ArrowDirectionLeft || direction == ArrowDirectionRight)
        {
            position.SetX(position.GetX() + arrowOffset);
        }
    }

    bool flipVertically = false;
    bool flipHorizontally = false;

    if (direction == ArrowDirectionUp)
    {
        flipVertically = true;
    }
    else if (direction == ArrowDirectionLeft)
    {
        flipHorizontally = true;
    }

    Image *pImage = (direction == ArrowDirectionUp || direction == ArrowDirectionDown) ? pVerticalImage : pHorizontalImage;
    pImage->Draw(position, flipHorizontally, flipVertically, color);
}

void Arrow::Reset()
{
    isMouseOver = false;
    isMouseDown = false;
    SetIsClicked(false);
}
