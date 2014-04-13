/**
 * Basic header/include file for OptionsScreen.cpp.
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

#ifndef OPTIONSSCREEN_H
#define OPTIONSSCREEN_H

#include "Screen.h"
#include "../Animation.h"
#include "../EasingFunctions.h"
#include "../Image.h"
#include "../Video.h"
#include "../UserInterface/Slider.h"
#include "../Events/ImageButtonEventProvider.h"

enum SelectedPage
{
    SelectedPageGameplay,
    SelectedPageAudio,
};

class OptionsScreen : public Screen, public ImageButtonEventListener
{
public:
    OptionsScreen();
    ~OptionsScreen();

    void LoadResources();
    void Init();
    void Update(int delta);
    void Draw();

    void OnButtonClicked(ImageButton *pSender);

private:
    Image *pFadeSprite;

    double fadeOpacity;
    EasingFunction *pFadeInEase;
    EasingFunction *pFadeOutEase;

    // Global visuals
    Video *pBackgroundVideo;

    ImageButton *pGameplayButton;
    Image *pGameplaySelectedSprite;
    ImageButton *pAudioButton;
    Image *pAudioSelectedSprite;

    Image *pDividerSprite;

    ImageButton *pDefaultsButton;
    ImageButton *pBackButton;

    // Gameplay visuals
    ImageButton *pTutorialsButtonOff;
    ImageButton *pTutorialsButtonOn;
    ImageButton *pEnableHintsButtonOff;
    ImageButton *pEnableHintsButtonOn;
    ImageButton *pFullscreenModeButtonOff;
    ImageButton *pFullscreenModeButtonOn;
    ImageButton *pEnableSkippingUnseenDialogButtonOff;
    ImageButton *pEnableSkippingUnseenDialogButtonOn;
#ifdef ENABLE_DEBUG_MODE
    ImageButton *pDebugModeButtonOff;
    ImageButton *pDebugModeButtonOn;
#endif

    // Audio visuals
    Slider *pBackgroundMusicSlider;
    Slider *pSoundEffectsSlider;
    Slider *pVoiceSlider;

    SelectedPage selectedPage;

    EasingFunction *pGameplayPositionInEase;
    EasingFunction *pAudioPositionInEase;
    EasingFunction *pOpacityInEase;
    EasingFunction *pOpacityOutEase;

    bool finishedLoadingAnimations;
};

#endif
