/**
 * The main game screen of the game.
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

#include "GameScreen.h"
#include "../Game.h"
#include "../globals.h"
#include "../ResourceLoader.h"
#include "../CaseInformation/Case.h"
#include "../CaseInformation/CommonCaseResources.h"

const int LoadingDotsUpdateDelayMs = 500;

GameScreen::GameScreen()
{
    caseIsReady = false;
    caseNeedsReset = false;
    startedLoadingResources = false;
    loadingTextStage = 0;
    timeSinceLastLoadingDotsUpdate = 0;

    pConfrontationEntranceBackgroundVideo = NULL;
    pConfrontationEntranceVfxVideo = NULL;

    EventProviders::GetCaseParsingEventProvider()->ClearListener(this);
    EventProviders::GetCaseParsingEventProvider()->RegisterListener(this);
}

GameScreen::~GameScreen()
{
    EventProviders::GetCaseParsingEventProvider()->ClearListener(this);
}

void GameScreen::LoadResources()
{
    if (CommonCaseResources::GetInstance()->GetAnimationManager()->GetVideoFromId("ConfrontationEntranceBackground") != NULL)
    {
        return;
    }

    CommonCaseResources::GetInstance()->GetAnimationManager()->AddVideo("ConfrontationEntranceBackground", &pConfrontationEntranceBackgroundVideo, true /* shouldLoop */);
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddImage("ConfrontationEntranceBlock", ResourceLoader::GetInstance()->LoadImage("image/ConfrontationEntranceBlock.png"));
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddVideo("ConfrontationEntranceVfx", &pConfrontationEntranceVfxVideo);

    pConfrontationEntranceBackgroundVideo->SetVideoAttributes("video/ConfrontationIntroBackground.mov", 30, 42, 960, 360);
    pConfrontationEntranceBackgroundVideo->LoadFile();
    pConfrontationEntranceVfxVideo->SetVideoAttributes("video/ConfrontationIntroVFX.mov", 108, 42, 960, 360);
    pConfrontationEntranceVfxVideo->AddSound(0, "ConfrontationEntrance");
    pConfrontationEntranceVfxVideo->LoadFile();

    CommonCaseResources::GetInstance()->GetAnimationManager()->AddVideo("SpeedLines", &pSpeedLinesVideo, true /* shouldLoop */);

    pSpeedLinesVideo->SetVideoAttributes("video/SpeedLines.mov", 30, 42, 960, 360);
    pSpeedLinesVideo->LoadFile();
}

void GameScreen::UnloadResources()
{
    caseIsReady = false;
    CommonCaseResources::GetInstance()->GetAnimationManager()->DeleteVideo("ConfrontationEntranceBackground");
    CommonCaseResources::GetInstance()->GetSpriteManager()->DeleteImage("ConfrontationEntranceBlock");
    CommonCaseResources::GetInstance()->GetAnimationManager()->DeleteVideo("ConfrontationEntranceVfx");

    CommonCaseResources::GetInstance()->GetAnimationManager()->DeleteVideo("SpeedLines");
}

void GameScreen::Init()
{
    MLIScreen::Init();
    caseNeedsReset = true;
    isFinishing = false;
}

int GameScreen::LoadCaseStatic(void *pData)
{
    LoadCaseParameters *pParams = reinterpret_cast<LoadCaseParameters *>(pData);

    ResourceLoader::GetInstance()->LoadCase(pParams->GetCaseFilePath());
    Case::LoadFromXml(pParams->GetCaseFilePath());

    delete pParams;
    return 0;
}

int GameScreen::UnloadCaseStatic(void *pData)
{
    Case::GetInstance()->UnloadResources();
    return 0;
}

