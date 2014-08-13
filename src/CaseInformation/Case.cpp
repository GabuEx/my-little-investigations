/**
 * Root object in the case hierarchy; owns all of the managers whose job
 * is to run the currently-loaded case.
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

#include "Case.h"
#include "../FileFunctions.h"
#include "../miniz.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../ResourceLoader.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"
#include "../Events/EventProviders.h"
#include "../Events/CaseParsingEventProvider.h"

#include <ctime>

Case *Case::pInstance = NULL;
SDL_sem *Case::pInstanceSemaphore = SDL_CreateSemaphore(1);

Case::Case()
    : playerCharacterId("")
    , loadStage("")
{
    pAnimationManager = new AnimationManager(ManagerSourceCaseFile);
    pAudioManager = new AudioManager();
    pContentManager = new ContentManager();
    pDialogCharacterManager = new DialogCharacterManager();
    pDialogCutsceneManager = new DialogCutsceneManager();
    pEvidenceManager = new EvidenceManager();
    pFieldCharacterManager = new FieldCharacterManager();
    pFieldCutsceneManager = new FieldCutsceneManager();
    pFlagManager = new FlagManager();
    pPartnerManager = new PartnerManager();
    pSpriteManager = new SpriteManager(ManagerSourceCaseFile);

    isFinished = false;
    isLoadingSprites = false;
    isReady = false;
    waitUntilLoaded = true;
    wantsToLoadResources = false;

    pCurrentArea = NULL;

    pLoadStageSemaphore = SDL_CreateSemaphore(1);
}

Case::Case(const Case &other)
    : playerCharacterId("")
    , loadStage("")
{
    pAnimationManager = new AnimationManager(ManagerSourceCaseFile);
    pAudioManager = new AudioManager();
    pContentManager = new ContentManager();
    pDialogCharacterManager = new DialogCharacterManager();
    pDialogCutsceneManager = new DialogCutsceneManager();
    pEvidenceManager = new EvidenceManager();
    pFieldCharacterManager = new FieldCharacterManager();
    pFieldCutsceneManager = new FieldCutsceneManager();
    pFlagManager = new FlagManager();
    pPartnerManager = new PartnerManager();
    pSpriteManager = new SpriteManager(ManagerSourceCaseFile);

    isFinished = false;
    isLoadingSprites = false;
    isReady = false;
    waitUntilLoaded = true;
    wantsToLoadResources = false;

    pCurrentArea = NULL;

    pLoadStageSemaphore = SDL_CreateSemaphore(1);
}

Case::~Case()
{
    delete pAnimationManager;
    pAnimationManager = NULL;
    delete pAudioManager;
    pAudioManager = NULL;
    delete pContentManager;
    pContentManager = NULL;
    delete pDialogCharacterManager;
    pDialogCharacterManager = NULL;
    delete pDialogCutsceneManager;
    pDialogCutsceneManager = NULL;
    delete pEvidenceManager;
    pEvidenceManager = NULL;
    delete pFieldCharacterManager;
    pFieldCharacterManager = NULL;
    delete pFieldCutsceneManager;
    pFieldCutsceneManager = NULL;
    delete pFlagManager;
    pFlagManager = NULL;
    delete pPartnerManager;
    pPartnerManager = NULL;
    delete pSpriteManager;
    pSpriteManager = NULL;

    SDL_DestroySemaphore(pLoadStageSemaphore);
    pLoadStageSemaphore = NULL;
}

void Case::LoadFromXml(const string &caseFilePath)
{
    ReplaceInstance();
    pInstance->isUnloaded = false;

    pInstance->filePath = caseFilePath;
    pInstance->uuid = GetUuidFromFilePath(caseFilePath);
    LoadDialogsSeenListForCase(pInstance->uuid);

    {
        XmlReader reader("case.xml");
        reader.StartElement("Case");

        pInstance->SetLoadStage("animations");
        pInstance->pAnimationManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("audio");
        pInstance->pAudioManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("case information");
        pInstance->pContentManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("dialog resources");
        pInstance->pDialogCharacterManager->LoadFromXml(&reader);
        pInstance->pDialogCutsceneManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("evidence");
        pInstance->pEvidenceManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("field resources");
        pInstance->pFieldCharacterManager->LoadFromXml(&reader);
        pInstance->pFieldCutsceneManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("flags");
        pInstance->pFlagManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("partner information");
        pInstance->pPartnerManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("sprite sheets");
        pInstance->pSpriteManager->LoadFromXml(&reader);

        reader.StartElement("ParentLocationListsBySpriteSheetId");
        reader.StartList("Entry");

        while (reader.MoveToNextListItem())
        {
            string spriteSheetId = reader.ReadTextElement("SpriteSheetId");

            reader.StartElement("LocationList");
            reader.StartList("Entry");

            while (reader.MoveToNextListItem())
            {
                string locationId = reader.ReadTextElement("LocationId");

                pInstance->parentLocationListsBySpriteSheetId[spriteSheetId].push_back(locationId);
            }

            reader.EndElement();
        }

        reader.EndElement();

        reader.StartElement("ParentLocationListsByVideoId");
        reader.StartList("Entry");

        while (reader.MoveToNextListItem())
        {
            string videoId = reader.ReadTextElement("VideoId");

            reader.StartElement("LocationList");
            reader.StartList("Entry");

            while (reader.MoveToNextListItem())
            {
                string locationId = reader.ReadTextElement("LocationId");

                pInstance->parentLocationListsByVideoId[videoId].push_back(locationId);
            }

            reader.EndElement();
        }

        reader.EndElement();

        reader.EndElement();
    }

    pInstance->playerCharacterId = pInstance->pFieldCharacterManager->playerCharacterId;

    pInstance->isReady = true;
    EventProviders::GetCaseParsingEventProvider()->RaiseCaseParsingComplete("case.xml");
}

void Case::ValidateXml()
{
    // TODO: Check to make sure that everything exists that should exist.
    /*ReplaceInstance();
    pInstance->isUnloaded = false;

    {
        XmlReader reader("case.xml");
        reader.StartElement("Case");

        pInstance->SetLoadStage("animations");
        pInstance->pAnimationManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("audio");
        pInstance->pAudioManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("case information");
        pInstance->pContentManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("dialog resources");
        pInstance->pDialogCharacterManager->LoadFromXml(&reader);
        pInstance->pDialogCutsceneManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("evidence");
        pInstance->pEvidenceManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("field resources");
        pInstance->pFieldCharacterManager->LoadFromXml(&reader);
        pInstance->pFieldCutsceneManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("flags");
        pInstance->pFlagManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("partner information");
        pInstance->pPartnerManager->LoadFromXml(&reader);

        pInstance->SetLoadStage("sprite sheets");
        pInstance->pSpriteManager->LoadFromXml(&reader);

        reader.StartElement("ParentLocationListsBySpriteSheetId");
        reader.StartList("Entry");

        while (reader.MoveToNextListItem())
        {
            string spriteSheetId = reader.ReadTextElement("SpriteSheetId");

            reader.StartElement("LocationList");
            reader.StartList("Entry");

            while (reader.MoveToNextListItem())
            {
                string locationId = reader.ReadTextElement("LocationId");

                pInstance->parentLocationListsBySpriteSheetId[spriteSheetId].push_back(locationId);
            }

            reader.EndElement();
        }

        reader.EndElement();

        reader.StartElement("ParentLocationListsByVideoId");
        reader.StartList("Entry");

        while (reader.MoveToNextListItem())
        {
            string videoId = reader.ReadTextElement("VideoId");

            reader.StartElement("LocationList");
            reader.StartList("Entry");

            while (reader.MoveToNextListItem())
            {
                string locationId = reader.ReadTextElement("LocationId");

                pInstance->parentLocationListsByVideoId[videoId].push_back(locationId);
            }

            reader.EndElement();
        }

        reader.EndElement();

        reader.EndElement();
    }

    {
        XmlReader reader("caseMetadata.xml");
        reader.StartElement("CaseMetadata");

        pInstance->uuid = reader.ReadTextElement("UUID");

        reader.EndElement();
    }

    pInstance->playerCharacterId = pInstance->pFieldCharacterManager->playerCharacterId;

    pInstance->isReady = true;
    EventProviders::GetCaseParsingEventProvider()->RaiseCaseParsingComplete("case.xml");

    pInstance->SetLoadStage("");*/
}

