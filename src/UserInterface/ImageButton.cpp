/**
 * A button containing mouse-over, mouse-down, and mouse-off images,
 * cycles between them according to mouse input, and detects clicks.
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

#include "ImageButton.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../Events/ImageButtonEventProvider.h"

ImageButton::ImageButton(Image *pMouseOffImage, Image *pMouseOverImage, Image *pMouseDownImage, int xPosition, int yPosition)
{
    this->pMouseOffImage = pMouseOffImage;
    this->pMouseOverImage = pMouseOverImage;
    this->pMouseDownImage = pMouseDownImage;

    this->xPosition = xPosition;
    this->yPosition = yPosition;

    this->isEnabled = true;
    this->clickSoundEffect = "ButtonClick1";

    Reset();
}

ImageButton::~ImageButton()
{
    delete pMouseOffImage;
    pMouseOffImage = NULL;
    delete pMouseOverImage;
    pMouseOverImage = NULL;
    delete pMouseDownImage;
    pMouseDownImage = NULL;
}

void ImageButton::Update(int delta)
{
    isMouseOver = false;
    isMouseDown = false;

    if (isEnabled)
    {
        Image *pCurrentImage = GetCurrentSprite();

        RectangleWH positionRect = RectangleWH(GetXPosition(), GetYPosition(), pCurrentImage->width, pCurrentImage->height);
        bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

        if (MouseHelper::ClickedOnRect(positionRect))
        {
            OnClicked();
        }

        if (MouseHelper::MouseDownOnRect(positionRect) && !isPressed)
        {
            isMouseDown = true;
        }
        else if (MouseHelper::MouseOverRect(positionRect) && !isPressed)
        {
            isMouseOver = true;
        }
    }
}

void ImageButton::Draw()
{
    Draw(1.0);
}

void ImageButton::Draw(Vector2 offset)
{
    Draw(offset, 1.0);
}

void ImageButton::Draw(double opacity)
{
    Draw(Vector2(0, 0), opacity);
}

void ImageButton::Draw(Vector2 offset, double opacity)
{
    double overlayColor = isEnabled ? 1.0 : 0.5;
    GetCurrentSprite()->Draw(Vector2(GetXPosition(), GetYPosition()) + offset, Color(opacity, overlayColor, overlayColor, overlayColor));
}

void ImageButton::Reset()
{
    isMouseOver = false;
    isMouseDown = false;
}

bool ImageButton::IsReady()
{
    return pMouseOffImage->IsReady() && pMouseOverImage->IsReady() && pMouseDownImage->IsReady();
}

Image * ImageButton::GetCurrentSprite()
{
    if (isMouseDown)
    {
        return pMouseDownImage;
    }
    else if (isMouseOver)
    {
        return pMouseOverImage;
    }
    else
    {
        return pMouseOffImage;
    }
}

void ImageButton::OnClicked()
{
    EventProviders::GetImageButtonEventProvider()->RaiseClicked(this);
    playSound(GetClickSoundEffect());
}
