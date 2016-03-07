/**
 * Basic header/include file for TextButton.cpp.
 *
 * @author mad-mix
 * @since 1.0.7
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

#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include <deque>
#include <string>

#include "../MLIFont.h"
#include "../EasingFunctions.h"
#include "../LocalizableContent.h"
#include "../UserInterface/TextWidget.h"

using namespace std;

class TextButton : public ILocalizableTextOwner
{
public:
    static void Initialize(Image *pCheckMarkImage, Image *pBoxImage);

    TextButton(const string &textId, MLIFont *pFont, bool checkable = false);
    TextButton(const TextButton &rhs);

    virtual ~TextButton();

    void Update(int delta);
    void Draw() const;
    bool IsReady();
    void Reset();

    bool GetCheckable() const { return this->checkable; }
    void SetCheckable(bool checkable) { this->checkable = checkable; }

    const string & GetText() const { return this->textWidget.GetText(); }
    void SetTextId(const string &textId) { this->textWidget.SetTextId(textId); }

    double GetWidth() const { return this->width; }
    void SetWidth(double width) { this->width = width; }

    double GetHeight() const { return this->height; }
    void SetHeight(double height) { this->height = height; }

    double GetX() const { return this->x; }
    double GetLeft() const;
    void SetX(double x, HAlignment alignment = HAlignmentLeft) { this->x = x; this->hAlignment = alignment; }

    double GetY() const { return this->y; }
    double GetTop() const { return this->y - GetHeight() / 2; }
    void SetY(double y) { this->y = y + GetHeight() / 2; }

    MLIFont * GetFont() const { return this->pFont; }
    void SetFont(MLIFont *pFont) { this->pFont = pFont; }

    double GetOpacity() const { return this->opacity; }
    void SetOpacity(double opacity) { this->opacity = opacity; textWidget.SetOpacity(opacity); }

    const Color & GetTextColor() const { return this->textColor; }
    void SetTextColor(const Color &textColor) { this->textColor = textColor; }

    MLIFont * GetDisabledFont() const { return this->pDisabledFont == NULL ? this->pFont : this->pDisabledFont; }
    void SetDisabledFont(MLIFont *pDisabledFont) { this->pDisabledFont = pDisabledFont; }

    const Color & GetDisabledTextColor() const { return this->disabledTextColor; }
    void SetDisabledTextColor(const Color &disabledTextColor) { this->disabledTextColor = disabledTextColor; }

    bool GetChecked() const { return this->checked; }
    void SetChecked(bool checked) { this->checked = checked; }

    bool GetIsEnabled() const { return this->enabled; }
    void SetIsEnabled(bool enabled) { this->enabled = enabled; }

    const string & GetClickSoundEffect() { return this->clickSoundEffect; }
    void SetClickSoundEffect(const string &clickSoundEffect) { this->clickSoundEffect = clickSoundEffect; }

    void MoveTo(double newX, HAlignment hAlignment, double newY, int timeMS);
    void Reappear(double newX, HAlignment hAlignment, double newY, int timeMS);

    void SetMaxWidth(double maxWidth);

    void ReloadLocalizableText() override;

private:
    bool IsAnimationPlaying() const;
    void UpdateAnimation(int delta);

    void OnClicked();
    MLIFont *GetDrawingFont() const;
    Color GetDrawingColor() const;
    void DrawInternal(double x, double y, double opacity, bool enabled) const;

    void UpdateSize();

    double x;
    HAlignment hAlignment;
    double y;
    double width;
    double height;
    double opacity;

    TextWidget textWidget;

    MLIFont *pFont;
    MLIFont *pDisabledFont;

    Color textColor;
    Color disabledTextColor;
    Color mouseDownTextColor;
    Color mouseOverTextColor;

    bool checkable;
    bool checked;
    bool enabled;

    static Image *pCheckMarkImage;
    static Image *pBoxImage;

    bool isMouseOver;
    bool isMouseDown;

    string clickSoundEffect;

    EasingFunction *pXEase;
    HAlignment transitionHAlignment;
    EasingFunction *pYEase;
    EasingFunction *pFadeInEase;
    EasingFunction *pFadeOutEase;

    TextButton *pOldButton;
};

#endif // TEXTBUTTON_H
