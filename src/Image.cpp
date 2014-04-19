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
#include "Video.h"
#endif

#include <algorithm>

using namespace std;

vector<Image *> Image::activeSpriteList;
vector<Image *> Image::newSpriteList;
vector<Image *> Image::deletedSpriteList;
SDL_sem *Image::pSpriteListSemaphore = SDL_CreateSemaphore(1);
bool Image::isReloadingSprites = false;

Image::Image(void)
{
    valid = false;
    pSurface = NULL;
    pTexture = NULL;
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
    if (gUiThreadId != SDL_ThreadID() || !loadImmediately)
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
    if (pTexture != NULL)
    {
        SDL_DestroyTexture(pTexture);
        pTexture = NULL;
    }

    pTexture = SDL_CreateTextureFromSurface(gpRenderer, pSurface);
    SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);

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

    if (pTexture != NULL)
    {
        SDL_DestroyTexture(pTexture);
        pTexture = NULL;
    }
}

void Image::ReloadFromSource()
{
    if (pSource != NULL)
    {
        pSource->DoReload();
    }
}

void Image::FlagResourceLoaderSource(string originFilePath)
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

    Image::Draw(pTexture, position, clipRect, flipHorizontally, flipVertically, scale, color);
}

void Image::Draw(
    SDL_Texture *pTexture,
    Vector2 position,
    RectangleWH clipRect,
    bool flipHorizontally,
    bool flipVertically,
    double scale,
    Color color)
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

    if (position.GetX() + clipRect.GetWidth() >= gScreenWidth)
    {
        double widthAdjustment = position.GetX() + clipRect.GetWidth() - gScreenWidth;

        if (flipHorizontally)
        {
            clipRect.SetX(clipRect.GetX() + widthAdjustment);
        }

        clipRect.SetWidth(clipRect.GetWidth() - widthAdjustment);
    }

    if (position.GetY() + clipRect.GetHeight() >= gScreenHeight)
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
            (Uint16)(clipRect.GetWidth() * horizontalScaleToUse * scale + 0.5),
            (Uint16)(clipRect.GetHeight() * verticalScaleToUse * scale + 0.5)
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
