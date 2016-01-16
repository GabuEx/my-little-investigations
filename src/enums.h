/**
 * Enums for use in the game.
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

#ifndef ENUMS_H
#define ENUMS_H

#ifdef CASE_CREATOR
#include <QString>

typedef QString StringType;
#else
#include <string>

using namespace std;

typedef string StringType;
#endif

enum DirectNavigationDirection
{
    DirectNavigationDirectionNone,
    DirectNavigationDirectionBack,
    DirectNavigationDirectionForward,
};

enum CharacterPosition
{
    CharacterPositionNone,
    CharacterPositionUnknown,
    CharacterPositionOffscreen,
    CharacterPositionLeft,
    CharacterPositionRight,
};

enum CharacterDirection
{
    CharacterDirectionLeft = 0,
    CharacterDirectionRight = 1,
    CharacterDirectionNone = -1,
};

enum FieldCharacterDirection
{
    FieldCharacterDirectionUp = 0,
    FieldCharacterDirectionDiagonalUp = 1,
    FieldCharacterDirectionSide = 2,
    FieldCharacterDirectionDiagonalDown = 3,
    FieldCharacterDirectionDown = 4,
    FieldCharacterDirectionCount = 5,
    FieldCharacterDirectionNone = -1,
};

enum FieldCharacterState
{
    FieldCharacterStateStanding = 0,
    FieldCharacterStateWalking = 1,
    FieldCharacterStateRunning = 2,
    FieldCharacterStateNone = -1,
};

enum FadeAction
{
    FadeActionPause,
    FadeActionResume,
    FadeActionStop,
};

enum ArrowDirection
{
    ArrowDirectionUp,
    ArrowDirectionDown,
    ArrowDirectionLeft,
    ArrowDirectionRight,
};

enum TabRow
{
    TabRowTop,
    TabRowDialog,
    TabRowBottom,
};

enum CursorType
{
    CursorTypeNormal,
    CursorTypeLook,
    CursorTypeTalk,
    CursorTypeExitNorth,
    CursorTypeExitNorthEast,
    CursorTypeExitEast,
    CursorTypeExitSouthEast,
    CursorTypeExitSouth,
    CursorTypeExitSouthWest,
    CursorTypeExitWest,
    CursorTypeExitNorthWest,
    CursorTypeExitDoor,
    CursorTypeNoExit,
    CursorTypeCustom,
};

enum FieldCustomCursorState
{
    FieldCustomCursorStateOff,
    FieldCustomCursorStateNone,
    FieldCustomCursorStateLow,
    FieldCustomCursorStateMid,
    FieldCustomCursorStateHigh,
    FieldCustomCursorStateExtreme,
};

enum TransitionDirection
{
    TransitionDirectionNorth,
    TransitionDirectionNorthEast,
    TransitionDirectionEast,
    TransitionDirectionSouthEast,
    TransitionDirectionSouth,
    TransitionDirectionSouthWest,
    TransitionDirectionWest,
    TransitionDirectionNorthWest,
    TransitionDirectionDoor,
};

enum ManagerSource
{
    ManagerSourceCaseFile,
    ManagerSourceCommonResources,
};

enum SelectionScreenType
{
    SelectionScreenTypeCaseSelection,
    SelectionScreenTypeSaveGame,
    SelectionScreenTypeLoadGame,
};

enum HAlignment
{
    HAlignmentLeft,
    HAlignmentCenter,
    HAlignmentRight
};

enum VAlignment
{
    VAlignmentTop,
    VAlignmentCenter,
    VAlignmentBottom
};

CharacterDirection StringToCharacterDirection(const StringType &s);
StringType CharacterDirectionToString(CharacterDirection d);
CharacterPosition StringToCharacterPosition(const StringType &s);
StringType CharacterPositionToString(const CharacterPosition p);
FieldCharacterDirection StringToFieldCharacterDirection(const StringType &s);
StringType FieldCharacterDirectionToString(FieldCharacterDirection d);
FieldCharacterState StringToFieldCharacterState(const StringType &s);
FieldCustomCursorState StringToFieldCustomCursorState(const StringType &s);
TransitionDirection StringToTransitionDirection(const StringType &s);

#endif
