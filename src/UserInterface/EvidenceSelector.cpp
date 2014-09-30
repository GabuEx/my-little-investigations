/**
 * Handles the selection of evidence.
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

#include "EvidenceSelector.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../Utils.h"
#include "../CaseInformation/Case.h"

const int itemHorizontalSpacing = 71;
const int itemVerticalSpacing = 71;
const int numItemsPerRow = 10;
const int numRows = 2;
const int numItems = numRows * numItemsPerRow;

const int yOffset = 36;
const int descriptionXOffset = 59;
const int descriptionWidth = 669;
const int descriptionLineSeparationHeight = 0;
const int descriptionYOffset = yOffset + 151;

Image *EvidenceSelector::pContainerImage = NULL;
Image *EvidenceSelector::pSelectionImage = NULL;

EvidenceSelector::EvidenceSelectorItem::EvidenceSelectorItem(int xPosition, int yPosition)
    : position(Vector2(xPosition, yPosition))
    , boundingRect(RectangleWH(xPosition, yPosition, pContainerImage->width, pContainerImage->height))
    , evidenceId("")
{
    selectionOffset = 0;
    pSelectionOffsetEase = new GravityBounceEase(0, 10, 300);

    pEvidenceSprite = NULL;
}

EvidenceSelector::EvidenceSelectorItem::EvidenceSelectorItem(const EvidenceSelector::EvidenceSelectorItem &other)
    : position(other.position)
    , boundingRect(other.boundingRect)
    , evidenceId("")
{
    selectionOffset = 0;
    pSelectionOffsetEase = new GravityBounceEase(0, 10, 300);

    pEvidenceSprite = NULL;
}

EvidenceSelector::EvidenceSelectorItem::~EvidenceSelectorItem()
{
    delete pSelectionOffsetEase;
    pSelectionOffsetEase = NULL;
}

void EvidenceSelector::EvidenceSelectorItem::LoadEvidence(const string &evidenceId)
{
    SetEvidenceId(evidenceId);

    if (evidenceId.length() > 0)
    {
        pEvidenceSprite = Case::GetInstance()->GetEvidenceManager()->GetSmallSpriteForId(evidenceId);
    }
    else
    {
        pEvidenceSprite = NULL;
    }
}

void EvidenceSelector::EvidenceSelectorItem::Update(int delta, bool &isSelected, bool isSetAsideForCombination)
{
    UpdateState(isSelected, isSetAsideForCombination);

    if (isSelected)
    {
        if (!pSelectionOffsetEase->GetIsStarted() || pSelectionOffsetEase->GetIsFinished())
        {
            pSelectionOffsetEase->Begin();
        }

        pSelectionOffsetEase->Update(delta);
        selectionOffset = pSelectionOffsetEase->GetCurrentValue();
    }
    else
    {
        selectionOffset = 0;
    }
}

void EvidenceSelector::EvidenceSelectorItem::UpdateState(bool &isSelected, bool isSetAsideForCombination)
{
    if (GetEvidenceId().length() > 0 && !isSetAsideForCombination)
    {
        if (MouseHelper::ClickedOnRect(boundingRect))
        {
            isSelected = true;
            playSound(GetClickSoundEffect());
        }
    }
}

void EvidenceSelector::EvidenceSelectorItem::Draw(double xOffset, double yOffset, bool isSelected, bool isSetAsideForCombination)
{
    pContainerImage->Draw(Vector2(position.GetX() + xOffset, position.GetY() + yOffset));

    if (pEvidenceSprite != NULL)
    {
        pEvidenceSprite->Draw(Vector2(position.GetX() + 3 + xOffset, position.GetY() + 3 + yOffset), isSetAsideForCombination ? Color(1.0, 0.5, 0.5, 0.5) : Color::White);
    }

    if (isSelected)
    {
        pSelectionImage->Draw(Vector2(position.GetX() - 10 + selectionOffset + xOffset, position.GetY() - 10 + selectionOffset + yOffset));
        pSelectionImage->Draw(Vector2(position.GetX() + pContainerImage->width - pSelectionImage->width + 10 - selectionOffset + xOffset, position.GetY() - 10 + selectionOffset + yOffset), true /* flipHorizontal */, false /* flipVertical */, Color::White);
        pSelectionImage->Draw(Vector2(position.GetX() - 10 + selectionOffset + xOffset, position.GetY() + pContainerImage->height - pSelectionImage->height + 10 - selectionOffset + yOffset), false /* flipHorizontal */, true /* flipVertical */, Color::White);
        pSelectionImage->Draw(Vector2(position.GetX() + pContainerImage->width - pSelectionImage->width + 10 - selectionOffset + xOffset, position.GetY() + pContainerImage->height - pSelectionImage->height + 10 - selectionOffset + yOffset), true /* flipHorizontal */, true /* flipVertical */, Color::White);
    }
    else if (isSetAsideForCombination)
    {
        pSelectionImage->Draw(Vector2(position.GetX() + xOffset, position.GetY() + yOffset));
        pSelectionImage->Draw(Vector2(position.GetX() + pContainerImage->width - pSelectionImage->width + xOffset, position.GetY() + yOffset), true /* flipHorizontal */, false /* flipVertical */, Color::White);
        pSelectionImage->Draw(Vector2(position.GetX() + xOffset, position.GetY() + pContainerImage->height - pSelectionImage->height + yOffset), false /* flipHorizontal */, true /* flipVertical */, Color::White);
        pSelectionImage->Draw(Vector2(position.GetX() + pContainerImage->width - pSelectionImage->width + xOffset, position.GetY() + pContainerImage->height - pSelectionImage->height + yOffset), true /* flipHorizontal */, true /* flipVertical */, Color::White);
    }
}

