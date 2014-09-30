/**
 * The save/load screen of the game.
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

#include "SelectionScreen.h"
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

MLIFont *SelectionScreen::pLargeFont = NULL;
MLIFont *SelectionScreen::pMediumFont = NULL;
MLIFont *SelectionScreen::pSmallFont = NULL;

const int FadeFromBlackDurationMs = 300;
const int SelectorWidth = 248;

SelectionScreen::SelectionScreen(SelectionScreenType type)
{
    pFadeSprite = NULL;

    fadeOpacity = 0;
    pFadeInEase = new LinearEase(1, 0, FadeFromBlackDurationMs);
    pFadeOutEase = new LinearEase(0, 1, FadeFromBlackDurationMs);

    pBackgroundVideo = NULL;

    pSelector = new Selector(193, 76, SelectorWidth, 377);

    pScreenshotBorderSprite = NULL;
    pScreenshotSprite = NULL;
    pScreenshotFullSizeSprite = NULL;

    fullSizeScreenshotFadeOpacity = 0;
    pFullSizeScreenshotFadeInEase = new LinearEase(0, 1, FadeFromBlackDurationMs);
    pFullSizeScreenshotFadeOutEase = new LinearEase(1, 0, FadeFromBlackDurationMs);

    lastCaseTitle = "";
    lastCaseUuid = "";

    selectionIsCompatible = true;
    selectionRequiredVersion = Version(0, 0, 0);

    caseTitle = "";
    pDividerSprite = NULL;

    pStartCaseButton = NULL;
    pSelectCaseButton = NULL;
    pSaveButton = NULL;
    pLoadButton = NULL;
    pDeleteButton = NULL;
    pBackButton = NULL;

    this->type = type;

    finishedLoadingAnimations = false;
    caseSelected = false;
    canDelete = false;

    pDeleteConfirmOverlay = NULL;
    pEnterSaveNameOverlay = NULL;
    pIncompatibleCaseNotificationOverlay = NULL;

    pDescriptionWidget = NULL;

    EventProviders::GetTextButtonEventProvider()->ClearListener(this);
    EventProviders::GetPromptOverlayEventProvider()->ClearListener(this);
    EventProviders::GetSelectorEventProvider()->ClearListener(this);
    EventProviders::GetTextButtonEventProvider()->RegisterListener(this);
    EventProviders::GetPromptOverlayEventProvider()->RegisterListener(this);
    EventProviders::GetSelectorEventProvider()->RegisterListener(this);
}

SelectionScreen::~SelectionScreen()
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

    delete pFullSizeScreenshotFadeInEase;
    pFullSizeScreenshotFadeInEase = NULL;
    delete pFullSizeScreenshotFadeOutEase;
    pFullSizeScreenshotFadeOutEase = NULL;

    delete pScreenshotBorderSprite;
    pScreenshotBorderSprite = NULL;
    delete pDividerSprite;
    pDividerSprite = NULL;

    delete pStartCaseButton;
    pStartCaseButton = NULL;
    delete pSelectCaseButton;
    pSelectCaseButton = NULL;
    delete pSaveButton;
    pSaveButton = NULL;
    delete pLoadButton;
    pLoadButton = NULL;
    delete pDeleteButton;
    pDeleteButton = NULL;
    delete pBackButton;
    pBackButton = NULL;

    delete pDeleteConfirmOverlay;
    pDeleteConfirmOverlay = NULL;
    delete pEnterSaveNameOverlay;
    pEnterSaveNameOverlay = NULL;
    delete pIncompatibleCaseNotificationOverlay;
    pIncompatibleCaseNotificationOverlay = NULL;

    delete pDescriptionWidget;
    pDescriptionWidget = NULL;

    EventProviders::GetTextButtonEventProvider()->ClearListener(this);
    EventProviders::GetPromptOverlayEventProvider()->ClearListener(this);
    EventProviders::GetSelectorEventProvider()->ClearListener(this);
}

void SelectionScreen::LoadResources()
{
    EnsureFonts();

    MLIFont *pTitleScreenFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("MenuFont");
    MLIFont *pStartFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("SelectionScreen/StartFont");

    finishedLoadingAnimations = false;

    delete pFadeSprite;
    pFadeSprite = ResourceLoader::GetInstance()->LoadImage("image/TitleScreen/FadeInBackground.png");

    delete pBackgroundVideo;
    pBackgroundVideo = new Video(true /* shouldLoop */);
    pBackgroundVideo->SetVideoAttributes("video/CaseSelectionBackground.mov", 4, 167, 960, 540);
    pBackgroundVideo->LoadFile();

    delete pScreenshotBorderSprite;
    pScreenshotBorderSprite = ResourceLoader::GetInstance()->LoadImage("image/CaseSelectionScreen/ScreenshotBorder.png");

    delete pDividerSprite;
    pDividerSprite = ResourceLoader::GetInstance()->LoadImage("image/CaseSelectionScreen/RightDivider.png");

    delete pStartCaseButton;
    pStartCaseButton = new TextButton(pgLocalizableContent->GetText("SelectionScreen/StartCaseText"), pStartFont);
    pStartCaseButton->SetX(580);
    pStartCaseButton->SetY(426);

    delete pSelectCaseButton;
    pSelectCaseButton = new TextButton(pgLocalizableContent->GetText("SelectionScreen/SelectCaseText"), pStartFont);
    pSelectCaseButton->SetX(580);
    pSelectCaseButton->SetY(426);
    pSelectCaseButton->SetClickSoundEffect("ButtonClick3");

    delete pSaveButton;
    pSaveButton = new TextButton(pgLocalizableContent->GetText("SelectionScreen/SaveGameText"), pStartFont);
    pSaveButton->SetX(587);
    pSaveButton->SetY(426);

    delete pLoadButton;
    pLoadButton = new TextButton(pgLocalizableContent->GetText("SelectionScreen/LoadGameText"), pStartFont);
    pLoadButton->SetX(587);
    pLoadButton->SetY(426);

    delete pDeleteButton;
    pDeleteButton = new TextButton(pgLocalizableContent->GetText("SelectionScreen/DeleteGameText"), pStartFont);
    pDeleteButton->SetX(571);
    pDeleteButton->SetY(365);

    delete pBackButton;
    pBackButton = new TextButton(pgLocalizableContent->GetText("SelectionScreen/BackText"), pTitleScreenFont);
    pBackButton->SetX(840);
    pBackButton->SetY(488);
    pBackButton->SetClickSoundEffect("ButtonClick4");

    delete pDeleteConfirmOverlay;
    pDeleteConfirmOverlay = new PromptOverlay(pgLocalizableContent->GetText("SelectionScreen/DeleteConfirmationText"), false /* allowsTextEntry */);
    pDeleteConfirmOverlay->AddButton(pgLocalizableContent->GetText("SelectionScreen/YesText"));
    pDeleteConfirmOverlay->AddButton(pgLocalizableContent->GetText("SelectionScreen/NoText"));
    pDeleteConfirmOverlay->FinalizeButtons();

    delete pEnterSaveNameOverlay;
    pEnterSaveNameOverlay = new PromptOverlay(pgLocalizableContent->GetText("SelectionScreen/SaveNamePromptText"), true /* allowsTextEntry */);
    pEnterSaveNameOverlay->SetMaxPixelWidth(SelectorWidth, CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("HandwritingLargeFont"));

    delete pIncompatibleCaseNotificationOverlay;
    pIncompatibleCaseNotificationOverlay = new PromptOverlay(pgLocalizableContent->GetText("SelectionScreen/IncompatibleCaseFormatText"), false /* allowsTextEntry */);
    pIncompatibleCaseNotificationOverlay->AddButton(pgLocalizableContent->GetText("SelectionScreen/OKText"));
    pIncompatibleCaseNotificationOverlay->FinalizeButtons();

    delete pDescriptionWidget;
    pDescriptionWidget = new TextWidget("", pSmallFont, Color(1.0, 0.0, 0.0, 0.0), HAlignmentCenter, VAlignmentTop);
    pDescriptionWidget->SetX(525);
    pDescriptionWidget->SetY(274);
    pDescriptionWidget->SetWidth(248);

    finishedLoadingAnimations = true;
}