void GameScreen::Update(int delta)
{
    if (!caseIsReady && gCaseFilePath.length() > 0)
    {
        SDL_CreateThread(GameScreen::LoadCaseStatic, "LoadCaseThread", new LoadCaseParameters(gCaseFilePath));
        gCaseFilePath = "";
    }

    if (isFinishing)
    {
        if (Case::GetInstance()->GetIsUnloaded())
        {
            ResourceLoader::GetInstance()->SnapLoadStepQueue();

            while (ResourceLoader::GetInstance()->HasLoadStep())
            {
                ResourceLoader::GetInstance()->TryRunOneLoadStep();
            }

            ResourceLoader::GetInstance()->UnloadCase();
            Case::DestroyInstance();
            caseIsReady = false;
            isFinishing = false;

            // If the case file path contains a path, then we're loading a new case,
            // so we only want to do go back to the title screen if it doesn't.
            if (gCaseFilePath.length() == 0)
            {
                Game::GetInstance()->PrepareMenuMode();
                isFinished = true;
                nextScreenId = TITLE_SCREEN_ID;
            }
        }

        return;
    }

    if (!caseIsReady)
    {
        timeSinceLastLoadingDotsUpdate += delta;

        while (timeSinceLastLoadingDotsUpdate > LoadingDotsUpdateDelayMs)
        {
            timeSinceLastLoadingDotsUpdate -= LoadingDotsUpdateDelayMs;
            loadingTextStage++;

            if (loadingTextStage > 2)
            {
                loadingTextStage = 0;
            }
        }

        return;
    }
    else if (Case::GetInstance()->IsLoading())
    {
        if (Case::GetInstance()->GetWantsToLoadResources())
        {
            Case::GetInstance()->LoadResources();
            startedLoadingResources = true;
        }
        else if (startedLoadingResources && !ResourceLoader::GetInstance()->HasLoadStep() && !ResourceLoader::GetInstance()->HasImageTexturesToLoad())
        {
            Case::GetInstance()->SetIsLoadingSprites(false);
            startedLoadingResources = false;
        }

        return;
    }

    if (caseNeedsReset)
    {
        Case::GetInstance()->Reset();
        Case::GetInstance()->Begin();
        caseNeedsReset = false;
    }

    if (Case::GetInstance()->GetIsFinished())
    {
        stopMusic();
        isFinishing = true;
        SDL_CreateThread(GameScreen::UnloadCaseStatic, "UnloadCaseThread", this);
        return;
    }

    Case::GetInstance()->Update(delta);
}

void GameScreen::UpdateAudio(int delta)
{
    if (caseIsReady)
    {
        Case::GetInstance()->GetAudioManager()->Update(delta);
    }
}

void GameScreen::Draw()
{
    if (isFinishing)
    {
        return;
    }

    if (!caseIsReady)
    {
        char loadingStringForWidth[256];
        char loadingString[256];

        string loadStage = Case::GetInstance()->GetLoadStage();

        sprintf(loadingStringForWidth, pgLocalizableContent->GetText("GameScreen/LoadingFormatTextForWidth").c_str(), loadStage.c_str());

        MLIFont *pFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("MouseOverFont");
        int stringWidth = pFont->GetWidth(string(loadingStringForWidth));

        switch (loadingTextStage)
        {
        case 0:
            sprintf(loadingString, pgLocalizableContent->GetText("GameScreen/LoadingFormatText0").c_str(), loadStage.c_str());
            break;

        case 1:
            sprintf(loadingString, pgLocalizableContent->GetText("GameScreen/LoadingFormatText1").c_str(), loadStage.c_str());
            break;

        case 2:
            sprintf(loadingString, pgLocalizableContent->GetText("GameScreen/LoadingFormatText2").c_str(), loadStage.c_str());
            break;

        default:
            throw MLIException("We should never enter loading text stage 3 or higher.");
        }

        pFont->Draw(string(loadingString), (Vector2(gScreenWidth, gScreenHeight) * 0.5) - (Vector2(stringWidth, pFont->GetLineHeight()) * 0.5));

        return;
    }
    else if (Case::GetInstance()->IsLoading())
    {
        return;
    }
    else if (caseNeedsReset)
    {
        return;
    }

    Case::GetInstance()->Draw();
}

void GameScreen::OnCaseParsingComplete(const string &caseFileName)
{
    if (caseFileName == "case.xml")
    {
        if (gSaveFilePath.length() > 0)
        {
            Case::GetInstance()->Reset();
            caseNeedsReset = false;
            Case::GetInstance()->LoadFromSaveFile(gSaveFilePath);
            gSaveFilePath = "";
        }

        caseIsReady = true;
    }
}
