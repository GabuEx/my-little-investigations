/**
 * Draws a given font as needed in the game.
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

#include "MLIFont.h"
#include "globals.h"
#include "ResourceLoader.h"
#include "AutoSemaphore.h"
#include "utf8cpp/utf8.h"

#include <iostream>

const unsigned int CacheSize = 256;

MLIFont::MLIFont(const string &ttfFilePath, int fontSize, int strokeWidth, bool invertedColors)
    : ttfFilePath(ttfFilePath),
      fontSize(fontSize),
      strokeWidth(strokeWidth),
      cache(CacheSize, new CacheItemHandler(this)),
      invertedColors(invertedColors)
{
    pTtfFont = NULL;
    pAccessSemaphore = SDL_CreateSemaphore(1);

    Reinit();
}

#ifdef GAME_EXECUTABLE
MLIFont::MLIFont(const string &fontId, int strokeWidth, bool invertedColors)
    : strokeWidth(strokeWidth),
      cache(CacheSize, new CacheItemHandler(this)),
      invertedColors(invertedColors)
{
    ttfFilePath = pgLocalizableContent->GetFont(fontId).Filename;
    fontSize = pgLocalizableContent->GetFont(fontId).Size;

    pTtfFont = NULL;
    pAccessSemaphore = SDL_CreateSemaphore(1);

    Reinit();
}
#endif

MLIFont::~MLIFont()
{
    if (pTtfFont != NULL)
    {
        TTF_CloseFont(pTtfFont);
    }
    pTtfFont = NULL;

    SDL_DestroySemaphore(pAccessSemaphore);
    pAccessSemaphore = NULL;
}

void MLIFont::Reinit()
{
    // clean up first

    if (pTtfFont != NULL)
        TTF_CloseFont(pTtfFont);
    pTtfFont = NULL;
    cache.clear();
    kernedWidthCache.clear();

    // setup scale
    scale = (GetIsFullscreen() ? GetScreenScale() : 1.0);

    // setup font
    #ifdef GAME_EXECUTABLE
        pTtfFont = ResourceLoader::GetInstance()->LoadFont(ttfFilePath, fontSize * scale);
    #else
        pTtfFont = TTF_OpenFont(ttfFilePath.c_str(), fontSize * scale + 0.5);
    #endif
}

Image *MLIFont::RenderGlyph(uint32_t c)
{
    CheckScale();

    // render char
    SDL_Color whiteColor = {255, 255, 255, 255};
    if (invertedColors)
    {
        whiteColor.b = 255 - whiteColor.b;
        whiteColor.g = 255 - whiteColor.g;
        whiteColor.a = 255 - whiteColor.a;
    }

    string utf8string;
    utf8::unchecked::append(c, back_inserter(utf8string));

    SDL_Surface *pSurface = TTF_RenderUTF8_Blended(pTtfFont, utf8string.c_str(), whiteColor);
    if (pSurface == NULL)
    {
        return NULL;
    }

    int scaledStrokeWidth = strokeWidth * GetFontScale() + 0.5;

    // render outlines
    if (strokeWidth > 0)
    {
        SDL_Color blackColor = {0, 0, 0, 255};
        if (invertedColors)
        {
            blackColor.b = 255 - blackColor.b;
            blackColor.g = 255 - blackColor.g;
            blackColor.a = 255 - blackColor.a;
        }

#ifndef MLI_SDL_FONT_OUTLINING
        SDL_Surface *pSurfaceOutline = TTF_RenderUTF8_Blended(pTtfFont, utf8string.c_str(), blackColor);

        SDL_Surface *pSurfaceOutlinedText = SDL_CreateRGBSurface(
                    0,
                    pSurface->w + scaledStrokeWidth * 2,
                    pSurface->h + scaledStrokeWidth * 2,
                    pSurface->format->BitsPerPixel,
                    pSurface->format->Rmask,
                    pSurface->format->Gmask,
                    pSurface->format->Bmask,
                    pSurface->format->Amask);

        SDL_SetSurfaceBlendMode(pSurfaceOutline, SDL_BLENDMODE_BLEND);

        SDL_Rect dstRect = {0, 0, pSurface->w, pSurface->h};

        for (dstRect.x = 0; dstRect.x <= scaledStrokeWidth * 2; dstRect.x++)
        {
            for (dstRect.y = 0; dstRect.y <= scaledStrokeWidth * 2; dstRect.y++)
            {
                SDL_BlitSurface(pSurfaceOutline, NULL, pSurfaceOutlinedText, &dstRect);
            }
        }

        dstRect.x = scaledStrokeWidth;
        dstRect.y = scaledStrokeWidth;
        SDL_SetSurfaceBlendMode(pSurfaceOutline, SDL_BLENDMODE_BLEND);
        SDL_BlitSurface(pSurface, NULL, pSurfaceOutlinedText, &dstRect);

        SDL_FreeSurface(pSurface);
        SDL_FreeSurface(pSurfaceOutline);

        pSurface = pSurfaceOutlinedText;
#else
        TTF_SetFontOutline(pTtfFont, scaledStrokeWidth);

        SDL_Surface *pSurfaceOutlinedText = TTF_RenderUTF8_Blended(pTtfFont, utf8string.c_str(), blackColor);

        SDL_SetSurfaceBlendMode(pSurfaceOutlinedText, SDL_BLENDMODE_BLEND);
        SDL_Rect dstRect = {scaledStrokeWidth, scaledStrokeWidth, pSurface->w, pSurface->h};
        SDL_BlitSurface(pSurface, NULL, pSurfaceOutlinedText, &dstRect);

        SDL_FreeSurface(pSurface);

        pSurface = pSurfaceOutlinedText;

        TTF_SetFontOutline(pTtfFont, 0);
#endif
    }

    // create image
    Image *pImage = Image::Load(pSurface, true);
    pImage->FlagFontSource(this);
    pImage->SetUseScreenScaling(false);

    return pImage;
}

int MLIFont::GetKernedWidth(uint32_t c1, uint32_t c2)
{
    CheckScale();

    pair<uint32_t, uint32_t> charPair(c1, c2);

    map<pair<uint32_t, uint32_t>, int>::const_iterator it = kernedWidthCache.find(charPair);
    if (it != kernedWidthCache.end())
    {
        return it->second;
    }
    else
    {
        int h;
        int combinedWidth, w2;
        string str1, str2;
        utf8::unchecked::append(c1, back_inserter(str1));
        utf8::unchecked::append(c2, back_inserter(str2));

        TTF_SizeUTF8(pTtfFont, str2.c_str(), &w2, &h);
        TTF_SizeUTF8(pTtfFont, (str1 + str2).c_str(), &combinedWidth, &h);

        int kernedWidth1 = (combinedWidth - w2) + strokeWidth/* * 2*/;
        kernedWidthCache[charPair] = kernedWidth1;
        return kernedWidth1;
    }
}

