/**
 * Static class to aid in the handling of text input.
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

#include "TextInputHelper.h"

#include "MLIFont.h"
#include "Utils.h"

string TextInputHelper::currentText = "";
unsigned int TextInputHelper::caretPosition = 0;
bool TextInputHelper::isCaretShowing = true;
int TextInputHelper::timeBeforeCaretToggle = 0;

SDL_Keycode TextInputHelper::keyDownForRepeat = SDLK_UNKNOWN;
int TextInputHelper::msSinceKeyDown = -1;
bool TextInputHelper::isRepeating = false;

int TextInputHelper::maxPixelWidth = -1;
MLIFont *TextInputHelper::pFontToCheckAgainst = NULL;

bool TextInputHelper::userHasConfirmed = false;
bool TextInputHelper::userHasCanceled = false;
bool TextInputHelper::isInSession = false;

const int DurationBeforeRepeatingMs = 500;
const int CaretToggleDurationMs = 500;

void TextInputHelper::Init()
{
    Reset();
}

void TextInputHelper::StartSession()
{
    SDL_StartTextInput();
    isInSession = true;
}

void TextInputHelper::EndSession()
{
    SDL_StopTextInput();
    isInSession = false;
}

void TextInputHelper::NotifyKeyState(SDL_Keycode keycode, bool isDown)
{
    if(isDown)
    {
		HandleSpecialKey(keycode);

		if (keycode == SDLK_BACKSPACE ||
			keycode == SDLK_DELETE ||
			keycode == SDLK_LEFT ||
			keycode == SDLK_RIGHT)
		{
			keyDownForRepeat = keycode;
			msSinceKeyDown = 0;
		}
    }

    else
    {
		if (keycode == keyDownForRepeat)
		{
			keyDownForRepeat = SDLK_UNKNOWN;
			msSinceKeyDown = -1;
		}
    }
}

void TextInputHelper::NotifyTextInput(const string &newText)
{
    int newCaretPosition;
    string textWithInsertion = InsertString(newText, currentText, caretPosition, &newCaretPosition);
    SetNewText(textWithInsertion, newCaretPosition);
}

void TextInputHelper::Update(int delta)
{
    if (msSinceKeyDown >= 0)
    {
        msSinceKeyDown += delta;

        if (msSinceKeyDown > DurationBeforeRepeatingMs)
        {
            HandleSpecialKey(keyDownForRepeat);
        }
    }

    timeBeforeCaretToggle -= delta;

    while (timeBeforeCaretToggle <= 0)
    {
        timeBeforeCaretToggle += CaretToggleDurationMs;
        isCaretShowing = !isCaretShowing;
    }
}

void TextInputHelper::Reset()
{
    ResetCaret();

    currentText = "";
    caretPosition = 0;

    keyDownForRepeat = SDLK_UNKNOWN;
    msSinceKeyDown = -1;
    isRepeating = false;

    userHasConfirmed = false;
    userHasCanceled = false;
    isInSession = false;
}

bool TextInputHelper::SetNewText(const string &newText, unsigned int newCaretPosition)
{
    // If we have a max pixel width and the new text will push us past that width,
    // then we won't do anything.
    if (maxPixelWidth >= 0 && pFontToCheckAgainst != NULL && pFontToCheckAgainst->GetWidth(newText) > maxPixelWidth)
    {
        return false;
    }

    currentText = newText;
    caretPosition = newCaretPosition;
    ResetCaret();
    return true;
}

void TextInputHelper::HandleSpecialKey(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_BACKSPACE:
        if (caretPosition > 0)
        {
            int newCaretPosition = GetPreviousInsertionPosition(currentText, caretPosition);
            string textWithDeletion = DeleteFromString(currentText, newCaretPosition, caretPosition);

            SetNewText(textWithDeletion, newCaretPosition);
        }
        break;

    case SDLK_DELETE:
        if (caretPosition < currentText.length())
        {
            int newCaretPosition = GetNextInsertionPosition(currentText, caretPosition);
            string textWithDeletion = DeleteFromString(currentText, caretPosition, newCaretPosition);

            SetNewText(textWithDeletion, caretPosition);
        }
        break;

    case SDLK_LEFT:
        if (caretPosition > 0)
        {
            int newCaretPosition = GetPreviousInsertionPosition(currentText, caretPosition);
            SetNewText(currentText, newCaretPosition);
        }
        break;

    case SDLK_RIGHT:
        if (caretPosition < currentText.length())
        {
            int newCaretPosition = GetNextInsertionPosition(currentText, caretPosition);
            SetNewText(currentText, newCaretPosition);
        }
        break;

    case SDLK_RETURN:
        if (currentText.length() > 0)
        {
            userHasConfirmed = true;
        }
        break;

    case SDLK_ESCAPE:
        userHasCanceled = true;
        break;
    }
}

void TextInputHelper::ResetCaret()
{
    timeBeforeCaretToggle = CaretToggleDurationMs;
    isCaretShowing = true;
}
