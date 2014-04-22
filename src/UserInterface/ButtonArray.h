/**
 * Basic header/include file for ButtonArray.cpp.
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

#ifndef BUTTONARRAY_H
#define BUTTONARRAY_H

#include "Arrow.h"
#include "Tab.h"
#include "../MLIFont.h"
#include "../Rectangle.h"
#include "../Image.h"
#include "../Sprite.h"
#include "../Events/ButtonEventProvider.h"

const int AnimationDuration = 300;
const int TextHeight = 29;

class Button
{
public:
    static void Initialize(MLIFont *pTextFont, Image *pCheckMarkImage);

    Button(int id, const string &text);

    ~Button();

    int GetId() const { return this->id; }

    int GetXPosition() const { return this->xPosition; }
    void SetXPosition(int xPosition) { this->xPosition = xPosition; }

    int GetYPosition() const { return this->yPosition; }
    void SetYPosition(int yPosition) { this->yPosition = yPosition; }

    static string GetClickSoundEffect() { return "ButtonClick3"; }

    string GetCustomIconId() const { return this->customIconId; }
    void SetCustomIconId(const string &customIconId);

    bool GetIsHidden() const { return this->isHidden; }
    void SetIsHidden(bool isHidden) { this->isHidden = isHidden; }

    bool GetIsHiding() const { return this->isHiding; }
    void SetIsHiding(bool isHiding) { this->isHiding = isHiding; }

    string GetText() const { return this->text; }

    int GetInAnimationDelay() { return this->pInEase->GetAnimationStartDelay(); }
    void SetInAnimationDelay(int value) { this->pInEase->SetAnimationStartDelay(value); }

    int GetOutAnimationDelay() { return this->pOutEase->GetAnimationStartDelay(); }
    void SetOutAnimationDelay(int value) { this->pOutEase->SetAnimationStartDelay(value); }

    bool GetShowCheckMark() const { return this->showCheckMark; }
    void SetShowCheckMark(bool showCheckMark) { this->showCheckMark = showCheckMark; }

    int GetLockCount() const { return this->lockCount; }
    void SetLockCount(int lockCount) { this->lockCount = lockCount; }

    int GetUnlockedLockCount() const { return this->unlockedLockCount; }
    void SetUnlockedLockCount(int unlockedLockCount) { this->unlockedLockCount = unlockedLockCount; }

    bool GetIsDisabled() const { return this->isDisabled; }
    void SetIsDisabled(bool isDisabled) { this->isDisabled = isDisabled; }

    bool GetIsReady();

    void Show();
    void ShowInstantly();
    void Hide();
    void Update(int delta);
    void Draw();
    void Draw(double xOffset, double yOffset);
    void Reset();

private:
    void OnClicked();

    static MLIFont *pTextFont;
    static Image *pCheckMarkImage;
    static Sprite *pLockSprite;
    static Animation *pUnlockingAnimation;

    int id;
    string customIconId;
    Sprite *pCustomIconSprite;

    bool isMouseOver;
    bool isMouseDown;

    EasingFunction *pInEase;
    EasingFunction *pOutEase;
    int animationOffset;

    int xPosition;
    int yPosition;

    bool isHidden;
    bool isHiding;
    string text;

    bool showCheckMark;
    int lockCount;
    int unlockedLockCount;
    bool isDisabled;
};

class ButtonArrayLoadParameters
{
public:
    string text;
    bool isDisabled;
    bool isLocked;
    bool showCheckBox;
    int lockCount;
    int unlockedLockCount;
    string customIconId;

    ButtonArrayLoadParameters()
        : text("")
        , isDisabled(false)
        , isLocked(false)
        , showCheckBox(false)
        , lockCount(0)
        , unlockedLockCount(0)
        , customIconId("")
    {
    }
};

class ButtonArray : public ButtonEventListener
{
public:
    static void Initialize(MLIFont *pTextFont);

    ButtonArray();
    ButtonArray(int maxVisibleButtonCount, int x, int y, int width, int height, int desiredPadding);
    virtual ~ButtonArray();

    bool GetIsCancelable() const { return this->isCancelable; }
    void SetIsCancelable(bool isCancelable) { this->isCancelable = isCancelable; }

    int GetCount() const { return (int)buttonList.size(); }
    bool GetIsClosed();

    bool GetIsReady();

    void Load(vector<ButtonArrayLoadParameters> loadParametersList);
    void Show();
    void Close();
    void Update(int delta);
    void Draw();
    void Draw(double xOffset, double yOffset);
    void ReorderOutAnimations(int newFirstOutButtonId);
    void Reset();

    void OnButtonClicked(Button *pButton, int id);

private:
    void UpdateButtons();
    void OnCanceled();

    static MLIFont *pTextFont;

    RectangleWH dialogRect;
    int desiredPadding;

    vector<Button *> buttonList;
    bool buttonClicked;
    unsigned int topButtonIndex;
    Tab *pBackTab;

    Button **ppVisibleButtons;
    unsigned int maxVisibleButtonCount;
    unsigned int visibleButtonCount;
    Arrow *pUpArrow;
    Arrow *pDownArrow;

    bool isCancelable;
};

#endif
