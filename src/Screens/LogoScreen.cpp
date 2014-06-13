/**
 * The logo display screen of the game.
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

#include "LogoScreen.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../ResourceLoader.h"
#include "../CaseInformation/CommonCaseResources.h"

const int opacityFadeDurationMs = 1000;
const int disclaimerShowDurationMs = 3000;

LogoScreen::LogoScreen()
{
    pDisclaimerSprite = NULL;
    pLogoVideo = NULL;

    pInEase = new LinearEase(0, 1, opacityFadeDurationMs);
    pOutEase = new LinearEase(1, 0, opacityFadeDurationMs);

    imageOpacity = 0;
    timeShownDisclaimer = 0;

    finishedLoadingAnimations = false;
}

LogoScreen::~LogoScreen()
{
    UnloadResources();
    delete pInEase;
    pInEase = NULL;
    delete pOutEase;
    pOutEase = NULL;
}

void LogoScreen::LoadResources()
{
    delete pDisclaimerSprite;
    pDisclaimerSprite = ResourceLoader::GetInstance()->LoadImage("image/Disclaimer.png");

    delete pLogoVideo;
    pLogoVideo = new Video(false /* shouldLoop */);
    pLogoVideo->SetVideoAttributes("video/TeamLogoAnimation.mov", 550, 40, 960, 540);
    pLogoVideo->LoadFile();

    finishedLoadingAnimations = true;
}

void LogoScreen::UnloadResources()
{
    delete pDisclaimerSprite;
    pDisclaimerSprite = NULL;
    delete pLogoVideo;
    pLogoVideo = NULL;
}

void LogoScreen::Init()
{
    MLIScreen::Init();

    imageOpacity = 0;
    timeShownDisclaimer = 0;

    pInEase->Begin();
}

void LogoScreen::Update(int delta)
{
    if (!finishedLoadingAnimations || !pDisclaimerSprite->IsReady())
    {
        return;
    }

    if (pOutEase->GetIsFinished())
    {
        pLogoVideo->Update(delta);

        if (pLogoVideo->IsFinished() || MouseHelper::ClickedAnywhere() || KeyboardHelper::ClickPressed())
        {
            nextScreenId = TITLE_SCREEN_ID;
            UnloadResources();
            stopMusic();
            isFinished = true;
            return;
        }
    }
    else
    {
        if (MouseHelper::ClickedAnywhere() || KeyboardHelper::ClickPressed())
        {
            if (!pOutEase->GetIsFinished())
            {
                pInEase->Finish();
                pOutEase->Begin();
                pOutEase->Finish();
                MouseHelper::HandleClick();
            }
        }

        if (!pInEase->GetIsFinished())
        {
            pInEase->Update(delta);
            imageOpacity = pInEase->GetCurrentValue();
        }
        else if (pOutEase->GetIsStarted())
        {
            pOutEase->Update(delta);
            imageOpacity = pOutEase->GetCurrentValue();

            if (pOutEase->GetIsFinished())
            {
                pLogoVideo->Begin();
                CommonCaseResources::GetInstance()->GetAudioManager()->PlayBgmWithId("LogoBGM");
            }
        }
        else
        {
            timeShownDisclaimer += delta;

            if (timeShownDisclaimer > disclaimerShowDurationMs)
            {
                pOutEase->Begin();
            }
        }
    }
}

void LogoScreen::Draw()
{
    if (!finishedLoadingAnimations || !pDisclaimerSprite->IsReady())
    {
        return;
    }

    if (!pOutEase->GetIsFinished())
    {
        pDisclaimerSprite->Draw(Vector2(gScreenWidth - pDisclaimerSprite->width, gScreenHeight - pDisclaimerSprite->height) * 0.5, Color(imageOpacity, 1, 1, 1));
    }
    else
    {
        pLogoVideo->Draw(Vector2(0, 0));
    }
}
