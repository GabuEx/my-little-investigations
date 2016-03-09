/**
 * Static class to aid in the handling of keyboard operations.
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

#include <assert.h>

#include "KeyboardHelper.h"

bool KeyboardHelper::left = false;
bool KeyboardHelper::right = false;
bool KeyboardHelper::up = false;
bool KeyboardHelper::down = false;
bool KeyboardHelper::running = false;
bool KeyboardHelper::clicking = false;
bool KeyboardHelper::clickhandled = false;
bool KeyboardHelper::uphandled = false;
bool KeyboardHelper::downhandled = false;

SDL_Keycode KeyboardHelper::actionKeys[Count][AllowedAlternateKeyCount] = {    // Default values
    { SDLK_w,       SDLK_UP     },  // Up
    { SDLK_s,       SDLK_DOWN   },  // Down
    { SDLK_a,       SDLK_LEFT   },  // Left
    { SDLK_d,       SDLK_RIGHT  },  // Right
    { SDLK_LSHIFT,  SDLK_RSHIFT },  // Run
    { SDLK_SPACE,   SDLK_RETURN }   // Click
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
    left = right = up = down = running = clicking = false;
    clickhandled = uphandled = downhandled = false;

    // Provide layout independent keys (no qwerty / azerty problem)
    // Now that SDL has been init, this will work
    actionKeys[Up][0]       = SDL_GetKeyFromScancode(SDL_SCANCODE_W);
    actionKeys[Down][0]     = SDL_GetKeyFromScancode(SDL_SCANCODE_S);
    actionKeys[Left][0]     = SDL_GetKeyFromScancode(SDL_SCANCODE_A);
    actionKeys[Right][0]    = SDL_GetKeyFromScancode(SDL_SCANCODE_D);
}

void KeyboardHelper::SetLeftState(bool isDown)
{
    left = isDown;
}

void KeyboardHelper::SetRightState(bool isDown)
{
    right = isDown;
}

void KeyboardHelper::SetUpState(bool isDown)
{
    up = isDown;
    uphandled = false;
}

void KeyboardHelper::SetDownState(bool isDown)
{
    down = isDown;
    downhandled = false;
}

void KeyboardHelper::SetClickState(bool isDown)
{
    clicking = isDown;
    clickhandled = false;
}

void KeyboardHelper::SetRunState(bool isDown)
{
    running = isDown;
}

bool KeyboardHelper::GetLeftState()
{
    return left;
}

bool KeyboardHelper::GetRightState()
{
    return right;
}

bool KeyboardHelper::GetUpState()
{
    return up;
}

bool KeyboardHelper::GetDownState()
{
    return down;
}

bool KeyboardHelper::GetClickState()
{
    return clicking;
}

bool KeyboardHelper::GetRunState()
{
    return running;
}

bool KeyboardHelper::GetMoving()
{
    return (left || right || up || down);
}

bool KeyboardHelper::ClickPressed()
{
    bool result = (!clickhandled) && clicking;
    return result;
}

bool KeyboardHelper::UpPressed()
{
    bool result = (!uphandled) && up;
    return result;
}

bool KeyboardHelper::DownPressed()
{
    bool result = (!downhandled) && down;
    return result;
}

void KeyboardHelper::UpdateKeyState()
{
    downhandled = uphandled = clickhandled = true;
}

bool KeyboardHelper::IsActionKey(HandledAction action, SDL_Keycode key)
{
    assert(action >= 0 && action < Count);

    for(int i = 0; i < AllowedAlternateKeyCount; i++)
    {
        if(key == actionKeys[action][i])
        {
            return true;
        }
    }

    return false;
}

SDL_Keycode KeyboardHelper::GetKeyForAction(HandledAction action, int alternate)
{
    assert(action >= 0 && action < Count);
    assert(alternate >= 0 && alternate < AllowedAlternateKeyCount);

    return actionKeys[action][alternate];
}

void KeyboardHelper::SetKeyForAction(HandledAction action, SDL_Keycode key, int alternate)
{
    assert(action >= 0 && action < Count);
    assert(alternate >= 0 && alternate < AllowedAlternateKeyCount);

    actionKeys[action][alternate] = key;
}

void KeyboardHelper::ReadConfig(XmlReader& configReader)
{
    for(int actionIt = 0; actionIt < Count; actionIt++)
    {
        for(int alternateIt = 0; alternateIt < AllowedAlternateKeyCount; alternateIt++)
        {
            string nodeName = GetActionNodeName((HandledAction) actionIt, alternateIt);

            if (configReader.ElementExists(nodeName.c_str()))
            {
                SDL_Keycode key = SDL_GetKeyFromName( configReader.ReadTextElement( nodeName.c_str() ).c_str() );

                if(key != SDLK_UNKNOWN)
                {
                    KeyboardHelper::SetKeyForAction( (HandledAction) actionIt, key, alternateIt );
                }
            }
        }
    }
}

void KeyboardHelper::WriteConfig(XmlWriter& configWriter)
{
    for(int actionIt = 0; actionIt < Count; actionIt++)
    {
        for(int alternateIt = 0; alternateIt < AllowedAlternateKeyCount; alternateIt++)
        {
            string nodeName = GetActionNodeName((HandledAction) actionIt, alternateIt);

            configWriter.WriteTextElement(
                nodeName.c_str(),
                SDL_GetKeyName(KeyboardHelper::GetKeyForAction((HandledAction) actionIt, alternateIt))
            );
        }
    }
}

string KeyboardHelper::GetActionNodeName(HandledAction action, int alternate)
{
    ostringstream oss;
    oss << actionNames[action] << alternate;
    return oss.str();
}
