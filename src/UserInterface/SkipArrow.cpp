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
#include "../CaseInformation/CommonCaseResources.h"

const int ArrowSeparation = 2; // px

const Color NormalColor = Color(1.0, 1.0, 1.0, 1.0);
const Color MouseOverColor = Color(1.0, 1.0, 1.0, 0.0);
const Color MouseDownColor = Color(1.0, 1.0, 0.0, 0.0);
const Color DisabledColor = Color(1.0, 0.5, 0.5, 0.5);

Image *SkipArrow::pArrowImage = NULL;
Image *SkipArrow::pArrowImageInverted = NULL;
MLIFont *SkipArrow::pFont = NULL;
MLIFont *SkipArrow::pInvertedFont = NULL;

void SkipArrow::Initialize(Image *pArrowImage,
        Image *pArrowImageInverted)
{
    SkipArrow::pArrowImage = pArrowImage;
    SkipArrow::pArrowImageInverted = pArrowImageInverted;

    SkipArrow::pFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("SkipArrow/Font");
    SkipArrow::pInvertedFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("SkipArrow/InvertedFont");
}

SkipArrow::SkipArrow(int xPosition, int yPosition, int bounceDistance, bool isClickable, bool isFFwd)
{
    this->xPosition = xPosition;
    this->yPosition = yPosition;

    textWidget.SetX(xPosition);
    textWidget.SetY(yPosition);

    textWidget.SetText(isFFwd ? pgLocalizableContent->GetText("SkipArrow/FastForwardText") : pgLocalizableContent->GetText("SkipArrow/SkipText"));
    textWidget.SetFont(pFont);
    textWidget.SetTextColor(NormalColor);
    textWidget.FitSizeToContent();

    width = (pArrowImage->width + ArrowSeparation + textWidget.GetWidth());
    height = max(pArrowImage->height, (Uint16)textWidget.GetHeight());

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

    textWidget = other.textWidget;

    width = other.width;
    height = other.height;

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
    UpdateTextWidget();
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

    textWidget.Draw();
    Image *pArrowSprite = isDown ? pArrowImageInverted : pArrowImage;
    pArrowSprite->Draw(Vector2(position.GetX() + textWidget.GetWidth() + ArrowSeparation + (GetIsEnabled() ? arrowOffset : 0), position.GetY() + hitboxRect.GetHeight() / 2 - pArrowSprite->height / 2), false /* flipHorizontally */, false /* flipVertically */, color);
}

void SkipArrow::Reset()
{
    isMouseOver = false;
    isMouseDown = false;
    SetIsClicked(false);
    isDown = false;
}

void SkipArrow::UpdateTextWidget()
{
    textWidget.SetFont(isDown ? pInvertedFont : pFont);
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
    textWidget.SetTextColor(color);
}
