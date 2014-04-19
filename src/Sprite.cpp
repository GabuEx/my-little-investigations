/**
 * Handles the retrieval and drawing of sprite from sprite sheets.
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

#include "Sprite.h"
#include "CaseInformation/Case.h"
#include "CaseInformation/CommonCaseResources.h"

const string CommonFilesId = "CommonFiles";

Sprite::Sprite(XmlReader *pReader)
{
    pSpriteSheetImage = NULL;
    spriteDrawOffset = Vector2(0, 0);
    pSpriteSheetSemaphore = SDL_CreateSemaphore(1);
    managerSource = ManagerSourceCommonResources;

    pReader->StartElement("Sprite");

    if (pReader->ElementExists("SpriteSheetImageId"))
    {
        spriteSheetImageId = pReader->ReadTextElement("SpriteSheetImageId");

        pReader->StartElement("SpriteClipRect");
        spriteClipRect = RectangleWH(pReader);
        pReader->EndElement();

        if (pReader->ElementExists("Offset"))
        {
            pReader->StartElement("Offset");
            spriteDrawOffset = Vector2(pReader);
            pReader->EndElement();

            pReader->StartElement("OriginalSize");
            originalSize = Vector2(pReader);
            pReader->EndElement();
        }
    }

    pReader->EndElement();
}

Sprite::~Sprite()
{
    SDL_DestroySemaphore(pSpriteSheetSemaphore);
    pSpriteSheetSemaphore = NULL;
}

string Sprite::GetSpriteSheetImageId() const
{
    return spriteSheetImageId;
}

RectangleWH Sprite::GetSpriteClipRect() const
{
    return spriteClipRect;
}

double Sprite::GetWidth()
{
    return spriteClipRect.GetWidth();
}

double Sprite::GetHeight()
{
    return spriteClipRect.GetHeight();
}

void Sprite::Draw(Vector2 position)
{
    Draw(position, Color::White, 1.0 /* scale */, false /* flipHorizontally */);
}

void Sprite::Draw(Vector2 position, Color color)
{
    Draw(position, color, 1.0 /* scale */, false /* flipHorizontally */);
}

void Sprite::Draw(Vector2 position, Color color, double scale, bool flipHorizontally)
{
    if (GetSpriteSheetImage() == NULL)
    {
        return;
    }

    Vector2 pixelSnappedPosition =
        Vector2(
            (int)(position.GetX() + (originalSize.GetX() > 0 && flipHorizontally ? originalSize.GetX() - GetWidth() - spriteDrawOffset.GetX() : spriteDrawOffset.GetX())),
            (int)(position.GetY() + spriteDrawOffset.GetY()));

    GetSpriteSheetImage()->Draw(
        pixelSnappedPosition,
        spriteClipRect,
        flipHorizontally,
        false /* flipVertically */,
        scale,
        color);
}

void Sprite::DrawClipped(Vector2 position, RectangleWH clipRect)
{
    DrawClipped(position, clipRect, false /* flipHorizontally */);
}

void Sprite::DrawClipped(Vector2 position, RectangleWH clipRect, bool flipHorizontally)
{
    DrawClipped(position, clipRect, flipHorizontally, Color::White);
}

void Sprite::DrawClipped(Vector2 position, RectangleWH clipRect, bool flipHorizontally, Color color)
{
    if (GetSpriteSheetImage() == NULL)
    {
        return;
    }

    // Adjust the clip rect to account for the fact that we've eliminated blank space
    // that was around the source image.
    if (spriteDrawOffset.GetX() > 0 || spriteDrawOffset.GetY() > 0)
    {
        Vector2 oldClipRectPosition(clipRect.GetX(), clipRect.GetY());

        clipRect.SetX(clipRect.GetX() - spriteDrawOffset.GetX());
        clipRect.SetY(clipRect.GetY() - spriteDrawOffset.GetY());

        position += Vector2(clipRect.GetX(), clipRect.GetY()) - oldClipRectPosition;

        if (clipRect.GetX() + clipRect.GetWidth() > GetWidth())
        {
            clipRect.SetWidth(GetWidth() - clipRect.GetX());
        }

        if (clipRect.GetY() + clipRect.GetHeight() > GetHeight())
        {
            clipRect.SetHeight(GetHeight() - clipRect.GetY());
        }
    }

    Vector2 pixelSnappedPosition =
        Vector2(
            (int)(position.GetX() + (originalSize.GetX() > 0 && flipHorizontally ? originalSize.GetX() - GetWidth() - spriteDrawOffset.GetX() : spriteDrawOffset.GetX())),
            (int)(position.GetY() + spriteDrawOffset.GetY()));

    GetSpriteSheetImage()->Draw(
        pixelSnappedPosition,
        RectangleWH(
            spriteClipRect.GetX() + clipRect.GetX(),
            spriteClipRect.GetY() + clipRect.GetY(),
            clipRect.GetWidth(),
            clipRect.GetHeight()),
        flipHorizontally,
        false /* flipVertically */,
        1.0,
        color);
}

void Sprite::UpdateReadiness(string newLocationId, bool *pLoadSprite, bool *pDeleteSprite)
{
    vector<string> parentLocationList = Case::GetInstance()->GetParentLocationListForSpriteSheetId(spriteSheetImageId);
    bool isNeeded = false;

    *pLoadSprite = false;
    *pDeleteSprite = false;

    for (unsigned int i = 0; i < parentLocationList.size(); i++)
    {
        if (parentLocationList[i] == newLocationId ||
            parentLocationList[i] == CommonFilesId ||
            parentLocationList[i] == Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId())
        {
            isNeeded = true;
        }
    }

    bool isReady = IsReady();

    if (isNeeded && !isReady)
    {
        *pLoadSprite = true;
    }
    else if (!isNeeded && isReady)
    {
        *pDeleteSprite = true;

        SDL_SemWait(pSpriteSheetSemaphore);
        pSpriteSheetImage = NULL;
        SDL_SemPost(pSpriteSheetSemaphore);
    }
}

bool Sprite::IsReady()
{
    bool isReady;

    SDL_SemWait(pSpriteSheetSemaphore);
    Image *pImage = GetSpriteSheetImageNoCache();
    isReady = pImage != NULL && pImage->IsReady();
    SDL_SemPost(pSpriteSheetSemaphore);

    return isReady;
}

Image * Sprite::GetSpriteSheetImage()
{
    if (managerSource == ManagerSourceCaseFile && Case::GetInstance()->IsLoading())
    {
        return NULL;
    }

    Image *pSprite = NULL;

    SDL_SemWait(pSpriteSheetSemaphore);
    if (pSpriteSheetImage == NULL)
    {
        pSpriteSheetImage = GetSpriteSheetImageNoCache();
    }

    pSprite = pSpriteSheetImage;
    SDL_SemPost(pSpriteSheetSemaphore);

    return pSprite;
}

Image * Sprite::GetSpriteSheetImageNoCache()
{
    SpriteManager *pSpriteManager = NULL;

    switch (managerSource)
    {
        case ManagerSourceCaseFile:
            pSpriteManager = Case::GetInstance()->GetSpriteManager();
            break;

        case ManagerSourceCommonResources:
            pSpriteManager = CommonCaseResources::GetInstance()->GetSpriteManager();
            break;
    }

    return pSpriteManager->GetImageFromId(spriteSheetImageId);
}
