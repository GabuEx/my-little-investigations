/**
 * Handles a clickable arrow to handle skipping and fast-forwarding.
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

#include "SkipArrow.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"

const int ArrowSeparation = 2; // px

const Color NormalColor = Color(1.0, 1.0, 1.0, 1.0);
const Color MouseOverColor = Color(1.0, 1.0, 1.0, 0.0);
const Color MouseDownColor = Color(1.0, 1.0, 0.0, 0.0);
const Color DisabledColor = Color(1.0, 0.5, 0.5, 0.5);

Image *SkipArrow::pArrowImage = NULL;
Image *SkipArrow::pArrowImageInverted = NULL;
Image *SkipArrow::pFFwdImage = NULL;
Image *SkipArrow::pSkipImage = NULL;
Image *SkipArrow::pFFwdImageInverted = NULL;
Image *SkipArrow::pSkipImageInverted = NULL;

void SkipArrow::Initialize(
        Image *pArrowImage,
        Image *pArrowImageInverted,
        Image *pFFwdImage,
        Image *pSkipImage,
        Image *pFFwdImageInverted,
        Image *pSkipImageInverted)
{
    SkipArrow::pArrowImage = pArrowImage;
    SkipArrow::pArrowImageInverted = pArrowImageInverted;
    SkipArrow::pFFwdImage = pFFwdImage;
    SkipArrow::pSkipImage = pSkipImage;
    SkipArrow::pFFwdImageInverted = pFFwdImageInverted;
    SkipArrow::pSkipImageInverted = pSkipImageInverted;
}

SkipArrow::SkipArrow(int xPosition, int yPosition, int bounceDistance, bool isClickable, bool isFFwd)
{
    this->xPosition = xPosition;
    this->yPosition = yPosition;

    Image *pTextSprite = isFFwd ? pFFwdImage : pSkipImage;

    width = (pArrowImage->width + ArrowSeparation + pTextSprite->width);
    height = max(pArrowImage->height, pTextSprite->height);

    this->bounceDistance = bounceDistance;

    hitboxRect = RectangleWH(xPosition, yPosition, width + bounceDistance, height);

    this->isClickable = isClickable;
    this->isFFwd = isFFwd;
    isEnabled = true;

    arrowOffset = 0;
    pArrowOffsetEase = new GravityBounceEase(0, bounceDistance, 300);

    Reset();
}

SkipArrow::SkipArrow(const SkipArrow& other)
{
    xPosition = other.xPosition;
    yPosition = other.yPosition;
    bounceDistance = other.bounceDistance;

    hitboxRect = RectangleWH(xPosition, yPosition, width + bounceDistance, height);

    isClickable = other.isClickable;
    isFFwd = other.isFFwd;
    isEnabled = true;

    Image *pTextSprite = isFFwd ? pFFwdImage : pSkipImage;

    width = (pArrowImage->width + ArrowSeparation + pTextSprite->width);
    height = max(pArrowImage->height, pTextSprite->height);

    arrowOffset = 0;
    pArrowOffsetEase = new GravityBounceEase(0, bounceDistance, 300);

    Reset();
}

void SkipArrow::Update(int delta)
{
    if (!pArrowOffsetEase->GetIsStarted() || pArrowOffsetEase->GetIsFinished())
    {
        pArrowOffsetEase->Begin();
    }

    pArrowOffsetEase->Update(delta);
    arrowOffset = pArrowOffsetEase->GetCurrentValue();

    if (isClickable && GetIsEnabled())
    {
        bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

        isMouseOver = MouseHelper::MouseOverRect(hitboxRect) && !isPressed;
        isMouseDown = MouseHelper::MouseDownOnRect(hitboxRect) && !isPressed;
        SetIsClicked(MouseHelper::ClickedOnRect(hitboxRect));

        if (GetIsClicked())
        {
            playSound(GetClickSoundEffect());
            isDown = !isDown;

            MouseHelper::HandleClick();
        }
    }
    else
    {
        Reset();
    }
}

void SkipArrow::Draw()
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

    Vector2 position = Vector2(xPosition, yPosition);

    Image *pTextSprite = isFFwd ? (isDown ? pFFwdImageInverted : pFFwdImage) : (isDown ? pSkipImageInverted : pSkipImage);
    Image *pArrowSprite = isDown ? pArrowImageInverted : pArrowImage;

    pTextSprite->Draw(Vector2(position.GetX(), position.GetY() + hitboxRect.GetHeight() / 2 - pTextSprite->height / 2), false /* flipHorizontally */, false /* flipVertically */, color);
    pArrowSprite->Draw(Vector2(position.GetX() + pTextSprite->width + ArrowSeparation + (GetIsEnabled() ? arrowOffset : 0), position.GetY() + hitboxRect.GetHeight() / 2 - pArrowSprite->height / 2), false /* flipHorizontally */, false /* flipVertically */, color);
}

void SkipArrow::Reset()
{
    isMouseOver = false;
    isMouseDown = false;
    SetIsClicked(false);
    isDown = false;
}
