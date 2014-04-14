/**
 * Handles an overlay that prompts the user for some form of input,
 * either text or clicking a button.
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

#include "PromptOverlay.h"

#include "../Color.h"
#include "../globals.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../TextInputHelper.h"
#include "../Utils.h"
#include "../Events/PromptOverlayEventProvider.h"

Font *PromptButton::pTextFont = NULL;
Font *PromptOverlay::pTextFont = NULL;
Font *PromptOverlay::pTextEntryFont = NULL;
Image *PromptOverlay::pDarkeningImage = NULL;

const Color NormalTextColor = Color(1.0, 1.0, 1.0, 1.0);
const Color MouseOverTextColor = Color(1.0, 1.0, 1.0, 0.0);
const Color MouseDownTextColor = Color(1.0, 1.0, 0.0, 0.0);
const Color DisabledTextColor = Color(1.0, 0.5, 0.5, 0.5);

const int FadeMsDuration = 300;
const double ButtonSpacing = 50;

void PromptButton::Initialize(Font *pTextFont)
{
    PromptButton::pTextFont = pTextFont;
}

void PromptButton::Update(int delta)
{
    if (isEnabled)
    {
        isClicked = false;
        isMouseDown = false;
        isMouseOver = false;

        RectangleWH positionRect = RectangleWH(position.GetX(), position.GetY(), pTextFont->GetWidth(text), pTextFont->GetHeight(text));
        bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

        if (MouseHelper::ClickedOnRect(positionRect))
        {
            playSound(GetClickSoundEffect());
            isClicked = true;
            return;
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

void PromptButton::Draw(double opacity)
{
    Color baseColor;

    if (!isEnabled)
    {
        baseColor = DisabledTextColor;
    }
    else if (isMouseDown)
    {
        baseColor = MouseDownTextColor;
    }
    else if (isMouseOver)
    {
        baseColor = MouseOverTextColor;
    }
    else
    {
        baseColor = NormalTextColor;
    }

    pTextFont->Draw(text, position, Color(opacity, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()));
}

void PromptButton::Reset()
{
    isClicked = false;
    isMouseDown = false;
    isMouseOver = false;
}

void PromptOverlay::Initialize(Font *pTextFont, Font *pTextEntryFont, Image *pDarkeningImage)
{
    PromptOverlay::pTextFont = pTextFont;
    PromptOverlay::pTextEntryFont = pTextEntryFont;
    PromptOverlay::pDarkeningImage = pDarkeningImage;
}

PromptOverlay::PromptOverlay(string headerText, bool allowsTextEntry)
{
    yOffset = 0;

    SetHeaderText(headerText);
    this->allowsTextEntry = allowsTextEntry;

    if (allowsTextEntry)
    {
        AddButton("OK");
        AddButton("Cancel");
        FinalizeButtons();
    }

    pFadeInEase = new LinearEase(0, 1, FadeMsDuration);
    pFadeOutEase = new LinearEase(1, 0, FadeMsDuration);
    isShowing = false;

    maxPixelWidth = -1;
    pFontToCheckAgainst = NULL;
}

PromptOverlay::~PromptOverlay()
{
    for (unsigned int i = 0; i < buttonList.size(); i++)
    {
        delete buttonList[i];
    }

    buttonList.clear();

    delete pFadeInEase;
    pFadeInEase = NULL;
    delete pFadeOutEase;
    pFadeOutEase = NULL;
}

void PromptOverlay::SetHeaderText(string headerText)
{
    this->headerTextLines = split(headerText, '\n');

    yOffset = (pTextFont->GetLineHeight() * (this->headerTextLines.size() - 1)) / 2;
}

void PromptOverlay::AddButton(string text)
{
    buttonTextList.push_back(text);
}

void PromptOverlay::FinalizeButtons()
{
    // If we've already finalized the buttons, do nothing.
    if (buttonList.size() > 0)
    {
        return;
    }

    // First we'll find the max button width - we'll scale the
    // spacing between buttons based on that.
    double maxButtonWidth = 0;

    for (unsigned int i = 0; i < buttonTextList.size(); i++)
    {
        double width = pTextFont->GetWidth(buttonTextList[i]);

        if (width > maxButtonWidth)
        {
            maxButtonWidth = width;
        }
    }

    double currentButtonXPosition = (gScreenWidth - maxButtonWidth * buttonTextList.size() - ButtonSpacing * (buttonTextList.size() - 1)) / 2;

    // Now that we know the max button width, we'll create and place the buttons.
    for (unsigned int i = 0; i < buttonTextList.size(); i++)
    {
        string text = buttonTextList[i];
        buttonList.push_back(new PromptButton(Vector2(currentButtonXPosition, gScreenHeight / 2 + pTextFont->GetLineHeight() + (allowsTextEntry ? pTextEntryFont->GetLineHeight() / 2 : 0) + yOffset), text));
        currentButtonXPosition += maxButtonWidth + ButtonSpacing;
    }

    buttonTextList.clear();
}

void PromptOverlay::Begin(string initialText)
{
    Reset();

    pFadeInEase->Begin();
    pFadeOutEase->Reset();

    fadeOpacity = 0;
    isShowing = true;

    TextInputHelper::Reset();
    TextInputHelper::SetCurrentText(initialText);

    if (maxPixelWidth >= 0 && pFontToCheckAgainst != NULL)
    {
        TextInputHelper::SetMaxPixelWidth(maxPixelWidth, pFontToCheckAgainst);
    }

    textEntered = TextInputHelper::GetCurrentText();
}

void PromptOverlay::Update(int delta)
{
    if (pFadeOutEase->GetIsStarted() && !pFadeOutEase->GetIsFinished())
    {
        pFadeOutEase->Update(delta);
        fadeOpacity = pFadeOutEase->GetCurrentValue();

        if (pFadeOutEase->GetIsFinished())
        {
            isShowing = false;
        }
    }
    else if (pFadeInEase->GetIsStarted() && !pFadeInEase->GetIsFinished())
    {
        pFadeInEase->Update(delta);
        fadeOpacity = pFadeInEase->GetCurrentValue();

        if (pFadeInEase->GetIsFinished() && allowsTextEntry)
        {
            TextInputHelper::StartSession();
        }
    }

    if (pFadeInEase->GetIsFinished() && !pFadeOutEase->GetIsStarted())
    {
        bool promptIsFinished = false;
        string returnValue = "";

        for (unsigned int i = 0; i < buttonList.size(); i++)
        {
            buttonList[i]->Update(delta);
        }

        if (allowsTextEntry)
        {
            textEntered = TextInputHelper::GetCurrentText();
            buttonList[0]->SetIsEnabled(textEntered.length() > 0);

            if (TextInputHelper::GetUserHasConfirmed() || buttonList[0]->GetIsClicked())
            {
                returnValue = textEntered;
                promptIsFinished = true;
            }
            else if (TextInputHelper::GetUserHasCanceled() || buttonList[1]->GetIsClicked())
            {
                returnValue = "";
                promptIsFinished = true;
            }
        }
        else
        {
            for (unsigned int i = 0; i < buttonList.size(); i++)
            {
                if (buttonList[i]->GetIsClicked())
                {
                    returnValue = buttonList[i]->GetText();
                    promptIsFinished = true;
                }
            }
        }

        if (promptIsFinished)
        {
            pFadeOutEase->Begin();

            if (allowsTextEntry)
            {
                TextInputHelper::EndSession();
            }

            EventProviders::GetPromptOverlayEventProvider()->RaisePromptOverlayValueReturned(this, returnValue);
        }
    }
}

void PromptOverlay::Draw()
{
    pDarkeningImage->Draw(Vector2(0, 0), Color(fadeOpacity * 0.75, 1, 1, 1));

    if (allowsTextEntry)
    {
        Vector2 textEnteredPosition = Vector2((gScreenWidth - pTextEntryFont->GetWidth(textEntered)) / 2, (gScreenHeight - pTextEntryFont->GetLineHeight()) / 2 + yOffset);
        pTextEntryFont->Draw(textEntered, textEnteredPosition, Color(fadeOpacity, 1, 1, 1));

        if (TextInputHelper::GetIsCaretShowing())
        {
            SDL_Rect rect =
            {
                (int)(textEnteredPosition.GetX() + pTextEntryFont->GetWidth(textEntered.substr(0, TextInputHelper::GetCaretPosition()))),
                (int)textEnteredPosition.GetY(),
                2,
                (int)pTextEntryFont->GetLineHeight()
            };

            // If we're in fullscreen mode, then we'll want to apply the screen scale and offsets,
            // rounding to the nearest pixel.
            if (gIsFullscreen)
            {
                rect.x = (int)(rect.x * gScreenScale + gHorizontalOffset + 0.5);
                rect.y = (int)(rect.y * gScreenScale + gVerticalOffset + 0.5);
                rect.w = (int)(rect.w * gScreenScale + 0.5);
                rect.h = (int)(rect.h * gScreenScale + 0.5);
            }

            SDL_SetRenderDrawColor(gpRenderer, 255, 255, 255, (Uint8)(fadeOpacity * 255));
            SDL_SetRenderDrawBlendMode(gpRenderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(gpRenderer, &rect);
        }
    }

    int textLineTop = gScreenHeight / 2 - pTextFont->GetLineHeight() * (1 + headerTextLines.size()) - (allowsTextEntry ? 2 * pTextEntryFont->GetLineHeight() / 3 : 0) + yOffset;

    for (unsigned int i = 0; i < headerTextLines.size(); i++)
    {
        pTextFont->Draw(headerTextLines[i], Vector2((gScreenWidth - pTextFont->GetWidth(headerTextLines[i])) / 2, textLineTop + pTextFont->GetLineHeight() * i), Color(fadeOpacity, 1, 1, 1));
    }

    for (unsigned int i = 0; i < buttonList.size(); i++)
    {
        buttonList[i]->Draw(fadeOpacity);
    }
}

void PromptOverlay::Reset()
{
    for (unsigned int i = 0; i < buttonList.size(); i++)
    {
        buttonList[i]->Reset();
    }

    fadeOpacity = 0;
    isShowing = false;
}

void PromptOverlay::KeepOpen()
{
    this->pFadeOutEase->Reset();
}
