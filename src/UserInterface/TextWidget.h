/**
 * Basic header/include file for TextWidget.cpp.
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

#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <string>
#include <algorithm>

#include "../enums.h"
#include "../MLIFont.h"
#include "../Utils.h"

using namespace std;
class TextWidget
{
public:

    TextWidget(const string &text = "", MLIFont *pFont = NULL, Color textColor = Color(1.0, 0.0, 0.0, 0.0),
               HAlignment hAlignment = HAlignmentLeft, VAlignment vAlignment = VAlignmentTop);
    ~TextWidget();

    void Draw() const { Draw(x, y); }
    void Draw(double x, double y) const;
    void Update(int delta) { }
    bool IsReady() const { return true; }

    const string & GetText() const { return this->text; }
    void SetText(const string &text);

    MLIFont * GetFont() const { return this->pFont; }
    void SetFont(MLIFont *pFont) { this->pFont = pFont; }

    const Color & GetTextColor() const { return this->textColor; }
    void SetTextColor(const Color &textColor) { this->textColor = textColor; }

    HAlignment GetTextAlignment() const { return this->hAlignment; }
    void SetTextAlignment(HAlignment textAlignment) { this->hAlignment = textAlignment; }

    double GetOpacity() const { return this->textColor.GetA(); }
    void SetOpacity(double opacity) { return this->textColor.SetA(opacity); }

    double GetX() const { return this->x; }
    void SetX(double x) { this->x = x; }

    double GetY() const { return this->y; }
    void SetY(double y) { this->y = y; }

    double GetWidth() const { return this->width; }
    void SetWidth(double width) { this->width = width; }

    double GetHeight() const { return this->height; }
    void SetHeight(double height) { this->height = height; }

    void WrapText() { WrapText(GetWidth()); }
    void WrapText(double width);

    void FitSizeToContent();

private:

    double GetTextHeight() const;
    double GetTextWidth() const;
    void SplitText(bool wrap, double maxWidth);

    string text;
    MLIFont *pFont;
    Color textColor;
    HAlignment hAlignment;
    VAlignment vAlignment;

    double x;
    double y;
    double width;
    double height;

    vector<pair<string::const_iterator, string::const_iterator> > lines;
};

#endif // TEXTWIDGET_H
