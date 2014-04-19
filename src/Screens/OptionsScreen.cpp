/**
 * The options screen of the game.
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

#include "OptionsScreen.h"
#include "../FileFunctions.h"
#include "../Game.h"
#include "../globals.h"
#include "../ResourceLoader.h"
#include "../CaseInformation/Case.h"
#include "../CaseInformation/CommonCaseResources.h"

const int FadeFromBlackDurationMs = 300;
const int TransitionDurationMs = 300;

OptionsScreen::OptionsScreen()
{
    pFadeSprite = NULL;

    fadeOpacity = 0;
    pFadeInEase = new LinearEase(1, 0, FadeFromBlackDurationMs);
    pFadeOutEase = new LinearEase(0, 1, FadeFromBlackDurationMs);

    pBackgroundVideo = NULL;

    pGameplayButton = NULL;
    pGameplaySelectedSprite = NULL;
    pAudioButton = NULL;
    pAudioSelectedSprite = NULL;

    pDividerSprite = NULL;

    pDefaultsButton = NULL;
    pBackButton = NULL;

    pTutorialsButtonOff = NULL;
    pTutorialsButtonOn = NULL;
    pEnableHintsButtonOff = NULL;
    pEnableHintsButtonOn = NULL;
    pFullscreenModeButtonOff = NULL;
    pFullscreenModeButtonOn = NULL;
    pEnableSkippingUnseenDialogButtonOff = NULL;
    pEnableSkippingUnseenDialogButtonOn = NULL;
#ifdef ENABLE_DEBUG_MODE
    pDebugModeButtonOff = NULL;
    pDebugModeButtonOn = NULL;
#endif

    pBackgroundMusicSlider = NULL;
    pSoundEffectsSlider = NULL;
    pVoiceSlider = NULL;

    selectedPage = SelectedPageGameplay;

    pGameplayPositionInEase = new SCurveEase(272, 0, TransitionDurationMs);
    pAudioPositionInEase = new SCurveEase(316, 0, TransitionDurationMs);
    pOpacityInEase = new LinearEase(0, 1, TransitionDurationMs);
    pOpacityOutEase = new LinearEase(1, 0, TransitionDurationMs);

    finishedLoadingAnimations = false;

    EventProviders::GetImageButtonEventProvider()->ClearListener(this);
    EventProviders::GetImageButtonEventProvider()->RegisterListener(this);
}

OptionsScreen::~OptionsScreen()
{
    delete pFadeSprite;
    pFadeSprite = NULL;

    delete pFadeInEase;
    pFadeInEase = NULL;
    delete pFadeOutEase;
    pFadeOutEase = NULL;

    delete pBackgroundVideo;
    pBackgroundVideo = NULL;

    delete pGameplayButton;
    pGameplayButton = NULL;
    delete pGameplaySelectedSprite;
    pGameplaySelectedSprite = NULL;
    delete pAudioButton;
    pAudioButton = NULL;
    delete pAudioSelectedSprite;
    pAudioSelectedSprite = NULL;

    delete pDividerSprite;
    pDividerSprite = NULL;

    delete pDefaultsButton;
    pDefaultsButton = NULL;
    delete pBackButton;
    pBackButton = NULL;

    delete pTutorialsButtonOff;
    pTutorialsButtonOff = NULL;
    delete pTutorialsButtonOn;
    pTutorialsButtonOn = NULL;
    delete pEnableHintsButtonOff;
    pEnableHintsButtonOff = NULL;
    delete pEnableHintsButtonOn;
    pEnableHintsButtonOn = NULL;
    delete pFullscreenModeButtonOff;
    pFullscreenModeButtonOff = NULL;
    delete pFullscreenModeButtonOn;
    pFullscreenModeButtonOn = NULL;
    delete pEnableSkippingUnseenDialogButtonOff;
    pEnableSkippingUnseenDialogButtonOff = NULL;
    delete pEnableSkippingUnseenDialogButtonOn;
    pEnableSkippingUnseenDialogButtonOn = NULL;
#ifdef ENABLE_DEBUG_MODE
    delete pDebugModeButtonOff;
    pDebugModeButtonOff = NULL;
    delete pDebugModeButtonOn;
    pDebugModeButtonOn = NULL;
#endif

    delete pBackgroundMusicSlider;
    pBackgroundMusicSlider = NULL;
    delete pSoundEffectsSlider;
    pSoundEffectsSlider = NULL;
    delete pBackgroundMusicSlider;
    pBackgroundMusicSlider = NULL;

    delete pGameplayPositionInEase;
    pGameplayPositionInEase = NULL;
    delete pAudioPositionInEase;
    pAudioPositionInEase = NULL;
    delete pOpacityInEase;
    pOpacityInEase = NULL;
    delete pOpacityOutEase;
    pOpacityOutEase = NULL;

    EventProviders::GetImageButtonEventProvider()->ClearListener(this);
}

void OptionsScreen::LoadResources()
{
    finishedLoadingAnimations = false;

    delete pFadeSprite;
    pFadeSprite = ResourceLoader::GetInstance()->LoadImage("image/TitleScreen/FadeInBackground.png");

    delete pBackgroundVideo;
    pBackgroundVideo = new Video(true /* shouldLoop */);
    pBackgroundVideo->SetVideoAttributes("video/OptionsBackground.mov", 16, 42, 960, 540);
    pBackgroundVideo->LoadFile();

    delete pGameplayButton;
    pGameplayButton =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/GameplayMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/GameplayMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/GameplayMouseDown.png"),
            562,
            182
        );
    pGameplayButton->SetClickSoundEffect("ButtonClick3");

    delete pGameplaySelectedSprite;
    pGameplaySelectedSprite = ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/GameplaySelected.png");

    delete pAudioButton;
    pAudioButton =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/AudioMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/AudioMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/AudioMouseDown.png"),
            607,
            182
        );
    pAudioButton->SetClickSoundEffect("ButtonClick3");

    delete pAudioSelectedSprite;
    pAudioSelectedSprite = ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/AudioSelected.png");

    delete pDividerSprite;
    pDividerSprite = ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/Divider.png");

    delete pDefaultsButton;
    pDefaultsButton =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/DefaultsMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/DefaultsMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/DefaultsMouseDown.png"),
            36,
            488
        );
    pDefaultsButton->SetClickSoundEffect("ButtonClick2");

    delete pBackButton;
    pBackButton =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/BackMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/BackMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/BackMouseDown.png"),
            821,
            488
        );
    pBackButton->SetClickSoundEffect("ButtonClick4");

    delete pTutorialsButtonOff;
    pTutorialsButtonOff =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/TutorialsCheckBoxOffMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/TutorialsCheckBoxOffMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/TutorialsCheckBoxOffMouseDown.png"),
            354,
            251
        );
    pTutorialsButtonOff->SetClickSoundEffect("ButtonClick2");

    delete pTutorialsButtonOn;
    pTutorialsButtonOn =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/TutorialsCheckBoxOnMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/TutorialsCheckBoxOnMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/TutorialsCheckBoxOnMouseDown.png"),
            354,
            251
        );
    pTutorialsButtonOn->SetClickSoundEffect("ButtonClick2");

    delete pEnableHintsButtonOff;
    pEnableHintsButtonOff =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/HintsCheckBoxOffMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/HintsCheckBoxOffMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/HintsCheckBoxOffMouseDown.png"),
            354,
            319
        );
    pEnableHintsButtonOff->SetClickSoundEffect("ButtonClick2");

    delete pEnableHintsButtonOn;
    pEnableHintsButtonOn =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/HintsCheckBoxOnMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/HintsCheckBoxOnMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/HintsCheckBoxOnMouseDown.png"),
            354,
            319
        );
    pEnableHintsButtonOn->SetClickSoundEffect("ButtonClick2");

    delete pFullscreenModeButtonOff;
    pFullscreenModeButtonOff =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/FullscreenCheckBoxOffMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/FullscreenCheckBoxOffMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/FullscreenCheckBoxOffMouseDown.png"),
            354,
            397
        );
    pFullscreenModeButtonOff->SetClickSoundEffect("ButtonClick2");

    delete pFullscreenModeButtonOn;
    pFullscreenModeButtonOn =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/FullscreenCheckBoxOnMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/FullscreenCheckBoxOnMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/FullscreenCheckBoxOnMouseDown.png"),
            354,
            397
        );
    pFullscreenModeButtonOn->SetClickSoundEffect("ButtonClick2");

    delete pEnableSkippingUnseenDialogButtonOff;
    pEnableSkippingUnseenDialogButtonOff =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SkipDialogCheckBoxOffMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SkipDialogCheckBoxOffMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SkipDialogCheckBoxOffMouseDown.png"),
            354,
            459
        );
    pEnableSkippingUnseenDialogButtonOff->SetClickSoundEffect("ButtonClick2");

    delete pEnableSkippingUnseenDialogButtonOn;
    pEnableSkippingUnseenDialogButtonOn =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SkipDialogCheckBoxOnMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SkipDialogCheckBoxOnMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/SkipDialogCheckBoxOnMouseDown.png"),
            354,
            459
        );
    pEnableSkippingUnseenDialogButtonOn->SetClickSoundEffect("ButtonClick2");

