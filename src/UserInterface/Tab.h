/**
 * Basic header/include file for Tab.cpp.
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

#ifndef TAB_H
#define TAB_H

#include "../Animation.h"
#include "../EasingFunctions.h"
#include "../enums.h"
#include "../MLIFont.h"
#include "../Rectangle.h"
#include "../Image.h"
#include "../Sprite.h"
#include <string>

using namespace std;

const int TabWidth = 183; // px
const int TabHeight = 38; // px

class Tab
{
public:
    static void Initialize(int dialogRowYPosition, int bottomRowYPosition, Image *pDownImage, Image *pUpImage, MLIFont *pFont);

    Tab();
    Tab(int xPosition, bool isClickable);
    Tab(int xPosition, bool isClickable, string text);
    Tab(int xPosition, bool isClickable, string text, bool useCancelClickSoundEffect);
    Tab(int xPosition, bool isClickable, string text, bool useCancelClickSoundEffect, TabRow row);
    Tab(int xPosition, bool isClickable, string text, bool useCancelClickSoundEffect, TabRow row, bool canPulse);

    ~Tab();

    string GetText() const { return this->text; }
    void SetText(string text) { this->text = text; }

    bool GetIsEnabled() const { return this->isEnabled; }
    void SetIsEnabled(bool isEnabled) { this->isEnabled = isEnabled; }

    bool GetIsClicked() const { return this->isClicked; }
    void SetIsClicked(bool isClicked) { this->isClicked = isClicked; }

    bool GetIsHidden() const { return this->isHidden; }
    void SetIsHidden(bool isHidden, bool shouldAnimate = true);

    bool SetIsPulsing() const { return this->isPulsing; }
    void SetIsPulsing(bool isPulsing);

    bool GetUseCancelClickSoundEffect() const { return this->useCancelClickSoundEffect; }
    void SetUseCancelClickSoundEffect(bool useCancelClickSoundEffect) { this->useCancelClickSoundEffect = useCancelClickSoundEffect; }

    string GetClickSoundEffect();

    Sprite * GetTabImage() { return this->pTabImage; }
    void SetTabImage(Sprite *pTabImage) { this->pTabImage = pTabImage; }

    void UpdatePosition(int delta);
    void Update();
    void Update(int delta);
    void Draw();
    void Draw(double xOffset, double yOffset);
    void Reset();

private:
    void Init(int xPosition, bool isClickable, string text, bool useCancelClickSoundEffect, TabRow row, bool canPulse);
    int GetYPositionFromRow() const;
    bool GetShouldShowPulse();

    static int dialogRowYPosition;
    static int bottomRowYPosition;
    static Image *pDownSprite;
    static Image *pUpSprite;
    static int width;
    static int height;
    static int textAreaWidth;
    static int textAreaHeight;
    static MLIFont *pFont;

    int xPosition;
    TabRow row;
    RectangleWH areaRect;

    bool isClickable;
    bool isMouseOver;
    bool isMouseDown;

    string clickSoundEffect;

    string text;

    Sprite *pTabImage;

    bool isEnabled;
    bool isClicked;
    bool useCancelClickSoundEffect;
    bool isHidden;
    bool isPulsing;
    bool wasPulsing;

    int hideAnimationOffset;
    EasingFunction *pTopRowHideEase;
    EasingFunction *pTopRowShowEase;
    EasingFunction *pBottomRowHideEase;
    EasingFunction *pBottomRowShowEase;
    EasingFunction *pCurrentEasingFunction;

    static int pulseAnimationCount;
    Animation *pPulseAnimation;
};

#endif