void Case::Begin()
{
    pCurrentArea->Begin(Case::GetInstance()->GetContentManager()->GetInitialLocationId(), false /* isLoadingFromSaveFile */);
}

void Case::Update(int delta)
{
    pCurrentArea->Update(delta);
}

void Case::Draw()
{
    pCurrentArea->Draw();
}

void Case::DrawForScreenshot()
{
    pCurrentArea->DrawForScreenshot();
}

void Case::Reset()
{
    pContentManager->Reset();
    pEvidenceManager->Reset();
    pFieldCutsceneManager->Reset();
    pFlagManager->Reset();

    SetIsFinished(false);
    SetLoadStage("");

    pCurrentArea = Case::GetInstance()->GetContentManager()->GetAreaFromId(Case::GetInstance()->GetContentManager()->GetInitialAreaId());
}

void Case::SaveToSaveFile(const string &filePath, const string &fileExtension, const string &saveName)
{
    XmlWriter writer(filePath.c_str(), fileExtension.length() > 0 ? fileExtension.c_str() : NULL);

    writer.StartElement("Case");

    pContentManager->SaveToSaveFile(&writer);
    pEvidenceManager->SaveToSaveFile(&writer);
    pFieldCutsceneManager->SaveToSaveFile(&writer);
    pFlagManager->SaveToSaveFile(&writer);
    pPartnerManager->SaveToSaveFile(&writer);

    pCurrentArea->SaveToSaveFile(&writer);

    writer.EndElement();

    writer.StartElement("CaseMetadata");

    writer.WriteTextElement("SaveName", saveName);
    writer.WriteIntElement("Timestamp", (int)time(NULL));

    void *pPngMemory = NULL;
    size_t pngSize = 0;
    Case::GetInstance()->GetFieldScreenshot(&pPngMemory, &pngSize);

    writer.WritePngElement("Screenshot", pPngMemory, pngSize);

    free(pPngMemory);

    writer.EndElement();
}

