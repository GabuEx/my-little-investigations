/**
 * Handles a selector.
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

#include "Selector.h"
#include "../FileFunctions.h"
#include "../globals.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../ResourceLoader.h"
#include "../XmlReader.h"
#include "../CaseInformation/CommonCaseResources.h"
#include "../Events/SelectorEventProvider.h"
#include <algorithm>

MLIFont *Selector::pLargeFont = NULL;
MLIFont *Selector::pMediumFont = NULL;
MLIFont *Selector::pSmallFont = NULL;

Image *Selector::pDividerSprite = NULL;
Image *Selector::pHighlightSprite = NULL;
Image *Selector::pStarSprite = NULL;

const int HeaderOffset = 5; // px
const int DividerPadding = 3; // px
const int TextPadding = 8; // px

CaseSelectorItem::CaseSelectorItem(Image *pScreenshotSprite, Image *pScreenshotFullSizeSprite, const string &caseUuid, const string &caseTitle, const string &caseDescription, const string &caseFilePath, bool isCompatible, Version requiredVersion)
{
    this->shouldDisplayStar = IsCaseCompleted(caseUuid);
    this->pScreenshotSprite = pScreenshotSprite;
    this->pScreenshotFullSizeSprite = pScreenshotFullSizeSprite;
    this->caseUuid = caseUuid;
    this->caseTitle = caseTitle;
    this->caseDescription = caseDescription;
    this->caseFilePath = caseFilePath;
    this->isCompatible = isCompatible;
    this->requiredVersion = requiredVersion;
}

void SelectorSection::DeleteItemAt(unsigned int index)
{
    delete itemList[index];
    itemList.erase(itemList.begin() + index);
}

void Selector::Initialize(Image *pDividerSprite, Image *pHighlightSprite, Image *pStarSprite)
{
    Selector::pDividerSprite = pDividerSprite;
    Selector::pHighlightSprite = pHighlightSprite;
    Selector::pStarSprite = pStarSprite;
}

Selector::Selector(int xPosition, int yPosition, int width, int height)
{
    this->xPosition = xPosition;
    this->yPosition = yPosition;
    this->width = width;
    this->height = height;

    this->topIndex = 0;
    this->selectedIndex = 0;
    this->mouseOverIndex = -1;

    this->pUpArrow = new Arrow(xPosition + width / 2 - 12, yPosition - 10, ArrowDirectionUp, 10, true /* isClickable */);
    this->pDownArrow = new Arrow(xPosition + width / 2 - 12, yPosition + height, ArrowDirectionDown, 10, true /* isClickable */);

    Reset();
}

Selector::Selector(const Selector &other)
{
    this->xPosition = other.xPosition;
    this->yPosition = other.yPosition;
    this->width = other.width;
    this->height = other.height;

    this->topIndex = 0;
    this->selectedIndex = 0;
    this->mouseOverIndex = -1;

    this->pUpArrow = new Arrow(xPosition + width / 2 - 12, yPosition - 10, ArrowDirectionUp, 10, true /* isClickable */);
    this->pDownArrow = new Arrow(xPosition + width / 2 - 12, yPosition + height, ArrowDirectionDown, 10, true /* isClickable */);

    Reset();
}

Selector::~Selector()
{
    delete pUpArrow;
    pUpArrow = NULL;
    delete pDownArrow;
    pDownArrow = NULL;

    Reset();
}

void Selector::Init()
{
    topIndex = 0;
    selectedIndex = 0;
    mouseOverIndex = -1;

    if (sectionList.size() > 0 && sectionList[0]->GetCount() > 0)
    {
        EventProviders::GetSelectorEventProvider()->RaiseSelectionChanged(this, sectionList[0]->GetItemAt(0));
    }
}

void Selector::Update(int delta)
{
    UpdateState();
    UpdateAnimation(delta);
}

void Selector::UpdateAnimation(int delta)
{
    unsigned int currentSection = 0;
    unsigned int currentSectionStartIndex = 0;
    unsigned int currentIndex = 0;

    GetCurrentSectionAndIndex(&currentSection, &currentSectionStartIndex, &currentIndex);

    if (GetExtent() > height)
    {
        pUpArrow->SetIsEnabled(topIndex > 0);
        pDownArrow->SetIsEnabled(GetPartialExtentFrom(currentSection, currentIndex - currentSectionStartIndex) > height);

        pUpArrow->Update(delta);
        pDownArrow->Update(delta);

        if (pUpArrow->GetIsClicked())
        {
            topIndex--;
        }
        else if (pDownArrow->GetIsClicked())
        {
            topIndex++;
        }
    }
}

