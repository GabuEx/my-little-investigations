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

const int GameplayButtonX = 562;
const int GameplayButtonY = 182;
const int SelectedGameplayButtonX = 290;
const int SelectedGameplayButtonY = 182;

const int AudioButtonX = 607;
const int AudioButtonY = 182;
const int SelectedAudioButtonX = 290;
const int SelectedAudioButtonY = 182;

OptionsScreen::OptionsScreen()
{
    pFadeSprite = NULL;

    fadeOpacity = 0;
    pFadeInEase = new LinearEase(1, 0, FadeFromBlackDurationMs);
    pFadeOutEase = new LinearEase(0, 1, FadeFromBlackDurationMs);

    pBackgroundVideo = NULL;

    pGameplayButton = NULL;
    pAudioButton = NULL;

    pDividerSprite = NULL;

    pDefaultsButton = NULL;
    pBackButton = NULL;

    pTutorialsButton = NULL;
    pEnableHintsButton = NULL;
    pFullscreenModeButton = NULL;
    pEnableSkippingUnseenDialogButton = NULL;
#ifdef ENABLE_DEBUG_MODE
    pDebugModeButton = NULL;
#endif

    pBackgroundMusicSlider = NULL;
    pSoundEffectsSlider = NULL;
    pVoiceSlider = NULL;

    selectedPage = SelectedPageGameplay;

    pOpacityInEase = new LinearEase(0, 1, TransitionDurationMs);
    pOpacityOutEase = new LinearEase(1, 0, TransitionDurationMs);

    finishedLoadingAnimations = false;

    EventProviders::GetTextButtonEventProvider()->ClearListener(this);
    EventProviders::GetTextButtonEventProvider()->RegisterListener(this);
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
    delete pAudioButton;
    pAudioButton = NULL;

    delete pDividerSprite;
    pDividerSprite = NULL;

    delete pDefaultsButton;
    pDefaultsButton = NULL;
    delete pBackButton;
    pBackButton = NULL;

    delete pTutorialsButton;
    pTutorialsButton = NULL;
    delete pEnableHintsButton;
    pEnableHintsButton = NULL;
    delete pFullscreenModeButton;
    pFullscreenModeButton = NULL;
    delete pEnableSkippingUnseenDialogButton;
    pEnableSkippingUnseenDialogButton = NULL;
#ifdef ENABLE_DEBUG_MODE
    delete pDebugModeButton;
    pDebugModeButton = NULL;
#endif

    delete pBackgroundMusicSlider;
    pBackgroundMusicSlider = NULL;
    delete pSoundEffectsSlider;
    pSoundEffectsSlider = NULL;
    delete pVoiceSlider;
    pVoiceSlider = NULL;

    delete pOpacityInEase;
    pOpacityInEase = NULL;
    delete pOpacityOutEase;
    pOpacityOutEase = NULL;

    EventProviders::GetTextButtonEventProvider()->ClearListener(this);
}

