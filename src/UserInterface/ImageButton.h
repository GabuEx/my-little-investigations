/**
 * Basic header/include file for ImageButton.cpp.
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

#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include "../Image.h"
#include <string>

using namespace std;

class ImageButton
{
public:
    ImageButton(Image *pMouseOffImage, Image *pMouseOverImage, Image *pMouseDownImage, int xPosition, int yPosition);
    ~ImageButton();

    int GetXPosition() const { return this->xPosition; }
    void SetXPosition(int xPosition) { this->xPosition = xPosition; }

    int GetYPosition() const { return this->yPosition; }
    void SetYPosition(int yPosition) { this->yPosition = yPosition; }

    string GetClickSoundEffect() { return this->clickSoundEffect; }
    void SetClickSoundEffect(const string &clickSoundEffect) { this->clickSoundEffect = clickSoundEffect; }

    bool GetIsEnabled() { return this->isEnabled; }
    void SetIsEnabled(bool isEnabled) { this->isEnabled = isEnabled;}

    void Update(int delta);
    void Draw();
    void Draw(Vector2 offset);
    void Draw(double opacity);
    void Draw(Vector2 offset, double opacity);
    void Reset();

    bool IsReady();

private:
    Image * GetCurrentSprite();
    void OnClicked();

    Image *pMouseOffImage;
    Image *pMouseOverImage;
    Image *pMouseDownImage;

    string clickSoundEffect;

    bool isMouseOver;
    bool isMouseDown;

    int xPosition;
    int yPosition;

    bool isEnabled;
};

#endif
