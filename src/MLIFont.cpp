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
#include "utf8cpp/utf8.h"

#include <iostream>

#define MAX_WIDTH 512

MLIFont::MLIFont(const string &ttfFilePath, int fontSize, int strokeWidth, bool isBold)
    : ttfFilePath(ttfFilePath),
      fontSize(fontSize),
      strokeWidth(strokeWidth),
      isBold(isBold)

{
    pTtfFont = NULL;
    pTextSpriteSheet = NULL;

    vector<pair<uint32_t, uint32_t> > ranges;

    // basic latin
    ranges.push_back(pair<uint32_t, uint32_t>(0x0020, 0x007F));

    // russian
//    ranges.push_back(pair<uint32_t, uint32_t>(0x0401, 0x0401)); // ё
//    ranges.push_back(pair<uint32_t, uint32_t>(0x0410, 0x044F));
    Reinit(ranges);
}

MLIFont::~MLIFont()
{
    if (pTtfFont != NULL)
    {
        TTF_CloseFont(pTtfFont);
    }
    pTtfFont = NULL;

    delete pTextSpriteSheet;
    pTextSpriteSheet = NULL;
}

void MLIFont::Reinit(const vector<pair<uint32_t, uint32_t> > &ranges)
{
    charsToRender.clear();
    for(vector<pair<uint32_t, uint32_t> >::const_iterator it = ranges.begin(); it != ranges.end(); it++)
    {
        for (uint32_t c = (*it).first; c <= (*it).second; c++)
        {
            charsToRender.insert(c);
        }
    }
    Reinit();
}