void EvidenceSelector::EvidenceSelectorItem::Reset()
{
    SetEvidenceId("");
    pEvidenceSprite = NULL;
}

void EvidenceSelector::Initialize(Image *pContainerImage, Image *pSelectionImage)
{
    EvidenceSelector::pContainerImage = pContainerImage;
    EvidenceSelector::pSelectionImage = pSelectionImage;
}

EvidenceSelector::EvidenceSelector(bool isCancelable)
{
    Init(isCancelable, false /* isForCombination */);
}

EvidenceSelector::EvidenceSelector(bool isCancelable, bool isForCombination)
{
    Init(isCancelable, isForCombination);
}

EvidenceSelector::~EvidenceSelector()
{
    if (ppItems != NULL)
    {
        for (int i = 0; i < numItems; i++)
        {
            delete ppItems[i];
        }

        delete [] ppItems;
        ppItems = NULL;
    }

    delete pEvidenceDescription;
    pEvidenceDescription = NULL;

    delete pInEase;
    pInEase = NULL;
    delete pOutEase;
    pOutEase = NULL;

    delete pLeftArrow;
    pLeftArrow = NULL;
    delete pRightArrow;
    pRightArrow = NULL;

    delete pPresentTab;
    pPresentTab = NULL;
    delete pCombineWithTab;
    pCombineWithTab = NULL;
    delete pCombineTab;
    pCombineTab = NULL;
    delete pCancelTab;
    pCancelTab = NULL;
    delete pProfilesTab;
    pProfilesTab = NULL;
    delete pEvidenceTab;
    pEvidenceTab = NULL;
}

