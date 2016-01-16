/**
 * Basic header/include file for Sprite.cpp.
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

#ifndef SPRITE_H
#define SPRITE_H

#include "Color.h"
#include "enums.h"
#include "Rectangle.h"
#include "Vector2.h"
#include "Image.h"

class XmlReader;

class Sprite
{
public:
    Sprite()
    {
        pSpriteSheetImage = NULL;
        pSpriteSheetSemaphore = NULL;
        managerSource = ManagerSourceCommonResources;
    }

    Sprite(const string &spriteSheetImageId, const RectangleWH &spriteClipRect)
    {
        this->spriteSheetImageId = spriteSheetImageId;
        this->spriteClipRect = spriteClipRect;
        pSpriteSheetImage = NULL;
        pSpriteSheetSemaphore = SDL_CreateSemaphore(1);
        managerSource = ManagerSourceCommonResources;
    }

    Sprite(XmlReader *pReader);
    ~Sprite();

    void SetManagerSource(ManagerSource managerSource) { this->managerSource = managerSource; }

    string GetSpriteSheetImageId() const;
    RectangleWH GetSpriteClipRect() const;
    double GetWidth();
    double GetHeight();
    void Draw(Vector2 position);
    void Draw(Vector2 position, Color color);
    void Draw(Vector2 position, Color color, double scale, bool flipHorizontally);
    void DrawClipped(Vector2 position, RectangleWH clipRect);
    void DrawClipped(Vector2 position, RectangleWH clipRect, bool flipHorizontally);
    void DrawClipped(Vector2 position, RectangleWH clipRect, bool flipHorizontally, Color color);

    void UpdateReadiness(const string &newLocationId, bool *pLoadSprite, bool *pDeleteSprite);
    bool IsReady();

//private:
    Image * GetSpriteSheetImage();
    Image * GetSpriteSheetImageNoCache();

    string spriteSheetImageId;
    Image *pSpriteSheetImage;
    Vector2 spriteDrawOffset;
    Vector2 originalSize;
    RectangleWH spriteClipRect;

    ManagerSource managerSource;

    SDL_sem *pSpriteSheetSemaphore;
};

#endif