#ifdef ENABLE_DEBUG_MODE
    delete pDebugModeButtonOff;
    pDebugModeButtonOff =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("_Debug/image/OptionsScreen/DebugModeCheckBoxOffMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("_Debug/image/OptionsScreen/DebugModeCheckBoxOffMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("_Debug/image/OptionsScreen/DebugModeCheckBoxOffMouseDown.png"),
            354,
            484
        );
    pDebugModeButtonOff->SetClickSoundEffect("ButtonClick2");

    delete pDebugModeButtonOn;
    pDebugModeButtonOn =
        new ImageButton(
            ResourceLoader::GetInstance()->LoadImage("_Debug/image/OptionsScreen/DebugModeCheckBoxOnMouseOff.png"),
            ResourceLoader::GetInstance()->LoadImage("_Debug/image/OptionsScreen/DebugModeCheckBoxOnMouseOver.png"),
            ResourceLoader::GetInstance()->LoadImage("_Debug/image/OptionsScreen/DebugModeCheckBoxOnMouseDown.png"),
            354,
            484
        );
    pDebugModeButtonOn->SetClickSoundEffect("ButtonClick2");
#endif

    delete pBackgroundMusicSlider;
    pBackgroundMusicSlider = new Slider("Background music", 317, 259);
    delete pSoundEffectsSlider;
    pSoundEffectsSlider = new Slider("Sound effects", 317, 344);
    delete pVoiceSlider;
    pVoiceSlider = new Slider("Voice", 317, 433);

    finishedLoadingAnimations = true;
}