void Selector::UpdateState()
{
    int yCurrent = yPosition;
    int xCurrent = xPosition;
    unsigned int currentSection = 0;
    unsigned int currentSectionStartIndex = 0;
    unsigned int currentIndex = 0;

    EnsureFonts();
    GetCurrentSectionAndIndex(&currentSection, &currentSectionStartIndex, &currentIndex);

    // If we have a header, we'll add that to the y-position.
    if (sectionList[currentSection]->GetTitle().length() > 0 && currentIndex - currentSectionStartIndex != 0)
    {
        yCurrent += pMediumFont->GetLineHeight() + DividerPadding + pDividerSprite->height + DividerPadding;
    }

    mouseOverIndex = -1;

    if (sectionList.size() > 0 && sectionList[0]->GetCount() > 0)
    {
        while (yCurrent - yPosition < height)
        {
            // If we have a header, we'll add that to the y-position.
            if (currentIndex - currentSectionStartIndex == 0)
            {
                yCurrent += pMediumFont->GetLineHeight() + DividerPadding + pDividerSprite->height + DividerPadding;
            }

            if (yCurrent - yPosition + TextPadding + pLargeFont->GetLineHeight() + TextPadding >= height)
            {
                break;
            }

            RectangleWH currentItemHitboxRect(xCurrent, yCurrent, width, TextPadding + pLargeFont->GetLineHeight() + TextPadding);

            bool mouseOver =
                MouseHelper::MouseOverRect(currentItemHitboxRect) ||
                MouseHelper::MouseDownOnRect(currentItemHitboxRect);
            bool clicked =
                MouseHelper::ClickedOnRect(currentItemHitboxRect);

            if (clicked && selectedIndex != currentIndex)
            {
                playSound(GetClickSoundEffect());
                selectedIndex = currentIndex;

                EventProviders::GetSelectorEventProvider()->RaiseSelectionChanged(this, sectionList[currentSection]->GetItemAt(selectedIndex - currentSectionStartIndex));
            }
            else if (mouseOver && selectedIndex != currentIndex)
            {
                mouseOverIndex = currentIndex;
            }

            if (mouseOver || clicked)
            {
                break;
            }

            yCurrent += TextPadding + pLargeFont->GetLineHeight() + TextPadding;
            currentIndex++;

            if (currentIndex - currentSectionStartIndex == sectionList[currentSection]->GetCount())
            {
                currentSection++;
                currentSectionStartIndex = currentIndex;

                if (currentSection == sectionList.size())
                {
                    break;
                }
            }
        }
    }
}

void Selector::Draw()
{
    int yCurrent = yPosition;
    int xCurrent = xPosition;
    unsigned int currentSection = 0;
    unsigned int currentSectionStartIndex = 0;
    unsigned int currentIndex = 0;

    EnsureFonts();
    GetCurrentSectionAndIndex(&currentSection, &currentSectionStartIndex, &currentIndex);

    // If we have a header, we'll draw that first.
    if (sectionList[currentSection]->GetTitle().length() > 0 && currentIndex - currentSectionStartIndex != 0)
    {
        pMediumFont->Draw(sectionList[currentSection]->GetTitle(), Vector2(xCurrent - HeaderOffset, yCurrent), Color(1.0, 0.0, 0.0, 0.0));
        yCurrent += pMediumFont->GetLineHeight() + DividerPadding;
        pDividerSprite->Draw(Vector2(xCurrent, yCurrent));
        yCurrent += pDividerSprite->height + DividerPadding;
    }

    if (sectionList[0]->GetCount() > 0)
    {
        while (yCurrent - yPosition < height)
        {
            int extraY = 0;

            // If we have a header, we'll add that to the y-position.
            if (currentIndex - currentSectionStartIndex == 0)
            {
                extraY += pMediumFont->GetLineHeight() + DividerPadding + pDividerSprite->height + DividerPadding;
            }

            if (yCurrent - yPosition + extraY >= height)
            {
                break;
            }

            // If we have a header, we'll draw that first.
            if (currentIndex - currentSectionStartIndex == 0)
            {
                pMediumFont->Draw(sectionList[currentSection]->GetTitle(), Vector2(xCurrent - HeaderOffset, yCurrent), Color(1.0, 0.0, 0.0, 0.0));
                yCurrent += pMediumFont->GetLineHeight() + DividerPadding;
                pDividerSprite->Draw(Vector2(xCurrent, yCurrent));
                yCurrent += pDividerSprite->height + DividerPadding;
            }

            if (yCurrent - yPosition + TextPadding + pLargeFont->GetLineHeight() + TextPadding >= height)
            {
                break;
            }

            yCurrent += TextPadding;

            if (currentIndex == selectedIndex || currentIndex == mouseOverIndex)
            {
                pHighlightSprite->Draw(Vector2(xCurrent + width / 2 - pHighlightSprite->width / 2, yCurrent + pLargeFont->GetLineHeight() / 2 - pHighlightSprite->height / 2));
            }

            pLargeFont->Draw(sectionList[currentSection]->GetItemDisplayStringAt(currentIndex - currentSectionStartIndex), Vector2(xCurrent, yCurrent), Color(1.0, 0.0, 0.0, 0.0));

            if (sectionList[currentSection]->GetItemShouldDisplayStarAt(currentIndex - currentSectionStartIndex))
            {
                pStarSprite->Draw(Vector2(xCurrent - pStarSprite->width - 2, yCurrent + pLargeFont->GetLineHeight() / 2 - pStarSprite->height / 2 - 2));
            }

            yCurrent += pLargeFont->GetLineHeight() + TextPadding;
            currentIndex++;

            if (currentIndex - currentSectionStartIndex == sectionList[currentSection]->GetCount())
            {
                currentSection++;
                currentSectionStartIndex = currentIndex;

                if (currentSection == sectionList.size())
                {
                    break;
                }
            }
        }

        if (GetExtent() > height)
        {
            pUpArrow->Draw();
            pDownArrow->Draw();
        }
    }
}