void SelectionScreen::UnloadResources()
{
    finishedLoadingAnimations = false;

    delete pFadeSprite;
    pFadeSprite = NULL;

    delete pBackgroundVideo;
    pBackgroundVideo = NULL;

    delete pScreenshotBorderSprite;
    pScreenshotBorderSprite = NULL;
    delete pDividerSprite;
    pDividerSprite = NULL;

    delete pStartCaseButton;
    pStartCaseButton = NULL;
    delete pSelectCaseButton;
    pSelectCaseButton = NULL;
    delete pSaveButton;
    pSaveButton = NULL;
    delete pLoadButton;
    pLoadButton = NULL;
    delete pDeleteButton;
    pDeleteButton = NULL;
    delete pBackButton;
    pBackButton = NULL;

    delete pDescriptionWidget;
    pDescriptionWidget = NULL;
}

void SelectionScreen::Init()
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
    pDeleteConfirmOverlay->Reset();
    pEnterSaveNameOverlay->Reset();
    pIncompatibleCaseNotificationOverlay->Reset();

    caseSelected = false;
    canDelete = false;

    // If we're loading or selecting a case, we'll allow the player to select whichever case they want.
    // On the other hand, if we're saving, then we'll just skip straight to creating a save file -
    // it makes no sense to have the player select a case in this situation.
    if (type == SelectionScreenTypeSaveGame)
    {
        lastCaseUuid = Case::GetInstance()->GetUuid();
        gCaseFilePath = Case::GetInstance()->GetFilePath();
        selectionIsCompatible = true;
        OnButtonClicked(pSelectCaseButton);
    }
    else
    {
        pSelector->PopulateWithCases(type == SelectionScreenTypeLoadGame /* requireSaveFilesExist */);
    }
}