void MLIFont::Reinit()
{
    // clean up first

    if (pTtfFont != NULL)
        TTF_CloseFont(pTtfFont);
    pTtfFont = NULL;

    delete pTextSpriteSheet;
    pTextSpriteSheet = NULL;

    renderedTextClipRectMap.clear();
    charPairToKernedWidthMap.clear();

    // setup scale
    scale = (GetEnableFullscreen() ? GetScreenScale() : 1.0);

    // setup font
    #ifdef GAME_EXECUTABLE
        pTtfFont = ResourceLoader::GetInstance()->LoadFont(ttfFilePath, fontSize * scale);
    #else
        pTtfFont = TTF_OpenFont(ttfFilePath.c_str(), fontSize * scale);
    #endif

    if (isBold)
    {
        TTF_SetFontStyle(pTtfFont, TTF_STYLE_BOLD);
    }

    // render chars
    map<uint32_t, SDL_Surface *> renderedText;

    SDL_Color whiteColor = {255, 255, 255, 255};

    for (set<uint32_t>::const_iterator it = charsToRender.begin(); it != charsToRender.end(); it++)
    {
        uint32_t c = (*it);

        string utf8string;
        utf8::unchecked::append(c, back_inserter(utf8string));

        SDL_Surface *pTextSurface = TTF_RenderUTF8_Blended(pTtfFont, utf8string.c_str(), whiteColor);
        if (pTextSurface != NULL)
        {
            renderedText[c] = pTextSurface;
        }
    }

    // render outlines
    if (strokeWidth > 0)
    {
        SDL_Color blackColor = {0, 0, 0, 255};

        for (map<uint32_t, SDL_Surface *>::iterator it = renderedText.begin(); it != renderedText.end(); it++)
        {
            string utf8string;
            utf8::unchecked::append((*it).first, back_inserter(utf8string));

            SDL_Surface *pSurface = (*it).second;
            SDL_Surface *pSurfaceOutline = TTF_RenderUTF8_Blended(pTtfFont, utf8string.c_str(), blackColor);

            SDL_Surface *pSurfaceOutlinedText = SDL_CreateRGBSurface(
                        0,
                        pSurface->w + strokeWidth * 2,
                        pSurface->h + strokeWidth * 2,
                        pSurface->format->BitsPerPixel,
                        pSurface->format->Rmask,
                        pSurface->format->Gmask,
                        pSurface->format->Bmask,
                        pSurface->format->Amask);

            SDL_Rect dstRect = {0, 0, pSurface->w, pSurface->h};

            dstRect.x = 0;
            dstRect.y = 0;
            SDL_SetSurfaceBlendMode(pSurfaceOutline, SDL_BLENDMODE_BLEND);
            SDL_BlitSurface(pSurfaceOutline, NULL, pSurfaceOutlinedText, &dstRect);

            dstRect.x = 0;
            dstRect.y = strokeWidth * 2;
            SDL_SetSurfaceBlendMode(pSurfaceOutline, SDL_BLENDMODE_BLEND);
            SDL_BlitSurface(pSurfaceOutline, NULL, pSurfaceOutlinedText, &dstRect);

            dstRect.x = strokeWidth * 2;
            dstRect.y = 0;
            SDL_SetSurfaceBlendMode(pSurfaceOutline, SDL_BLENDMODE_BLEND);
            SDL_BlitSurface(pSurfaceOutline, NULL, pSurfaceOutlinedText, &dstRect);

            dstRect.x = strokeWidth * 2;
            dstRect.y = strokeWidth * 2;
            SDL_SetSurfaceBlendMode(pSurfaceOutline, SDL_BLENDMODE_BLEND);
            SDL_BlitSurface(pSurfaceOutline, NULL, pSurfaceOutlinedText, &dstRect);

            dstRect.x = strokeWidth;
            dstRect.y = strokeWidth;
            SDL_SetSurfaceBlendMode(pSurfaceOutline, SDL_BLENDMODE_BLEND);
            SDL_BlitSurface(pSurface, NULL, pSurfaceOutlinedText, &dstRect);

            SDL_FreeSurface(pSurface);
            SDL_FreeSurface(pSurfaceOutline);

            (*it).second = pSurfaceOutlinedText;
        }
    }

    // although we render outlines on our own, we need to set outline width to get correct results from TTF_Size functions
    // also, this will flush internal SDL_TTF cache, and, since we using our own cache, it is outside the 'if (strokeWidth > 0)'
    TTF_SetFontOutline(pTtfFont, strokeWidth);

    // calc metrics we need to make singular surface
    int maxHeight = 0;
    int totalWidth = 0;
    for (map<uint32_t, SDL_Surface *>::const_iterator it = renderedText.begin(); it != renderedText.end(); it++)
    {
        SDL_Surface *pTextSurface = (*it).second;

        renderedTextClipRectMap[(*it).first] = RectangleWH(totalWidth, 0, pTextSurface->w, pTextSurface->h);

        if (pTextSurface->h > maxHeight)
            maxHeight = pTextSurface->h;

        totalWidth += pTextSurface->w;
    }

    SDL_Surface *pSurface = (*renderedText.begin()).second;

    // We'll make a singular surface to blit all of the individual characters to.
    SDL_Surface *pTextSpriteSheetSurface =
        SDL_CreateRGBSurface(
            0,
            totalWidth,
            maxHeight,
            pSurface->format->BitsPerPixel,
            pSurface->format->Rmask,
            pSurface->format->Gmask,
            pSurface->format->Bmask,
            pSurface->format->Amask);

    SDL_FillRect(pTextSpriteSheetSurface, NULL, SDL_MapRGBA(pTextSpriteSheetSurface->format, 0x00, 0x00, 0x00, 0x00));

    for (map<uint32_t, SDL_Surface *>::const_iterator it = renderedText.begin(); it != renderedText.end(); it++)
    {
        SDL_Surface *surface = (*it).second;
        RectangleWH &rect = renderedTextClipRectMap[(*it).first];
        SDL_Rect sdlRect = {(Sint16)rect.GetX(), (Sint16)rect.GetY(), (Uint16)rect.GetWidth(), (Uint16)rect.GetHeight()};
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
        SDL_BlitSurface(surface, NULL, pTextSpriteSheetSurface, &sdlRect);
    }

    // calc kerned width
    for (map<uint32_t, SDL_Surface *>::const_iterator it1 = renderedText.begin(); it1 != renderedText.end(); it1++)
    {
        for (map<uint32_t, SDL_Surface *>::const_iterator it2 = renderedText.begin(); it2 != renderedText.end(); it2++)
        {
            int w, h;
            uint32_t c1 = (*it1).first;
            uint32_t c2 = (*it2).first;
            string charPairString;
            utf8::unchecked::append(c1, back_inserter(charPairString));
            utf8::unchecked::append(c2, back_inserter(charPairString));
            TTF_SizeUTF8(pTtfFont, charPairString.c_str(), &w, &h);
            charPairToKernedWidthMap[pair<uint32_t, uint32_t>(c1, c2)] = w;
        }
    }

    pTextSpriteSheet = Image::Load(pTextSpriteSheetSurface, false /* loadImmediately */);
    pTextSpriteSheet->FlagFontSource(this);
    pTextSpriteSheet->SetUseScreenScaling(false);

    // free surfaces
    for (map<uint32_t, SDL_Surface *>::const_iterator it = renderedText.begin(); it != renderedText.end(); it++)
    {
        SDL_FreeSurface((*it).second);
    }
}

void MLIFont::Draw(const string &s, Vector2 position)
{
    Draw(s, position, Color::White, RectangleWH(-1, -1, -1, -1), 1.0);
}

void MLIFont::Draw(const string &s, Vector2 position, double scale)
{
    Draw(s, position, Color::White, RectangleWH(-1, -1, -1, -1), scale);
}

