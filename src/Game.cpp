/**
 * Highest-level game object; initializes the game and sets everything up,
 * and then provides an entry point into the rest of the game
 * for the purposes of running the update/draw loop.
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

#include "Game.h"
#include "FileFunctions.h"
#include "globals.h"
#include "Rectangle.h"
#include "SharedUtils.h"

#ifdef GAME_EXECUTABLE
#include "mli_audio.h"
#include "MouseHelper.h"
#include "KeyboardHelper.h"
#include "ResourceLoader.h"
#include "TextInputHelper.h"
#include "CaseContent/Dialog.h"
#include "CaseInformation/Case.h"
#include "CaseInformation/CommonCaseResources.h"
#include "Events/EventProviders.h"
#include "Screens/LogoScreen.h"
#include "Screens/TitleScreen.h"
#include "Screens/GameScreen.h"
#include "Screens/OptionsScreen.h"
#include "Screens/LanguageScreen.h"
#include "Screens/SelectionScreen.h"
#include "UserInterface/Slider.h"
#include "UserInterface/Selector.h"
#include "UserInterface/TextButton.h"
#else
#include "MLIFont.h"
#include "Screens/CheckForUpdatesScreen.h"
#endif

#include "resource.h"

#ifdef GAME_EXECUTABLE
#include <iostream>
#include <time.h>
#include <stdlib.h>

extern "C"
{
    #include <libavformat/avformat.h>
}
#endif

#ifndef LAUNCHER
#include <SDL2/SDL_syswm.h>
#endif

#include "MLIException.h"

Game * Game::pInstance = NULL;
string MLIScreen::currentScreenId = "";
string MLIScreen::lastScreenId = "";

#ifdef GAME_EXECUTABLE
string bgmIdList[] =
{
    "LogoBGM",
    "MenuBGM",
};

const unsigned int builtInBgmCount = sizeof(bgmIdList) / sizeof(bgmIdList[0]);

string sfxIdList[] =
{
    "BreakdownSwish",
    "ButtonClick1",
    "ButtonClick2",
    "ButtonClick3",
    "ButtonClick4",
    "ConfrontationEntrance",
    "Interjection",
    "LetterBlip",
    "MenuAmbiance",
    "NewAddition",
    "NewPartner",
    "TabPulse",
    "Unlocking",
};

const unsigned int builtInSfxCount = sizeof(sfxIdList) / sizeof(sfxIdList[0]);
#endif

bool Game::CreateAndInit()
{
    // If an instance already exists, we'll just keep it as is.
    if (pInstance != NULL)
    {
        return true;
    }

#ifdef GAME_EXECUTABLE
    LoadCompletedCases();
    PopulateCaseSignatureMap();
#endif

    gScreenWidth = 960;
    gScreenHeight = 540;
    gFramerate = 60.0; // FPS (0 for unlimited)

#ifdef GAME_EXECUTABLE
    gIsFullscreen = gEnableFullscreen;

    // Seed the pseudo-random number generator.
    srand((unsigned int)time(NULL));
#endif

    // Turn on ALL THE THINGS.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        return false;
    }

    atexit(SDL_Quit);

#ifdef GAME_EXECUTABLE
    SDL_StopTextInput();

    // Register all codecs.
    av_register_all();

    gUiThreadId = SDL_ThreadID();
#else
    // Initialize networking for the purposes of checking for updates.
    curl_global_init(CURL_GLOBAL_ALL);
    gpCurlHandle = curl_easy_init();
#endif

#ifdef GAME_EXECUTABLE
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);

    #ifdef MLI_DEBUG
        #ifdef WIN32
            // Force stdout to console, not text files.
            freopen("CON", "a", stdout);
            freopen("CON", "a", stderr);
        #endif
    #endif

    #ifdef MLI_DEBUG
    cout << gTitle << endl;
    #endif

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    double nativeAspectRatio = (double)mode.w / mode.h;
    double gameAspectRatio = (double)gScreenWidth / gScreenHeight;
    bool constraintIsVertical = nativeAspectRatio > gameAspectRatio;

    gScreenScale = constraintIsVertical ? (double)mode.h / gScreenHeight : (double)mode.w / gScreenWidth;

    if (constraintIsVertical)
    {
        gHorizontalOffset = (Uint16)floor(0.5 + (mode.w - gScreenWidth * gScreenScale) / 2);
    }
    else
    {
        gVerticalOffset = (Uint16)floor(0.5 + (mode.h - gScreenHeight * gScreenScale) / 2);
    }
#endif

    SDL_WindowFlags flags = SDL_WINDOW_SHOWN;

#ifdef GAME_EXECUTABLE
    if (gIsFullscreen)
    {
        flags = (SDL_WindowFlags)(flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
#endif

    gpWindow = SDL_CreateWindow(gTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gScreenWidth, gScreenHeight, flags);

    // If we couldn't get a window, we're pretty screwed.
    if (gpWindow == NULL)
    {
        return false;
    }

#ifdef __WINDOWS
#ifndef LAUNCHER
    // Load and set the window icon - this is needed on Windows,
    // since just having the icon in the .exe doesn't set it for things like
    // the Alt+Tab menu.
    struct SDL_SysWMinfo info;
    SDL_VERSION(&info.version);

    if (SDL_GetWindowWMInfo(gpWindow, &info) && info.subsystem == SDL_SYSWM_WINDOWS)
    {
        HWND hwnd = info.info.win.window;

        const HANDLE hBigIcon =
            LoadImage(
                GetModuleHandle(NULL),
                MAKEINTRESOURCE(IDI_ICON),
                IMAGE_ICON,
                GetSystemMetrics(SM_CXICON),
                GetSystemMetrics(SM_CYICON),
                0);

        if (hBigIcon != NULL)
        {
            SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<WPARAM>(hBigIcon));
        }
    }
#endif
#endif

    gpRenderer = SDL_CreateRenderer(gpWindow, -1, 0);

    // Ditto for the renderer.
    if (gpRenderer == NULL)
    {
        return false;
    }

    // And for the renderer info, too.
    SDL_RendererInfo rendererInfo;

    if (SDL_GetRendererInfo(gpRenderer, &rendererInfo) < 0)
    {
        return false;
    }

    gMaxTextureWidth = rendererInfo.max_texture_width;
    gMaxTextureHeight = rendererInfo.max_texture_height;

#ifdef GAME_EXECUTABLE
    // Initialize audio subsystems.
    initAudio();

    // Set initial volume levels.
    setVolumeMusic(gBackgroundMusicVolume);
    setVolumeSound(gSoundEffectsVolume);
    setVolumeAmbiance(gSoundEffectsVolume);
    setVolumeDialog(gVoiceVolume);

    for (unsigned int i = 0; i < builtInBgmCount; i++)
    {
        ResourceLoader::GetInstance()->PreloadMusic(bgmIdList[i], "BGM/" + bgmIdList[i]);
    }

    for (unsigned int i = 0; i < builtInSfxCount; i++)
    {
        ResourceLoader::GetInstance()->PreloadSound(sfxIdList[i], "SFX/" + sfxIdList[i]);
    }

    TTF_Init();

    MouseHelper::Init();
    KeyboardHelper::Init();
    TextInputHelper::Init();
    EventProviders::Init();

    // We'll be drawing a custom cursor, so let's hide the system cursor.
    SDL_ShowCursor(SDL_DISABLE);
#endif

    pInstance = new Game();
    pInstance->Init();

    return true;
}

#ifdef GAME_EXECUTABLE
void Game::PrepareGameMode()
{
    screenFromIdMap[TITLE_SCREEN_ID]->UnloadResources();
    screenFromIdMap[CASE_SELECTION_SCREEN_ID]->UnloadResources();
    screenFromIdMap[LANGUAGE_SCREEN_ID]->UnloadResources();
    screenFromIdMap[GAME_SCREEN_ID]->LoadResources();
    screenFromIdMap[SAVE_SCREEN_ID]->LoadResources();
}

void Game::PrepareMenuMode()
{
    screenFromIdMap[GAME_SCREEN_ID]->UnloadResources();
    screenFromIdMap[SAVE_SCREEN_ID]->UnloadResources();
    screenFromIdMap[TITLE_SCREEN_ID]->LoadResources();
    screenFromIdMap[CASE_SELECTION_SCREEN_ID]->LoadResources();
    screenFromIdMap[LANGUAGE_SCREEN_ID]->LoadResources();
}

void Game::SetOverlayById(const string &overlayId)
{
    pOverlayScreen = screenFromIdMap[overlayId];
    pOverlayScreen->Init();
}
#endif

void Game::Update(int delta)
{
#ifdef GAME_EXECUTABLE
    if (pOverlayScreen != NULL)
    {
        pOverlayScreen->UpdateAudio(delta);
    }

    if (pCurrentScreen != NULL)
    {
        pCurrentScreen->UpdateAudio(delta);
    }

    if (pOverlayScreen != NULL)
    {
        pOverlayScreen->Update(delta);

        if (pOverlayScreen->GetIsFinished())
        {
            pOverlayScreen = NULL;
        }
    }
#endif

    if (
#ifdef GAME_EXECUTABLE
        pOverlayScreen == NULL &&
#endif
        pCurrentScreen != NULL)
    {
        pCurrentScreen->Update(delta);

        if (pCurrentScreen->GetIsFinished())
        {
            string nextScreenId = pCurrentScreen->GetNextScreenId();

            if (nextScreenId.length() == 0)
            {
                isFinished = true;
            }
            else if (screenFromIdMap.count(nextScreenId) > 0)
            {
                MLIScreen::SetLastScreenId(MLIScreen::GetCurrentScreenId());
                MLIScreen::SetCurrentScreenId(nextScreenId);

                pCurrentScreen = screenFromIdMap[nextScreenId];
                pCurrentScreen->Init();
            }
            else
            {
                ThrowException("Unknown screen ID.");
            }
        }
    }
}

void Game::Draw()
{
#ifdef GAME_EXECUTABLE
    if (pOverlayScreen != NULL)
    {
        pOverlayScreen->Draw();
    }
    else
#endif
    if (pCurrentScreen != NULL)
    {
        pCurrentScreen->Draw();
    }
}

#ifdef GAME_EXECUTABLE
bool Game::GetShowCursor()
{
    if (pCurrentScreen != NULL)
    {
        return pCurrentScreen->GetShowCursor();
    }
    else
    {
        return false;
    }
}
#endif

void Game::Finish()
{
    if (pInstance == NULL)
    {
        return;
    }

#ifdef GAME_EXECUTABLE
    // The game's done, so now we should free resources and shut down.
    #ifdef MLI_DEBUG
    cout << "Exiting game..." << endl;
    #endif

    // If we have a case open, we want to free all its resources.
    Case::DestroyInstance();

    // Stop playing music/SFX/dialog and shut down the audio thread.
    quitAudio();

    // Unload preloaded music and free memory
    for (unsigned int i = 0; i < builtInBgmCount; i++)
    {
        ResourceLoader::GetInstance()->UnloadMusic(bgmIdList[i]);
    }
#else
    // Cleanup network connectivity functionality.
    curl_easy_cleanup(gpCurlHandle);
    gpCurlHandle = NULL;

    curl_global_cleanup();
#endif

    // This game object is now defunct, so delete it.
    delete pInstance;
    pInstance = NULL;

#ifdef GAME_EXECUTABLE
    // Close all open fonts and shut down the font thread.
    TTF_Quit();

    EventProviders::Close();
#endif

    // Free the renderer and window.
    if (gpRenderer != NULL)
    {
        SDL_DestroyRenderer(gpRenderer);
        gpRenderer = NULL;
    }

    if (gpWindow != NULL)
    {
        SDL_DestroyWindow(gpWindow);
        gpWindow = NULL;
    }
}

Game::~Game()
{
    for (map<string, MLIScreen *>::iterator iter = screenFromIdMap.begin(); iter != screenFromIdMap.end(); ++iter)
    {
        delete iter->second;
    }
}

void Game::Init()
{
    isFinished = false;

#ifdef GAME_EXECUTABLE
    MLIFont *pMouseOverFont = new MLIFont("MouseOverFont", 1, false);
    MLIFont *pDialogFont = new MLIFont("DialogFont", 0, false);
    MLIFont *pEvidenceSelectorNameFont = new MLIFont("EvidenceSelectorNameFont", 0, false);
    MLIFont *pEvidenceSelectorDescriptionFont = new MLIFont("EvidenceSelectorDescriptionFont", 0, false);
    MLIFont *pPartnerInformationHeaderFont = new MLIFont("PartnerInformationHeaderFont", 0, false);
    MLIFont *pPartnerInformationContentFont = new MLIFont("PartnerInformationContentFont", 0, false);
    MLIFont *pConfrontationNumberingFont = new MLIFont("ConfrontationNumberingFont", 0, false);
    MLIFont *pTabFont = new MLIFont("TabFont", 0, false);
    MLIFont *pHandwritingLargeFont = new MLIFont("HandwritingLargeFont", 0, false);
    MLIFont *pHandwritingLargeFontOutlined = new MLIFont("HandwritingLargeFont", 1, false);
    MLIFont *pHandwritingMediumFont = new MLIFont("HandwritingMediumFont", 0, false);
    MLIFont *pHandwritingMediumFontOutlined = new MLIFont("HandwritingMediumFont", 1, false);
    MLIFont *pHandwritingSmallFont = new MLIFont("HandwritingSmallFont", 0, false);
    MLIFont *pPromptOverlayFont = new MLIFont("PromptOverlayFont", 0, false);
    MLIFont *pPromptOverlayTextFont = new MLIFont("PromptOverlayTextFont", 0, false);
    MLIFont *pDisclaimerFont = new MLIFont("DisclaimerFont", 1, false);
    MLIFont *pMenuFont = new MLIFont("MenuFont", 1, false);
    MLIFont *pSkipArrowFont = new MLIFont("SkipArrowFont", 1, false);
    MLIFont *pSkipArrowFontInverted = new MLIFont("SkipArrowFont", 1, true);

    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("MouseOverFont", pMouseOverFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("DialogFont", pDialogFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("EvidenceSelectorNameFont", pEvidenceSelectorNameFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("EvidenceSelectorDescriptionFont", pEvidenceSelectorDescriptionFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("PartnerInformationHeaderFont", pPartnerInformationHeaderFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("PartnerInformationContentFont", pPartnerInformationContentFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("ConfrontationNumberingFont", pConfrontationNumberingFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("TabFont", pTabFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("HandwritingLargeFont", pHandwritingLargeFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("HandwritingMediumFont", pHandwritingMediumFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("HandwritingSmallFont", pHandwritingSmallFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("PromptOverlayFont", pPromptOverlayFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("PromptOverlayTextFont", pPromptOverlayTextFont);

    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("DisclaimerFont", pDisclaimerFont);

    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("MenuFont", pMenuFont);

    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("OptionsScreen/TabFont", pHandwritingLargeFontOutlined);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("OptionsScreen/SelectedTabFont", pHandwritingLargeFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("OptionsScreen/OptionFont", pHandwritingMediumFontOutlined);

    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("SelectionScreen/StartFont", pHandwritingLargeFontOutlined);

    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("SkipArrow/Font", pSkipArrowFont);
    CommonCaseResources::GetInstance()->GetFontManager()->AddFont("SkipArrow/InvertedFont", pSkipArrowFontInverted);

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddImage("MultipleChoiceDarkening", ResourceLoader::GetInstance()->LoadImage("image/MultipleChoiceDarkening.png"));

    Arrow::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/ArrowHorizontal.png"),
        ResourceLoader::GetInstance()->LoadImage("image/ArrowVertical.png"));

    SkipArrow::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/ArrowHorizontalDouble.png"),
        ResourceLoader::GetInstance()->LoadImage("image/ArrowHorizontalDoubleInverted.png")
       /* ResourceLoader::GetInstance()->LoadImage("image/FFwdOff.png"),
        ResourceLoader::GetInstance()->LoadImage("image/SkipOff.png"),
        ResourceLoader::GetInstance()->LoadImage("image/FFwdOn.png"),
        ResourceLoader::GetInstance()->LoadImage("image/SkipOn.png")*/);

    Slider::Initialize(
        pHandwritingMediumFont,
        ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SliderTrack.png"),
        ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SliderThumb.png"));

    Selector::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/CaseSelectionScreen/LeftDivider.png"),
        ResourceLoader::GetInstance()->LoadImage("image/CaseSelectionScreen/SelectionHighlight.png"),
        ResourceLoader::GetInstance()->LoadImage("image/CaseSelectionScreen/CaseCompletedStar.png"));

    // +3 to overlap the tab's border with the dialog background's border.
    Tab::Initialize(
        gScreenHeight - Dialog::Height - TabHeight + 3,
        gScreenHeight - TabHeight,
        ResourceLoader::GetInstance()->LoadImage("image/Tab.png"),
        ResourceLoader::GetInstance()->LoadImage("image/TabTop.png"),
        pTabFont);

    Image *pCheckMarkImage = ResourceLoader::GetInstance()->LoadImage("image/CheckMark.png");

    Button::Initialize(
        pDialogFont,
        pCheckMarkImage);

    TextButton::Initialize(
        pCheckMarkImage,
        ResourceLoader::GetInstance()->LoadImage("image/Box.png"));

    ButtonArray::Initialize(
        pDialogFont);

    EvidenceSelector::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/EvidenceSelectorItem.png"),
        ResourceLoader::GetInstance()->LoadImage("image/EvidenceSelectorItemSelection.png"));

    EvidenceDescription::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/EvidenceSelectorDescription.png"),
        pEvidenceSelectorNameFont,
        pEvidenceSelectorDescriptionFont);

    Dialog::Initialize(
        dialogTextArea.GetX(), dialogTextArea.GetY(), dialogTextArea.GetWidth(), dialogTextArea.GetHeight(),
        dialogPadding,
        pDialogFont);

    Notification::Initialize(
        3, 363, 954, 174,
        30,
        pDialogFont,
        480, 180 + TabHeight / 2,
        ResourceLoader::GetInstance()->LoadImage("image/ArrowVerticalDouble.png"));

    Location::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/LocationFadeBackground.png"));
    Confrontation::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/ConfrontationDarkBackground.png"),
        ResourceLoader::GetInstance()->LoadImage("image/ConfrontationHealthCircle.png"),
        pConfrontationNumberingFont);
    Encounter::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/DialogBackground.png"),
        ResourceLoader::GetInstance()->LoadImage("image/EncounterDarkBackground.png"));
    PartnerInformation::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/PartnerInfoBackground.png"),
        pPartnerInformationHeaderFont,
        pPartnerInformationContentFont);

    PromptButton::Initialize(
        pPromptOverlayFont);
    PromptOverlay::Initialize(
        pPromptOverlayFont,
        pPromptOverlayTextFont,
        ResourceLoader::GetInstance()->LoadImage("image/ConfrontationDarkBackground.png"));

    DialogCharacterManager::Initialize(
        ResourceLoader::GetInstance()->LoadImage("image/BreakdownFlashForeground.png"));

    MouseHelper::LoadCursors();