void SelectionScreen::Update(int delta)
{
    if (!finishedLoadingAnimations)
    {
        return;
    }

    if (!pFadeSprite->IsReady() ||
        !pBackgroundVideo->IsReady() ||
        !pDividerSprite->IsReady() ||
        !pStartCaseButton->IsReady() ||
        !pSelectCaseButton->IsReady() ||
        !pSaveButton->IsReady() ||
        !pLoadButton->IsReady() ||
        !pDeleteButton->IsReady() ||
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
        if (nextScreenId == GAME_SCREEN_ID)
        {
            Game::GetInstance()->PrepareGameMode();
        }

        isFinished = true;
        return;
    }

    pBackgroundVideo->Update(delta);

    if (pFadeOutEase->GetIsStarted())
    {
        return;
    }

    pSelector->UpdateAnimation(delta);

    if (pFullSizeScreenshotFadeInEase->GetIsStarted())
    {
        pFullSizeScreenshotFadeInEase->Update(delta);
        fullSizeScreenshotFadeOpacity = pFullSizeScreenshotFadeInEase->GetCurrentValue();

        if (pFullSizeScreenshotFadeInEase->GetIsFinished())
        {
            pFullSizeScreenshotFadeInEase->Reset();
        }
    }
    else if (pFullSizeScreenshotFadeOutEase->GetIsStarted())
    {
        pFullSizeScreenshotFadeOutEase->Update(delta);
        fullSizeScreenshotFadeOpacity = pFullSizeScreenshotFadeOutEase->GetCurrentValue();

        if (pFullSizeScreenshotFadeOutEase->GetIsFinished())
        {
            pFullSizeScreenshotFadeOutEase->Reset();
        }
    }

    if (pDeleteConfirmOverlay->GetIsShowing())
    {
        pDeleteConfirmOverlay->Update(delta);
        return;
    }
    else if (pEnterSaveNameOverlay->GetIsShowing())
    {
        pEnterSaveNameOverlay->Update(delta);
        return;
    }
    else if (pIncompatibleCaseNotificationOverlay->GetIsShowing())
    {
        pIncompatibleCaseNotificationOverlay->Update(delta);
        return;
    }

    if (fullSizeScreenshotFadeOpacity == 1)
    {
        MouseHelper::SetCursorType(CursorTypeLook);

        if (MouseHelper::ClickedAnywhere() || KeyboardHelper::ClickPressed())
        {
            pFullSizeScreenshotFadeOutEase->Begin();
            playSound("ButtonClick4");
            MouseHelper::HandleClick();
            return;
        }

        return;
    }
    else if (fullSizeScreenshotFadeOpacity == 0 && pScreenshotFullSizeSprite != NULL)
    {
        RectangleWH screenshotRect = RectangleWH(526, 77, pScreenshotSprite->width, pScreenshotSprite->height);

        if (MouseHelper::MouseOverRect(screenshotRect) ||
            MouseHelper::MouseDownOnRect(screenshotRect) ||
            MouseHelper::ClickedOnRect(screenshotRect))
        {
            MouseHelper::SetCursorType(CursorTypeLook);
        }

        if (MouseHelper::ClickedOnRect(screenshotRect))
        {
            pFullSizeScreenshotFadeInEase->Begin();
            playSound("ButtonClick1");
            MouseHelper::HandleClick();
            return;
        }
    }
    else if (fullSizeScreenshotFadeOpacity > 0 && fullSizeScreenshotFadeOpacity < 1)
    {
        MouseHelper::SetCursorType(CursorTypeLook);
        return;
    }

    pSelector->UpdateState();

    if (!caseSelected)
    {
        if (type == SelectionScreenTypeCaseSelection)
        {
            pStartCaseButton->Update(delta);
        }
        else
        {
            pSelectCaseButton->Update(delta);
        }
    }
    else if (type == SelectionScreenTypeSaveGame)
    {
        pSaveButton->Update(delta);
    }
    else if (type == SelectionScreenTypeLoadGame && pSelector->GetSection(0)->GetCount() > 0)
    {
        pLoadButton->Update(delta);
    }

    if (canDelete)
    {
        pDeleteButton->Update(delta);
    }

    pBackButton->Update(delta);
}

