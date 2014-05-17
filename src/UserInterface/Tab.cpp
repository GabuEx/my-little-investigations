/**
 * A clickable tab appearing either at the bottom or top, or at the top of the dialog box.
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

#include "Tab.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../ResourceLoader.h"
#include "../CaseInformation/CommonCaseResources.h"

const Color NormalTextColor = Color(1.0, 1.0, 1.0, 1.0);
const Color MouseOverTextColor = Color(1.0, 1.0, 1.0, 0.0);
const Color MouseDownTextColor = Color(1.0, 1.0, 0.0, 0.0);
const Color DisabledTextColor = Color(1.0, 0.5, 0.5, 0.5);

const int HideAnimationDuration = 300; // ms
const int HideDistance = TabHeight; // ms

int Tab::dialogRowYPosition = 0;
int Tab::bottomRowYPosition = 0;
Image *Tab::pDownSprite = NULL;
Image *Tab::pUpSprite = NULL;
int Tab::width = 0;
int Tab::height = 0;
int Tab::textAreaWidth = 0;
int Tab::textAreaHeight = 0;
MLIFont *Tab::pFont = NULL;

int Tab::pulseAnimationCount = 0;

const string pulseSpriteSheetId = "__TabPulseSpriteSheet";
const string pulseSpriteIdFormat = "__TabPulseSprite%0d";

void Tab::Initialize(int dialogRowYPosition, int bottomRowYPosition, Image *pDownSprite, Image *pUpSprite, MLIFont *pFont)
{
    Tab::dialogRowYPosition = dialogRowYPosition;
    Tab::bottomRowYPosition = bottomRowYPosition;
    Tab::pDownSprite = pDownSprite;
    Tab::pUpSprite = pUpSprite;
    Tab::width = pDownSprite->width;
    Tab::height = pDownSprite->height;
    Tab::textAreaWidth = pDownSprite->width - 6;
    Tab::textAreaHeight = pDownSprite->height - 6;
    Tab::pFont = pFont;

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddImage(pulseSpriteSheetId, ResourceLoader::GetInstance()->LoadImage("image/TabPulseAnimationSpriteSheet.png"));

    for (unsigned int i = 0; i < 33; i += 5)
    {
        int y = i / 5;

        for (unsigned int x = 0; x < 5 && i + x < 33; x++)
        {
            char spriteId[256];
            sprintf(spriteId, pulseSpriteIdFormat.c_str(), i + x);

            CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(string(spriteId), pulseSpriteSheetId, RectangleWH(x * 217, y * 72, 217, 72));
        }
    }
}

Tab::Tab()
{
    Init(0, false, "", false, TabRowDialog, false);
}

Tab::Tab(int xPosition, bool isClickable)
{
    Init(xPosition, isClickable, "", false, TabRowDialog, false);
}

Tab::Tab(int xPosition, bool isClickable, const string &text)
{
    Init(xPosition, isClickable, text, false, TabRowDialog, false);
}

Tab::Tab(int xPosition, bool isClickable, const string &text, bool useCancelClickSoundEffect)
{
    Init(xPosition, isClickable, text, useCancelClickSoundEffect, TabRowDialog, false);
}

Tab::Tab(int xPosition, bool isClickable, const string &text, bool useCancelClickSoundEffect, TabRow row)
{
    Init(xPosition, isClickable, text, useCancelClickSoundEffect, row, false);
}

Tab::Tab(int xPosition, bool isClickable, const string &text, bool useCancelClickSoundEffect, TabRow row, bool canPulse)
{
    Init(xPosition, isClickable, text, useCancelClickSoundEffect, row, canPulse);
}

Tab::~Tab()
{
    delete pTopRowHideEase;
    pTopRowHideEase = NULL;
    delete pTopRowShowEase;
    pTopRowShowEase = NULL;
    delete pBottomRowHideEase;
    pBottomRowHideEase = NULL;
    delete pBottomRowShowEase;
    pBottomRowShowEase = NULL;
}

void Tab::SetIsHidden(bool isHidden, bool shouldAnimate)
{
    if (this->isHidden == isHidden)
    {
        return;
    }

    this->isHidden = isHidden;

    if (!shouldAnimate)
    {
        pCurrentEasingFunction = NULL;
        return;
    }

    if (isHidden)
    {
        switch (row)
        {
            case TabRowTop:
                pCurrentEasingFunction = pTopRowHideEase;
                break;

            case TabRowDialog:
                pCurrentEasingFunction = pBottomRowHideEase;
                break;

            case TabRowBottom:
                pCurrentEasingFunction = pBottomRowHideEase;
                break;
        }
    }
    else
    {
        switch (row)
        {
            case TabRowTop:
                pCurrentEasingFunction = pTopRowShowEase;
                break;

            case TabRowDialog:
                pCurrentEasingFunction = pBottomRowShowEase;
                break;

            case TabRowBottom:
                pCurrentEasingFunction = pBottomRowShowEase;
                break;
        }
    }

    if (pCurrentEasingFunction != NULL)
    {
        pCurrentEasingFunction->Begin();
    }

    if (!isHidden && pPulseAnimation != NULL)
    {
        pPulseAnimation->Begin();
    }
}

void Tab::SetIsPulsing(bool isPulsing)
{
    if (this->isPulsing == isPulsing)
    {
        return;
    }

    this->isPulsing = isPulsing;

    if (isPulsing)
    {
        pPulseAnimation->Begin();
    }
}

string Tab::GetClickSoundEffect()
{
    if (clickSoundEffect.length() == 0)
    {
        clickSoundEffect = (GetUseCancelClickSoundEffect() ? "ButtonClick4" : "ButtonClick3");
    }

    return clickSoundEffect;
}

void Tab::UpdatePosition(int delta)
{
    if (pCurrentEasingFunction != NULL)
    {
        pCurrentEasingFunction->Update(delta);
        hideAnimationOffset = (int)pCurrentEasingFunction->GetCurrentValue();

        if (pCurrentEasingFunction->GetIsFinished())
        {
            pCurrentEasingFunction = NULL;
        }
    }
    else if (!isHidden)
    {
        hideAnimationOffset = 0;
    }
    else
    {
        switch (row)
        {
            case TabRowTop:
                hideAnimationOffset = -HideDistance;
                break;

            case TabRowDialog:
                hideAnimationOffset = HideDistance;
                break;

            case TabRowBottom:
                hideAnimationOffset = HideDistance;
                break;
        }
    }
}

void Tab::Update()
{
    Update(-1);
}

void Tab::Update(int delta)
{
    if (isClickable && GetIsEnabled() && pCurrentEasingFunction == NULL && !isHidden)
    {
        bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

        isMouseOver = MouseHelper::MouseOverRect(areaRect) && !isPressed;
        isMouseDown = MouseHelper::MouseDownOnRect(areaRect) && !isPressed;
        SetIsClicked(MouseHelper::ClickedOnRect(areaRect));

        if (GetIsClicked())
        {
            playSound(GetClickSoundEffect());
        }
    }
    else
    {
        isMouseOver = false;
        isMouseDown = false;
        SetIsClicked(false);
    }

    if (GetShouldShowPulse())
    {
        if (!wasPulsing)
        {
            playSound("TabPulse");
        }

        if (delta >= 0)
        {
            pPulseAnimation->Update(delta);
        }

        wasPulsing = true;
    }
    else
    {
        wasPulsing = false;
    }

    if (isMouseOver || isMouseDown || GetIsClicked())
    {
        if (MouseHelper::GetCursorType() != CursorTypeCustom)
        {
            MouseHelper::SetCursorType(CursorTypeNormal);
        }

        MouseHelper::SetMouseOverText("");
    }
}

void Tab::Draw()
{
    Draw(0, 0);
}

void Tab::Draw(double xOffset, double yOffset)
{
    if (GetText().length() > 0)
    {
        Color textColor;

        if (!GetIsEnabled() && isClickable)
        {
            textColor = DisabledTextColor;
        }
        else if (isMouseDown)
        {
            textColor = MouseDownTextColor;
        }
        else if (isMouseOver)
        {
            textColor = MouseOverTextColor;
        }
        else
        {
            textColor = NormalTextColor;
        }

        int yPosition = GetYPositionFromRow() + (int)yOffset;
        Vector2 centerPoint = Vector2(xPosition + xOffset + 3 + textAreaWidth / 2, yPosition + textAreaHeight / 2);
        Vector2 textPosition = Vector2(centerPoint.GetX() - pFont->GetWidth(GetText()) / 2, yPosition + 1 + (hideAnimationOffset > 0 ? hideAnimationOffset : 0));

        if (GetTabImage() != NULL)
        {
            textPosition.SetX(textPosition.GetX() + GetTabImage()->GetWidth() / 2);
        }

        Image *pBackgroundSprite = row == TabRowTop ? pUpSprite : pDownSprite;
        int clipTop = hideAnimationOffset < 0 ? -hideAnimationOffset : 0;
        int clipHeight = TabHeight - (hideAnimationOffset > 0 ? hideAnimationOffset : clipTop);

        pBackgroundSprite->Draw(Vector2(xPosition + xOffset, yPosition + hideAnimationOffset + clipTop), RectangleWH(0, clipTop, TabWidth, clipHeight), false, false, 1.0, Color::White);
        pFont->Draw(GetText(), textPosition, textColor, RectangleWH(xPosition + xOffset - textPosition.GetX(), yPosition - textPosition.GetY() + clipTop, TabWidth, clipHeight));

        if (GetTabImage() != NULL)
        {
            double heightOffset = pBackgroundSprite->height - 3 - GetTabImage()->GetHeight();

            int tabClipTop = (int)(hideAnimationOffset < -heightOffset ? -hideAnimationOffset - heightOffset : 0);
            int tabClipHeight = (int)(GetTabImage()->GetHeight() - (hideAnimationOffset > 0 ? hideAnimationOffset : tabClipTop));

            Vector2 tabPosition = Vector2(
                textPosition.GetX() - GetTabImage()->GetWidth() - 4,
                yPosition + hideAnimationOffset + tabClipTop + pBackgroundSprite->height - 3 - GetTabImage()->GetHeight());

            GetTabImage()->DrawClipped(
                Vector2(tabPosition.GetX(), tabPosition.GetY()),
                RectangleWH(0, tabClipTop, GetTabImage()->GetWidth(), tabClipHeight));
        }

        // We don't need to account for the animation offset for the pulse,
        // since we don't show the pulse unless the offset is 0.
        if (GetShouldShowPulse())
        {
            pPulseAnimation->Draw(
                Vector2(
                    xPosition + xOffset + (pBackgroundSprite->width - pPulseAnimation->GetSize().GetX()) / 2 + 1,
                    yPosition + (pBackgroundSprite->height - pPulseAnimation->GetSize().GetY()) / 2 + 1));
        }
    }
}

void Tab::Reset()
{
    isMouseOver = false;
    isMouseDown = false;
    isClicked = false;
}

void Tab::Init(int xPosition, bool isClickable, const string &text, bool useCancelClickSoundEffect, TabRow row, bool canPulse)
{
    this->xPosition = xPosition;
    this->row = row;
    this->areaRect = RectangleWH(xPosition, GetYPositionFromRow(), width, height);
    this->isClickable = isClickable;

    this->text = text;
    this->isEnabled = true;
    this->useCancelClickSoundEffect = useCancelClickSoundEffect;
    this->pTabImage = NULL;

    this->isMouseOver = false;
    this->isMouseDown = false;
    this->isClicked = false;
    this->isHidden = false;
    this->isPulsing = false;
    this->wasPulsing = false;

    this->hideAnimationOffset = 0;
    this->pTopRowHideEase = new QuadraticEase(0, -HideDistance, HideAnimationDuration);
    this->pTopRowShowEase = new QuadraticEase(-HideDistance, 0, HideAnimationDuration);
    this->pBottomRowHideEase = new QuadraticEase(0, HideDistance, HideAnimationDuration);
    this->pBottomRowShowEase = new QuadraticEase(HideDistance, 0, HideAnimationDuration);
    this->pCurrentEasingFunction = NULL;

    if (canPulse)
    {
        char animationId[256];

        sprintf(animationId, "PulseAnimation%d", ++pulseAnimationCount);
        CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(string(animationId), &pPulseAnimation);

        for (unsigned int i = 0; i < 33; i++)
        {
            char spriteId[256];

            sprintf(spriteId, pulseSpriteIdFormat.c_str(), i);
            pPulseAnimation->AddFrame(42, string(spriteId));
        }
    }
    else
    {
        pPulseAnimation = NULL;
    }
}

int Tab::GetYPositionFromRow() const
{
    switch (row)
    {
        case TabRowTop:
            return 0;

        case TabRowDialog:
            return dialogRowYPosition;

        case TabRowBottom:
            return bottomRowYPosition;

        default:
            return 0;
    }
}

bool Tab::GetShouldShowPulse()
{
    return
        isPulsing &&
        !isHidden &&
        isEnabled &&
        isClickable &&
        pPulseAnimation != NULL &&
        (pCurrentEasingFunction == NULL || pCurrentEasingFunction->GetIsFinished());
}