void MLIFont::Draw(const string &s, Vector2 position)
{
    DrawInternal(s, position, Color::White, 1.0, RectangleWH(0, 0, -1, -1));
}

void MLIFont::Draw(const string &s, Vector2 position, double scale)
{
    DrawInternal(s, position, Color::White, scale, RectangleWH(0, 0, -1, -1));
}

void MLIFont::Draw(const string &s, Vector2 position, Color color)
{
    DrawInternal(s, position, color, 1.0, RectangleWH(0, 0, -1, -1));
}

void MLIFont::Draw(const string &s, Vector2 position, Color color, double scale)
{
    DrawInternal(s, position, color, scale, RectangleWH(0, 0, -1, -1));
}

void MLIFont::Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect)
{
    DrawInternal(s, position, color, 1.0, clipRect);
}

void MLIFont::Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect, double scale)
{
    DrawInternal(s, position, color, scale, clipRect);
}

void MLIFont::DrawInternal(const string &s, Vector2 position, Color color, double scale, RectangleWH clipRect)
{
    AutoSemaphore sem(pAccessSemaphore);

    // If we're trying to draw an empty string, we can just return -
    // we're not gonna draw anything anyhow.
    if (s.length() == 0)
    {
        return;
    }

    CheckScale();

    double x = position.GetX();
    double y = position.GetY();

    for (string::const_iterator it = s.begin(); it < s.end();)
    {
        uint32_t c = 0;
        try
        {
            c = utf8::next(it, s.end());
        }
        catch (utf8::not_enough_room ex)
        {
            break;
        }

        Image *pGlyphImage = cache[c];
        if (pGlyphImage == NULL)
        {
            continue;
        }

        RectangleWH characterClipRect(0, 0, pGlyphImage->width / GetFontScale(), pGlyphImage->height / GetFontScale());
        RectangleWH originalCharacterClipRect = characterClipRect;

        if (clipRect.GetWidth() < 0 || clipRect.GetX() < originalCharacterClipRect.GetWidth())
        {
            if (clipRect.GetWidth() >= 0)
            {
                if (clipRect.GetX() > 0)
                {
                    characterClipRect.SetX(originalCharacterClipRect.GetX() + clipRect.GetX());
                    characterClipRect.SetWidth(originalCharacterClipRect.GetWidth() - clipRect.GetX());
                }

                if (clipRect.GetWidth() < characterClipRect.GetWidth())
                {
                    characterClipRect.SetWidth(clipRect.GetWidth());
                }

                if (clipRect.GetY() > 0)
                {
                    characterClipRect.SetY(min(originalCharacterClipRect.GetY() + clipRect.GetY(), originalCharacterClipRect.GetY() + originalCharacterClipRect.GetHeight()));
                    characterClipRect.SetHeight(originalCharacterClipRect.GetHeight() - (characterClipRect.GetY() - originalCharacterClipRect.GetY()));
                }

                if (clipRect.GetHeight() < characterClipRect.GetHeight())
                {
                    characterClipRect.SetHeight(clipRect.GetHeight());
                }
            }

            if (characterClipRect.GetWidth() > 0 && characterClipRect.GetHeight() > 0)
            {
                characterClipRect.SetHeight(characterClipRect.GetHeight() * GetFontScale());
                characterClipRect.SetWidth(characterClipRect.GetWidth() * GetFontScale());
                pGlyphImage->Draw(Vector2(x, y), characterClipRect, false, false, scale, color);
            }
        }

        double deltaX = pGlyphImage->width;

        if (it < s.end())
        {
            try
            {
                uint32_t c2 = utf8::peek_next(it, s.end());
                deltaX = GetKernedWidth(c, c2);
            }
            catch (utf8::not_enough_room ex)
            {
            }
        }

        x += deltaX / GetFontScale();
    }
}