void Selector::Reset()
{
    for (unsigned int i = 0; i < sectionList.size(); i++)
    {
        delete sectionList[i];
    }

    sectionList.clear();
}

void Selector::DeleteCurrentItem()
{
    unsigned int currentSection = 0;
    unsigned int currentSectionStartIndex = 0;
    unsigned int currentIndex = 0;

    GetCurrentSectionAndIndex(&currentSection, &currentSectionStartIndex, &currentIndex);

    unsigned int indexInSection = selectedIndex - currentSectionStartIndex;

    SelectorSection *pCurrentSection = sectionList[currentSection];
    pCurrentSection->DeleteItemAt(indexInSection);

    if (pCurrentSection->GetCount() == 0)
    {
        sectionList.erase(sectionList.begin() + currentSection);
        delete pCurrentSection;

        if (currentSection == sectionList.size() && currentSection > 0)
        {
            currentSection--;
        }

        if (sectionList.size() > 0)
        {
            pCurrentSection = sectionList[currentSection];
        }
        else
        {
            pCurrentSection = NULL;
        }
    }

    if (pCurrentSection != NULL)
    {
        if (currentSection == sectionList.size() - 1 && selectedIndex >= pCurrentSection->GetCount() && selectedIndex > 0)
        {
            selectedIndex--;
        }

        GetCurrentSectionAndIndex(&currentSection, &currentSectionStartIndex, &currentIndex);

        EventProviders::GetSelectorEventProvider()->RaiseSelectionChanged(this, sectionList[currentSection]->GetItemAt(selectedIndex - currentSectionStartIndex));
    }
}