void MLIFont::Draw(const string &s, Vector2 position, Color color)
{
    Draw(s, position, color, RectangleWH(-1, -1, -1, -1), 1.0);
}

void MLIFont::Draw(const string &s, Vector2 position, Color color, double scale)
{
    Draw(s, position, color, RectangleWH(-1, -1, -1, -1), scale);
}

void MLIFont::Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect)
{
    Draw(s, position, color, clipRect, 1.0);
}

void MLIFont::Draw(const string &s, Vector2 position, Color color, RectangleWH clipRect, double scale)
{
    // If we're trying to draw an empty string, we can just return -
    // we're not gonna draw anything anyhow.
    if (s.length() == 0)
    {
        return;
    }

    Vector2 originalPosition = position;
    RectangleWH originalClipRect = clipRect;

    DrawInternal(s, position, color, clipRect, scale, &charPairToKernedWidthMap, &renderedTextClipRectMap, &renderedTextClipRectMap);
}

void MLIFont::DrawInternal(const string &s, Vector2 position, Color color, RectangleWH clipRect, double scale, map<pair<uint32_t, uint32_t>, int> *pKernedWidthMap, map<uint32_t, RectangleWH> *pClipRectMap, map<uint32_t, RectangleWH> *pClipRectMapForWidth)
{
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

        RectangleWH characterClipRect = (*pClipRectMap)[c];

        if (characterClipRect.GetWidth() == 0)
        {
            continue;
        }

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
                pTextSpriteSheet->Draw(position, characterClipRect, false /* flipHorizontally */, false /* flipVertically */, scale, color);
            }
        }

        // To update the position while still accounting for kerning,
        // we add the width of the character sprite to the position,
        // but then subtract off the difference between the widths of this and the next
        // characters minus their combined kerned widths.
        double deltaX = characterClipRect.GetWidth();

        if (it < s.end())
        {
            try
            {
                uint32_t c2 = utf8::peek_next(it, s.end());
                deltaX += GetKerningDelta(pKernedWidthMap, pClipRectMapForWidth, c, c2);
            }
            catch (utf8::not_enough_room ex)
            {
            }
        }

        position = Vector2(position.GetX() + deltaX * scale / GetFontScale(), position.GetY());

        if (clipRect.GetWidth() >= 0)
        {
            clipRect.SetX(clipRect.GetX() - deltaX * scale / GetFontScale());
            clipRect.SetWidth(clipRect.GetWidth() - deltaX * scale / GetFontScale());

            if (clipRect.GetWidth() <= 0)
            {
                break;
            }
        }
    }
}

int MLIFont::GetWidth(const string &s)
{
    Vector2 position(0, 0);

    for (string::const_iterator it = s.begin(); it < s.end();)
    {
        uint32_t c = 0;
        try
        {
            c = utf8::next(it, s.end());
        }
        catch (utf8::not_enough_room ex)
        {

        }
        RectangleWH characterClipRect = renderedTextClipRectMap[c];

        if (characterClipRect.GetWidth() == 0)
        {
            continue;
        }

        // To update the position while still accounting for kerning,
        // we add the width of the character sprite to the position,
        // but then subtract off the difference between the widths of this and the next
        // characters minus their combined kerned widths.
        position = Vector2(position.GetX() + characterClipRect.GetWidth(), position.GetY());

        if (it < s.end())
        {
            try
            {
                uint32_t c2 = utf8::peek_next(it, s.end());
                position = Vector2(position.GetX() + GetKerningDelta(&charPairToKernedWidthMap, &renderedTextClipRectMap, c, c2), position.GetY());
            }
            catch (utf8::not_enough_room ex)
            {

            }
        }
    }

    return position.GetX() / GetFontScale();
}

int MLIFont::GetKerningDelta(map<pair<uint32_t, uint32_t>, int> *pKernedWidthMap, map<uint32_t, RectangleWH> *pClipRectMap, uint32_t c1, uint32_t c2)
{
    return ((*pKernedWidthMap)[pair<uint32_t, uint32_t>(c1, c2)] - ((*pClipRectMap)[c1].GetWidth() + (*pClipRectMap)[c2].GetWidth())) / GetFontScale();
}

int MLIFont::GetHeight(const string &s)
{
    int w, h;

    TTF_SizeUTF8(pTtfFont, s.c_str(), &w, &h);
    return h / GetFontScale();
}

int MLIFont::GetLineHeight()
{
    return TTF_FontHeight(pTtfFont) / GetFontScale();
}

int MLIFont::GetLineAscent()
{
    return TTF_FontAscent(pTtfFont) / GetFontScale();
}
