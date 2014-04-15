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
#include "Image.h"
#include "ResourceLoader.h"

#define MAX_WIDTH 512

const int minCharValue = 32;
const int maxCharValue = 128;

MLIFont::MLIFont(string ttfFilePath, int fontSize, int strokeWidth, bool isBold)
{
#ifdef GAME_EXECUTABLE
    pTtfFont = ResourceLoader::GetInstance()->LoadFont(ttfFilePath, fontSize);
#else
    pTtfFont = TTF_OpenFont(ttfFilePath.c_str(), fontSize);
#endif
    pTextSpriteSheet = NULL;

    if (isBold)
    {
        TTF_SetFontStyle(pTtfFont, TTF_STYLE_BOLD);
    }

    this->strokeWidth = strokeWidth;
    Reinit();
}

MLIFont::~MLIFont()
{
    TTF_CloseFont(pTtfFont);
    pTtfFont = NULL;

    delete pTextSpriteSheet;
    pTextSpriteSheet = NULL;
}

void MLIFont::Reinit()
{
    SDL_Surface *pRenderedText[maxCharValue - minCharValue];
    SDL_Surface *pRenderedTextOutlines[maxCharValue - minCharValue];
    int maxHeight = 0;
    int totalWidth = 0;

    pRenderedTextClipRectMap.clear();
    pRenderedTextOutlineClipRectMap.clear();
    charPairToKernedWidthMap.clear();

    SDL_Color whiteColor = {255, 255, 255, 255};

    for (int i = minCharValue; i < maxCharValue; i++)
    {
        char c = (char)i;
        string s(&c, 1);

        SDL_Surface *pTextSurface = TTF_RenderUTF8_Blended(pTtfFont, s.c_str(), whiteColor);

        if (pTextSurface != NULL)
        {
            if (pTextSurface->h > maxHeight)
            {
                maxHeight = pTextSurface->h;
            }

            pRenderedTextClipRectMap[c] = RectangleWH(totalWidth, 0, pTextSurface->w, pTextSurface->h);
            totalWidth += pTextSurface->w;

            pRenderedText[i - minCharValue] = pTextSurface;
        }
        else
        {
            pRenderedTextClipRectMap[c] = RectangleWH(0, 0, 0, 0);
        }
    }

    int maxStrokeHeight = 0;
    int totalStrokeWidth = 0;

    if (strokeWidth > 0)
    {
        //TTF_SetFontOutline(pTtfFont, strokeWidth);
        SDL_Color blackColor = {0, 0, 0, 255};

        for (int i = minCharValue; i < maxCharValue; i++)
        {
            char c = (char)i;
            string s(&c, 1);

            SDL_Surface *pTextOutlineSurface = TTF_RenderUTF8_Blended(pTtfFont, s.c_str(), blackColor);

            if (pTextOutlineSurface != NULL)
            {
                if (pTextOutlineSurface->h > maxStrokeHeight)
                {
                    maxStrokeHeight = pTextOutlineSurface->h;
                }

                pRenderedTextOutlineClipRectMap[c] = RectangleWH(totalStrokeWidth, maxHeight, pTextOutlineSurface->w, pTextOutlineSurface->h);
                totalStrokeWidth += pTextOutlineSurface->w;

                pRenderedTextOutlines[i - minCharValue] = pTextOutlineSurface;
            }
            else
            {
                pRenderedTextOutlineClipRectMap[c] = RectangleWH(0, 0, 0, 0);
            }
        }

        //TTF_SetFontOutline(pTtfFont, 0);
    }

    // We'll make a singular surface to blit all of the individual characters to.
    SDL_Surface *pTextSpriteSheetSurface =
        SDL_CreateRGBSurface(
            0,
            totalWidth > totalStrokeWidth ? totalWidth : totalStrokeWidth,
            maxHeight + maxStrokeHeight,
            pRenderedText[0]->format->BitsPerPixel,
            pRenderedText[0]->format->Rmask,
            pRenderedText[0]->format->Gmask,
            pRenderedText[0]->format->Bmask,
            pRenderedText[0]->format->Amask);

    SDL_FillRect(pTextSpriteSheetSurface, NULL, SDL_MapRGBA(pTextSpriteSheetSurface->format, 0x00, 0x00, 0x00, 0x00));

    for (int i = minCharValue; i < maxCharValue; i++)
    {
        char c = (char)i;

        if (pRenderedTextClipRectMap[c].GetWidth() == 0)
        {
            continue;
        }

        SDL_Rect dstRect = {(Sint16)pRenderedTextClipRectMap[c].GetX(), (Sint16)pRenderedTextClipRectMap[c].GetY(), (Uint16)pRenderedTextClipRectMap[c].GetWidth(), (Uint16)pRenderedTextClipRectMap[c].GetHeight()};
        SDL_SetSurfaceBlendMode(pRenderedText[i - minCharValue], SDL_BLENDMODE_NONE);
        SDL_BlitSurface(pRenderedText[i - minCharValue], NULL, pTextSpriteSheetSurface, &dstRect);

        if (strokeWidth > 0)
        {
            SDL_Rect dstRectOutline = {(Sint16)pRenderedTextOutlineClipRectMap[c].GetX(), (Sint16)pRenderedTextOutlineClipRectMap[c].GetY(), (Uint16)pRenderedTextOutlineClipRectMap[c].GetWidth(), (Uint16)pRenderedTextOutlineClipRectMap[c].GetHeight()};
            SDL_SetSurfaceBlendMode(pRenderedTextOutlines[i - minCharValue], SDL_BLENDMODE_NONE);
            SDL_BlitSurface(pRenderedTextOutlines[i - minCharValue], NULL, pTextSpriteSheetSurface, &dstRectOutline);
        }
    }

    for (int i = minCharValue; i < maxCharValue; i++)
    {
        char c1 = (char)i;

        if (pRenderedTextClipRectMap[c1].GetWidth() == 0)
        {
            continue;
        }

        for (int j = minCharValue; j < maxCharValue; j++)
        {
            char c2 = (char)j;

            if (pRenderedTextClipRectMap[c2].GetWidth() == 0)
            {
                continue;
            }

            int w;
            int h;
            string charPairString = string(&c1, 1) + string(&c2, 1);
            TTF_SizeText(pTtfFont, charPairString.c_str(), &w, &h);
            charPairToKernedWidthMap[charPairString] = w;
        }
    }

    delete pTextSpriteSheet;
    pTextSpriteSheet = Image::Load(pTextSpriteSheetSurface, false /* loadImmediately */);
    pTextSpriteSheet->FlagFontSource(this);

    if (strokeWidth > 0)
    {
        //TTF_SetFontOutline(pTtfFont, strokeWidth);
    }

    for (int i = minCharValue; i < maxCharValue; i++)
    {
        char c = (char)i;

        if (pRenderedTextClipRectMap[c].GetWidth() == 0)
        {
            continue;
        }

        SDL_FreeSurface(pRenderedText[i - minCharValue]);

        if (strokeWidth > 0)
        {
            SDL_FreeSurface(pRenderedTextOutlines[i - minCharValue]);
        }
    }
}

