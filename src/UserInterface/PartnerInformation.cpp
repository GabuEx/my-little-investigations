/**
 * Displays information about the player's current partner.
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

#include "PartnerInformation.h"
#include "../SharedUtils.h"
#include "../CaseInformation/Case.h"
#include <math.h>

Vector2 PartnerInformation::backgroundImagePosition = Vector2(60, 45);
RectangleWH PartnerInformation::partnerNameRectangle = RectangleWH(63, 47, 220, 42);
Vector2 PartnerInformation::profileImagePosition = Vector2(63, 92);
Vector2 PartnerInformation::passiveAbilityHeadingPosition = Vector2(305, 64);
Vector2 PartnerInformation::passiveAbilityDescriptionPosition = Vector2(312, 106);
Vector2 PartnerInformation::activeAbilityHeadingPosition = Vector2(305, 185);
Vector2 PartnerInformation::activeAbilityDescriptionPosition = Vector2(312, 228);

Image *PartnerInformation::pBackgroundImage = NULL;
MLIFont *PartnerInformation::pHeadingFont = NULL;
MLIFont *PartnerInformation::pBodyFont = NULL;

const int AbilityDescriptionWidth = 558;
const int AbilityDescriptionHeight = 63;
const int AbilityDescriptionExtraLineHeight = 5;

void PartnerInformation::Initialize(Image *pBackgroundImage, MLIFont *pHeadingFont, MLIFont *pBodyFont)
{
    PartnerInformation::pBackgroundImage = pBackgroundImage;
    PartnerInformation::pHeadingFont = pHeadingFont;
    PartnerInformation::pBodyFont = pBodyFont;
}

PartnerInformation::PartnerInformation()
    : pPartner(NULL)
    , partnerId("")
{
}

Partner * PartnerInformation::GetPartner()
{
    if (pPartner == NULL)
    {
        pPartner = Case::GetInstance()->GetPartnerManager()->GetPartnerFromId(partnerId);

        deque<string> passiveAbilityDescriptionWords = split(pPartner->GetPassiveAbilityDescription(), ' ');
        deque<string> activeAbilityDescriptionWords = split(pPartner->GetActiveAbilityDescription(), ' ');

        string abilityDescriptionLineThusFar = "";

        while (passiveAbilityDescriptionWords.size() > 0)
        {
            if (pBodyFont->GetWidth(abilityDescriptionLineThusFar + " " + passiveAbilityDescriptionWords.front()) > AbilityDescriptionWidth)
            {
                passiveAbilityDescriptionLines.push_back(abilityDescriptionLineThusFar);
                abilityDescriptionLineThusFar = "";
            }

            if (abilityDescriptionLineThusFar.length() > 0)
            {
                abilityDescriptionLineThusFar += " ";
            }

            abilityDescriptionLineThusFar += passiveAbilityDescriptionWords.front();
            passiveAbilityDescriptionWords.pop_front();
        }

        passiveAbilityDescriptionLines.push_back(abilityDescriptionLineThusFar);
        abilityDescriptionLineThusFar = "";

        while (activeAbilityDescriptionWords.size() > 0)
        {
            if (pBodyFont->GetWidth(abilityDescriptionLineThusFar + " " + activeAbilityDescriptionWords.front()) > AbilityDescriptionWidth)
            {
                activeAbilityDescriptionLines.push_back(abilityDescriptionLineThusFar);
                abilityDescriptionLineThusFar = "";
            }

            if (abilityDescriptionLineThusFar.length() > 0)
            {
                abilityDescriptionLineThusFar += " ";
            }

            abilityDescriptionLineThusFar += activeAbilityDescriptionWords.front();
            activeAbilityDescriptionWords.pop_front();
        }

        activeAbilityDescriptionLines.push_back(abilityDescriptionLineThusFar);
    }

    return pPartner;
}

void PartnerInformation::Draw(int animationOffsetPartner)
{
    Draw(animationOffsetPartner, 0);
}

void PartnerInformation::Draw(int animationOffsetPartner, int yOffset)
{
    if (GetPartner() == NULL)
    {
        return;
    }

    Vector2 offsetVector = Vector2(animationOffsetPartner, yOffset);
    Vector2 partnerNamePosition =
       Vector2(
            partnerNameRectangle.GetX() + partnerNameRectangle.GetWidth() / 2 - pHeadingFont->GetWidth(GetPartner()->GetName()) / 2,
            partnerNameRectangle.GetY() + partnerNameRectangle.GetHeight() / 2 - pHeadingFont->GetHeight(GetPartner()->GetName()) / 2 - 1);
    partnerNamePosition = Vector2(floor(0.5 + partnerNamePosition.GetX()), floor(0.5 + partnerNamePosition.GetY()));

    pBackgroundImage->Draw(backgroundImagePosition + offsetVector);
    pHeadingFont->Draw(GetPartner()->GetName(), partnerNamePosition + offsetVector);
    GetPartner()->GetProfileImageSprite()->Draw(profileImagePosition + offsetVector);

    double currentYOffset = (AbilityDescriptionHeight - (double)passiveAbilityDescriptionLines.size() * pBodyFont->GetLineHeight() - ((double)passiveAbilityDescriptionLines.size() - 1) * AbilityDescriptionExtraLineHeight) / 2;

    for (unsigned int i = 0; i < passiveAbilityDescriptionLines.size(); i++)
    {
        pBodyFont->Draw(passiveAbilityDescriptionLines[i], passiveAbilityDescriptionPosition + offsetVector + Vector2(0, currentYOffset));
        currentYOffset += pBodyFont->GetLineHeight() + AbilityDescriptionExtraLineHeight;
    }

    currentYOffset = (AbilityDescriptionHeight - activeAbilityDescriptionLines.size() * pBodyFont->GetLineHeight() - ((double)activeAbilityDescriptionLines.size() - 1) * AbilityDescriptionExtraLineHeight) / 2;

    for (unsigned int i = 0; i < activeAbilityDescriptionLines.size(); i++)
    {
        pBodyFont->Draw(activeAbilityDescriptionLines[i], activeAbilityDescriptionPosition + offsetVector + Vector2(0, currentYOffset));
        currentYOffset += pBodyFont->GetLineHeight() + AbilityDescriptionExtraLineHeight;
    }

    pHeadingFont->Draw(gpLocalizableContent->GetText("PartnerInformation/PassiveAbilityText"), passiveAbilityHeadingPosition + offsetVector);
    pHeadingFont->Draw(gpLocalizableContent->GetText("PartnerInformation/ActiveAbilityText"), activeAbilityHeadingPosition + offsetVector);
}