void SelectionScreen::Draw()
{
    if (!finishedLoadingAnimations)
    {
        return;
    }

    if (!pFadeSprite->IsReady() ||
        !pDividerSprite->IsReady() ||
        !pStartCaseButton->IsReady() ||
        !pSelectCaseButton->IsReady() ||
        !pSaveButton->IsReady() ||
        !pLoadButton->IsReady() ||
        !pDeleteButton->IsReady() ||
        !pBackButton->IsReady())
    {
        return;
    }

    EnsureFonts();

    pBackgroundVideo->Draw(Vector2(0, 0));
    pSelector->Draw();

    pScreenshotBorderSprite->Draw(Vector2(525, 76));
    pScreenshotSprite->Draw(Vector2(526, 77));
    pMediumFont->Draw(caseTitle, Vector2(649 - pMediumFont->GetWidth(caseTitle) / 2, 229), Color(1.0, 0.0, 0.0, 0.0));
    pDividerSprite->Draw(Vector2(576, 265));

    pDescriptionWidget->Draw();

    if (!caseSelected)
    {
        if (type == SelectionScreenTypeCaseSelection)
        {
            pStartCaseButton->Draw();
        }
        else
        {
            pSelectCaseButton->Draw();
        }
    }
    else if (type == SelectionScreenTypeSaveGame)
    {
        pSaveButton->Draw();
    }
    else if (type == SelectionScreenTypeLoadGame && pSelector->GetSection(0)->GetCount() > 0)
    {
        pLoadButton->Draw();
    }

    if (canDelete)
    {
        pDeleteButton->Draw();
    }

    pBackButton->Draw();

    if (pScreenshotFullSizeSprite != NULL && fullSizeScreenshotFadeOpacity > 0)
    {
        pScreenshotFullSizeSprite->Draw(Vector2(0, 0), Color(fullSizeScreenshotFadeOpacity, 1, 1, 1));
    }

    if (pDeleteConfirmOverlay->GetIsShowing())
    {
        pDeleteConfirmOverlay->Draw();
    }
    else if (pEnterSaveNameOverlay->GetIsShowing())
    {
        pEnterSaveNameOverlay->Draw();
    }
    else if (pIncompatibleCaseNotificationOverlay->GetIsShowing())
    {
        pIncompatibleCaseNotificationOverlay->Draw();
    }

    pFadeSprite->Draw(Vector2(0, 0), Color(fadeOpacity, 1, 1, 1));
}