void MLIFont::Draw(string s, Vector2 position)
{
    Draw(s, position, Color::White, RectangleWH(-1, -1, -1, -1), 1.0);
}

void MLIFont::Draw(string s, Vector2 position, double scale)
{
    Draw(s, position, Color::White, RectangleWH(-1, -1, -1, -1), scale);
}

void MLIFont::Draw(string s, Vector2 position, Color color)
{
    Draw(s, position, color, RectangleWH(-1, -1, -1, -1), 1.0);
}

void MLIFont::Draw(string s, Vector2 position, Color color, double scale)
{
    Draw(s, position, color, RectangleWH(-1, -1, -1, -1), scale);
}

void MLIFont::Draw(string s, Vector2 position, Color color, RectangleWH clipRect)
{
    Draw(s, position, color, clipRect, 1.0);
}

void MLIFont::Draw(string s, Vector2 position, Color color, RectangleWH clipRect, double scale)
{
    // If we're trying to draw an empty string, we can just return -
    // we're not gonna draw anything anyhow.
    if (s.length() == 0)
    {
        return;
    }

    Vector2 originalPosition = position;
    RectangleWH originalClipRect = clipRect;

    if (strokeWidth > 0)
    {
        DrawInternal(s, position, color, clipRect, scale, &charPairToKernedWidthMap, &pRenderedTextOutlineClipRectMap, &pRenderedTextClipRectMap);
        position = originalPosition + (Vector2(strokeWidth, strokeWidth) * 2);

        if (clipRect.GetWidth() >= 0)
        {
            clipRect = RectangleWH(originalClipRect.GetX() - strokeWidth * 2, originalClipRect.GetY() - strokeWidth * 2, originalClipRect.GetWidth() - strokeWidth * 2, originalClipRect.GetHeight() - strokeWidth * 2);
        }

        DrawInternal(s, position, color, clipRect, scale, &charPairToKernedWidthMap, &pRenderedTextOutlineClipRectMap, &pRenderedTextClipRectMap);
        position = originalPosition + Vector2(strokeWidth, strokeWidth);

        if (clipRect.GetWidth() >= 0)
        {
            clipRect = RectangleWH(originalClipRect.GetX() - strokeWidth, originalClipRect.GetY() - strokeWidth, originalClipRect.GetWidth() - strokeWidth, originalClipRect.GetHeight() - strokeWidth);
        }
    }

    DrawInternal(s, position, color, clipRect, scale, &charPairToKernedWidthMap, &pRenderedTextClipRectMap, &pRenderedTextClipRectMap);
}

