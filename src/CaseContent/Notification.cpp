/**
 * Handles a notification in the game.
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

#include "Notification.h"
#include "../globals.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../KeyboardHelper.h"
#include "../Utils.h"

const int AnimationDuration = 200;
const int PartnerAnimationDelay = 200;

RectangleWH Notification::textAreaRect = RectangleWH(0, 0, 0, 0);
double Notification::desiredPadding = 0;
MLIFont *Notification::pTextFont = NULL;

Vector2 Notification::evidenceCenterPoint;
Image *Notification::pDirectionArrow = NULL;

void Notification::Initialize(
        int textAreaLeft,
        int textAreaTop,
        int textAreaWidth,
        int textAreaHeight,
        double desiredPadding,
        MLIFont *pTextFont,
        int evidenceCenterPointX,
        int evidenceCenterPointY,
        Image *pDirectionArrow)
{
    Notification::textAreaRect = RectangleWH(textAreaLeft, textAreaTop, textAreaWidth, textAreaHeight);
    Notification::desiredPadding = desiredPadding;
    Notification::pTextFont = pTextFont;
    Notification::evidenceCenterPoint = Vector2(evidenceCenterPointX, evidenceCenterPointY);
    Notification::pDirectionArrow = pDirectionArrow;
}

Notification::Notification()
{
    Init("", "", "", "");
}

Notification::Notification(const string &rawNotificationText)
{
    Init(rawNotificationText, "", "", "");
}

Notification::Notification(const string &rawNotificationText, const string &partnerId)
{
    Init(rawNotificationText, partnerId, "", "");
}

Notification::Notification(const string &rawNotificationText, const string &oldEvidenceId, const string &newEvidenceId)
{
    Init(rawNotificationText, "", oldEvidenceId, newEvidenceId);
}

Notification::Notification(const string &rawNotificationText, const string &partnerId, const string &oldEvidenceId, const string &newEvidenceId)
{
    Init(rawNotificationText, partnerId, oldEvidenceId, newEvidenceId);
}

void Notification::Init(const string &rawNotificationText, const string &partnerId, const string &oldEvidenceId, const string &newEvidenceId)
{
    pPartnerInformation = NULL;
    pOldEvidenceDescription = NULL;
    pNewEvidenceDescription = NULL;
    animationOffsetText = 0;
    animationOffsetPartner = 0;
    animationOffsetEvidence = 0;
    arrowOffset = 0;
    isFinished = false;
    this->rawNotificationText = rawNotificationText;

    if (partnerId.length() > 0 && partnerId != "None")
    {
        pPartnerInformation = new PartnerInformation();
        pPartnerInformation->SetPartnerId(partnerId);
    }

    if (oldEvidenceId.length() > 0)
    {
        pOldEvidenceDescription = new EvidenceDescription();
        pOldEvidenceDescription->SetEvidenceId(oldEvidenceId);
    }

    if (newEvidenceId.length() > 0)
    {
        pNewEvidenceDescription = new EvidenceDescription();
        pNewEvidenceDescription->SetEvidenceId(newEvidenceId);
    }

    pDownArrow = new Arrow(927, 505, ArrowDirectionDown, 10 /* bounceDistance */, false /* isClickable */);
    pInEaseText = new LinearEase(gScreenWidth, 0, AnimationDuration);

    if (partnerId.length() > 0 || newEvidenceId.length() > 0)
    {
        pInEaseText->SetAnimationStartDelay(PartnerAnimationDelay);
    }

    pInEasePartner = new LinearEase(gScreenWidth, 0, AnimationDuration);
    pInEaseEvidence = new LinearEase(gScreenWidth, 0, AnimationDuration);
    pOutEaseText = new LinearEase(0, -gScreenWidth, AnimationDuration);

    if (partnerId.length() > 0 || newEvidenceId.length() > 0)
    {
        pOutEaseText->SetAnimationStartDelay(PartnerAnimationDelay);
    }

    pOutEasePartner = new LinearEase(0, -gScreenWidth, AnimationDuration);
    pOutEaseEvidence = new LinearEase(0, -gScreenWidth, AnimationDuration);

    pArrowBounceEase = new GravityBounceEase(0, 10, 300);
}

