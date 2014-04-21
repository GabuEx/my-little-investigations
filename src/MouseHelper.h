/**
 * Basic header/include file for MouseHelper.cpp.
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

#ifndef MOUSEHELPER_H
#define MOUSEHELPER_H

#include "Animation.h"
#include "EasingFunctions.h"
#include "enums.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "Vector2.h"
#include <map>
#include <vector>

using namespace std;

class MouseHelper
{
public:
    static void Init();
    static void LoadCursors();
    static void UpdateState(bool isLeftButtonDown, int mouseX, int mouseY, bool drawCursor);
    static void UpdateTiming();
    static void UpdateCursor(int delta);
    static void DrawCursor();

    static CursorType GetCursorType();
    static void SetCursorType(CursorType cursorType);
    static void ApplyCursorUpdate();

    static bool MouseOverRect(RectangleWH rectangle);
    static bool MouseDownOnRect(RectangleWH rectangle);

    static bool MouseOverPolygon(GeometricPolygon polygon);
    static bool MouseDownOnPolygon(GeometricPolygon polygon);

    static bool PressedAndHeldAnywhere();
    static bool DoublePressedAndHeldAnywhere();

    static bool ClickedOnRect(RectangleWH rectangle);
    static bool ClickedOnPolygon(GeometricPolygon polygon);
    static bool ClickedAnywhere();
    static bool DoubleClickedOnRect(RectangleWH rectangle);
    static bool DoubleClickedOnPolygon(GeometricPolygon polygon);
    static bool DoubleClickedAnywhere();

    static Vector2 GetMousePosition();
    static Vector2 GetCursorSize();

    static void HandleClick();
    static void HandleDoubleClick();

    static bool GetDrawCursor() { return MouseHelper::drawCursor; }

    static string GetMouseOverText() { return MouseHelper::mouseOverText; }
    static void SetMouseOverText(const string &mouseOverText) { MouseHelper::mouseOverText = mouseOverText; }

private:

    static bool IsMouseInRect(RectangleWH rectangle);
    static bool IsMouseInPolygon(GeometricPolygon polygon);
    static bool IsMouseInViewport();

    class MouseOverText
    {
    public:
        MouseOverText(const string &text);
        MouseOverText(const MouseOverText &other);
        ~MouseOverText();

        void Update(int delta);
        void Draw(Vector2 cursorPosition);
        void FadeOut();

        string GetText() const { return this->text; }
        bool GetIsFadingOut() const { return this->isFadingOut; }
        bool GetIsFinished();

    private:
        string text;
        double currentOffset;
        double currentOpacity;
        EasingFunction *pOffsetInEase;
        EasingFunction *pOffsetOutEase;
        EasingFunction *pOpacityInEase;
        EasingFunction *pOpacityOutEase;
        bool isFadingOut;
        Vector2 fadeOutMousePosition;
    };

    static bool clickPossible;
    static bool doubleClickPossible;
    static bool doubleClickWasPossible;
    static bool clicked;
    static bool doubleClicked;
    static bool previousWasLeftButtonDown;
    static Vector2 previousMousePosition;
    static bool drawCursor;

    static Uint32 initialLeftButtonDownTime;
    static Vector2 initialLeftButtonDownPosition;
    static Uint32 initialClickTime;

    static map<CursorType, Animation *> animationByCursorTypeMap;
    static Animation *pCurrentCursorAnimation;
    static CursorType cursorTypeToUpdateTo;
    static CursorType currentCursorType;

    static string mouseOverText;
    static vector<MouseOverText *> currentMouseOverTextList;
};

#endif
