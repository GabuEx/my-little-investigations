/**
 * Basic header/include file for Font.cpp.
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

#ifndef MLIFONT_H
#define MLIFONT_H

#ifdef __OSX
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL2/SDL_ttf.h>
#endif
#include <map>

#include "Color.h"
#include "Rectangle.h"
#include "Image.h"
#include "Vector2.h"

using namespace std;

class MLIFont
{
public:
    MLIFont(string ttfFilePath, int fontSize, int strokeWidth = 0, bool isBold = false);
    ~MLIFont();

    void Reinit();
    void Draw(string s, Vector2 position);
    void Draw(string s, Vector2 position, double scale);
    void Draw(string s, Vector2 position, Color color);
    void Draw(string s, Vector2 position, Color color, double scale);
    void Draw(string s, Vector2 position, Color color, RectangleWH clipRect);
    void Draw(string s, Vector2 position, Color color, RectangleWH clipRect, double scale);

    int GetWidth(string s);
    int GetKerningDelta(map<string, int> *pKernedWidthMap, map<char, RectangleWH> *pClipRectMap, char c1, char c2);
    int GetHeight(string s);
    int GetLineHeight();
    int GetLineAscent();

private:
    void DrawInternal(string s, Vector2 position, Color color, RectangleWH clipRect, double scale, map<string, int> *pKernedWidthMap, map<char, RectangleWH> *pClipRectMap, map<char, RectangleWH> *pClipRectMapForWidth);

    TTF_Font *pTtfFont;
    int strokeWidth;

    Image *pTextSpriteSheet;

    map<char, RectangleWH> pRenderedTextClipRectMap;
    map<char, RectangleWH> pRenderedTextOutlineClipRectMap;
    map<string, int> charPairToKernedWidthMap;
};

#endif
