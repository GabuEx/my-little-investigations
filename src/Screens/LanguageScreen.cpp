/**
 * The language selection screen of the game.
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

#include "LanguageScreen.h"
#include "../FileFunctions.h"
#include "../Game.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../ResourceLoader.h"
#include "../Utils.h"
#include "../XmlReader.h"
#include "../CaseInformation/Case.h"
#include "../CaseInformation/CommonCaseResources.h"

#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <map>

const int FadeFromBlackDurationMs = 300;

LanguageScreen::LanguageScreen()
{
    pFadeSprite = NULL;

    fadeOpacity = 0;
    pFadeInEase = new LinearEase(1, 0, FadeFromBlackDurationMs);
    pFadeOutEase = new LinearEase(0, 1, FadeFromBlackDurationMs);

    pBackgroundVideo = NULL;

    pSelector = new Selector(270, 182, 460, 320);

    pApplyButton = NULL;
    pBackButton = NULL;

    finishedLoadingAnimations = false;

    EventProviders::GetTextButtonEventProvider()->ClearListener(this);
    EventProviders::GetSelectorEventProvider()->ClearListener(this);
    EventProviders::GetTextButtonEventProvider()->RegisterListener(this);
    EventProviders::GetSelectorEventProvider()->RegisterListener(this);
}

LanguageScreen::~LanguageScreen()
{
    delete pFadeSprite;
    pFadeSprite = NULL;

    delete pFadeInEase;
    pFadeInEase = NULL;
    delete pFadeOutEase;
    pFadeOutEase = NULL;

    delete pBackgroundVideo;
    pBackgroundVideo = NULL;

    delete pSelector;
    pSelector = NULL;

    delete pApplyButton;
    pApplyButton = NULL;
    delete pBackButton;
    pBackButton = NULL;

    EventProviders::GetTextButtonEventProvider()->ClearListener(this);
    EventProviders::GetSelectorEventProvider()->ClearListener(this);
}

void LanguageScreen::LoadResources()
{
    MLIFont *pButtonFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("MenuFont");

    finishedLoadingAnimations = false;

    delete pFadeSprite;
    pFadeSprite = ResourceLoader::GetInstance()->LoadImage("image/TitleScreen/FadeInBackground.png");

    delete pBackgroundVideo;
    pBackgroundVideo = new Video(true /* shouldLoop */);
    pBackgroundVideo->SetVideoAttributes("video/OptionsBackground.mov", 16, 42, 960, 540);
    pBackgroundVideo->LoadFile();

    delete pApplyButton;
    pApplyButton = new TextButton("LanguageScreen/ApplyText", pButtonFont);
    pApplyButton->SetX(36);
    pApplyButton->SetY(488);
    pApplyButton->SetClickSoundEffect("ButtonClick2");

    delete pBackButton;
    pBackButton = new TextButton("LanguageScreen/BackText", pButtonFont);
    pBackButton->SetX(932, HAlignmentRight);
    pBackButton->SetY(488);
    pBackButton->SetClickSoundEffect("ButtonClick4");

    finishedLoadingAnimations = true;
}

void LanguageScreen::UnloadResources()
{
    finishedLoadingAnimations = false;

    delete pFadeSprite;
    pFadeSprite = NULL;

    delete pBackgroundVideo;
    pBackgroundVideo = NULL;

    delete pApplyButton;
    pApplyButton = NULL;
    delete pBackButton;
    pBackButton = NULL;
}

void LanguageScreen::Init()
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

    pSelector->Reset();

    vector<string> localizedResourcesFilePaths = GetLanguageResourcesFilePaths();
    vector<LanguageSelectorItem *> languageSelectorItems;
    unsigned int selectedIndex = 0;

    SelectorSection *pSection = new SelectorSection("LanguageScreen/SelectLanguageText");

    for (string filePath : localizedResourcesFilePaths)
    {
        string fileName = GetFileNameFromFilePath(filePath);
        string languageName;

        if (ResourceLoader::GetInstance()->LoadTemporaryCommonLocalizedResources(filePath))
        {
            XmlReader localizableContentReader("XML/LocalizableContent.xml");
            LocalizableContent temporaryLocalizableContent(&localizableContentReader);

            languageName = temporaryLocalizableContent.GetText("LanguageName");
        }
        else
        {
            continue;
        }

        languageSelectorItems.push_back(new LanguageSelectorItem(fileName, languageName));
    }

    sort(languageSelectorItems.begin(), languageSelectorItems.end(), &LanguageSelectorItem::CompareByLanguageName);

    unsigned int index = 0;
    for (LanguageSelectorItem *pItem : languageSelectorItems)
    {
        if (pItem->GetLocalizedResourcesFileName() == gLocalizedResourcesFileName)
        {
            selectedIndex = index;
        }

        pSection->AddItem(pItem);
        index++;
    }

    pSelector->AddSection(pSection);

    if (selectedIndex < pSelector->GetSection(0)->GetCount())
    {
        pSelector->SelectItem(0, selectedIndex);
    }
}

void LanguageScreen::Update(int delta)
{
    if (!finishedLoadingAnimations)
    {
        return;
    }

    if (!pFadeSprite->IsReady() ||
        !pBackgroundVideo->IsReady() ||
        !pApplyButton->IsReady() ||
        !pBackButton->IsReady())
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
        isFinished = true;
        return;
    }

    pBackgroundVideo->Update(delta);

    if (pFadeOutEase->GetIsStarted())
    {
        return;
    }

    pSelector->Update(delta);

    pApplyButton->SetIsEnabled(pSelector->GetSectionCount() > 0);
    pApplyButton->Update(delta);
    pBackButton->Update(delta);
}

void LanguageScreen::Draw()
{
    if (!finishedLoadingAnimations)
    {
        return;
    }

    if (!pFadeSprite->IsReady() ||
        !pApplyButton->IsReady() ||
        !pBackButton->IsReady())
    {
        return;
    }

    pBackgroundVideo->Draw(Vector2(0, 0));
    pSelector->Draw();

    pApplyButton->Draw();
    pBackButton->Draw();

    pFadeSprite->Draw(Vector2(0, 0), Color(fadeOpacity, 1, 1, 1));
}

void LanguageScreen::OnSelectorSelectionChanged(Selector *pSender, SelectorItem *pSelectedItem)
{
    if (pSender == pSelector)
    {
        LanguageSelectorItem *pLanguageSelectorItem = dynamic_cast<LanguageSelectorItem *>(pSelectedItem);

        if (pLanguageSelectorItem != NULL)
        {
            localizedResourcesFileName = pLanguageSelectorItem->GetLocalizedResourcesFileName();
            languageName = pLanguageSelectorItem->GetLanguageName();
        }
    }
}

void LanguageScreen::OnButtonClicked(TextButton *pSender)
{
    if (pSender == pApplyButton)
    {
        if (ResourceLoader::GetInstance()->LoadNewCommonLocalizedResources(GetLocalizedCommonResourcesDirectoryPath() + localizedResourcesFileName))
        {
            XmlReader localizableContentReader("XML/LocalizableContent.xml");
            gpLocalizableContent->LoadNewLanguage(&localizableContentReader);

            gLocalizedResourcesFileName = localizedResourcesFileName;
            SaveConfigurations();
        }
    }
    else if (pSender == pBackButton)
    {
        nextScreenId = TITLE_SCREEN_ID;
        pFadeOutEase->Begin();
    }
}
