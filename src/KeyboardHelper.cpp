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

SDL_Scancode KeyboardHelper::upKey[2] = {SDL_SCANCODE_W, SDL_SCANCODE_UP};
SDL_Scancode KeyboardHelper::downKey[2] = {SDL_SCANCODE_S, SDL_SCANCODE_DOWN};
SDL_Scancode KeyboardHelper::leftKey[2] = {SDL_SCANCODE_A, SDL_SCANCODE_LEFT};
SDL_Scancode KeyboardHelper::rightKey[2] = {SDL_SCANCODE_D, SDL_SCANCODE_RIGHT};
SDL_Scancode KeyboardHelper::runKey[2] = {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT};
SDL_Scancode KeyboardHelper::clickKey[2] = {SDL_SCANCODE_SPACE, SDL_SCANCODE_RETURN};

void KeyboardHelper::Init()
{
    left = right = up = down = running = false;
}

void KeyboardHelper::LeftState(bool isDown)
{
    left = isDown;
}

void KeyboardHelper::RightState(bool isDown)
{
    right = isDown;
}

void KeyboardHelper::UpState(bool isDown)
{
    up = isDown;
}

void KeyboardHelper::DownState(bool isDown)
{
    down = isDown;
}

void KeyboardHelper::RunState(bool isDown)
{
    running = isDown;
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

bool KeyboardHelper::IsUpKey(SDL_Scancode key)
{
    return (key == upKey[0] || key == upKey[1]);
}

bool KeyboardHelper::IsDownKey(SDL_Scancode key)
{
    return (key == downKey[0] || key == downKey[1]);
}

bool KeyboardHelper::IsLeftKey(SDL_Scancode key)
{
    return (key == leftKey[0] || key == leftKey[1]);
}

bool KeyboardHelper::IsRightKey(SDL_Scancode key)
{
    return (key == rightKey[0] || key == rightKey[1]);
}

bool KeyboardHelper::IsRunKey(SDL_Scancode key)
{
    return (key == runKey[0] || key == runKey[1]);
}

bool KeyboardHelper::IsClickKey(SDL_Scancode key)
{
    return (key == clickKey[0] || key == clickKey[1]);
}

SDL_Scancode KeyboardHelper::GetUpKey(int id)
{
    return upKey[id];
}

SDL_Scancode KeyboardHelper::GetDownKey(int id)
{
    return downKey[id];
}

SDL_Scancode KeyboardHelper::GetLeftKey(int id)
{
    return leftKey[id];
}

SDL_Scancode KeyboardHelper::GetRightKey(int id)
{
    return rightKey[id];
}

SDL_Scancode KeyboardHelper::GetRunKey(int id)
{
    return runKey[id];
}

SDL_Scancode KeyboardHelper::GetClickKey(int id)
{
    return clickKey[id];
}

void KeyboardHelper::SetUpKey(SDL_Scancode key, int id)
{
    upKey[id] = key;
}

void KeyboardHelper::SetDownKey(SDL_Scancode key, int id)
{
    downKey[id] = key;
}

void KeyboardHelper::SetLeftKey(SDL_Scancode key, int id)
{
    leftKey[id] = key;
}

void KeyboardHelper::SetRightKey(SDL_Scancode key, int id)
{
    rightKey[id] = key;
}

void KeyboardHelper::SetRunKey(SDL_Scancode key, int id)
{
    runKey[id] = key;
}

void KeyboardHelper::SetClickKey(SDL_Scancode key, int id)
{
    clickKey[id] = key;
}

void KeyboardHelper::ReadConf(XmlReader& configReader)
{
	if (configReader.ElementExists("UpKey1"))
	{
		KeyboardHelper::SetUpKey((SDL_Scancode) configReader.ReadIntElement("UpKey1"), 0);
	}

	if (configReader.ElementExists("UpKey2"))
	{
		KeyboardHelper::SetDownKey((SDL_Scancode) configReader.ReadIntElement("UpKey2"), 1);
	}

	if (configReader.ElementExists("DownKey1"))
	{
		KeyboardHelper::SetDownKey((SDL_Scancode) configReader.ReadIntElement("DownKey1"), 0);
	}

	if (configReader.ElementExists("DownKey2"))
	{
		KeyboardHelper::SetDownKey((SDL_Scancode) configReader.ReadIntElement("DownKey2"), 1);
	}

	if (configReader.ElementExists("LeftKey1"))
	{
		KeyboardHelper::SetLeftKey((SDL_Scancode) configReader.ReadIntElement("LeftKey1"), 0);
	}

	if (configReader.ElementExists("LeftKey2"))
	{
		KeyboardHelper::SetLeftKey((SDL_Scancode) configReader.ReadIntElement("LeftKey2"), 1);
	}

	if (configReader.ElementExists("RightKey1"))
	{
		KeyboardHelper::SetRightKey((SDL_Scancode) configReader.ReadIntElement("RightKey1"), 0);
	}

	if (configReader.ElementExists("RightKey2"))
	{
		KeyboardHelper::SetRightKey((SDL_Scancode) configReader.ReadIntElement("RightKey2"), 1);
	}

	if (configReader.ElementExists("RunKey1"))
	{
		KeyboardHelper::SetRunKey((SDL_Scancode) configReader.ReadIntElement("RunKey1"), 0);
	}

	if (configReader.ElementExists("RunKey2"))
	{
		KeyboardHelper::SetRunKey((SDL_Scancode) configReader.ReadIntElement("RunKey2"), 1);
	}

	if (configReader.ElementExists("ClickKey1"))
	{
		KeyboardHelper::SetClickKey((SDL_Scancode) configReader.ReadIntElement("ClickKey1"), 0);
	}

	if (configReader.ElementExists("ClickKey2"))
	{
		KeyboardHelper::SetClickKey((SDL_Scancode) configReader.ReadIntElement("ClickKey2"), 1);
	}
}

void KeyboardHelper::WriteConf(XmlWriter& configWriter)
{
    configWriter.WriteIntElement("UpKey1", KeyboardHelper::GetUpKey(0));
    configWriter.WriteIntElement("UpKey2", KeyboardHelper::GetUpKey(1));
    configWriter.WriteIntElement("DownKey1", KeyboardHelper::GetDownKey(0));
    configWriter.WriteIntElement("DownKey2", KeyboardHelper::GetDownKey(1));
    configWriter.WriteIntElement("LeftKey1", KeyboardHelper::GetLeftKey(0));
    configWriter.WriteIntElement("LeftKey2", KeyboardHelper::GetLeftKey(1));
    configWriter.WriteIntElement("RightKey1", KeyboardHelper::GetRightKey(0));
    configWriter.WriteIntElement("RightKey2", KeyboardHelper::GetRightKey(1));
    configWriter.WriteIntElement("RunKey1", KeyboardHelper::GetRunKey(0));
    configWriter.WriteIntElement("RunKey2", KeyboardHelper::GetRunKey(1));
    configWriter.WriteIntElement("ClickKey1", KeyboardHelper::GetClickKey(0));
    configWriter.WriteIntElement("ClickKey2", KeyboardHelper::GetClickKey(1));
}