Notification::~Notification()
{
    delete pPartnerInformation;
    pPartnerInformation = NULL;

    delete pOldEvidenceDescription;
    pOldEvidenceDescription = NULL;
    delete pNewEvidenceDescription;
    pNewEvidenceDescription = NULL;

    delete pDownArrow;
    pDownArrow = NULL;

    delete pInEaseText;
    pInEaseText = NULL;
    delete pInEasePartner;
    pInEasePartner = NULL;
    delete pInEaseEvidence;
    pInEaseEvidence = NULL;
    delete pOutEaseText;
    pOutEaseText = NULL;
    delete pOutEasePartner;
    pOutEasePartner = NULL;
    delete pOutEaseEvidence;
    pOutEaseEvidence = NULL;
    delete pArrowBounceEase;
    pArrowBounceEase = NULL;
}

string Notification::GetNotificationSoundEffect()
{
    if (notificationSoundEffect.length() == 0)
    {
        notificationSoundEffect = (pPartnerInformation != NULL ? "NewPartner" : "NewAddition");
    }

    return notificationSoundEffect;
}

void Notification::Begin()
{
    if (rawNotificationText.length() > 0 && textLines.empty())
    {
        double allowedWidth = textAreaRect.GetWidth() - desiredPadding * 2;
        deque<string> wordList = split(rawNotificationText, ' ');

        while (!wordList.empty())
        {
            string curString = "";
            double curTextWidth = 0;
            bool lineDone = false;
            bool addSpace = false;

            while (!lineDone)
            {
                string curWord = (addSpace ? " " : "") + wordList.front();
                double curStringWidth = pTextFont->GetWidth(curWord);

                if (curTextWidth + curStringWidth < allowedWidth)
                {
                    curString += curWord;
                    curTextWidth += curStringWidth;
                    wordList.pop_front();
                    addSpace = true;

                    if (wordList.empty())
                    {
                        lineDone = true;
                    }
                }
                else
                {
                    lineDone = true;
                }
            }

            textLines.push_back(curString);
        }

        double yPosition = textAreaRect.GetY() + textAreaRect.GetHeight() / 2;

        for (unsigned int i = 0; i < textLines.size(); i++)
        {
            yPosition -= pTextFont->GetHeight(textLines[i]) / 2;
        }

        for (unsigned int i = 0; i < textLines.size(); i++)
        {
            Vector2 textLineSize = Vector2(pTextFont->GetWidth(textLines[i]), pTextFont->GetHeight(textLines[i]));
            textLinePositions.push_back(Vector2(textAreaRect.GetX() + textAreaRect.GetWidth() / 2 - textLineSize.GetX() / 2, yPosition));
            yPosition += textLineSize.GetY();
        }
    }

    SetIsFinished(false);
    playSound(GetNotificationSoundEffect());
    animationOffsetText = gScreenWidth;
    animationOffsetPartner = gScreenWidth;
    pInEaseText->Begin();
    pInEasePartner->Begin();
    pInEaseEvidence->Begin();
}