void OptionsScreen::LoadResources()
{
    MLIFont *pTitleScreenFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("MenuFont");
    MLIFont *pTabFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("OptionsScreen/TabFont");
    MLIFont *pSelectedTabFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("OptionsScreen/SelectedTabFont");
    MLIFont *pOptionFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("OptionsScreen/OptionFont");

    finishedLoadingAnimations = false;

    delete pFadeSprite;
    pFadeSprite = ResourceLoader::GetInstance()->LoadImage("image/TitleScreen/FadeInBackground.png");

    delete pBackgroundVideo;
    pBackgroundVideo = new Video(true /* shouldLoop */);
    pBackgroundVideo->SetVideoAttributes("video/OptionsBackground.mov", 16, 42, 960, 540);
    pBackgroundVideo->LoadFile();

    delete pGameplayButton;
    pGameplayButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/GameplayText"), pTabFont);
    pGameplayButton->SetIsEnabled(false);
    pGameplayButton->SetDisabledFont(pSelectedTabFont);
    pGameplayButton->SetDisabledTextColor(Color(1.0, 0.0, 0.0, 0.0));
    pGameplayButton->SetX(SelectedGameplayButtonX);
    pGameplayButton->SetY(SelectedGameplayButtonY);
    pGameplayButton->SetClickSoundEffect("ButtonClick3");

    delete pAudioButton;
    pAudioButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/AudioText"), pTabFont);
    pAudioButton->SetDisabledFont(pSelectedTabFont);
    pAudioButton->SetDisabledTextColor(Color(1.0, 0.0, 0.0, 0.0));
    pAudioButton->SetX(AudioButtonX);
    pAudioButton->SetY(AudioButtonY);
    pAudioButton->SetClickSoundEffect("ButtonClick3");

    delete pDividerSprite;
    pDividerSprite = ResourceLoader::GetInstance()->LoadImage("image/OptionsScreen/Divider.png");

    delete pDefaultsButton;
    pDefaultsButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/DefaultsText"), pTitleScreenFont);
    pDefaultsButton->SetX(36);
    pDefaultsButton->SetY(488);
    pDefaultsButton->SetClickSoundEffect("ButtonClick2");

    delete pBackButton;
    pBackButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/BackText"), pTitleScreenFont);
    pBackButton->SetX(821);
    pBackButton->SetY(488);
    pBackButton->SetClickSoundEffect("ButtonClick4");

    // gameplay page buttons {

    vector<TextButton *> buttons;

    delete pTutorialsButton;
    pTutorialsButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/EnableTutorialsText"), pOptionFont, true /*checkable*/);
    pTutorialsButton->SetChecked(gEnableTutorials);
    buttons.push_back(pTutorialsButton);

    delete pEnableHintsButton;
    pEnableHintsButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/EnableHintsText"), pOptionFont, true /*checkable*/);
    pEnableHintsButton->SetChecked(gEnableHints);
    buttons.push_back(pEnableHintsButton);

    delete pFullscreenModeButton;
    pFullscreenModeButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/FullscreenModeText"), pOptionFont, true /*checkable*/);
    pFullscreenModeButton->SetChecked(gEnableFullscreen);
    buttons.push_back(pFullscreenModeButton);

    delete pEnableSkippingUnseenDialogButton;
    pEnableSkippingUnseenDialogButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/EnableSkipText"), pOptionFont, true /*checkable*/);
    pEnableSkippingUnseenDialogButton->SetChecked(gEnableSkippingUnseenDialog);
    buttons.push_back(pEnableSkippingUnseenDialogButton);

#ifdef ENABLE_DEBUG_MODE
    delete pDebugModeButton;
    pDebugModeButton = new TextButton(pgLocalizableContent->GetText("OptionsScreen/DebugModeText"), pOptionFont, true /*checkable*/);
    buttons.push_back(pDebugModeButton);
#endif

    RectangleWH buttonsRect(354, 251, 350, 254);

    // setup some common properties
    for (vector<TextButton *>::iterator it = buttons.begin(); it != buttons.end(); it++)
    {
        (*it)->SetMaxWidth(buttonsRect.GetWidth());
        (*it)->SetClickSoundEffect("ButtonClick2");
    }

    // place buttons
    double buttonsHeight = 0;
    for (vector<TextButton *>::iterator it = buttons.begin(); it != buttons.end(); it++)
    {
        buttonsHeight += (*it)->GetHeight();
    }

    double spacing = buttons.size() > 1
            ? (buttonsRect.GetHeight() - buttonsHeight) / (buttons.size() - 1)
            : 0;

    double y = buttonsRect.GetY();
    for (vector<TextButton *>::iterator it = buttons.begin(); it != buttons.end(); it++)
    {
        (*it)->SetX(buttonsRect.GetX());
        (*it)->SetY(y);
        y += (*it)->GetHeight();
        y += spacing;
    }

    // } gameplay page buttons

    delete pBackgroundMusicSlider;
    pBackgroundMusicSlider = new Slider(pgLocalizableContent->GetText("OptionsScreen/BackgroundMusicText"), 317, 259);
    delete pSoundEffectsSlider;
    pSoundEffectsSlider = new Slider(pgLocalizableContent->GetText("OptionsScreen/SoundEffectsText"), 317, 344);
    delete pVoiceSlider;
    pVoiceSlider = new Slider(pgLocalizableContent->GetText("OptionsScreen/VoiceText"), 317, 433);

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
        !pAudioButton->IsReady() ||
        !pDividerSprite->IsReady() ||
        !pTutorialsButton->IsReady() ||
        !pEnableHintsButton->IsReady() ||
        !pFullscreenModeButton->IsReady() ||
        !pEnableSkippingUnseenDialogButton->IsReady()
#ifdef ENABLE_DEBUG_MODE
        || !pDebugModeButton->IsReady()
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
        (pOpacityInEase->GetIsStarted() && !pOpacityInEase->GetIsFinished()) ||
        (pOpacityOutEase->GetIsStarted() && !pOpacityOutEase->GetIsFinished());

    if (pOpacityInEase->GetIsStarted() && !pOpacityInEase->GetIsFinished())
    {
        pOpacityInEase->Update(delta);
    }

    if (pOpacityOutEase->GetIsStarted() && !pOpacityOutEase->GetIsFinished())
    {
        pOpacityOutEase->Update(delta);
    }

    pGameplayButton->Update(delta);
    pAudioButton->Update(delta);

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
        pTutorialsButton->Update(delta);
        pEnableHintsButton->Update(delta);
        pFullscreenModeButton->Update(delta);
        pEnableSkippingUnseenDialogButton->Update(delta);

#ifdef ENABLE_DEBUG_MODE
        pDebugModeButton->Update(delta);
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
        !pAudioButton->IsReady() ||
        !pDividerSprite->IsReady() ||
        !pTutorialsButton->IsReady() ||
        !pFullscreenModeButton->IsReady() ||
        !pEnableSkippingUnseenDialogButton->IsReady()
#ifdef ENABLE_DEBUG_MODE
        || !pDebugModeButton->IsReady()
#endif
        )
    {
        return;
    }

    pBackgroundVideo->Draw(Vector2(0, 0));
    pDefaultsButton->Draw();
    pBackButton->Draw();

    pDividerSprite->Draw(Vector2(319, 237));

    pGameplayButton->Draw();
    pAudioButton->Draw();

    double gameplayOpacity = selectedPage == SelectedPageGameplay ? pOpacityInEase->GetCurrentValue() : pOpacityOutEase->GetCurrentValue();
    double audioOpacity = selectedPage == SelectedPageAudio ? pOpacityInEase->GetCurrentValue() : pOpacityOutEase->GetCurrentValue();

    pTutorialsButton->SetOpacity(gameplayOpacity);
    pTutorialsButton->Draw();
    pEnableHintsButton->SetOpacity(gameplayOpacity);
    pEnableHintsButton->Draw();
    pFullscreenModeButton->SetOpacity(gameplayOpacity);
    pFullscreenModeButton->Draw();
    pEnableSkippingUnseenDialogButton->SetOpacity(gameplayOpacity);
    pEnableSkippingUnseenDialogButton->Draw();