void SelectionScreen::OnSelectorSelectionChanged(Selector *pSender, SelectorItem *pSelectedItem)
{
    canDelete = false;
    selectionIsCompatible = true;

    if (pSender == pSelector)
    {
        CaseSelectorItem *pCaseSelectorItem = dynamic_cast<CaseSelectorItem *>(pSelectedItem);
        SaveLoadSelectorItem *pSaveLoadSelectorItem = dynamic_cast<SaveLoadSelectorItem *>(pSelectedItem);

        if (pCaseSelectorItem != NULL || pSaveLoadSelectorItem != NULL)
        {
            Image *pItemScreenshotSprite = NULL;
            Image *pItemFullScreenScreenshotSprite = NULL;
            string itemDescription = "";
            string itemFilePath = "";
            string itemSaveName = "";

            if (pCaseSelectorItem != NULL)
            {
                pItemScreenshotSprite = pCaseSelectorItem->GetScreenshotSprite();
                pItemFullScreenScreenshotSprite = pCaseSelectorItem->GetScreenshotFullSizeSprite();
                itemDescription = pCaseSelectorItem->GetCaseDescription();
                itemFilePath = pCaseSelectorItem->GetCaseFilePath();

                gCaseFilePath = itemFilePath;
                caseTitle = pCaseSelectorItem->GetCaseTitle();

                lastCaseTitle = pCaseSelectorItem->GetCaseTitle();
                lastCaseUuid = pCaseSelectorItem->GetCaseUuid();

                selectionIsCompatible = pCaseSelectorItem->GetIsCompatible();
                selectionRequiredVersion = pCaseSelectorItem->GetRequiredVersion();
            }
            else if (pSaveLoadSelectorItem != NULL)
            {
                pItemScreenshotSprite = pSaveLoadSelectorItem->GetScreenshotSprite();
                pItemFullScreenScreenshotSprite = NULL;
                itemDescription = pSaveLoadSelectorItem->GetDescription();
                itemFilePath = pSaveLoadSelectorItem->GetFilePath();
                itemSaveName = pSaveLoadSelectorItem->GetSaveName();
                canDelete = !IsAutosave(itemFilePath);
            }

            EnsureFonts();

            pScreenshotSprite = pItemScreenshotSprite;
            pScreenshotFullSizeSprite = pItemFullScreenScreenshotSprite;

            if (pScreenshotFullSizeSprite == NULL)
            {
                fullSizeScreenshotFadeOpacity = 0;
                pFullSizeScreenshotFadeInEase->Reset();
                pFullSizeScreenshotFadeOutEase->Reset();
            }

            pDescriptionWidget->SetText(itemDescription);
            pDescriptionWidget->WrapText();

            saveName = itemSaveName;
            filePath = itemFilePath;
            fileExtension = "";
        }
        else
        {
            NewSaveSelectorItem *pNewSaveSelectorItem = dynamic_cast<NewSaveSelectorItem *>(pSelectedItem);

            if (pNewSaveSelectorItem != NULL)
            {
                pScreenshotSprite = pNewSaveSelectorItem->GetScreenshot();
                pScreenshotFullSizeSprite = pNewSaveSelectorItem->GetScreenshotFullSize();

                if (pScreenshotFullSizeSprite == NULL)
                {
                    fullSizeScreenshotFadeOpacity = 0;
                    pFullSizeScreenshotFadeInEase->Reset();
                    pFullSizeScreenshotFadeOutEase->Reset();
                }

                caseTitle = lastCaseTitle;

                pDescriptionWidget->SetText("New save file.");

                string fileName = GetSaveFolderPathForCase(lastCaseUuid);

                char buf[16] = { '\0' };
                sprintf(buf, "%d", pSender->GetSection(0)->GetCount());

                saveName = "Save " + string(buf);
                filePath = fileName;
                fileExtension = ".sav";
            }
        }
    }
}