void Selector::PopulateWithCases(bool requireSaveFilesExist)
{
    Reset();

    SelectorSection *pOfficialCasesSection = new SelectorSection("Official Cases");
    SelectorSection *pCustomCasesSection = new SelectorSection("Custom Cases");
    SelectorSection *pIncompatibleCasesSection = new SelectorSection("Incompatible Cases");

    vector<string> caseFilePaths = GetCaseFilePaths();
    vector<CaseSelectorItem *> officialCaseList;
    vector<CaseSelectorItem *> customCaseList;
    vector<CaseSelectorItem *> incompatibleCaseList;

    for (unsigned int i = 0; i < caseFilePaths.size(); i++)
    {
        string caseFilePath = caseFilePaths[i];
        Image *pImageSprite = NULL;
        Image *pImageFullSizeSprite = NULL;
        string caseUuid = GetUuidFromFilePath(caseFilePath);
        string caseTitle;
        string caseDescription;

        // If we require that save files exist for cases we display,
        // and if save files do not presently exist for this case,
        // then we'll skip it.
        if (requireSaveFilesExist && !SaveFileExistsForCase(caseUuid))
        {
            continue;
        }

        bool isCorrectlySigned = IsCaseCorrectlySigned(caseFilePath);

        if (!ResourceLoader::GetInstance()->LoadTemporaryCase(caseFilePath))
        {
            continue;
        }

        XmlReader reader("caseMetadata.xml");

        reader.StartElement("CaseMetadata");
        pImageSprite = IsCaseCompleted(caseUuid) ? reader.ReadPngElement("ImageAfterCompletion") : reader.ReadPngElement("ImageBeforeCompletion");

        if (!IsCaseCompleted(caseUuid) && reader.ElementExists("ImageBeforeCompletionFullSize"))
        {
            pImageFullSizeSprite = reader.ReadPngElement("ImageBeforeCompletionFullSize");
        }
        else if (IsCaseCompleted(caseUuid) && reader.ElementExists("ImageAfterCompletionFullSize"))
        {
            pImageFullSizeSprite = reader.ReadPngElement("ImageAfterCompletionFullSize");
        }

        Version requiredVersion = Version(0, 9, 0);

        if (reader.ElementExists("RequiredVersion"))
        {
            reader.StartElement("RequiredVersion");
            requiredVersion = Version(&reader);
            reader.EndElement();
        }

        caseTitle = reader.ReadTextElement("Title");
        caseDescription = reader.ReadTextElement("Description");
        reader.EndElement();

        ResourceLoader::GetInstance()->UnloadTemporaryCase();

        if (requiredVersion > gVersion)
        {
            incompatibleCaseList.push_back(
                new CaseSelectorItem(
                    pImageSprite,
                    pImageFullSizeSprite,
                    caseUuid,
                    caseTitle,
                    caseDescription,
                    caseFilePath,
                    false /* isCompatible */,
                    requiredVersion));
        }
        else if (isCorrectlySigned)
        {
            officialCaseList.push_back(
                new CaseSelectorItem(
                    pImageSprite,
                    pImageFullSizeSprite,
                    caseUuid,
                    caseTitle,
                    caseDescription,
                    caseFilePath,
                    true /* isCompatible */,
                    requiredVersion));
        }
        else
        {
            customCaseList.push_back(
                new CaseSelectorItem(
                    pImageSprite,
                    pImageFullSizeSprite,
                    caseUuid,
                    caseTitle,
                    caseDescription,
                    caseFilePath,
                    true /* isCompatible */,
                    requiredVersion));
        }
    }

    sort(officialCaseList.begin(), officialCaseList.end(), CaseSelectorItem::CompareByCaseTitle);
    sort(customCaseList.begin(), customCaseList.end(), CaseSelectorItem::CompareByCaseTitle);
    sort(incompatibleCaseList.begin(), incompatibleCaseList.end(), CaseSelectorItem::CompareByCaseTitle);

    for (unsigned int i = 0; i < officialCaseList.size(); i++)
    {
        pOfficialCasesSection->AddItem(officialCaseList[i]);
    }

    for (unsigned int i = 0; i < customCaseList.size(); i++)
    {
        pCustomCasesSection->AddItem(customCaseList[i]);
    }

    for (unsigned int i = 0; i < incompatibleCaseList.size(); i++)
    {
        pIncompatibleCasesSection->AddItem(incompatibleCaseList[i]);
    }

    if (pOfficialCasesSection->GetCount() > 0)
    {
        AddSection(pOfficialCasesSection);
    }

    if (pCustomCasesSection->GetCount() > 0)
    {
        AddSection(pCustomCasesSection);
    }

    if (pIncompatibleCasesSection->GetCount() > 0)
    {
        AddSection(pIncompatibleCasesSection);
    }

    Init();
}

void Selector::EnsureFonts()
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

void Selector::GetCurrentSectionAndIndex(unsigned int *pCurrentSection, unsigned int *pCurrentSectionStartIndex, unsigned int *pCurrentIndex)
{
    unsigned int currentSection = 0;
    unsigned int currentSectionStartIndex = 0;
    unsigned int currentIndex = 0;

    while (currentSectionStartIndex + sectionList[currentSection]->GetCount() <= topIndex)
    {
        currentSectionStartIndex += sectionList[currentSection]->GetCount();
        currentSection++;
    }

    currentIndex = topIndex;

    *pCurrentSection = currentSection;
    *pCurrentSectionStartIndex = currentSectionStartIndex;
    *pCurrentIndex = currentIndex;
}

int Selector::GetExtent()
{
    return GetPartialExtentFrom(0, 0);
}

int Selector::GetPartialExtentFrom(int sectionIndex, int itemIndex)
{
    int totalExtent = 0;
    EnsureFonts();

    for (unsigned int currentSection = sectionIndex; currentSection < sectionList.size(); currentSection++)
    {
        totalExtent += pMediumFont->GetLineHeight() + DividerPadding;
        totalExtent += pDividerSprite->height + DividerPadding;

        for (unsigned int currentItem = itemIndex; currentItem < sectionList[currentSection]->GetCount(); currentItem++)
        {
            totalExtent += TextPadding;
            totalExtent += pLargeFont->GetLineHeight() + TextPadding;
        }

        itemIndex = 0;
    }

    return totalExtent;
}