void Case::Autosave()
{
    SaveToSaveFile(GetSaveFolderPathForCase(uuid) + "00000000-0000-0000-0000-000000000000.sav", "", "Autosave");
}

void Case::GetFieldScreenshot(void **ppPngMemory, size_t *pPngSize)
{
    gIsSavingScreenshot = true;
    gScreenshotWidth = 246;
    gScreenshotHeight = 138;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Uint32 targetPixelFormat = SDL_PIXELFORMAT_ABGR8888;
#else
    Uint32 targetPixelFormat = SDL_PIXELFORMAT_RGBA8888;
#endif

    SDL_SetRenderDrawColor(gpRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gpRenderer);

    DrawForScreenshot();

    Uint8 targetBytesPerPixel = SDL_BYTESPERPIXEL(targetPixelFormat);
    int targetPitch = gScreenshotWidth * targetBytesPerPixel;
    Uint8 *pPixels = new Uint8[gScreenshotHeight * targetPitch];

    SDL_Rect rect = { 0, 0, gScreenshotWidth, gScreenshotHeight };
    SDL_RenderReadPixels(gpRenderer, &rect, targetPixelFormat, pPixels, targetPitch);

    *ppPngMemory = tdefl_write_image_to_png_file_in_memory(pPixels, gScreenshotWidth, gScreenshotHeight, targetBytesPerPixel, pPngSize);
    delete [] pPixels;

    gIsSavingScreenshot = false;
}

void Case::LoadFromSaveFile(const string &filePath)
{
    XmlReader reader(filePath.c_str());

    reader.StartElement("Case");

    pContentManager->LoadFromSaveFile(&reader);
    pEvidenceManager->LoadFromSaveFile(&reader);
    pFieldCutsceneManager->LoadFromSaveFile(&reader);
    pFlagManager->LoadFromSaveFile(&reader);
    pPartnerManager->LoadFromSaveFile(&reader);

    Area::LoadFromSaveFile(&reader, &pCurrentArea);

    reader.EndElement();

    SetIsFinished(false);
    SetLoadStage("");
}

void Case::CacheState()
{
    pEvidenceManager->CacheState();
    pFlagManager->CacheState();
    pPartnerManager->CacheState();
}

void Case::LoadCachedState()
{
    pEvidenceManager->LoadCachedState();
    pFlagManager->LoadCachedState();
    pPartnerManager->LoadCachedState();
}

string Case::GetLoadStage()
{
    string loadStage;

    SDL_SemWait(pLoadStageSemaphore);
    loadStage = this->loadStage;
    SDL_SemPost(pLoadStageSemaphore);

    return loadStage;
}

void Case::SetLoadStage(const string &loadStage)
{
    SDL_SemWait(pLoadStageSemaphore);
    this->loadStage = loadStage;
    SDL_SemPost(pLoadStageSemaphore);
}

bool Case::IsLoading()
{
    if (!waitUntilLoaded)
    {
        return false;
    }

    if (isLoadingSprites)
    {
        return true;
    }

    if (ResourceLoader::GetInstance()->HasImageTexturesToLoad())
    {
        return true;
    }

    return false;
}

bool Case::IsReady()
{
    return isReady;
}

void Case::LoadResources()
{
    wantsToLoadResources = false;
    ResourceLoader::GetInstance()->SnapLoadStepQueue();
}

void Case::UpdateLoadedTextures(const string &newLocationId, bool waitUntilLoaded)
{
    SetWaitUntilLoaded(waitUntilLoaded);
    SetIsLoadingSprites(true);
    cout << "Loading sprites for location \"" << newLocationId << "\"." << endl;
    pSpriteManager->FinishUpdateLoadedTextures(newLocationId);
    pAnimationManager->FinishUpdateLoadedTextures(newLocationId);
    SetWantsToLoadResources(true);
}

int Case::FinishUpdateLoadedTexturesStatic(void *pData)
{
    UpdateLoadedTexturesParameters *pParams = reinterpret_cast<UpdateLoadedTexturesParameters *>(pData);

    Case *pThis = pParams->pCase;
    string newLocationId = pParams->newLocationId;

    delete pParams;

    pThis->FinishUpdateLoadedTextures(newLocationId);

    return 0;
}

void Case::FinishUpdateLoadedTextures(const string &newLocationId)
{
    pSpriteManager->FinishUpdateLoadedTextures(newLocationId);
    pAnimationManager->FinishUpdateLoadedTextures(newLocationId);
    SetWantsToLoadResources(true);
}

void Case::UnloadResources()
{
    pAnimationManager->UnloadResources();
    pSpriteManager->UnloadResources();
    isUnloaded = true;
}

vector<string> Case::GetParentLocationListForSpriteSheetId(const string &id)
{
    return parentLocationListsBySpriteSheetId[id];
}

vector<string> Case::GetParentLocationListForVideoId(const string &id)
{
    return parentLocationListsByVideoId[id];
}
