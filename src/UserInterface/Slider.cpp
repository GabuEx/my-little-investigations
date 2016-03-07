/**
 * A slider that can be moved from a minimum and maximum value by
 * clicking and dragging the thumb to the position desired.
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

#include "Slider.h"
#include "../MouseHelper.h"

MLIFont *Slider::pHeaderFont = NULL;
Image *Slider::pTrackSprite = NULL;
Image *Slider::pThumbSprite = NULL;

const Vector2 TrackPositionOffset = Vector2(17, 38);

void Slider::Initialize(MLIFont *pHeaderFont, Image *pTrackSprite, Image *pThumbSprite)
{
    Slider::pHeaderFont = pHeaderFont;
    Slider::pTrackSprite = pTrackSprite;
    Slider::pThumbSprite = pThumbSprite;
}

Slider::Slider(const string &headerTextId, int xPosition, int yPosition)
{
    this->headerTextId = headerTextId;
    this->xPosition = xPosition;
    this->yPosition = yPosition;

    this->sliderXPosition = 0;
    this->currentValue = 0;

    ReloadLocalizableText();
    Reset();

    gpLocalizableContent->AddLocalizableTextOwner(this);
}

Slider::~Slider()
{
    gpLocalizableContent->RemoveLocalizableTextOwner(this);
}

int Slider::GetTrackXPosition()
{
    return (int)(GetXPosition() + TrackPositionOffset.GetX());
}

int Slider::GetTrackYPosition()
{
    return (int)(GetYPosition() + TrackPositionOffset.GetY());
}

void Slider::SetCurrentValue(double currentValue)
{
    this->currentValue = currentValue >= 0.0 ? (currentValue <= 1.0 ? currentValue : 1.0) : 0.0;
    this->sliderXPosition = (int)(currentValue * pTrackSprite->width);
}

void Slider::Update(int delta)
{
    RectangleWH positionRect = RectangleWH(GetTrackXPosition(), GetTrackYPosition(), pTrackSprite->width, pThumbSprite->height);
    bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

    if (!isDragging && MouseHelper::MouseDownOnRect(positionRect) && isPressed)
    {
        isDragging = true;
    }
    else if (isDragging && !isPressed)
    {
        isDragging = false;
    }

    if (isDragging)
    {
        sliderXPosition = (int)(MouseHelper::GetMousePosition().GetX() - GetTrackXPosition());

        if (sliderXPosition < 0)
        {
            sliderXPosition = 0;
        }
        else if (sliderXPosition > pTrackSprite->width)
        {
            sliderXPosition = pTrackSprite->width;
        }

        currentValue = (double)sliderXPosition / pTrackSprite->width;
    }
}

void Slider::Draw()
{
    Draw(1.0);
}

void Slider::Draw(double opacity)
{
    pHeaderFont->Draw(headerText, Vector2(GetXPosition(), GetYPosition()), Color(opacity, 0.0, 0.0, 0.0));
    pTrackSprite->Draw(Vector2(GetTrackXPosition(), GetTrackYPosition() + pThumbSprite->height / 2 - pTrackSprite->height / 2), Color(opacity, 1.0, 1.0, 1.0));
    pThumbSprite->Draw(Vector2(GetTrackXPosition() + sliderXPosition - pThumbSprite->width / 2, GetTrackYPosition()), Color(opacity, 1.0, 1.0, 1.0));
}

void Slider::Reset()
{
    isDragging = false;
}

void Slider::ReloadLocalizableText()
{
    headerText = gpLocalizableContent->GetText(headerTextId);
}
