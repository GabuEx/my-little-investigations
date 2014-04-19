/**
 * Basic header/include file for TitleScreen.cpp.
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

#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include "MLIScreen.h"
#include "../Animation.h"
#include "../EasingFunctions.h"
#include "../Image.h"
#include "../Video.h"
#include "../Events/ImageButtonEventProvider.h"
#include "../UserInterface/PromptOverlay.h"

class TitleScreen : public MLIScreen, public ImageButtonEventListener
{
public:
    TitleScreen();
    ~TitleScreen();

    void LoadResources();
    void UnloadResources();
    void Init();
    void Update(int delta);
    void Draw();

    void OnButtonClicked(ImageButton *pSender);

private:
    Image *pBackgroundSprite;
    Image *pTitleSprite;
    Image *pFadeInSprite;

    double fadeOpacity;
    EasingFunction *pLongFadeInEase;
    EasingFunction *pFadeInEase;
    EasingFunction *pFadeOutEase;

    Video *pSpikeVideo;
    Animation *pCandleAnimation;
    Video *pTwilightVideo;

    ImageButton *pNewGameButton;
    ImageButton *pLoadGameButton;
    ImageButton *pOptionsButton;
    ImageButton *pExitButton;

    PromptOverlay *pNoCasesPrompt;

    bool finishedLoadingAnimations;
};

#endif
