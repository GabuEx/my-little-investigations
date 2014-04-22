/**
 * Basic header/include file for Case.cpp.
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

#ifndef CASE_H
#define CASE_H

#include <stddef.h>
#include "AnimationManager.h"
#include "AudioManager.h"
#include "ContentManager.h"
#include "DialogCharacterManager.h"
#include "DialogCutsceneManager.h"
#include "EvidenceManager.h"
#include "FieldCharacterManager.h"
#include "FieldCutsceneManager.h"
#include "FlagManager.h"
#include "PartnerManager.h"
#include "SpriteManager.h"

#include <vector>
#include <map>

#include <SDL2/SDL_thread.h>

using namespace std;

class Case
{
public:
    static Case * GetInstance()
    {
        SDL_SemWait(pInstanceSemaphore);
        if (pInstance == NULL)
        {
            pInstance = new Case();
        }
        SDL_SemPost(pInstanceSemaphore);

        return pInstance;
    }

    static bool HasInstance()
    {
        bool hasInstance = false;

        SDL_SemWait(pInstanceSemaphore);
        hasInstance = pInstance != NULL;
        SDL_SemPost(pInstanceSemaphore);

        return hasInstance;
    }

    static void ReplaceInstance()
    {
        SDL_SemWait(pInstanceSemaphore);
        delete pInstance;
        pInstance = new Case();
        SDL_SemPost(pInstanceSemaphore);
    }

    static void DestroyInstance()
    {
        SDL_SemWait(pInstanceSemaphore);
        delete pInstance;
        pInstance = NULL;
        SDL_SemPost(pInstanceSemaphore);
    }

    static void LoadFromXml(const string &caseFilePath);
    static void ValidateXml();

    AnimationManager * GetAnimationManager() { return pAnimationManager; }
    AudioManager * GetAudioManager() { return pAudioManager; }
    ContentManager * GetContentManager() { return pContentManager; }
    DialogCharacterManager * GetDialogCharacterManager() { return pDialogCharacterManager; }
    DialogCutsceneManager * GetDialogCutsceneManager() { return pDialogCutsceneManager; }
    EvidenceManager * GetEvidenceManager() { return pEvidenceManager; }
    FieldCharacterManager * GetFieldCharacterManager() { return pFieldCharacterManager; }
    FieldCutsceneManager * GetFieldCutsceneManager() { return pFieldCutsceneManager; }
    FlagManager * GetFlagManager() { return pFlagManager; }
    PartnerManager * GetPartnerManager() { return pPartnerManager; }
    SpriteManager * GetSpriteManager() { return pSpriteManager; }

    void Begin();
    void Update(int delta);
    void Draw();
    void DrawForScreenshot();
    void Reset();

    void CacheState();
    void LoadCachedState();

    void SaveToSaveFile(const string &filePath, const string &fileExtension, const string &saveName);
    void Autosave();
    void GetFieldScreenshot(void **ppPngMemory, size_t *pPngSize);
    void LoadFromSaveFile(const string &filePath);

    string GetPlayerCharacterId() const { return this->playerCharacterId; }
    string GetFilePath() const { return this->filePath; }
    string GetUuid() const { return this->uuid; }

    bool GetIsFinished() const { return this->isFinished; }
    void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

    bool GetIsLoadingSprites() const { return this->isLoadingSprites; }
    void SetIsLoadingSprites(bool isLoadingSprites) { this->isLoadingSprites = isLoadingSprites; }

    bool GetWaitUntilLoaded() const { return this->waitUntilLoaded; }
    void SetWaitUntilLoaded(bool waitUntilLoaded) { this->waitUntilLoaded = waitUntilLoaded; }

    bool GetWantsToLoadResources() const { return this->wantsToLoadResources; }
    void SetWantsToLoadResources(bool wantsToLoadResources) { this->wantsToLoadResources = wantsToLoadResources; }

    bool GetIsUnloaded() const { return this->isUnloaded; }

    string GetLoadStage();
    void SetLoadStage(const string &loadStage);
    bool IsLoading();
    bool IsReady();
    void LoadResources();

    void UpdateLoadedTextures(const string &newLocationId, bool waitUntilLoaded = true);
    static int FinishUpdateLoadedTexturesStatic(void *pData);
    void FinishUpdateLoadedTextures(const string &newLocationId);

    void UnloadResources();

    vector<string> GetParentLocationListForSpriteSheetId(const string &id);
    vector<string> GetParentLocationListForVideoId(const string &id);

private:
    static Case *pInstance;
    static SDL_sem *pInstanceSemaphore;

    Case();
    Case(const Case &other);
    ~Case();

    AnimationManager *pAnimationManager;
    AudioManager *pAudioManager;
    ContentManager *pContentManager;
    DialogCharacterManager *pDialogCharacterManager;
    DialogCutsceneManager *pDialogCutsceneManager;
    EvidenceManager *pEvidenceManager;
    FieldCharacterManager *pFieldCharacterManager;
    FieldCutsceneManager *pFieldCutsceneManager;
    FlagManager *pFlagManager;
    PartnerManager *pPartnerManager;
    SpriteManager *pSpriteManager;

    string playerCharacterId;
    string filePath;
    string uuid;
    bool isFinished;
    bool isLoadingSprites;
    bool isReady;
    bool waitUntilLoaded;
    bool wantsToLoadResources;
    bool isUnloaded;
    string loadStage;
    SDL_sem *pLoadStageSemaphore;

    Area *pCurrentArea;

    map<string, vector<string> > parentLocationListsBySpriteSheetId;
    map<string, vector<string> > parentLocationListsByVideoId;

    class UpdateLoadedTexturesParameters
    {
    public:
        UpdateLoadedTexturesParameters(Case *pCase, const string &newLocationId)
        {
            this->pCase = pCase;
            this->newLocationId = newLocationId;
        }

        Case *pCase;
        string newLocationId;
    };
};

#endif