void EvidenceSelector::LoadEvidence()
{
    vector<string> *pEvidenceIds = Case::GetInstance()->GetEvidenceManager()->GetIds();
    int lastItemIndex = 0;
    int evidenceCount = 0;
    totalEvidenceCount = 0;

    for (unsigned int i = 0; i < pEvidenceIds->size(); i++)
    {
        if (Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdVisible((*pEvidenceIds)[i]) &&
            Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdProfile((*pEvidenceIds)[i]) == isShowingProfiles)
        {
            totalEvidenceCount++;
        }
    }

    pLeftArrow->SetIsEnabled(totalEvidenceCount > numRows * numItemsPerRow);
    pRightArrow->SetIsEnabled(totalEvidenceCount > numRows * numItemsPerRow);

    for (unsigned int i = 0; i < numRows * numItemsPerRow; i++)
    {
        ppItems[lastItemIndex++]->LoadEvidence("");
    }

    lastItemIndex = 0;
    pEvidenceTab->SetIsEnabled(true);
    pProfilesTab->SetIsEnabled(true);

    for (unsigned int i = 0; evidenceCount < (currentPage + 1) * (numRows * numItemsPerRow) && i < pEvidenceIds->size(); i++)
    {
        if (Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdVisible((*pEvidenceIds)[i]) &&
            Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdProfile((*pEvidenceIds)[i]) == isShowingProfiles)
        {
            evidenceCount++;

            if ((evidenceCount - 1) / (numRows * numItemsPerRow) == currentPage)
            {
                ppItems[lastItemIndex++]->LoadEvidence((*pEvidenceIds)[i]);
            }
        }
    }

    if (evidenceCount == 0)
    {
        isShowingProfiles = !isShowingProfiles;
        currentPage = 0;
        lastItemIndex = 0;

        for (unsigned int i = 0; evidenceCount < numRows * numItemsPerRow && i < pEvidenceIds->size(); i++)
        {
            if (Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdVisible((*pEvidenceIds)[i]) &&
                Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdProfile((*pEvidenceIds)[i]) == isShowingProfiles)
            {
                ppItems[lastItemIndex++]->LoadEvidence((*pEvidenceIds)[i]);
                evidenceCount++;
            }
        }

        pEvidenceTab->SetIsEnabled(false);
        pProfilesTab->SetIsEnabled(false);
    }

    pEvidenceDescription->SetEvidenceId("");

    if (evidenceCount > 0)
    {
        if (ppItems[0]->GetEvidenceId() != itemSetAsideForCombinationId)
        {
            selectedItemId = ppItems[0]->GetEvidenceId();
        }
        else if (evidenceCount > 1)
        {
            selectedItemId = ppItems[1]->GetEvidenceId();
        }
        else
        {
            selectedItemId = "";
        }
    }
}

void EvidenceSelector::Begin()
{
    Reset();

    LoadEvidence();

    SetEvidenceCombinationEncounter(NULL);
    pCombineWithTab->SetIsEnabled(Case::GetInstance()->GetEvidenceManager()->GetCanCombineEvidence());

    pEvidenceTab->SetIsHidden(true, false);
    pProfilesTab->SetIsHidden(true, false);
    pCombineTab->SetIsHidden(true, false);
    pCombineWithTab->SetIsHidden(true, false);
    pPresentTab->SetIsHidden(true, false);
    pCancelTab->SetIsHidden(true, false);

    pEvidenceTab->SetIsHidden(false);
    pProfilesTab->SetIsHidden(false);
    pCombineTab->SetIsHidden(false);
    pCombineWithTab->SetIsHidden(false);
    pPresentTab->SetIsHidden(false);
    pCancelTab->SetIsHidden(false);

    pEvidenceTab->UpdatePosition(0);
    pProfilesTab->UpdatePosition(0);
    pCombineTab->UpdatePosition(0);
    pCombineWithTab->UpdatePosition(0);
    pPresentTab->UpdatePosition(0);
    pCancelTab->UpdatePosition(0);
}