void SelectionScreen::OnButtonClicked(TextButton *pSender)
{
    if (pSender == pStartCaseButton)
    {
        if (selectionIsCompatible)
        {
            gCaseFilePath = filePath;
            nextScreenId = GAME_SCREEN_ID;
            CommonCaseResources::GetInstance()->GetAudioManager()->StopCurrentBgm(false /* isInstant */);
            pFadeOutEase->Begin();
        }
        else
        {
            char promptOverlayText[1024];
            sprintf(promptOverlayText, pgLocalizableContent->GetText("SelectionScreen/IncompatibleCaseText").c_str(), ((string)gVersion).c_str(), ((string)selectionRequiredVersion).c_str());

            pIncompatibleCaseNotificationOverlay->SetHeaderText(promptOverlayText);
            pIncompatibleCaseNotificationOverlay->Begin();
        }
    }
    else if (pSender == pSelectCaseButton)
    {
        caseSelected = true;
        vector<string> filePaths = GetSaveFilePathsForCase(lastCaseUuid);

        pSelector->Reset();
        SelectorSection *pSection = new SelectorSection(pgLocalizableContent->GetText("SelectionScreen/SaveFilesText"));

        if (type == SelectionScreenTypeSaveGame && ResourceLoader::GetInstance()->LoadTemporaryCase(gCaseFilePath))
        {
            Image *pImageSprite = NULL;
            Image *pImageFullSizeSprite = NULL;

            XmlReader reader("caseMetadata.xml");

            reader.StartElement("CaseMetadata");
            lastCaseTitle = reader.ReadTextElement("Title");
            pImageSprite = IsCaseCompleted(lastCaseUuid) ? reader.ReadPngElement("ImageAfterCompletion") : reader.ReadPngElement("ImageBeforeCompletion");

            if (!IsCaseCompleted(lastCaseUuid) && reader.ElementExists("ImageBeforeCompletionFullSize"))
            {
                pImageFullSizeSprite = reader.ReadPngElement("ImageBeforeCompletionFullSize");
            }
            else if (IsCaseCompleted(lastCaseUuid) && reader.ElementExists("ImageAfterCompletionFullSize"))
            {
                pImageFullSizeSprite = reader.ReadPngElement("ImageAfterCompletionFullSize");
            }

            reader.EndElement();

            ResourceLoader::GetInstance()->UnloadTemporaryCase();

            pSection->AddItem(new NewSaveSelectorItem(pImageSprite, pImageFullSizeSprite));
        }

        vector<SaveLoadSelectorItem *> selectorItemList;

        for (unsigned int i = 0; i < filePaths.size(); i++)
        {
            string filePath = filePaths[i];

            // If we're saving, then we should not allow the player to save to the autosave slot.
            if (type == SelectionScreenTypeSaveGame && IsAutosave(filePath))
            {
                continue;
            }

            XmlReader reader(filePath.c_str());

            reader.StartElement("CaseMetadata");

            string saveName = reader.ReadTextElement("SaveName");

            time_t timestamp = (time_t)reader.ReadIntElement("Timestamp");
            struct tm * timeinfo;

            timeinfo = localtime(&timestamp);
            char timeBuf[80] = { 0 };
            strftime(timeBuf, sizeof(timeBuf), "%I:%M %p", timeinfo);
            char monthDayBuf[80] = { 0 };
            strftime(monthDayBuf, sizeof(monthDayBuf), "%B %d", timeinfo);
            char yearBuf[80] = { 0 };
            strftime(yearBuf, sizeof(yearBuf), "%Y", timeinfo);

            char buf[256] = { 0 };
            sprintf(buf, "%s, %s, %s.",  timeBuf, monthDayBuf, yearBuf);

            string description = string(buf);

            // If the hours display contains a leading zero,
            // we'll remove that.
            if (description[0] == '0')
            {
                description = description.substr(1);
            }

            description = string("Save made ") + description;

            Image *pSprite = reader.ReadPngElement("Screenshot");

            reader.EndElement();

            selectorItemList.push_back(
                new SaveLoadSelectorItem(
                    saveName,
                    pSprite,
                    timestamp,
                    description,
                    filePath));
        }

        sort(selectorItemList.begin(), selectorItemList.end(), SaveLoadSelectorItem::CompareByTimestampDescending);

        for (unsigned int i = 0; i < selectorItemList.size(); i++)
        {
            pSection->AddItem(selectorItemList[i]);
        }

        pSelector->AddSection(pSection);
        pSelector->Init();
    }
    else if (pSender == pSaveButton)
    {
        pEnterSaveNameOverlay->Begin(saveName);
    }
    else if (pSender == pLoadButton)
    {
        if (Case::GetInstance()->IsReady())
        {
            if (Case::GetInstance()->GetUuid() == lastCaseUuid)
            {
                Case::GetInstance()->LoadFromSaveFile(filePath);
                gCaseFilePath = "";
            }
            else
            {
                Case::GetInstance()->SetIsFinished(true);
                gSaveFilePath = filePath;
            }
        }
        else
        {
            gSaveFilePath = filePath;
            nextScreenId = GAME_SCREEN_ID;
        }

        pFadeOutEase->Begin();
    }
    else if (pSender == pDeleteButton)
    {
        pDeleteConfirmOverlay->Begin();
    }
    else if (pSender == pBackButton)
    {
        // We don't allow the selection of a case when saving, so we'll want to immediately exit in that case.
        if (caseSelected && type != SelectionScreenTypeSaveGame)
        {
            caseSelected = false;
            pSelector->PopulateWithCases(true /* requireSaveFilesExist */);
        }
        else
        {
            nextScreenId = TITLE_SCREEN_ID;
            gCaseFilePath = "";
            pFadeOutEase->Begin();
        }
    }
}

void SelectionScreen::OnPromptOverlayValueReturned(PromptOverlay *pSender, const string &value)
{
    if (pSender == pDeleteConfirmOverlay && value == pgLocalizableContent->GetText("SelectionScreen/YesText"))
    {
        remove(filePath.c_str());
        pSelector->DeleteCurrentItem();

        if (pSelector->GetSectionCount() == 0)
        {
            // If we just deleted the last save file,
            // then we should exit out of the save file selection screen.
            OnButtonClicked(pBackButton);
        }
    }
    else if (pSender == pEnterSaveNameOverlay && value.length() > 0)
    {
        Case::GetInstance()->SaveToSaveFile(filePath, fileExtension, value);
        pFadeOutEase->Begin();
        gCaseFilePath = "";
        pEnterSaveNameOverlay->KeepOpen();
    }
}

void SelectionScreen::EnsureFonts()
{
    if (pLargeFont == NULL)
    {
        pLargeFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("HandwritingLargeFont");
    }

    if (pMediumFont == NULL)
    {
        pMediumFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("HandwritingMediumFont");
    }

    if (pSmallFont == NULL)
    {
        pSmallFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("HandwritingSmallFont");
    }
}
