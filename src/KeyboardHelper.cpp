/**
 * Static class to aid in the handling of keyboard operations.
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

 #include "KeyboardHelper.h"


bool KeyboardHelper::left = false;
bool KeyboardHelper::right = false;
bool KeyboardHelper::up = false;
bool KeyboardHelper::down = false;
bool KeyboardHelper::running = false;

void KeyboardHelper::Init()
{
    left = right = up = down = running = false;
}

void KeyboardHelper::LeftPress()
{
    left = true;
}

void KeyboardHelper::LeftRelease()
{
    left = false;
}

void KeyboardHelper::RightPress()
{
    right = true;
}

void KeyboardHelper::RightRelease()
{
    right = false;
}

void KeyboardHelper::UpPress()
{
    up = true;
}

void KeyboardHelper::UpRelease()
{
    up = false;
}

void KeyboardHelper::DownPress()
{
    down = true;
}

void KeyboardHelper::DownRelease()
{
    down = false;
}

void KeyboardHelper::RunPress()
{
    running = true;
}

void KeyboardHelper::RunRelease()
{
    running = false;
}

bool KeyboardHelper::GetRunning()
{
    return running;
}

bool KeyboardHelper::GetMoving()
{
    return (left || right || up || down);
}

Vector2 KeyboardHelper::GetPressedDirection()
{
    Vector2 result(0,0);

    if(left)
        result.SetX(result.GetX() - 50);
    if(right)
        result.SetX(result.GetX() + 50);
    if(up)
        result.SetY(result.GetY() - 50);
    if(down)
        result.SetY(result.GetY() + 50);

    return result;
}





































