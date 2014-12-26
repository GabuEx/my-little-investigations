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
#include <deque>

#include "globals.h"
#include "Color.h"
#include "Rectangle.h"
#include "Image.h"
#include "Vector2.h"
#include "Cache.h"

using namespace std;

class MLIFont
{
public:
    MLIFont(const string &ttfFilePath, int fontSize, int strokeWidth, bool invertedColors);

#ifdef GAME_EXECUTABLE
    MLIFont(const string &fontId, int strokeWidth, bool invertedColors);
#endif

    ~MLIFont();

    void Reinit();
    void Draw(const string &s, Vector2 position);
    void Draw(const string &s, Vector2 position, double scale);
    void Draw(const string &s, Vector2 position, Color color);
    void Draw(const string &s, Vector2 position, Color color, double scale);
    void Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect);
    void Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect, double scale);

    double GetWidth(const string &s);
    double GetHeight(const string &s);
    double GetLineHeight();
    double GetLineAscent();
    double GetLineDescent();

private:
    void DrawInternal(const string &s, Vector2 position, Color color, double scale, RectangleWH clipRect);
    Image * RenderGlyph(uint32_t c);
    int GetKernedWidth(uint32_t c1, uint32_t c2);

    TTF_Font *pTtfFont;
    void *pTtfFontMem;
    SDL_sem *pAccessSemaphore;

    class CacheItemHandler : public MRUCache<uint32_t, Image *>::ItemHandler
    {
    public:
        CacheItemHandler(MLIFont *pMLIFont) : ItemHandler(), pMLIFont(pMLIFont) {}
        void releaseItem(const uint32_t &key, Image * const &value) { delete value; }
        Image * newItem(const uint32_t &key) { return pMLIFont->RenderGlyph((uint32_t)key); }
    private:
        MLIFont *pMLIFont;
    };

    string ttfFilePath;
    int fontSize;
    int strokeWidth;
    bool isBold;

    MRUCache<uint32_t, Image *> cache;
    map<pair<uint32_t, uint32_t>, int> kernedWidthCache;

    bool GetIsFullscreen()
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
        if (scale != (GetIsFullscreen() ? GetScreenScale() : 1.0))
        {
            Reinit();
        }
    }

    double GetFontScale()
    {
        return scale;
    }

    // With what scale font have been rendered. We need this to rerender font with right size
    // when switch fullscreen/window.
    double scale;

    bool invertedColors;
};

#endif
