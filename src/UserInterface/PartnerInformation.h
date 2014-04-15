/**
 * Basic header/include file for PartnerInformation.cpp.
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

#ifndef PARTNERINFORMATION_H
#define PARTNERINFORMATION_H

#include "../MLIFont.h"
#include "../Rectangle.h"
#include "../Image.h"
#include "../Vector2.h"
#include "../CaseInformation/PartnerManager.h"
#include <vector>

using namespace std;

class PartnerInformation
{
public:
    static void Initialize(Image *pBackgroundImage, MLIFont *pHeadingFont, MLIFont *pBodyFont);

    PartnerInformation();

    string GetPartnerId() const { return this->partnerId; }
    void SetPartnerId(string partnerId) { this->partnerId = partnerId; }

    Partner * GetPartner();

    void Draw(int animationOffsetPartner);
    void Draw(int animationOffsetPartner, int yOffset);

private:
    static Vector2 backgroundImagePosition;
    static RectangleWH partnerNameRectangle;
    static Vector2 profileImagePosition;
    static Vector2 passiveAbilityHeadingPosition;
    static Vector2 passiveAbilityDescriptionPosition;
    static Vector2 activeAbilityHeadingPosition;
    static Vector2 activeAbilityDescriptionPosition;

    static Image *pBackgroundImage;
    static MLIFont *pHeadingFont;
    static MLIFont *pBodyFont;

    Partner *pPartner;
    string partnerId;

    vector<string> passiveAbilityDescriptionLines;
    vector<string> activeAbilityDescriptionLines;
};

#endif