void OptionsScreen::Init()
{
    MLIScreen::Init();

    fadeOpacity = 1;
    pFadeInEase->Begin();
    pFadeOutEase->Reset();

    while (!finishedLoadingAnimations)
    {
        SDL_Delay(1);
    }

    pBackgroundVideo->Begin();

    selectedPage = SelectedPageGameplay;

    pBackgroundMusicSlider->SetCurrentValue(gBackgroundMusicVolume);
    pSoundEffectsSlider->SetCurrentValue(gSoundEffectsVolume);
    pVoiceSlider->SetCurrentValue(gVoiceVolume);

    pGameplayPositionInEase->Begin();
    pGameplayPositionInEase->Finish();

    pAudioPositionInEase->Reset();

    pOpacityInEase->Begin();
    pOpacityInEase->Finish();

    pOpacityOutEase->Begin();
    pOpacityOutEase->Finish();
}

void OptionsScreen::Update(int delta)
{
    if (!finishedLoadingAnimations)
    {
        return;
    }

    if (!pFadeSprite->IsReady() ||
        !pBackgroundVideo->IsReady() ||
        !pGameplayButton->IsReady() ||
        !pGameplaySelectedSprite->IsReady() ||
        !pAudioButton->IsReady() ||
        !pAudioSelectedSprite->IsReady() ||
        !pDividerSprite->IsReady() ||
        !pTutorialsButtonOff->IsReady() ||
        !pTutorialsButtonOn->IsReady() ||
        !pEnableHintsButtonOff->IsReady() ||
        !pEnableHintsButtonOn->IsReady() ||
        !pFullscreenModeButtonOff->IsReady() ||
        !pFullscreenModeButtonOn->IsReady() ||
        !pEnableSkippingUnseenDialogButtonOff->IsReady() ||
        !pEnableSkippingUnseenDialogButtonOn->IsReady()
#ifdef ENABLE_DEBUG_MODE
        || !pDebugModeButtonOff->IsReady()
        || !pDebugModeButtonOn->IsReady()
#endif
        )
    {
        return;
    }

    if (!pFadeInEase->GetIsFinished())
    {
        pFadeInEase->Update(delta);
        fadeOpacity = pFadeInEase->GetCurrentValue();
    }
    else if (!pFadeOutEase->GetIsFinished())
    {
        pFadeOutEase->Update(delta);
        fadeOpacity = pFadeOutEase->GetCurrentValue();
    }

    if (pFadeOutEase->GetIsFinished())
    {
        SaveConfigurations();
        nextScreenId = TITLE_SCREEN_ID;
        isFinished = true;
    }

    if (pFadeOutEase->GetIsStarted())
    {
        return;
    }

    pBackgroundVideo->Update(delta);
    pDefaultsButton->Update(delta);
    pBackButton->Update(delta);

    if (isFinished)
    {
        return;
    }

    bool isTransitioning =
        (pGameplayPositionInEase->GetIsStarted() && !pGameplayPositionInEase->GetIsFinished()) ||
        (pAudioPositionInEase->GetIsStarted() && !pAudioPositionInEase->GetIsFinished()) ||
        (pOpacityInEase->GetIsStarted() && !pOpacityInEase->GetIsFinished()) ||
        (pOpacityOutEase->GetIsStarted() && !pOpacityOutEase->GetIsFinished());

    if (pGameplayPositionInEase->GetIsStarted() && !pGameplayPositionInEase->GetIsFinished())
    {
        pGameplayPositionInEase->Update(delta);
    }

    if (pAudioPositionInEase->GetIsStarted() && !pAudioPositionInEase->GetIsFinished())
    {
        pAudioPositionInEase->Update(delta);
    }

    if (pOpacityInEase->GetIsStarted() && !pOpacityInEase->GetIsFinished())
    {
        pOpacityInEase->Update(delta);
    }

    if (pOpacityOutEase->GetIsStarted() && !pOpacityOutEase->GetIsFinished())
    {
        pOpacityOutEase->Update(delta);
    }

    // We don't want any mouse clicks to go through when we're transitioning,
    // so we'll early-out here when we are.
    if (isTransitioning)
    {
        return;
    }

    if (selectedPage == SelectedPageGameplay)
    {
        pAudioButton->Update(delta);
    }
    else if (selectedPage == SelectedPageAudio)
    {
        pGameplayButton->Update(delta);
    }

    if (selectedPage == SelectedPageGameplay)
    {
        if (gEnableTutorials)
        {
            pTutorialsButtonOn->Update(delta);
        }
        else
        {
            pTutorialsButtonOff->Update(delta);
        }

        if (gEnableHints)
        {
            pEnableHintsButtonOn->Update(delta);
        }
        else
        {
            pEnableHintsButtonOff->Update(delta);
        }

        if (gEnableFullscreen)
        {
            pFullscreenModeButtonOn->Update(delta);
        }
        else
        {
            pFullscreenModeButtonOff->Update(delta);
        }

        if (gEnableSkippingUnseenDialog)
        {
            pEnableSkippingUnseenDialogButtonOn->Update(delta);
        }
        else
        {
            pEnableSkippingUnseenDialogButtonOff->Update(delta);
        }

#ifdef ENABLE_DEBUG_MODE
        if (gEnableDebugMode)
        {
            pDebugModeButtonOn->Update(delta);
        }
        else
        {
            pDebugModeButtonOff->Update(delta);
        }
#endif
    }
    else if (selectedPage == SelectedPageAudio)
    {
        pBackgroundMusicSlider->Update(delta);
        pSoundEffectsSlider->Update(delta);
        pVoiceSlider->Update(delta);

        double oldBackgroundMusicVolume = gBackgroundMusicVolume;
        double oldSoundEffectsVolume = gSoundEffectsVolume;
        double oldVoiceVolume = gVoiceVolume;

        gBackgroundMusicVolume = pBackgroundMusicSlider->GetCurrentValue();
        gSoundEffectsVolume = pSoundEffectsSlider->GetCurrentValue();
        gVoiceVolume = pVoiceSlider->GetCurrentValue();

        if (oldBackgroundMusicVolume != gBackgroundMusicVolume)
        {
            CommonCaseResources::GetInstance()->GetAudioManager()->SetMusicVolume(gBackgroundMusicVolume);

            if (Case::HasInstance())
            {
                Case::GetInstance()->GetAudioManager()->SetMusicVolume(gBackgroundMusicVolume);
            }
        }

        if (oldSoundEffectsVolume != gSoundEffectsVolume)
        {
            setVolumeSound(gSoundEffectsVolume);
            setVolumeAmbiance(gSoundEffectsVolume);
        }

        if (oldVoiceVolume != gVoiceVolume)
        {
            setVolumeDialog(gVoiceVolume);
        }
    }
}

