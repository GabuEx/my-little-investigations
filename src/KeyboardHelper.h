/**
 * Basic header/include file for KeyboardHelper.cpp.
 *
 * @author meh2481
 * @since 1.06
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

#include <SDL2/SDL_scancode.h>
#include "Vector2.h"

class KeyboardHelper
{
public:
    static void Init();

    static void LeftState(bool isDown);
    static void RightState(bool isDown);
    static void UpState(bool isDown);
    static void DownState(bool isDown);
    static void RunState(bool isDown);

    static Vector2 GetPressedDirection();
    static bool GetMoving();
    static bool GetRunning();

	static bool IsUpKey(SDL_Scancode);
	static bool IsDownKey(SDL_Scancode);
	static bool IsLeftKey(SDL_Scancode);
	static bool IsRightKey(SDL_Scancode);
	static bool IsRunKey(SDL_Scancode);
	static bool IsClickKey(SDL_Scancode);

private:
    static bool left, right, up, down, running;

	static SDL_Scancode upKey[2], downKey[2], leftKey[2], rightKey[2], runKey[2], clickKey[2];
};

#endif // KEYBOARDHELPER_H
