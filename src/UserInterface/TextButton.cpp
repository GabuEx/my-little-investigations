/**
 * Text button. Can be checkable. Sends OnButtonClicked() event to registered
 * listeners.
 * Can be animated through MoveTo() and Reappear() functions.
 *  MoveTo() - moving animation
 *  Reappear() - button fade out with disabled state at current position and
 *               fade in at new position.
 * Button don't interact with user while animated.
 *
 * @author mad-mix
 * @since 1.0.7
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

#include "TextButton.h"

#include "../Events/TextButtonEventProvider.h"
#include "../ResourceLoader.h"
#include "../Utils.h"
#include "../MouseHelper.h"
#include "../mli_audio.h"

Image *TextButton::pCheckMarkImage = NULL;
Image *TextButton::pBoxImage = NULL;

const Color DefaultTextColor = Color(1.0, 1.0, 1.0, 1.0);
const Color DefaultMouseOverTextColor = Color(1.0, 1.0, 1.0, 0.0);
const Color DefaultMouseDownTextColor = Color(1.0, 1.0, 0.0, 0.0);
const Color DefaultDisabledTextColor = Color(1.0, 0.5, 0.5, 0.5);
const Vector2 CheckMarkOffset = Vector2(8, -8); // offset required because check mark asymmetry
const int Padding = 8; // padding between check box and text

const string DefaultClickSoundEffect = "ButtonClick1";

void TextButton::Initialize(Image *pCheckMarkImage, Image *pBoxImage)
{
    TextButton::pCheckMarkImage = pCheckMarkImage;
    TextButton::pBoxImage = pBoxImage;
}

TextButton::TextButton(const string &textId, MLIFont *pFont, bool checkable) :
    x(0),
    hAlignment(HAlignmentLeft),
    y(0),
    width(0),
    height(0),
    opacity(1.0),
    textWidget(textId, pFont, DefaultTextColor, HAlignmentLeft, VAlignmentCenter),
    pFont(pFont),
    pDisabledFont(NULL),
    textColor(DefaultTextColor),
    disabledTextColor(DefaultDisabledTextColor),
    mouseDownTextColor(DefaultMouseDownTextColor),
    mouseOverTextColor(DefaultMouseOverTextColor),
    checkable(checkable),
    checked(false),
    enabled(true),
    clickSoundEffect(DefaultClickSoundEffect),
    pXEase(NULL),
    pYEase(NULL),
    pFadeInEase(NULL),
    pFadeOutEase(NULL),
    pOldButton(NULL)
{
    ReloadLocalizableText();
    gpLocalizableContent->AddLocalizableTextOwner(this);
}

TextButton::TextButton(const TextButton &rhs) :
    x(rhs.x),
    hAlignment(rhs.hAlignment),
    y(rhs.y),
    width(rhs.width),
    height(rhs.height),
    opacity(rhs.opacity),
    textWidget(rhs.textWidget),
    pFont(rhs.pFont),
    pDisabledFont(rhs.pDisabledFont),
    textColor(rhs.textColor),
    disabledTextColor(rhs.disabledTextColor),
    mouseDownTextColor(rhs.mouseDownTextColor),
    mouseOverTextColor(rhs.mouseOverTextColor),
    checkable(rhs.checkable),
    checked(rhs.checked),
    enabled(rhs.enabled),
    clickSoundEffect(rhs.clickSoundEffect),
    pXEase(NULL),
    pYEase(NULL),
    pFadeInEase(NULL),
    pFadeOutEase(NULL),
    pOldButton(NULL)
{

}

TextButton::~TextButton()
{
    gpLocalizableContent->RemoveLocalizableTextOwner(this);

    delete pXEase;
    pXEase = NULL;

    delete pYEase;
    pYEase = NULL;

    delete pFadeInEase;
    pFadeInEase = NULL;

    delete pFadeOutEase;
    pFadeOutEase = NULL;

    delete pOldButton;
    pOldButton = NULL;
}

void TextButton::Update(int delta)
{
    textWidget.SetFont(GetIsEnabled() ? GetFont() : GetDisabledFont());
    textWidget.SetTextColor(GetIsEnabled() ? GetTextColor() : GetDisabledTextColor());

    // button should not response on any user actions during animation
    // so we'll early-out here when we are.
    UpdateAnimation(delta);
    if (IsAnimationPlaying())
    {
        return;
    }

    if (GetIsEnabled())
    {
        RectangleWH positionRect = RectangleWH(GetLeft(), GetTop(), GetWidth(), GetHeight());
        bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

        if (MouseHelper::ClickedOnRect(positionRect))
        {
            OnClicked();
        }

        if (MouseHelper::MouseDownOnRect(positionRect) && !isPressed)
        {
            textWidget.SetTextColor(mouseDownTextColor);
        }
        else if (MouseHelper::MouseOverRect(positionRect) && !isPressed)
        {
            textWidget.SetTextColor(mouseOverTextColor);
        }
        else
        {
            textWidget.SetTextColor(GetTextColor());
        }
    }
}

void TextButton::Draw() const
{
    Vector2 drawingPoint(x, y);

    if (GetCheckable())
    {
        Color color = GetIsEnabled() ? Color(GetOpacity(), 1.0, 1.0, 1.0) : Color(GetOpacity(), 0.5, 0.5, 0.5);
        Vector2 checkBoxDrawingPoint(x, y - GetHeight() / 2);
        Vector2 checkBoxSize(
                    max(TextButton::pCheckMarkImage->width + CheckMarkOffset.GetX(), (double)TextButton::pBoxImage->width),
                    max(TextButton::pCheckMarkImage->height + CheckMarkOffset.GetY(), (double)TextButton::pBoxImage->height)
                    );
        Vector2 checkBoxCenter(
                    checkBoxSize.GetX() / 2,
                    max(GetHeight(), checkBoxSize.GetY()) / 2
                    );

        // draw box
        pBoxImage->Draw(checkBoxDrawingPoint + checkBoxCenter - Vector2(TextButton::pBoxImage->width / 2, TextButton::pBoxImage->height / 2), color);

        // draw check mark
        if (GetChecked())
        {
            TextButton::pCheckMarkImage->Draw(
                        checkBoxDrawingPoint + checkBoxCenter + CheckMarkOffset - Vector2(TextButton::pCheckMarkImage->width / 2, TextButton::pCheckMarkImage->height / 2),
                        color);
        }

        drawingPoint.SetX(drawingPoint.GetX() + checkBoxSize.GetX() + Padding);
    }

    textWidget.Draw(drawingPoint.GetX(), drawingPoint.GetY(), hAlignment);

    if (pFadeInEase != NULL && pFadeInEase->IsRunning() && pOldButton != NULL)
    {
        pOldButton->Draw();
    }
}

bool TextButton::IsReady()
{
    return true;
}

void TextButton::Reset()
{
    delete pXEase;
    pXEase = NULL;

    delete pYEase;
    pYEase = NULL;

    delete pFadeInEase;
    pFadeInEase = NULL;

    delete pFadeOutEase;
    pFadeOutEase = NULL;

    delete pOldButton;
    pOldButton = NULL;
}

double TextButton::GetLeft() const
{
    if (hAlignment == HAlignmentRight)
    {
        return this->x - GetWidth();
    }
    else if (hAlignment == HAlignmentCenter)
    {
        return this->x - GetWidth() / 2;
    }
    else
    {
        return this->x;
    }
}

void TextButton::MoveTo(double newX, HAlignment hAlignment, double newY, int timeMS)
{
    delete pFadeInEase;
    pFadeInEase = NULL;

    delete pFadeOutEase;
    pFadeOutEase = NULL;

    delete pOldButton;
    pOldButton = NULL;

    delete pXEase;
    pXEase = new SCurveEase(GetLeft(), newX, timeMS);
    pXEase->Begin();

    transitionHAlignment = hAlignment;

    delete pYEase;
    pYEase = new SCurveEase(GetTop(), newY, timeMS);
    pYEase->Begin();
}

void TextButton::Reappear(double newX, HAlignment hAlignment, double newY, int timeMS)
{
    delete pXEase;
    pXEase = NULL;

    delete pYEase;
    pYEase = NULL;

    delete pFadeInEase;
    pFadeInEase = new LinearEase(opacity, 0, timeMS);
    pFadeInEase->Begin();

    delete pFadeOutEase;
    pFadeOutEase = new LinearEase(0, opacity, timeMS);
    pFadeOutEase->Begin();

    delete pOldButton;
    pOldButton = new TextButton(*this);
    pOldButton->SetIsEnabled(false);

    SetX(newX, hAlignment);
    SetY(newY);
}

void TextButton::SetMaxWidth(double maxWidth)
{
    Vector2 checkBoxSize(
                max(TextButton::pCheckMarkImage->width + CheckMarkOffset.GetX(), (double)TextButton::pBoxImage->width),
                max(TextButton::pCheckMarkImage->height + CheckMarkOffset.GetY(), (double)TextButton::pBoxImage->height)
                );

    textWidget.SetWidth(maxWidth - checkBoxSize.GetX() - Padding);
    textWidget.WrapText();

    width = checkBoxSize.GetX() + Padding + textWidget.GetWidth();
    height = max(height, checkBoxSize.GetY());
    height = max(height, textWidget.GetHeight());
}

void TextButton::ReloadLocalizableText()
{
    UpdateSize();
}

bool TextButton::IsAnimationPlaying() const
{
    return (pXEase != NULL && pXEase->IsRunning())
            || (pYEase != NULL && pYEase->IsRunning())
            || (pFadeInEase != NULL && pFadeInEase->IsRunning())
            || (pFadeOutEase != NULL && pFadeOutEase->IsRunning());
}

void TextButton::UpdateAnimation(int delta)
{
    if (pXEase != NULL && pXEase->IsRunning())
    {
        pXEase->Update(delta);
        SetX(pXEase->GetCurrentValue(), transitionHAlignment);
    }

    if (pYEase != NULL && pYEase->IsRunning())
    {
        pYEase->Update(delta);
        SetY(pYEase->GetCurrentValue());
    }

    if (pFadeInEase != NULL && pFadeInEase->IsRunning() && pOldButton != NULL)
    {
        pFadeInEase->Update(delta);
        pOldButton->SetOpacity(pFadeInEase->GetCurrentValue());
    }

    if (pFadeOutEase != NULL && pFadeOutEase->IsRunning())
    {
        pFadeOutEase->Update(delta);
        SetOpacity(pFadeOutEase->GetCurrentValue());
    }
}

void TextButton::OnClicked()
{
    if (GetCheckable())
    {
        SetChecked(!GetChecked());
    }

    EventProviders::GetTextButtonEventProvider()->RaiseClicked(this);
    playSound(GetClickSoundEffect());
}

void TextButton::UpdateSize()
{
   Vector2 checkBoxSize(
        max(TextButton::pCheckMarkImage->width + CheckMarkOffset.GetX(), (double)TextButton::pBoxImage->width),
        max(TextButton::pCheckMarkImage->height + CheckMarkOffset.GetY(), (double)TextButton::pBoxImage->height)
        );

    textWidget.FitSizeToContent();

    SetWidth((GetCheckable() ? checkBoxSize.GetX() + Padding : 0) + textWidget.GetWidth());
    SetHeight(max((GetCheckable() ? checkBoxSize.GetY() : 0), textWidget.GetHeight()));
}
