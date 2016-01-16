/**
 * Implementation of the StringToX() functions for enums.
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

#include "enums.h"

CharacterDirection StringToCharacterDirection(const StringType &s)
{
    if (s == "Left")
    {
        return CharacterDirectionLeft;
    }
    else if (s == "Right")
    {
        return CharacterDirectionRight;
    }
    else
    {
        return CharacterDirectionNone;
    }
}

StringType CharacterDirectionToString(CharacterDirection d)
{
    if (d == CharacterDirectionLeft)
    {
        return "Left";
    }
    else if (d == CharacterDirectionRight)
    {
        return "Right";
    }
    else
    {
        return "None";
    }
}

CharacterPosition StringToCharacterPosition(const StringType &s)
{
    if (s == "Left")
    {
        return CharacterPositionLeft;
    }
    else if (s == "Right")
    {
        return CharacterPositionRight;
    }
    else if (s == "Unknown")
    {
        return CharacterPositionUnknown;
    }
    else if (s == "Offscreen")
    {
        return CharacterPositionOffscreen;
    }
    else
    {
        return CharacterPositionNone;
    }
}

StringType CharacterPositionToString(const CharacterPosition p)
{
    if (p == CharacterPositionLeft)
    {
        return "Left";
    }
    else if (p == CharacterPositionRight)
    {
        return "Right";
    }
    else if (p == CharacterPositionUnknown)
    {
        return "Unknown";
    }
    else if (p == CharacterPositionOffscreen)
    {
        return "Offscreen";
    }
    else
    {
        return "None";
    }
}

FieldCharacterDirection StringToFieldCharacterDirection(const StringType &s)
{
    if (s == "Up")
    {
        return FieldCharacterDirectionUp;
    }
    else if (s == "DiagonalUp")
    {
        return FieldCharacterDirectionDiagonalUp;
    }
    else if (s == "Side")
    {
        return FieldCharacterDirectionSide;
    }
    else if (s == "DiagonalDown")
    {
        return FieldCharacterDirectionDiagonalDown;
    }
    else if (s == "Down")
    {
        return FieldCharacterDirectionDown;
    }
    else
    {
        return FieldCharacterDirectionNone;
    }
}

StringType FieldCharacterDirectionToString(FieldCharacterDirection d)
{
    if (d == FieldCharacterDirectionUp)
    {
        return "Up";
    }
    else if (d == FieldCharacterDirectionDiagonalUp)
    {
        return "DiagonalUp";
    }
    else if (d == FieldCharacterDirectionSide)
    {
        return "Side";
    }
    else if (d == FieldCharacterDirectionDiagonalDown)
    {
        return "DiagonalDown";
    }
    else if (d == FieldCharacterDirectionDown)
    {
        return "Down";
    }
    else
    {
        return "None";
    }
}

FieldCharacterState StringToFieldCharacterState(const StringType &s)
{
    if (s == "Standing")
    {
        return FieldCharacterStateStanding;
    }
    else if (s == "Walking")
    {
        return FieldCharacterStateWalking;
    }
    else if (s == "Running")
    {
        return FieldCharacterStateRunning;
    }
    else
    {
        return FieldCharacterStateNone;
    }
}

FieldCustomCursorState StringToFieldCustomCursorState(const StringType &s)
{
    if (s == "None")
    {
        return FieldCustomCursorStateNone;
    }
    else if (s == "Low")
    {
        return FieldCustomCursorStateLow;
    }
    else if (s == "Mid")
    {
        return FieldCustomCursorStateMid;
    }
    else if (s == "High")
    {
        return FieldCustomCursorStateHigh;
    }
    else
    {
        return FieldCustomCursorStateExtreme;
    }
}

TransitionDirection StringToTransitionDirection(const StringType &s)
{
    if (s == "North")
    {
        return TransitionDirectionNorth;
    }
    else if (s == "NorthEast")
    {
        return TransitionDirectionNorthEast;
    }
    else if (s == "East")
    {
        return TransitionDirectionEast;
    }
    else if (s == "SouthEast")
    {
        return TransitionDirectionSouthEast;
    }
    else if (s == "South")
    {
        return TransitionDirectionSouth;
    }
    else if (s == "SouthWest")
    {
        return TransitionDirectionSouthWest;
    }
    else if (s == "West")
    {
        return TransitionDirectionWest;
    }
    else if (s == "NorthWest")
    {
        return TransitionDirectionNorthWest;
    }
    else if (s == "Door")
    {
        return TransitionDirectionDoor;
    }

    return TransitionDirectionNorth;
}
