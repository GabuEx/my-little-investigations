/**
 * Basic header/include file for Image.cpp.
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

#ifndef SMARTSPRITE_H
#define SMARTSPRITE_H

#include <SDL2/SDL.h>
#ifdef __OSX
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_thread.h>
#include <vector>

#include "Color.h"
#include "Rectangle.h"
#include "Vector2.h"
#include "Video.h"

using namespace std;

class MLIFont;

class Image
{
public:
    Image();
    ~Image();
    static Image * Load(SDL_Surface * sdlSurface, bool loadImmediately = false);
    static Image * Load(SDL_RWops * ops, bool loadImmediately = false);
    static Image * Load(const char * file, bool loadImmediately = false);
    static void ReloadImages();
    void Reload(SDL_Surface * sdlSurface, bool loadImmediately = false);
    void Reload(SDL_RWops * ops, bool loadImmediately = false);
    void LoadTextures();
    void UnloadTextures();

    bool IsReady() const { return valid; }

    void ReloadFromSource();
    void FlagResourceLoaderSource(const string &originFilePath);
    void FlagFontSource(MLIFont *pFont);
    void FlagVideoSource(Video::Frame *pVideoFrame);

    void Draw(Vector2 position);
    void Draw(Vector2 position, Color color);
    void Draw(Vector2 position, bool flipHorizontally, bool flipVertically, Color color);

    void Draw(
        Vector2 position,
        RectangleWH clipRect,
        bool flipHorizontally,
        bool flipVertically,
        double scale,
        Color color);

    static void Draw(
        SDL_Texture *pTexture,
        Vector2 position,
        RectangleWH clipRect,
        bool flipHorizontally,
        bool flipVertically,
        double scale,
        Color color,
        bool useScreenScaling = true);

    Uint16 width;
    Uint16 height;

    void SetUseScreenScaling(bool useScreenScaling) { this->useScreenScaling = useScreenScaling; }
    bool GetUseScreenScaling() const { return this->useScreenScaling; }

private:
    bool useScreenScaling;

    static vector<Image *> activeSpriteList;
    static vector<Image *> newSpriteList;
    static vector<Image *> deletedSpriteList;
    static SDL_sem *pSpriteListSemaphore;
    static bool isReloadingSprites;

    bool valid;
    SDL_Surface *pSurface;

    vector<SDL_Texture *> textureList;

    int textureCountX;
    int textureCountY;

    class Source
    {
    public:
        virtual ~Source() {}
        virtual void DoReload() = 0;
    };

    class ResourceLoaderSource : public Source
    {
    public:
        ResourceLoaderSource(Image *pSprite, const string &originFilePath)
        {
            this->pSprite = pSprite;
            this->originFilePath = originFilePath;
        }

        virtual ~ResourceLoaderSource()
        {
            pSprite = NULL;
        }

        void DoReload();

    private:
        Image *pSprite;
        string originFilePath;
    };

    class FontSource : public Source
    {
    public:
        FontSource(MLIFont *pFont)
        {
            this->pFont = pFont;
        }

        virtual ~FontSource()
        {
            pFont = NULL;
        }

        void DoReload();

    private:
        MLIFont *pFont;
    };

    Source *pSource;
};
#endif
