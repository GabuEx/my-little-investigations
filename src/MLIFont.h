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
#include <vector>
#include <set>

#include "globals.h"
#include "Color.h"
#include "Rectangle.h"
#include "Image.h"
#include "Vector2.h"

using namespace std;

class MLIFont
{
public:
    MLIFont(const string &ttfFilePath, int fontSize, int strokeWidth = 0, bool isBold = false);
    ~MLIFont();

    void Reinit();
    void Reinit(const vector<pair<uint32_t, uint32_t> > &ranges);
    void Draw(const string &s, Vector2 position);
    void Draw(const string &s, Vector2 position, double scale);
    void Draw(const string &s, Vector2 position, Color color);
    void Draw(const string &s, Vector2 position, Color color, double scale);
    void Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect);
    void Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect, double scale);

    int GetWidth(const string &s);
    int GetKerningDelta(map<pair<uint32_t, uint32_t>, int> *pKernedWidthMap, map<uint32_t, RectangleWH> *pClipRectMap, uint32_t char1, uint32_t char2);
    int GetHeight(const string &s);
    int GetLineHeight();
    int GetLineAscent();

private:
    void DrawInternal(const string &s, Vector2 position, Color color, RectangleWH clipRect, double scale, map<pair<uint32_t, uint32_t>, int> *pKernedWidthMap, map<uint32_t, RectangleWH> *pClipRectMap, map<uint32_t, RectangleWH> *pClipRectMapForWidth);

    TTF_Font *pTtfFont;
    int strokeWidth;

    Image *pTextSpriteSheet;

    map<uint32_t, RectangleWH> renderedTextClipRectMap;
    map<pair<uint32_t, uint32_t>, int> charPairToKernedWidthMap;

    set<uint32_t> charsToRender;

    string ttfFilePath;
    int fontSize;
    bool isBold;

    double GetIsFullscreen()
    {
        #ifdef GAME_EXECUTABLE
        return gIsFullscreen;
        #else
        return false;
        #endif // GAME_EXECUTABLE
    }

    double GetScreenScale()
    {
        #ifdef GAME_EXECUTABLE
        return gScreenScale;
        #else
        return 1.0;
        #endif // GAME_EXECUTABLE
    }

    void CheckScale()
    {
        // although such method cause some lags when switch fullscreen/window during the game,
        // i think it's acceptable price for properly scaled font
        if (scale != (GetIsFullscreen() ? GetScreenScale() : 1.0))
            Reinit();
    }

    double GetFontScale()
    {
        return scale;
    }

    // With what scale font have been rendered. We need this to rerender font with right size
    // when switch fullscreen/window.
    double scale;
};

#endif
