/**
 * Handles the display of images.
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

#include "Image.h"
#include "MLIFont.h"
#include "globals.h"

#ifdef GAME_EXECUTABLE
#include "ResourceLoader.h"
#endif

#include <iostream>
#include <algorithm>

using namespace std;

vector<Image *> Image::activeSpriteList;
vector<Image *> Image::newSpriteList;
vector<Image *> Image::deletedSpriteList;
SDL_sem *Image::pSpriteListSemaphore = SDL_CreateSemaphore(1);
bool Image::isReloadingSprites = false;

Image::Image(void)
{
    useScreenScaling = true;

    valid = false;
    pSurface = NULL;

    textureList.clear();

    textureCountX = 0;
    textureCountY = 0;

    width = 0;
    height = 0;
    pSource = NULL;

    if (!isReloadingSprites)
    {
        SDL_SemWait(pSpriteListSemaphore);
        activeSpriteList.push_back(this);
        SDL_SemPost(pSpriteListSemaphore);
    }
    else
    {
        newSpriteList.push_back(this);
    }
}

Image::~Image()
{
    if (!isReloadingSprites)
    {
        SDL_SemWait(pSpriteListSemaphore);
        activeSpriteList.erase(find(activeSpriteList.begin(), activeSpriteList.end(), this));
        SDL_SemPost(pSpriteListSemaphore);
    }
    else
    {
        deletedSpriteList.push_back(this);
    }

    UnloadTextures();

    delete pSource;
    pSource = NULL;
}

Image * Image::Load(SDL_Surface * sdlSurface, bool loadImmediately)
{
    Image *pImage = new Image();
    pImage->Reload(sdlSurface, loadImmediately);
    return pImage;
}

Image * Image::Load(SDL_RWops * ops, bool loadImmediately)
{
    return Image::Load(IMG_Load_RW(ops, true /* freesrc */), loadImmediately);
}

Image * Image::Load(const char * file, bool loadImmediately)
{
    Image * returnVal = Image::Load(SDL_RWFromFile(file,"rb"), loadImmediately);
    if(returnVal && returnVal->valid) return returnVal;
    cout << "Couldn't load Image from file: " << file << endl;
    return NULL;
}

void Image::ReloadImages()
{
    SDL_SemWait(pSpriteListSemaphore);

    isReloadingSprites = true;

    for (unsigned int i = 0; i < activeSpriteList.size(); i++)
    {
        activeSpriteList[i]->ReloadFromSource();
    }

    isReloadingSprites = false;

    for (unsigned int i = 0; i < deletedSpriteList.size(); i++)
    {
        activeSpriteList.erase(find(activeSpriteList.begin(), activeSpriteList.end(), deletedSpriteList[i]));
    }

    for (unsigned int i = 0; i < newSpriteList.size(); i++)
    {
        activeSpriteList.push_back(newSpriteList[i]);
    }

    deletedSpriteList.clear();
    newSpriteList.clear();

    SDL_SemPost(pSpriteListSemaphore);
}

void Image::Reload(SDL_Surface *pSurface, bool loadImmediately)
{
    UnloadTextures();
    this->pSurface = pSurface;

    if (this->pSurface == NULL)
    {
        cout << "Couldn't get SDL surface..." << endl;
        return;
    }

    this->width = this->pSurface->w;
    this->height = this->pSurface->h;

#ifdef GAME_EXECUTABLE
    Uint32 currentThreadId = SDL_ThreadID();

    if (gUiThreadId != currentThreadId || !loadImmediately)
    {
        ResourceLoader::GetInstance()->AddImage(this);
    }
    else
    {
#endif
        this->LoadTextures();
#ifdef GAME_EXECUTABLE
    }
#endif
}

void Image::Reload(SDL_RWops * ops, bool loadImmediately)
{
    Reload(IMG_Load_RW(ops, 1), loadImmediately);
}

