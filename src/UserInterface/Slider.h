/**
 * Basic header/include file for Slider.cpp.
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

#ifndef SLIDER_H
#define SLIDER_H

#include "../MLIFont.h"
#include "../Image.h"

class Slider
{
public:
    static void Initialize(MLIFont *pHeaderFont, Image *pTrackSprite, Image *pThumbSprite);

    Slider(string headerText, int xPosition, int yPosition);

    int GetXPosition() const { return this->xPosition; }
    void SetXPosition(int xPosition) { this->xPosition = xPosition; }

    int GetYPosition() const { return this->yPosition; }
    void SetYPosition(int yPosition) { this->yPosition = yPosition; }

    int GetTrackXPosition();
    int GetTrackYPosition();

    double GetCurrentValue() const { return this->currentValue; }
    void SetCurrentValue(double currentValue);

    void Update(int delta);
    void Draw();
    void Draw(double opacity);
    void Reset();

private:
    static MLIFont *pHeaderFont;
    static Image *pTrackSprite;
    static Image *pThumbSprite;

    string headerText;
    int xPosition;
    int yPosition;

    bool isDragging;
    int sliderXPosition;

    double currentValue;
};

#endif
