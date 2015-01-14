/**
 * Basic header/include file for Notification.cpp.
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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "../EasingFunctions.h"
#include "../MLIFont.h"
#include "../UserInterface/Arrow.h"
#include "../UserInterface/EvidenceDescription.h"
#include "../UserInterface/PartnerInformation.h"

class Notification
{
public:
    static void Initialize(
        int textAreaLeft,
        int textAreaTop,
        int textAreaWidth,
        int textAreaHeight,
        double desiredPadding,
        MLIFont *pTextFont,
        int evidenceCenterPointX,
        int evidenceCenterPointY,
        Image *pDirectionArrow);

    Notification();
    Notification(const string &rawNotificationText);
    Notification(const string &rawNotificationText, const string &partnerId);
    Notification(const string &rawNotificationText, const string &oldEvidenceId, const string &newEvidenceId);
    Notification(const string &rawNotificationText, const string &partnerId, const string &oldEvidenceId, const string &newEvidenceId);

    ~Notification();

    string GetNotificationSoundEffect();

    bool GetIsFinished() const { return this->isFinished; }
    void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

    void Begin();
    void Update(int delta, bool shouldCloseAutomatically);
    void Draw(double yOffset);
    void Reset();

private:
    void Init(const string &rawNotificationText, const string &partnerId, const string &oldEvidenceId, const string &newEvidenceId);

    static RectangleWH textAreaRect;
    static double desiredPadding;
    static MLIFont *pTextFont;

    static Vector2 evidenceCenterPoint;
    static Image *pDirectionArrow;

    string rawNotificationText;
    vector<string> textLines;
    vector<Vector2> textLinePositions;
    PartnerInformation *pPartnerInformation;
    EvidenceDescription *pOldEvidenceDescription;
    EvidenceDescription *pNewEvidenceDescription;
    Arrow *pDownArrow;
    int animationOffsetText;
    int animationOffsetPartner;
    int animationOffsetEvidence;
    int arrowOffset;
    EasingFunction *pInEaseText;
    EasingFunction *pInEasePartner;
    EasingFunction *pInEaseEvidence;
    EasingFunction *pOutEaseText;
    EasingFunction *pOutEasePartner;
    EasingFunction *pOutEaseEvidence;
    EasingFunction *pArrowBounceEase;
    string notificationSoundEffect;

    bool isFinished;
};

#endif