void Image::LoadTextures()
{
    for (vector<SDL_Texture *>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
    {
        SDL_DestroyTexture(*iter);
    }

    textureList.clear();

    textureCountX = 0;
    textureCountY = 0;

    if (pSurface->w <= gMaxTextureWidth && pSurface->h <= gMaxTextureHeight)
    {
        textureCountX = 1;
        textureCountY = 1;

        // If the surface will fit within a single texture, then we'll just create that single texture,
        // nothing fancy required.
        SDL_Texture *pTexture = SDL_CreateTextureFromSurface(gpRenderer, pSurface);
        SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);

        textureList.push_back(pTexture);
    }
    else
    {
        // On the other hand, if the surface won't fit within a single texture, then we'll need to slice it up
        // into subsurfaces that do all fit within a single texture, and store each of those as separate textures.
        const SDL_PixelFormat *pFormat = pSurface->format;

        textureCountX = (pSurface->w / gMaxTextureWidth) + (pSurface->w % gMaxTextureWidth > 0 ? 1 : 0);
        textureCountY = (pSurface->h / gMaxTextureHeight) + (pSurface->h % gMaxTextureHeight > 0 ? 1 : 0);

        for (int y = 0; y < textureCountY; y++)
        {
            for (int x = 0; x < textureCountX; x++)
            {
                int textureWidth = min(pSurface->w - gMaxTextureWidth * x, gMaxTextureWidth);
                int textureHeight = min(pSurface->h - gMaxTextureHeight * y, gMaxTextureHeight);

                SDL_Surface *pTempSurface =
                    SDL_CreateRGBSurface(
                        0, textureWidth, textureHeight,
                        pFormat->BitsPerPixel,
                        pFormat->Rmask, pFormat->Gmask, pFormat->Bmask, pFormat->Amask);

                SDL_Rect srcRect = { x * gMaxTextureWidth, y * gMaxTextureHeight, textureWidth, textureHeight };
                SDL_BlitSurface(pSurface, &srcRect, pTempSurface, NULL);

                SDL_Texture *pTexture = SDL_CreateTextureFromSurface(gpRenderer, pTempSurface);
                SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);

                textureList.push_back(pTexture);

                SDL_FreeSurface(pTempSurface);
            }
        }
    }

    SDL_FreeSurface(pSurface);
    pSurface = NULL;

    valid = true;
}