void EvidenceSelector::Update(int delta)
{
    pEvidenceTab->UpdatePosition(delta);
    pProfilesTab->UpdatePosition(delta);
    pCombineTab->UpdatePosition(delta);
    pCombineWithTab->UpdatePosition(delta);
    pPresentTab->UpdatePosition(delta);
    pCancelTab->UpdatePosition(delta);

    if (pOutEase->GetIsStarted() && !pOutEase->GetIsFinished())
    {
        pOutEase->Update(delta);
        animationOffset = (int)pOutEase->GetCurrentValue();
    }
    else if (pOutEase->GetIsFinished())
    {
        SetIsShowing(false);
    }
    else if (pInEase->GetIsStarted() && !pInEase->GetIsFinished())
    {
        pInEase->Update(delta);
        animationOffset = (int)pInEase->GetCurrentValue();
    }
    else
    {
        if (isShowingProfiles)
        {
            pEvidenceTab->Update();
        }
        else
        {
            pProfilesTab->Update();
        }

        if (isForCombination)
        {
            if (itemSetAsideForCombinationId.length() > 0)
            {
                pCombineTab->Update();
            }
            else
            {
                pCombineWithTab->Update();
            }
        }
        else
        {
            pPresentTab->Update();
        }

        if (isCancelable)
        {
            pCancelTab->Update();
        }

        pLeftArrow->Update(delta);
        pRightArrow->Update(delta);

        if (pLeftArrow->GetIsClicked())
        {
            currentPage--;

            if (currentPage < 0)
            {
                int extraPages = totalEvidenceCount == 0 ? 0 : (totalEvidenceCount - 1) / (numRows * numItemsPerRow);
                currentPage = extraPages;
            }
        }
        else if (pRightArrow->GetIsClicked())
        {
            int extraPages = totalEvidenceCount == 0 ? 0 : (totalEvidenceCount - 1) / (numRows * numItemsPerRow);
            currentPage++;

            if (currentPage > extraPages)
            {
                currentPage = 0;
            }
        }

        if (pLeftArrow->GetIsClicked() || pRightArrow->GetIsClicked())
        {
            LoadEvidence();
        }

        if (pProfilesTab->GetIsClicked())
        {
            pProfilesTab->SetIsClicked(false /* isClicked */);
            isShowingProfiles = true;
            currentPage = 0;
            LoadEvidence();
        }
        else if (pEvidenceTab->GetIsClicked())
        {
            pEvidenceTab->SetIsClicked(false /* isClicked */);
            isShowingProfiles = false;
            currentPage = 0;
            LoadEvidence();
        }

        for (int i = 0; i < numItems; i++)
        {
            if (ppItems[i]->GetEvidenceId().length() == 0)
            {
                break;
            }

            bool isSelected = ppItems[i] == GetSelectedItem();

            ppItems[i]->Update(delta, isSelected, ppItems[i] == GetItemSetAsideForCombination());

            if (isSelected)
            {
                selectedItemId = ppItems[i]->GetEvidenceId();
            }

            UpdateSelectedItem(ppItems[i]);
        }

        if (pCombineTab->GetIsClicked())
        {
            SetEvidenceCombinationEncounter(Case::GetInstance()->GetEvidenceManager()->GetEncounterForEvidenceCombination(itemSetAsideForCombinationId, selectedItemId));
            Close();
        }
        else if (pCombineWithTab->GetIsClicked())
        {
            itemSetAsideForCombinationId = selectedItemId;

            for (int i = 0; i < numItems; i++)
            {
                if (ppItems[i] != GetItemSetAsideForCombination() && ppItems[i]->GetEvidenceId().length() > 0)
                {
                    selectedItemId = ppItems[i]->GetEvidenceId();
                    UpdateSelectedItem(ppItems[i]);
                    break;
                }
            }

            pCombineWithTab->Reset();
        }
        else if (pPresentTab->GetIsClicked())
        {
            OnEvidencePresented(selectedItemId);
            Close();
        }

        if (pCancelTab->GetIsClicked())
        {
            if (itemSetAsideForCombinationId.length() > 0)
            {
                EvidenceSelectorItem *pItemSetAsideForCombination = NULL;

                for (int i = 0; i < numItems; i++)
                {
                    if (ppItems[i]->GetEvidenceId() == itemSetAsideForCombinationId)
                    {
                        pItemSetAsideForCombination = ppItems[i];
                        break;
                    }
                }

                if (pItemSetAsideForCombination != NULL)
                {
                    selectedItemId = pItemSetAsideForCombination->GetEvidenceId();
                    UpdateSelectedItem(pItemSetAsideForCombination);
                }

                itemSetAsideForCombinationId = "";

                pCombineTab->Reset();
            }
            else
            {
                Close();
            }
        }
    }
}

void EvidenceSelector::UpdateState()
{
    if (GetAcceptsInput())
    {
        if (isShowingProfiles)
        {
            pEvidenceTab->Update();
        }
        else
        {
            pProfilesTab->Update();
        }

        if (isForCombination)
        {
            if (itemSetAsideForCombinationId.length() > 0)
            {
                pCombineTab->Update();
            }
            else
            {
                pCombineWithTab->Update();
            }
        }
        else
        {
            pPresentTab->Update();
        }

        if (isCancelable)
        {
            pCancelTab->Update();
        }

        pLeftArrow->UpdateState();
        pRightArrow->UpdateState();

        for (int i = 0; i < numItems; i++)
        {
            if (ppItems[i]->GetEvidenceId().length() == 0)
            {
                break;
            }

            bool isSelected = false;

            ppItems[i]->UpdateState(isSelected, ppItems[i] == GetItemSetAsideForCombination());

            if (isSelected)
            {
                selectedItemId = ppItems[i]->GetEvidenceId();
            }

            UpdateSelectedItem(ppItems[i]);
        }

        if (pProfilesTab->GetIsClicked())
        {
            MouseHelper::HandleClick();
            isShowingProfiles = true;
            LoadEvidence();
        }
        else if (pEvidenceTab->GetIsClicked())
        {
            MouseHelper::HandleClick();
            isShowingProfiles = false;
            LoadEvidence();
        }
        else if (pPresentTab->GetIsClicked())
        {
            MouseHelper::HandleClick();
            OnEvidencePresented(selectedItemId);
            Close();
        }
        else if (pCancelTab->GetIsClicked())
        {
            MouseHelper::HandleClick();
            Close();
        }
    }
}