void OptionsScreen::Draw()
{
    if (!finishedLoadingAnimations)
    {
        return;
    }

    if (!pFadeSprite->IsReady() ||
        !pGameplayButton->IsReady() ||
        !pGameplaySelectedSprite->IsReady() ||
        !pAudioButton->IsReady() ||
        !pAudioSelectedSprite->IsReady() ||
        !pDividerSprite->IsReady() ||
        !pTutorialsButtonOff->IsReady() ||
        !pTutorialsButtonOn->IsReady() ||
        !pFullscreenModeButtonOff->IsReady() ||
        !pFullscreenModeButtonOn->IsReady() ||
        !pEnableSkippingUnseenDialogButtonOff->IsReady() ||
        !pEnableSkippingUnseenDialogButtonOn->IsReady()
#ifdef ENABLE_DEBUG_MODE
        || !pDebugModeButtonOff->IsReady()
        || !pDebugModeButtonOn->IsReady()
#endif
        )
    {
        return;
    }

    pBackgroundVideo->Draw(Vector2(0, 0));
    pDefaultsButton->Draw();
    pBackButton->Draw();

    pDividerSprite->Draw(Vector2(319, 237));

    double gameplayButtonXOffset = selectedPage == SelectedPageGameplay ? pGameplayPositionInEase->GetCurrentValue() - 272 : 0;
    double gameplaySelectedSpriteXOffset = selectedPage == SelectedPageGameplay ? pGameplayPositionInEase->GetCurrentValue() : 0;
    double audioButtonXOffset = selectedPage == SelectedPageAudio ? pAudioPositionInEase->GetCurrentValue() - 316 : 0;
    double audioSelectedSpriteXOffset = selectedPage == SelectedPageAudio ? pAudioPositionInEase->GetCurrentValue() : 0;
    double gameplayOpacity = selectedPage == SelectedPageGameplay ? pOpacityInEase->GetCurrentValue() : pOpacityOutEase->GetCurrentValue();
    double gameplayButtonOpacity = selectedPage == SelectedPageGameplay ? pOpacityOutEase->GetCurrentValue() : pOpacityInEase->GetCurrentValue();
    double audioOpacity = selectedPage == SelectedPageAudio ? pOpacityInEase->GetCurrentValue() : pOpacityOutEase->GetCurrentValue();
    double audioButtonOpacity = selectedPage == SelectedPageAudio ? pOpacityOutEase->GetCurrentValue() : pOpacityInEase->GetCurrentValue();

    pGameplayButton->Draw(Vector2(gameplayButtonXOffset, 0), gameplayButtonOpacity);
    pGameplaySelectedSprite->Draw(Vector2(292 + gameplaySelectedSpriteXOffset, 184), Color(gameplayOpacity, 1.0, 1.0, 1.0));
    pAudioButton->Draw(Vector2(audioButtonXOffset, 0), audioButtonOpacity);
    pAudioSelectedSprite->Draw(Vector2(292 + audioSelectedSpriteXOffset, 184), Color(audioOpacity, 1.0, 1.0, 1.0));

    if (gEnableTutorials)
    {
        pTutorialsButtonOn->Draw(gameplayOpacity);
    }
    else
    {
        pTutorialsButtonOff->Draw(gameplayOpacity);
    }

    if (gEnableHints)
    {
        pEnableHintsButtonOn->Draw(gameplayOpacity);
    }
    else
    {
        pEnableHintsButtonOff->Draw(gameplayOpacity);
    }

    if (gEnableFullscreen)
    {
        pFullscreenModeButtonOn->Draw(gameplayOpacity);
    }
    else
    {
        pFullscreenModeButtonOff->Draw(gameplayOpacity);
    }

    if (gEnableSkippingUnseenDialog)
    {
        pEnableSkippingUnseenDialogButtonOn->Draw(gameplayOpacity);
    }
    else
    {
        pEnableSkippingUnseenDialogButtonOff->Draw(gameplayOpacity);
    }

#ifdef ENABLE_DEBUG_MODE
    if (gEnableDebugMode)
    {
        pDebugModeButtonOn->Draw(gameplayOpacity);
    }
    else
    {
        pDebugModeButtonOff->Draw(gameplayOpacity);
    }
#endif

    pBackgroundMusicSlider->Draw(audioOpacity);
    pSoundEffectsSlider->Draw(audioOpacity);
    pVoiceSlider->Draw(audioOpacity);

    pFadeSprite->Draw(Vector2(0, 0), Color(fadeOpacity, 1, 1, 1));
}

