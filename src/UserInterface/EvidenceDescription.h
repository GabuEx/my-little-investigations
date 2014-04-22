/**
 * Basic header/include file for EvidenceDescription.cpp.
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

#ifndef EVIDENCEDESCRIPTION_H
#define EVIDENCEDESCRIPTION_H

#include "../MLIFont.h"
#include "../Image.h"
#include "../Sprite.h"

class EvidenceDescription
{
public:
    static void Initialize(Image *pBackgroundImage, MLIFont *pNameFont, MLIFont *pDescriptionFont);

    EvidenceDescription()
    {
        pEvidenceSprite = NULL;
        retrievedEvidenceInformation = false;
    }

    static int GetWidth() { return pBackgroundImage != NULL ? pBackgroundImage->width : 0; }
    static int GetHeight() { return pBackgroundImage != NULL ? pBackgroundImage->height : 0; }

    void SetEvidenceId(const string &evidenceId);
    void Draw(Vector2 position);

private:
    static Image *pBackgroundImage;
    static MLIFont *pNameFont;
    static MLIFont *pDescriptionFont;

    void EnsureEvidenceInformation();

    string evidenceId;
    Sprite *pEvidenceSprite;
    string evidenceName;
    vector<string> evidenceDescriptionLines;
    bool retrievedEvidenceInformation;
};

#endif
