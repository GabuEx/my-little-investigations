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

#include <assert.h>

#include "KeyboardHelper.h"

bool KeyboardHelper::left = false;
bool KeyboardHelper::right = false;
bool KeyboardHelper::up = false;
bool KeyboardHelper::down = false;
bool KeyboardHelper::running = false;

SDL_Keycode KeyboardHelper::actionKeys[Count][nbAlternate] = {	// Default values
	{ SDLK_w,		SDLK_UP		},	// Up
	{ SDLK_s,		SDLK_DOWN	},	// Down
	{ SDLK_a,		SDLK_LEFT	},	// Left
	{ SDLK_d,		SDLK_RIGHT	},	// Right
	{ SDLK_LSHIFT,	SDLK_RSHIFT	},	// Run
	{ SDLK_SPACE,	SDLK_RETURN	}	// Click
};

string KeyboardHelper::actionNames[Count] = {
    "Up",
    "Down",
    "Left",
    "Right",
    "Run",
    "Click"
};

void KeyboardHelper::Init()
{
    left = right = up = down = running = false;

    // Provide layout independant keys (no qwerty / azerty problem)
    // Now that SDL has been init, this will work
    actionKeys[Up][0]		= SDL_GetKeyFromScancode(SDL_SCANCODE_W);
    actionKeys[Down][0]		= SDL_GetKeyFromScancode(SDL_SCANCODE_S);
    actionKeys[Left][0]		= SDL_GetKeyFromScancode(SDL_SCANCODE_A);
    actionKeys[Right][0]	= SDL_GetKeyFromScancode(SDL_SCANCODE_D);
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

bool KeyboardHelper::IsActionKey(HandledActions action, SDL_Keycode key)
{
	assert(action >= 0 && action < Count);

    for(int i = 0; i < nbAlternate; i++)
    {
        if(key == actionKeys[action][i])
            return true;
    }

    return false;
}

SDL_Keycode KeyboardHelper::GetKeyForAction(HandledActions action, int alternate)
{
	assert(action >= 0 && action < Count);
	assert(alternate >= 0 && alternate < nbAlternate);

	return actionKeys[action][alternate];
}

void KeyboardHelper::SetKeyForAction(HandledActions action, SDL_Keycode key, int alternate)
{
	assert(action >= 0 && action < Count);
	assert(alternate >= 0 && alternate < nbAlternate);

	actionKeys[action][alternate] = key;
}

void KeyboardHelper::ReadConf(XmlReader& configReader)
{
    for(int actionIt = 0; actionIt < Count; actionIt++)
    {
        for(int alternateIt = 0; alternateIt < nbAlternate; alternateIt++)
        {
            string nodeName = GetActionNodeName((HandledActions) actionIt, alternateIt);

			if (configReader.ElementExists(nodeName.c_str()))
			{
					KeyboardHelper::SetKeyForAction(
						(HandledActions) actionIt,
						SDL_GetKeyFromName( configReader.ReadTextElement( nodeName.c_str() ).c_str() ),
						alternateIt
					);
			}
        }
    }
}

void KeyboardHelper::WriteConf(XmlWriter& configWriter)
{
    for(int actionIt = 0; actionIt < Count; actionIt++)
    {
        for(int alternateIt = 0; alternateIt < nbAlternate; alternateIt++)
        {
            string nodeName = GetActionNodeName((HandledActions) actionIt, alternateIt);

			configWriter.WriteTextElement(
				nodeName.c_str(),
				SDL_GetKeyName(KeyboardHelper::GetKeyForAction((HandledActions) actionIt, alternateIt))
			);
        }
    }
}

string KeyboardHelper::GetActionNodeName(HandledActions action, int alternate)
{
    ostringstream oss;
    oss << actionNames[action] << alternate;
    return oss.str();
}