#ifdef ENABLE_DEBUG_MODE
    pDebugModeButton->SetOpacity(gameplayOpacity);
    pDebugModeButton->Draw();
#endif

    pBackgroundMusicSlider->Draw(audioOpacity);
    pSoundEffectsSlider->Draw(audioOpacity);
    pVoiceSlider->Draw(audioOpacity);

    pFadeSprite->Draw(Vector2(0, 0), Color(fadeOpacity, 1, 1, 1));
}

void OptionsScreen::OnButtonClicked(TextButton *pSender)
{
    if (pSender == pGameplayButton)
    {
        pGameplayButton->SetIsEnabled(false);
        pGameplayButton->MoveTo(SelectedGameplayButtonX, SelectedGameplayButtonY, TransitionDurationMs);

        pAudioButton->SetIsEnabled(true);
        pAudioButton->Reappear(AudioButtonX, AudioButtonY, TransitionDurationMs);

        selectedPage = SelectedPageGameplay;

        pOpacityInEase->Begin();
        pOpacityOutEase->Begin();
    }
    else if (pSender == pAudioButton)
    {
        pGameplayButton->SetIsEnabled(true);
        pGameplayButton->Reappear(GameplayButtonX, GameplayButtonY, TransitionDurationMs);

        pAudioButton->SetIsEnabled(false);
        pAudioButton->MoveTo(SelectedAudioButtonX, SelectedGameplayButtonY, TransitionDurationMs);

        selectedPage = SelectedPageAudio;

        pOpacityInEase->Begin();
        pOpacityOutEase->Begin();
    }
    else if (pSender == pDefaultsButton)
    {
        gEnableTutorials = gEnableTutorialsDefault;
        pTutorialsButton->SetChecked(gEnableTutorials);
        gEnableHints = gEnableHintsDefault;
        pEnableHintsButton->SetChecked(gEnableHints);

        if (gEnableFullscreen != gEnableFullscreenDefault)
        {
            gToggleFullscreen = true;
        }

        gEnableFullscreen = gEnableFullscreenDefault;
        pFullscreenModeButton->SetChecked(gEnableFullscreen);
        gEnableSkippingUnseenDialog = gEnableSkippingUnseenDialogDefault;
        pEnableSkippingUnseenDialogButton->SetChecked(gEnableSkippingUnseenDialog);

        #ifdef ENABLE_DEBUG_MODE
        gEnableDebugMode = gEnableDebugModeDefault;
        pDebugModeButton->SetChecked(pDebugModeButton);
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
    else if (pSender == pTutorialsButton)
    {
        gEnableTutorials = pTutorialsButton->GetChecked();
    }
    else if (pSender == pEnableHintsButton)
    {
        gEnableHints = pEnableHintsButton->GetChecked();
    }
    else if (pSender == pFullscreenModeButton)
    {
        gToggleFullscreen = true;
        gEnableFullscreen = pFullscreenModeButton->GetChecked();
    }
    else if (pSender == pEnableSkippingUnseenDialogButton)
    {
        gEnableSkippingUnseenDialog = pEnableSkippingUnseenDialogButton->GetChecked();
    }
#ifdef ENABLE_DEBUG_MODE
    else if (pSender == pDebugModeButton)
    {
        gEnableDebugMode = pDebugModeButton->GetChecked();
    }
#endif
}