void Image::UnloadTextures()
{
    valid = false;

#ifdef GAME_EXECUTABLE
    ResourceLoader::GetInstance()->RemoveImage(this);
#endif

    if (pSurface != NULL)
    {
        SDL_FreeSurface(pSurface);
        pSurface = NULL;
    }

    for (vector<SDL_Texture *>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
    {
        SDL_DestroyTexture(*iter);
    }

    textureList.clear();

    textureCountX = 0;
    textureCountY = 0;
}

void Image::ReloadFromSource()
{
    if (pSource != NULL)
    {
        pSource->DoReload();
    }
}

void Image::FlagResourceLoaderSource(const string &originFilePath)
{
    delete pSource;
    pSource = new ResourceLoaderSource(this, originFilePath);
}

void Image::FlagFontSource(MLIFont *pFont)
{
    delete pSource;
    pSource = new FontSource(pFont);
}

void Image::Draw(Vector2 position)
{
    Draw(position, RectangleWH(0, 0, width, height), false, false, 1.0, Color::White);
}

void Image::Draw(Vector2 position, Color color)
{
    Draw(position, RectangleWH(0, 0, width, height), false, false, 1.0, color);
}

void Image::Draw(Vector2 position, bool flipHorizontally, bool flipVertically, Color color)
{
    Draw(position, RectangleWH(0, 0, width, height), flipHorizontally, flipVertically, 1.0, color);
}

void Image::Draw(
    Vector2 position,
    RectangleWH clipRect,
    bool flipHorizontally,
    bool flipVertically,
    double scale,
    Color color)
{

    // If this isn't a valid sprite, then we just won't draw anything.
    if (!valid)
    {
        return;
    }

    if (textureList.size() == 1)
    {
        // If we only have one texture, then we can just draw it without any processing.
        Image::Draw(textureList[0], position, clipRect, flipHorizontally, flipVertically, scale, color, useScreenScaling);
    }
    else
    {
        // Otherwise, we'll need to ensure that each texture gets drawn in its correct spot,
        // accounting for flipping.
        double startY = position.GetY() + (flipVertically ? height : 0);

        for (int y = 0; y < textureCountY; y++)
        {
            double startX = position.GetX() + (flipHorizontally ? width : 0);

            for (int x = 0; x < textureCountX; x++)
            {
                int textureWidth = 0;
                int textureHeight = 0;

                SDL_Texture *pTexture = textureList[y * textureCountY + x % textureCountX];

                SDL_QueryTexture(pTexture, NULL, NULL, &textureWidth, &textureHeight);

                if (flipHorizontally)
                {
                    startX -= textureWidth;
                }

                if (flipVertically)
                {
                    startY -= textureHeight;
                }

                // Adjust the clip rect accordingly for this portion of the image,
                // taking into account the position into the image we're drawing.
                RectangleWH portionClipRect(clipRect.GetX() - x * gMaxTextureWidth, clipRect.GetY() - y * gMaxTextureHeight, clipRect.GetWidth(), clipRect.GetHeight());

                if (portionClipRect.GetX() < 0)
                {
                    portionClipRect.SetWidth(portionClipRect.GetWidth() + portionClipRect.GetX());
                    portionClipRect.SetX(0);
                }

                if (portionClipRect.GetY() < 0)
                {
                    portionClipRect.SetHeight(portionClipRect.GetHeight() + portionClipRect.GetY());
                    portionClipRect.SetY(0);
                }

                if (portionClipRect.GetX() + portionClipRect.GetWidth() > textureWidth)
                {
                    portionClipRect.SetWidth(portionClipRect.GetWidth() - (textureWidth - portionClipRect.GetX()));
                }

                if (portionClipRect.GetY() + portionClipRect.GetHeight() > textureHeight)
                {
                    portionClipRect.SetWidth(portionClipRect.GetHeight() - (textureHeight - portionClipRect.GetY()));
                }

                Image::Draw(pTexture, Vector2(startX, startY), portionClipRect, flipHorizontally, flipVertically, scale, color, useScreenScaling);

                if (!flipHorizontally)
                {
                    startX += textureWidth;
                }

                if (!flipVertically)
                {
                    startY += textureHeight;
                }
            }
        }
    }
}

void Image::Draw(
    SDL_Texture *pTexture,
    Vector2 position,
    RectangleWH clipRect,
    bool flipHorizontally,
    bool flipVertically,
    double scale,
    Color color,
    bool useScreenScaling)
{
    // If the alpha channel of the color overlay is zero (i.e., completely transparent),
    // or if the entire sprite is off the screen, then we just won't draw anything.
    if (color.GetA() == 0 ||
        position.GetX() + clipRect.GetWidth() < 0 ||
        position.GetY() + clipRect.GetHeight() < 0 ||
        position.GetX() >= gScreenWidth ||
        position.GetY() >= gScreenHeight)
    {
        return;
    }

    double horizontalOffsetToUse = 0.0;
    double verticalOffsetToUse = 0.0;
    double horizontalScaleToUse = 1.0;
    double verticalScaleToUse = 1.0;

#ifdef GAME_EXECUTABLE
    if (gIsSavingScreenshot)
    {
        horizontalScaleToUse = (double)gScreenshotWidth / gScreenWidth;
        verticalScaleToUse = (double)gScreenshotHeight / gScreenHeight;
    }
    else if (gIsFullscreen)
    {
        horizontalOffsetToUse = gHorizontalOffset;
        verticalOffsetToUse = gVerticalOffset;
        horizontalScaleToUse = gScreenScale;
        verticalScaleToUse = gScreenScale;
    }
#endif

    // Adjust the clip rect such that we're also clipping to the screen as well.
    if (position.GetX() < 0)
    {
        double xAdjustment = -position.GetX();

        position.SetX(0);

        // We only want to shift the x-position of the clip rect if we're not flipping horizontally.
        // If we are, then we want to get the other side of the texture anyway.
        if (!flipHorizontally)
        {
            clipRect.SetX(clipRect.GetX() + xAdjustment);
        }

        clipRect.SetWidth(clipRect.GetWidth() - xAdjustment);
    }

    if (position.GetY() < 0)
    {
        double yAdjustment = -position.GetY();

        position.SetY(0);

        // We only want to shift the y-position of the clip rect if we're not flipping vertically.
        // If we are, then we want to get the other side of the texture anyway.
        if (!flipVertically)
        {
            clipRect.SetY(clipRect.GetY() + yAdjustment);
        }

        clipRect.SetHeight(clipRect.GetHeight() - yAdjustment);
    }

    // Note that if we're not using screen scaling, then the clip rect doesn't properly reflect
    // how large this image needs to be before it's off the edge of the screen.
    // To account for that, we'll divide by the scale to get the "effective" clip rect dimension
    // for use in this purpose.

    if (position.GetX() + clipRect.GetWidth() / (!useScreenScaling ? horizontalScaleToUse : 1.0) >= gScreenWidth)
    {
        double widthAdjustment = position.GetX() + clipRect.GetWidth() - gScreenWidth;

        if (flipHorizontally)
        {
            clipRect.SetX(clipRect.GetX() + widthAdjustment);
        }

        clipRect.SetWidth(clipRect.GetWidth() - widthAdjustment);
    }

    if (position.GetY() + clipRect.GetHeight() / (!useScreenScaling ? verticalScaleToUse : 1.0) >= gScreenHeight)
    {
        double heightAdjustment = position.GetY() + clipRect.GetHeight() - gScreenHeight;

        if (flipVertically)
        {
            clipRect.SetY(clipRect.GetY() + heightAdjustment);
        }

        clipRect.SetHeight(clipRect.GetHeight() - heightAdjustment);
    }

    // If the clip rect has been adjusted such that nothing will be drawn, then we won't draw anything.
    if (clipRect.GetWidth() <= 0 || clipRect.GetHeight() <= 0)
    {
        return;
    }

    SDL_Rect srcRect =
        {
            (Sint16)(clipRect.GetX() + 0.5),
            (Sint16)(clipRect.GetY() + 0.5),
            (Uint16)(clipRect.GetWidth() + 0.5),
            (Uint16)(clipRect.GetHeight() + 0.5)
        };
    SDL_Rect dstRect =
        {
            (Sint16)(horizontalOffsetToUse + position.GetX() * horizontalScaleToUse + 0.5),
            (Sint16)(verticalOffsetToUse + position.GetY() * verticalScaleToUse + 0.5),
            (Uint16)(clipRect.GetWidth() * (useScreenScaling ? horizontalScaleToUse : 1.0) * scale + 0.5),
            (Uint16)(clipRect.GetHeight() * (useScreenScaling ? verticalScaleToUse : 1.0) * scale + 0.5)
        };

    SDL_RendererFlip flags = SDL_FLIP_NONE;

    if (flipHorizontally)
    {
        flags = (SDL_RendererFlip)(flags | SDL_FLIP_HORIZONTAL);
    }

    if (flipVertically)
    {
        flags = (SDL_RendererFlip)(flags | SDL_FLIP_VERTICAL);
    }

    SDL_SetTextureColorMod(pTexture, color.GetIntR(), color.GetIntG(), color.GetIntB());
    SDL_SetTextureAlphaMod(pTexture, color.GetIntA());
    SDL_RenderCopyEx(gpRenderer, pTexture, &srcRect, &dstRect, 0, NULL, flags);
}

void Image::ResourceLoaderSource::DoReload()
{
#ifdef GAME_EXECUTABLE
    ResourceLoader::GetInstance()->ReloadImage(pSprite, originFilePath);
#endif
}

void Image::FontSource::DoReload()
{
    pFont->Reinit();
}