void Notification::Update(int delta, bool shouldCloseAutomatically)
{
    if ((pOutEaseText->GetIsStarted() && !pOutEaseText->GetIsFinished()) ||
        (pOutEasePartner->GetIsStarted() && !pOutEasePartner->GetIsFinished()) ||
        (pOutEaseEvidence->GetIsStarted() && !pOutEaseEvidence->GetIsFinished()))
    {
        if (pOutEaseText->GetIsStarted() && !pOutEaseText->GetIsFinished())
        {
            pOutEaseText->Update(delta);
            animationOffsetText = (int)pOutEaseText->GetCurrentValue();
        }

        if (pOutEasePartner->GetIsStarted() && !pOutEasePartner->GetIsFinished())
        {
            pOutEasePartner->Update(delta);
            animationOffsetPartner = (int)pOutEasePartner->GetCurrentValue();
        }

        if (pOutEaseEvidence->GetIsStarted() && !pOutEaseEvidence->GetIsFinished())
        {
            pOutEaseEvidence->Update(delta);
            animationOffsetEvidence = (int)pOutEaseEvidence->GetCurrentValue();
        }
    }
    else if (pOutEaseText->GetIsFinished() && pOutEasePartner->GetIsFinished() && pOutEaseEvidence->GetIsFinished())
    {
        SetIsFinished(true);
    }
    else if ((pInEaseText->GetIsStarted() && !pInEaseText->GetIsFinished()) ||
        (pInEasePartner->GetIsStarted() && !pInEasePartner->GetIsFinished()) ||
        (pInEaseEvidence->GetIsStarted() && !pInEaseEvidence->GetIsFinished()))
    {
        if (pInEaseText->GetIsStarted() && !pInEaseText->GetIsFinished())
        {
            pInEaseText->Update(delta);
            animationOffsetText = (int)pInEaseText->GetCurrentValue();
        }

        if (pInEasePartner->GetIsStarted() && !pInEasePartner->GetIsFinished())
        {
            pInEasePartner->Update(delta);
            animationOffsetPartner = (int)pInEasePartner->GetCurrentValue();
        }

        if (pInEaseEvidence->GetIsStarted() && !pInEaseEvidence->GetIsFinished())
        {
            pInEaseEvidence->Update(delta);
            animationOffsetEvidence = (int)pInEaseEvidence->GetCurrentValue();
        }
    }
    else
    {
        pDownArrow->Update(delta);

        if (MouseHelper::ClickedAnywhere() || KeyboardHelper::ClickPressed() || shouldCloseAutomatically)
        {
            pOutEaseText->Begin();
            pOutEasePartner->Begin();
            pOutEaseEvidence->Begin();
        }
    }

    if (!pArrowBounceEase->GetIsStarted() || pArrowBounceEase->GetIsFinished())
    {
        pArrowBounceEase->Begin();
    }

    pArrowBounceEase->Update(delta);
    arrowOffset = (int)pArrowBounceEase->GetCurrentValue();
}

void Notification::Draw(double yOffset)
{
    if (pInEaseText->GetIsFinished() && pInEasePartner->GetIsFinished() && pInEaseEvidence->GetIsFinished() && !pOutEaseText->GetIsStarted() && !pOutEasePartner->GetIsStarted() && !pOutEaseEvidence->GetIsStarted())
    {
        pDownArrow->Draw(yOffset);
    }

    for (unsigned int i = 0; i < textLines.size(); i++)
    {
        Vector2 textLinePosition = textLinePositions[i];

        pTextFont->Draw(textLines[i], Vector2(textLinePosition.GetX() + animationOffsetText, textLinePosition.GetY() + yOffset));
    }

    if (pPartnerInformation != NULL)
    {
        pPartnerInformation->Draw(animationOffsetPartner, (int)yOffset);
    }

    if (pNewEvidenceDescription != NULL)
    {
        if (pOldEvidenceDescription != NULL)
        {
            pOldEvidenceDescription->Draw(
                Vector2(
                    evidenceCenterPoint.GetX() - pOldEvidenceDescription->GetWidth() / 2 + animationOffsetEvidence,
                    evidenceCenterPoint.GetY() - pOldEvidenceDescription->GetHeight() - pDirectionArrow->height / 3));

            pNewEvidenceDescription->Draw(
                Vector2(
                    evidenceCenterPoint.GetX() - pNewEvidenceDescription->GetWidth() / 2 + animationOffsetEvidence,
                    evidenceCenterPoint.GetY() + pDirectionArrow->height / 3));

            pDirectionArrow->Draw(
                Vector2(
                    evidenceCenterPoint.GetX() - pDirectionArrow->width / 2 + animationOffsetEvidence,
                    evidenceCenterPoint.GetY() - pDirectionArrow->height / 2 - 5 + arrowOffset));
        }
        else
        {
            pNewEvidenceDescription->Draw(
                Vector2(
                    evidenceCenterPoint.GetX() - pNewEvidenceDescription->GetWidth() / 2 + animationOffsetEvidence,
                    evidenceCenterPoint.GetY() - pNewEvidenceDescription->GetHeight() / 2));
        }
    }
}

void Notification::Reset()
{
    SetIsFinished(false);
    pDownArrow->Reset();
    pInEaseText->Reset();
    pInEasePartner->Reset();
    pInEaseEvidence->Reset();
    pOutEaseText->Reset();
    pOutEasePartner->Reset();
    pOutEaseEvidence->Reset();
}
