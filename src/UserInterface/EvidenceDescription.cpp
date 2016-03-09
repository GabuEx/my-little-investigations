/**
 * Handles the detailed display of a piece of evidence.
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

#include "EvidenceDescription.h"
#include "../SharedUtils.h"
#include "../CaseInformation/Case.h"

const int descriptionWidth = 669;
const int descriptionLineSeparationHeight = 0;

Image *EvidenceDescription::pBackgroundImage = NULL;
MLIFont *EvidenceDescription::pNameFont = NULL;
MLIFont *EvidenceDescription::pDescriptionFont = NULL;

void EvidenceDescription::Initialize(Image *pBackgroundImage, MLIFont *pNameFont, MLIFont *pDescriptionFont)
{
    EvidenceDescription::pBackgroundImage = pBackgroundImage;
    EvidenceDescription::pNameFont = pNameFont;
    EvidenceDescription::pDescriptionFont = pDescriptionFont;
}

void EvidenceDescription::SetEvidenceId(const string &evidenceId)
{
    this->evidenceId = evidenceId;
    retrievedEvidenceInformation = false;
}

void EvidenceDescription::Draw(Vector2 position)
{
    EnsureEvidenceInformation();
    pBackgroundImage->Draw(position);

    if (pEvidenceSprite != NULL)
    {
        pEvidenceSprite->Draw(Vector2(position.GetX() + 3, position.GetY() + 3));
        pNameFont->Draw(evidenceName, Vector2(position.GetX() + 128 + 10, position.GetY() + 3 + 5));

        int totalDescriptionHeight = (int)(evidenceDescriptionLines.size() * pDescriptionFont->GetLineHeight() + (evidenceDescriptionLines.size() - 1) * descriptionLineSeparationHeight);
        int currentY = (int)(position.GetY() + (5 + pNameFont->GetLineHeight() + 130 - totalDescriptionHeight) / 2);

        for (unsigned int i = 0; i < evidenceDescriptionLines.size(); i++)
        {
            pDescriptionFont->Draw(evidenceDescriptionLines[i], Vector2(position.GetX() + 128 + 20, currentY));
            currentY += pDescriptionFont->GetLineHeight() + descriptionLineSeparationHeight;
        }
    }
}

void EvidenceDescription::EnsureEvidenceInformation()
{
    if (retrievedEvidenceInformation)
    {
        return;
    }

    retrievedEvidenceInformation = true;
    evidenceDescriptionLines.clear();

    if (evidenceId.length() == 0)
    {
        pEvidenceSprite = NULL;
        evidenceName = "";
        return;
    }

    pEvidenceSprite = Case::GetInstance()->GetEvidenceManager()->GetLargeSpriteForId(evidenceId);
    evidenceName = Case::GetInstance()->GetEvidenceManager()->GetNameForId(evidenceId);
    string evidenceDescription = Case::GetInstance()->GetEvidenceManager()->GetDescriptionForId(evidenceId);
    deque<string> wordList = split(evidenceDescription, ' ');

    while (!wordList.empty())
    {
        string curString = "";
        double curTextWidth = 0;
        bool lineDone = false;
        bool addSpace = false;

        while (!lineDone)
        {
            string curWord = (addSpace ? " " : "") + wordList.front();
            double curStringWidth = pDescriptionFont->GetWidth(curWord);

            if (curTextWidth + curStringWidth < descriptionWidth)
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

        evidenceDescriptionLines.push_back(curString);
    }
}