void MLIFont::DrawInternal(string s, Vector2 position, Color color, RectangleWH clipRect, double scale, map<string, int> *pKernedWidthMap, map<char, RectangleWH> *pClipRectMap, map<char, RectangleWH> *pClipRectMapForWidth)
{
    for (unsigned int i = 0; i < s.length(); i++)
    {
        RectangleWH characterClipRect = (*pClipRectMap)[s[i]];

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

        if (i < s.length() - 1)
        {
            deltaX += GetKerningDelta(pKernedWidthMap, pClipRectMapForWidth, s[i], s[i + 1]);
        }

        position = Vector2(position.GetX() + deltaX * scale, position.GetY());

        if (clipRect.GetWidth() >= 0)
        {
            clipRect.SetX(clipRect.GetX() - deltaX * scale);
            clipRect.SetWidth(clipRect.GetWidth() - deltaX * scale);

            if (clipRect.GetWidth() <= 0)
            {
                break;
            }
        }
    }
}

int MLIFont::GetWidth(string s)
{
    Vector2 position(0, 0);

    if (strokeWidth > 0)
    {
        for (unsigned int i = 0; i < s.length(); i++)
        {
            RectangleWH characterClipRect = pRenderedTextOutlineClipRectMap[s[i]];

            if (characterClipRect.GetWidth() == 0)
            {
                continue;
            }

            // To update the position while still accounting for kerning,
            // we add the width of the character sprite to the position,
            // but then subtract off the difference between the widths of this and the next
            // characters minus their combined kerned widths.
            position = Vector2(position.GetX() + characterClipRect.GetWidth(), position.GetY());

            if (i < s.length() - 1)
            {
                position = Vector2(position.GetX() + GetKerningDelta(&charPairToKernedWidthMap, &pRenderedTextClipRectMap, s[i], s[i + 1]), position.GetY());
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < s.length(); i++)
        {
            RectangleWH characterClipRect = pRenderedTextClipRectMap[s[i]];

            if (characterClipRect.GetWidth() == 0)
            {
                continue;
            }

            // To update the position while still accounting for kerning,
            // we add the width of the character sprite to the position,
            // but then subtract off the difference between the widths of this and the next
            // characters minus their combined kerned widths.
            position = Vector2(position.GetX() + characterClipRect.GetWidth(), position.GetY());

            if (i < s.length() - 1)
            {
                position = Vector2(position.GetX() + GetKerningDelta(&charPairToKernedWidthMap, &pRenderedTextClipRectMap, s[i], s[i + 1]), position.GetY());
            }
        }
    }

    return (int)position.GetX();
}

int MLIFont::GetKerningDelta(map<string, int> *pKernedWidthMap, map<char, RectangleWH> *pClipRectMap, char c1, char c2)
{
    return (int)((*pKernedWidthMap)[string(&c1, 1) + string(&c2, 1)] - ((*pClipRectMap)[c1].GetWidth() + (*pClipRectMap)[c2].GetWidth()));
}

int MLIFont::GetHeight(string s)
{
    int w, h;

    TTF_SizeUTF8(pTtfFont, s.c_str(), &w, &h);
    return h;
}

int MLIFont::GetLineHeight()
{
    return TTF_FontHeight(pTtfFont);
}

int MLIFont::GetLineAscent()
{
    return TTF_FontAscent(pTtfFont);
}