void EvidenceSelector::Draw()
{
    Draw(0);
}

void EvidenceSelector::Draw(double yOffset)
{
    pLeftArrow->Draw(animationOffset, yOffset);
    pRightArrow->Draw(animationOffset, yOffset);

    for (int i = 0; i < numItems; i++)
    {
        ppItems[i]->Draw(animationOffset, yOffset, ppItems[i] == GetSelectedItem(), ppItems[i] == GetItemSetAsideForCombination());
    }

    pEvidenceDescription->Draw(Vector2(descriptionXOffset + animationOffset, descriptionYOffset + yOffset));

    if (isShowingProfiles)
    {
        pEvidenceTab->Draw();
    }
    else
    {
        pProfilesTab->Draw();
    }

    if (isForCombination)
    {
        if (itemSetAsideForCombinationId.length() > 0)
        {
            pCombineTab->Draw();
        }
        else
        {
            pCombineWithTab->Draw();
        }
    }
    else
    {
        pPresentTab->Draw();
    }

    if (isCancelable)
    {
        pCancelTab->Draw();
    }
}

void EvidenceSelector::Reset()
{
    pPresentTab->Reset();
    pCombineWithTab->Reset();
    pCombineTab->Reset();
    pCancelTab->Reset();

    pLeftArrow->Reset();
    pRightArrow->Reset();

    for (int i = 0; i < numItems; i++)
    {
        ppItems[i]->Reset();
    }

    isShowingProfiles = false;
    currentPage = 0;
    itemSetAsideForCombinationId = "";
    selectedItemId = "";
    pEvidenceDescription->SetEvidenceId("");

    SetIsShowing(false);

    pInEase->Reset();
    pOutEase->Reset();
}

void EvidenceSelector::Show()
{
    SetIsShowing(true);
    animationOffset = gScreenWidth;

    vector<string> *pEvidenceIds = Case::GetInstance()->GetEvidenceManager()->GetIds();
    int evidenceCount = 0;

    for (unsigned int i = 0; i < numRows * numItemsPerRow && i < pEvidenceIds->size(); i++)
    {
        if (Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdVisible((*pEvidenceIds)[i]) &&
            Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdProfile((*pEvidenceIds)[i]) == isShowingProfiles)
        {
            evidenceCount++;
        }
    }

    if (evidenceCount > 0)
    {
        selectedItemId = ppItems[0]->GetEvidenceId();
    }

    pInEase->Reset();
    pOutEase->Reset();

    pInEase->Begin();
}

void EvidenceSelector::Close()
{
    pOutEase->Begin();
    OnClosing();

    pEvidenceTab->SetIsHidden(true);
    pProfilesTab->SetIsHidden(true);
    pCombineTab->SetIsHidden(true);
    pCombineWithTab->SetIsHidden(true);
    pPresentTab->SetIsHidden(true);
    pCancelTab->SetIsHidden(true);
}

EvidenceSelector::EvidenceSelectorItem * EvidenceSelector::GetSelectedItem()
{
    EvidenceSelectorItem *pItem = NULL;

    if (selectedItemId.length() > 0)
    {
        for (int i = 0; i < numItems; i++)
        {
            if (ppItems[i]->GetEvidenceId() == selectedItemId)
            {
                pItem = ppItems[i];
                break;
            }
        }
    }

    return pItem;
}

EvidenceSelector::EvidenceSelectorItem * EvidenceSelector::GetItemSetAsideForCombination()
{
    EvidenceSelectorItem *pItem = NULL;

    if (itemSetAsideForCombinationId.length() > 0)
    {
        for (int i = 0; i < numItems; i++)
        {
            if (ppItems[i]->GetEvidenceId() == itemSetAsideForCombinationId)
            {
                pItem = ppItems[i];
                break;
            }
        }
    }

    return pItem;
}