void OptionsScreen::OnButtonClicked(ImageButton *pSender)
{
    if (pSender == pGameplayButton)
    {
        selectedPage = SelectedPageGameplay;

        pGameplayPositionInEase->Begin();
        pAudioPositionInEase->Reset();
        pOpacityInEase->Begin();
        pOpacityOutEase->Begin();
    }
    else if (pSender == pAudioButton)
    {
        selectedPage = SelectedPageAudio;

        pGameplayPositionInEase->Reset();
        pAudioPositionInEase->Begin();
        pOpacityInEase->Begin();
        pOpacityOutEase->Begin();
    }
    else if (pSender == pDefaultsButton)
    {
        gEnableTutorials = gEnableTutorialsDefault;
        gEnableHints = gEnableHintsDefault;

        if (gEnableFullscreen != gEnableFullscreenDefault)
        {
            gToggleFullscreen = true;
        }

        gEnableFullscreen = gEnableFullscreenDefault;
        gEnableSkippingUnseenDialog = gEnableSkippingUnseenDialogDefault;

        #ifdef ENABLE_DEBUG_MODE
        gEnableDebugMode = gEnableDebugModeDefault;
        #endif

        if (gBackgroundMusicVolume != gBackgroundMusicVolumeDefault)
        {
            CommonCaseResources::GetInstance()->GetAudioManager()->SetMusicVolume(gBackgroundMusicVolumeDefault);

            if (Case::HasInstance())
            {
                Case::GetInstance()->GetAudioManager()->SetMusicVolume(gBackgroundMusicVolumeDefault);
            }
        }

        gBackgroundMusicVolume = gBackgroundMusicVolumeDefault;
        pBackgroundMusicSlider->SetCurrentValue(gBackgroundMusicVolume);

        if (gSoundEffectsVolume != gSoundEffectsVolumeDefault)
        {
            setVolumeSound(gSoundEffectsVolumeDefault);
            setVolumeAmbiance(gSoundEffectsVolumeDefault);
        }

        gSoundEffectsVolume = gSoundEffectsVolumeDefault;
        pSoundEffectsSlider->SetCurrentValue(gSoundEffectsVolume);

        if (gVoiceVolume != gVoiceVolumeDefault)
        {
            setVolumeDialog(gVoiceVolumeDefault);
        }

        gVoiceVolume = gVoiceVolumeDefault;
        pVoiceSlider->SetCurrentValue(gVoiceVolume);
    }
    else if (pSender == pBackButton)
    {
        pFadeOutEase->Begin();
    }
    else if (pSender == pTutorialsButtonOff)
    {
        gEnableTutorials = true;
    }
    else if (pSender == pTutorialsButtonOn)
    {
        gEnableTutorials = false;
    }
    else if (pSender == pEnableHintsButtonOff)
    {
        gEnableHints = true;
    }
    else if (pSender == pEnableHintsButtonOn)
    {
        gEnableHints = false;
    }
    else if (pSender == pFullscreenModeButtonOff)
    {
        gToggleFullscreen = true;
        gEnableFullscreen = true;
    }
    else if (pSender == pFullscreenModeButtonOn)
    {
        gToggleFullscreen = true;
        gEnableFullscreen = false;
    }
    else if (pSender == pEnableSkippingUnseenDialogButtonOff)
    {
        gEnableSkippingUnseenDialog = true;
    }
    else if (pSender == pEnableSkippingUnseenDialogButtonOn)
    {
        gEnableSkippingUnseenDialog = false;
    }
#ifdef ENABLE_DEBUG_MODE
    else if (pSender == pDebugModeButtonOff)
    {
        gEnableDebugMode = true;
    }
    else if (pSender == pDebugModeButtonOn)
    {
        gEnableDebugMode = false;
    }
#endif
}