#endif

#ifdef GAME_EXECUTABLE
    screenFromIdMap[LOGO_SCREEN_ID] = new LogoScreen();
    screenFromIdMap[TITLE_SCREEN_ID] = new TitleScreen();
    screenFromIdMap[GAME_SCREEN_ID] = new GameScreen();
    screenFromIdMap[OPTIONS_SCREEN_ID] = new OptionsScreen();
    screenFromIdMap[LANGUAGE_SCREEN_ID] = new LanguageScreen();
    screenFromIdMap[CASE_SELECTION_SCREEN_ID] = new SelectionScreen(SelectionScreenTypeCaseSelection);
    screenFromIdMap[LOAD_SCREEN_ID] = new SelectionScreen(SelectionScreenTypeLoadGame);
    screenFromIdMap[SAVE_SCREEN_ID] = new SelectionScreen(SelectionScreenTypeSaveGame);
#endif
#ifdef UPDATER
    screenFromIdMap[CHECK_FOR_UPDATES_SCREEN_ID] = new CheckForUpdatesScreen(gpUpdatingFont);
#endif

#ifdef GAME_EXECUTABLE
    screenFromIdMap[LOGO_SCREEN_ID]->LoadResources();
    screenFromIdMap[TITLE_SCREEN_ID]->LoadResources();

    MLIScreen::SetCurrentScreenId(LOGO_SCREEN_ID);
    pCurrentScreen = screenFromIdMap[LOGO_SCREEN_ID];
    pOverlayScreen = NULL;
#endif
#ifdef UPDATER
    screenFromIdMap[CHECK_FOR_UPDATES_SCREEN_ID]->LoadResources();

    MLIScreen::SetCurrentScreenId(CHECK_FOR_UPDATES_SCREEN_ID);
    pCurrentScreen = screenFromIdMap[CHECK_FOR_UPDATES_SCREEN_ID];
#endif

    pCurrentScreen->Init();

#ifdef GAME_EXECUTABLE
    screenFromIdMap[OPTIONS_SCREEN_ID]->LoadResources();
    screenFromIdMap[CASE_SELECTION_SCREEN_ID]->LoadResources();
    screenFromIdMap[LANGUAGE_SCREEN_ID]->LoadResources();
    screenFromIdMap[LOAD_SCREEN_ID]->LoadResources();
#endif
}