void EvidenceSelector::Init(bool isCancelable, bool isForCombination)
{
    this->pPresentTab = new Tab(gScreenWidth - TabWidth - (isCancelable ? TabWidth + 7 : 0), true /* isClickable */, pgLocalizableContent->GetText("EvidenceSelector/PresentText"));
    this->pCombineWithTab = new Tab(gScreenWidth - TabWidth - (isCancelable ? TabWidth + 7 : 0), true /* isClickable */, pgLocalizableContent->GetText("EvidenceSelector/CombineWithText"));
    this->pCombineTab = new Tab(gScreenWidth - TabWidth - (isCancelable ? TabWidth + 7 : 0), true /* isClickable */, pgLocalizableContent->GetText("EvidenceSelector/CombineText"));
    this->pCancelTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, pgLocalizableContent->GetText("EvidenceSelector/CancelText"), true /* isCancel */);
    this->pProfilesTab = new Tab((gScreenWidth - TabWidth) / 2, true /* isClickable */, pgLocalizableContent->GetText("EvidenceSelector/ToProfilesText"), false /* useCancelClickSoundEffect */, TabRowTop);
    this->pEvidenceTab = new Tab((gScreenWidth - TabWidth) / 2, true /* isClickable */, pgLocalizableContent->GetText("EvidenceSelector/ToEvidenceText"), false /* useCancelClickSoundEffect */, TabRowTop);

    this->pPresentTab->SetIsEnabled(false);
    this->pCombineWithTab->SetIsEnabled(false);
    this->pCombineTab->SetIsEnabled(false);
    this->pCancelTab->SetIsEnabled(true);
    this->pProfilesTab->SetIsEnabled(true);
    this->pEvidenceTab->SetIsEnabled(true);

    this->pLeftArrow = new Arrow(82, yOffset + 63, ArrowDirectionLeft, 10 /* bounceDistance */, true /* isClickable */);
    this->pRightArrow = new Arrow(858, yOffset + 63, ArrowDirectionRight, 10 /* bounceDistance */, true /* isClickable */);

    this->pLeftArrow->SetIsEnabled(false);
    this->pRightArrow->SetIsEnabled(false);

    this->ppItems = new EvidenceSelectorItem*[numItems];

    for (int y = 0; y < numRows; y++)
    {
        for (int x = 0; x < numItemsPerRow; x++)
        {
            this->ppItems[x + y * numItemsPerRow] = new EvidenceSelectorItem(128 + x * itemHorizontalSpacing + (x > numItemsPerRow / 2 ? 1 : 0), yOffset + 11 + y * itemVerticalSpacing);
        }
    }

    this->pEvidenceDescription = new EvidenceDescription();

    this->selectedItemId = "";
    this->itemSetAsideForCombinationId = "";
    this->pEvidenceDescription->SetEvidenceId("");

    this->isCancelable = isCancelable;
    this->isForCombination = isForCombination;
    this->isShowingProfiles = false;
    this->currentPage = 0;
    this->totalEvidenceCount = 0;

    this->pInEase = new QuadraticEase(gScreenWidth, 0, 250);
    this->pOutEase = new QuadraticEase(0, -gScreenWidth, 250);

    this->animationOffset = 0;
    this->isShowing = false;
    this->pEvidenceCombinationEncounter = NULL;
}

void EvidenceSelector::UpdateSelectedItem(EvidenceSelectorItem *pItem)
{
    if (selectedItemId == pItem->GetEvidenceId())
    {
        pEvidenceDescription->SetEvidenceId(pItem->GetEvidenceId());
    }

    pPresentTab->SetIsEnabled(selectedItemId.length() > 0);
    pCombineWithTab->SetIsEnabled(Case::GetInstance()->GetEvidenceManager()->GetCanCombineEvidence() && selectedItemId.length() > 0);
    pCombineTab->SetIsEnabled(selectedItemId.length() > 0);
}

void EvidenceSelector::OnEvidencePresented(const string &evidenceId)
{
    EventProviders::GetEvidenceSelectorEventProvider()->RaiseEvidenceSelectorEvidencePresented(this, evidenceId);
}

void EvidenceSelector::OnClosing()
{
    EventProviders::GetEvidenceSelectorEventProvider()->RaiseEvidenceSelectorClosing(this);
}
