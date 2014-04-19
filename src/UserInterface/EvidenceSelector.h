/**
 * Basic header/include file for EvidenceSelector.cpp.
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

#ifndef EVIDENCESELECTOR_H
#define EVIDENCESELECTOR_H

#include "Arrow.h"
#include "EvidenceDescription.h"
#include "Tab.h"
#include "../EasingFunctions.h"
#include "../MLIFont.h"
#include "../Image.h"

class Conversation;

class EvidenceSelector
{
private:
    class EvidenceSelectorItem
    {
        friend class EvidenceSelector;

    public:
        EvidenceSelectorItem(int xPosition, int yPosition);
        EvidenceSelectorItem(const EvidenceSelectorItem &other);
        ~EvidenceSelectorItem();

        string GetEvidenceId() const { return this->evidenceId; }
        static string GetClickSoundEffect() { return "ButtonClick2"; }

        void LoadEvidence(string evidenceId);

        void Update(int delta, bool &isSelected, bool isSetAsideForCombination);
        void UpdateState(bool &isSelected,  bool isSetAsideForCombination);
        void Draw(double xOffset, double yOffset, bool isSelected, bool isSetAsideForCombination);
        void Reset();

    private:
        void SetEvidenceId(string evidenceId) { this->evidenceId = evidenceId; }

        Vector2 position;
        RectangleWH boundingRect;
        Sprite *pEvidenceSprite;

        double selectionOffset;
        EasingFunction *pSelectionOffsetEase;

        string evidenceId;
    };

public:
    static void Initialize(Image *pContainerImage, Image *pSelectionImage);

    EvidenceSelector(bool isCancelable);
    EvidenceSelector(bool isCancelable, bool isForCombination);
    ~EvidenceSelector();

    bool GetIsShowing() const { return this->isShowing; }
    void SetIsShowing(bool isShowing) { this->isShowing = isShowing; }

    Conversation * GetEvidenceCombinationConversation() { return this->pEvidenceCombinationConversation; }
    void SetEvidenceCombinationConversation(Conversation *pEvidenceCombinationConversation) { this->pEvidenceCombinationConversation = pEvidenceCombinationConversation; }

    void LoadEvidence();

    void Begin();
    void Update(int delta);
    void UpdateState();
    void Draw();
    void Draw(double yOffset);
    void Reset();
    void Show();
    void Close();

    EvidenceSelectorItem * GetSelectedItem();
    EvidenceSelectorItem * GetItemSetAsideForCombination();

private:
    void Init(bool isCancelable, bool isForCombination);
    void UpdateSelectedItem(EvidenceSelectorItem *pItem);
    void OnEvidencePresented(string evidenceId);
    void OnClosing();

    bool GetAcceptsInput() { return this->pInEase->GetIsFinished() && !this->pOutEase->GetIsStarted(); }

    static Image *pContainerImage;
    static Image *pSelectionImage;

    int animationOffset;

    EvidenceSelectorItem **ppItems;

    string selectedItemId;
    string itemSetAsideForCombinationId;
    EvidenceDescription *pEvidenceDescription;

    Tab *pPresentTab;
    Tab *pCombineWithTab;
    Tab *pCombineTab;
    Tab *pCancelTab;
    Tab *pProfilesTab;
    Tab *pEvidenceTab;

    Arrow *pLeftArrow;
    Arrow *pRightArrow;

    bool isCancelable;
    bool isForCombination;
    bool isShowingProfiles;
    int currentPage;
    int totalEvidenceCount;

    EasingFunction *pInEase;
    EasingFunction *pOutEase;

    bool isShowing;

    Conversation *pEvidenceCombinationConversation;
};

#endif
