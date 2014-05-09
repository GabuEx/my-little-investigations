/**
 * Basic header/include file for KeyboardHelper.cpp.
 *
 * @author meh2481
 * @since 1.07
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

#ifndef KEYBOARDHELPER_H
#define KEYBOARDHELPER_H

#include <SDL2/SDL_keyboard.h>

#include "XmlReader.h"
#include "XmlWriter.h"
#include "Vector2.h"

const int AllowedAlternateKeyCount = 2;

class KeyboardHelper
{
public:

    enum HandledAction
    {
        Up      = 0,
        Down    = 1,
        Left    = 2,
        Right   = 3,
        Run     = 4,
        Click   = 5,

        Count,
    };

    static void Init();

    static void SetLeftState(bool isDown);
    static void SetRightState(bool isDown);
    static void SetUpState(bool isDown);
    static void SetDownState(bool isDown);
    static void SetRunState(bool isDown);
    static void SetClickState(bool isDown);

    static bool GetLeftState();
    static bool GetRightState();
    static bool GetUpState();
    static bool GetDownState();
    static bool GetMoving();
    static bool GetRunState();
    static bool GetClickState();

    static bool ClickPressed();
    static bool UpPressed();
    static bool DownPressed();

    static void UpdateKeyState();

    static bool IsActionKey(HandledAction, SDL_Keycode);

    static SDL_Keycode GetKeyForAction(HandledAction action, int alternate);
    static void SetKeyForAction(HandledAction action, SDL_Keycode key, int alternate);

    static void ReadConfig(XmlReader& configReader);
    static void WriteConfig(XmlWriter& configWriter);

private:

    static string GetActionNodeName(HandledAction action, int alternate);

    static bool left, right, up, down, running, clicking;
    static bool clickhandled, uphandled, downhandled;

    static SDL_Keycode  actionKeys[Count][AllowedAlternateKeyCount];
    static string       actionNames[Count];
};

#endif // KEYBOARDHELPER_H