double MLIFont::GetWidth(const string &s)
{
    AutoSemaphore sem(pAccessSemaphore);
    CheckScale();

    double x = 0;
    for (string::const_iterator it = s.begin(); it < s.end();)
    {
        uint32_t c = 0;
        try
        {
            c = utf8::next(it, s.end());
        }
        catch (utf8::not_enough_room ex)
        {
            break;
        }

        Image *pGlyphImage = cache[c];
        if (pGlyphImage == NULL)
        {
            continue;
        }

        double deltaX = pGlyphImage->width;

        if (it < s.end())
        {
            try
            {
                uint32_t c2 = utf8::peek_next(it, s.end());
                deltaX = GetKernedWidth(c, c2);
            }
            catch (utf8::not_enough_room ex)
            {
            }
        }

        x += deltaX / GetFontScale();
    }

    return x;
}

double MLIFont::GetHeight(const string &s)
{
    AutoSemaphore sem(pAccessSemaphore);
    CheckScale();
    int w, h;

    TTF_SizeUTF8(pTtfFont, s.c_str(), &w, &h);
    return (double)h / GetFontScale();
}

double MLIFont::GetLineHeight()
{
    AutoSemaphore sem(pAccessSemaphore);
    CheckScale();
    return (double)TTF_FontHeight(pTtfFont) / GetFontScale();
}

double MLIFont::GetLineAscent()
{
    AutoSemaphore sem(pAccessSemaphore);
    CheckScale();
    return (double)TTF_FontAscent(pTtfFont) / GetFontScale();
}

double MLIFont::GetLineDescent()
{
    AutoSemaphore sem(pAccessSemaphore);
    CheckScale();
    return (double)TTF_FontDescent(pTtfFont) / GetFontScale();
}
