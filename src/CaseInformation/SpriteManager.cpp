/**
 * Manager for all of the sprites in the game; provides their retrieval.
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

#include "SpriteManager.h"
#include "Case.h"
#include "../ResourceLoader.h"
#include "../XmlReader.h"
#include <stdio.h>

SpriteManager::SpriteManager(ManagerSource managerSource)
{
    this->managerSource = managerSource;
    pImageByIdSemaphore = SDL_CreateSemaphore(1);
}

SpriteManager::~SpriteManager()
{
    SDL_SemWait(pImageByIdSemaphore);
    for (map<string, Sprite *>::iterator iter = spriteByIdMap.begin(); iter != spriteByIdMap.end(); ++iter)
    {
        delete iter->second;
    }
    SDL_SemPost(pImageByIdSemaphore);

    SDL_DestroySemaphore(pImageByIdSemaphore);
    pImageByIdSemaphore = NULL;
}

Sprite * SpriteManager::GetSpriteFromId(const string &id)
{
    if (id.length() == 0)
    {
        return NULL;
    }

    return spriteByIdMap[id];
}

Image * SpriteManager::GetImageFromId(const string &id)
{
    Image *pSprite = NULL;

    SDL_SemWait(pImageByIdSemaphore);
    pSprite = smartSpriteByIdMap[id];
    SDL_SemPost(pImageByIdSemaphore);

    return pSprite;
}

void SpriteManager::AddSprite(const string &id, const string &spriteSheetId, RectangleWH spriteClipRect)
{
    spriteByIdMap[id] = new Sprite(spriteSheetId, spriteClipRect);
    spriteByIdMap[id]->SetManagerSource(managerSource);
}

void SpriteManager::AddImage(const string &id, Image *pSprite)
{
    SDL_SemWait(pImageByIdSemaphore);
    if (smartSpriteByIdMap[id] != NULL)
    {
        delete smartSpriteByIdMap[id];
    }
    smartSpriteByIdMap[id] = pSprite;
    SDL_SemPost(pImageByIdSemaphore);
}

void SpriteManager::LoadImageFromFilePath(const string &id)
{
    AddImage(id, ResourceLoader::GetInstance()->LoadImage(smartSpriteFilePathByIdMap[id]));
}

void SpriteManager::DeleteImage(const string &id)
{
    SDL_SemWait(pImageByIdSemaphore);
    if (smartSpriteByIdMap[id] != NULL)
    {
        delete smartSpriteByIdMap[id];
    }
    smartSpriteByIdMap.erase(id);
    SDL_SemPost(pImageByIdSemaphore);
}

void SpriteManager::LoadFromXml(XmlReader *pReader)
{
    pReader->StartElement("SpriteManager");

    pReader->StartElement("SpriteByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        spriteByIdMap[id] = new Sprite(pReader);
        spriteByIdMap[id]->SetManagerSource(managerSource);
    }

    pReader->EndElement();

    pReader->StartElement("SpriteSheetIdToFilePathHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string spriteSheetId = pReader->ReadTextElement("SpriteSheetId");
        string filePath = pReader->ReadTextElement("FilePath");

        smartSpriteFilePathByIdMap[spriteSheetId] = filePath;
    }

    pReader->EndElement();

    pReader->EndElement();
}

void SpriteManager::FinishUpdateLoadedTextures(const string &newLocationId)
{
    for (map<string, Sprite *>::iterator iter = spriteByIdMap.begin(); iter != spriteByIdMap.end(); ++iter)
    {
        bool loadSprite = false;
        bool deleteSprite = false;
        string spriteId = iter->first;
        Sprite *pSprite = iter->second;

        if (pSprite == NULL)
        {
            continue;
        }

        pSprite->UpdateReadiness(newLocationId, &loadSprite, &deleteSprite);

        if (loadSprite)
        {
            ResourceLoader::GetInstance()->AddImageIdToLoadList(pSprite->GetSpriteSheetImageId());
        }
        else if (deleteSprite)
        {
            ResourceLoader::GetInstance()->AddImageIdToDeleteList(pSprite->GetSpriteSheetImageId());
        }
    }
}

void SpriteManager::UnloadResources()
{
    SDL_SemWait(pImageByIdSemaphore);
    for (map<string, Image *>::iterator iter = smartSpriteByIdMap.begin(); iter != smartSpriteByIdMap.end(); ++iter)
    {
        ResourceLoader::GetInstance()->AddImageIdToDeleteList(iter->first);
    }
    SDL_SemPost(pImageByIdSemaphore);
}
