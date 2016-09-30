/**
 * Basic header/include file for SharedUtils.cpp.
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

#ifndef SHAREDUTILS_H
#define SHAREDUTILS_H

#ifndef CASE_CREATOR
#include <string>
#include <deque>

#ifndef LAUNCHER
#include "MLIFont.h"
#endif

using namespace std;

#define SharedUtilsCharType char
#define SharedUtilsStringType string
#define SharedUtilsStringListType deque<string>
#define SharedUtilsFontType MLIFont*
#else
#include <QChar>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QFontMetrics>
#include <QRect>

#define SharedUtilsCharType QChar
#define SharedUtilsStringType QString
#define SharedUtilsStringListType QStringList
#define SharedUtilsFontType QFont
#endif

#ifndef LAUNCHER
#include "enums.h"
#include "Rectangle.h"
#include "Vector2.h"

class IDialogEventsOwner
{
public:
    virtual void AddSpeedChangePosition(int position, double newMillisecondsPerCharacterUpdate) = 0;
    virtual void AddEmotionChangePosition(int position, const SharedUtilsStringType &newEmotionId) = 0;
    virtual void AddEmotionOtherChangePosition(int position, const SharedUtilsStringType &newEmotionId) = 0;
    virtual void AddPausePosition(int position, double millisecondDuration) = 0;
    virtual void AddAudioPausePosition(int position, double millisecondDuration) = 0;
    virtual void AddMouthChangePosition(int position, bool mouthIsOn) = 0;
    virtual void StartAside(int position) = 0;
    virtual void EndAside(int position, int eventEnd, int parsedStringLength, SharedUtilsStringType *pStringToPrependOnNext) = 0;
    virtual void StartEmphasis(int position) = 0;
    virtual void EndEmphasis(int position) = 0;
    virtual void AddPlaySoundPosition(int position, const SharedUtilsStringType &id) = 0;
    virtual void AddShakePosition(int position) = 0;
    virtual void AddScreenShakePosition(int position, double shakeIntensity) = 0;
    virtual void AddNextFramePosition(int position) = 0;
    virtual void AddPlayerDamagedPosition(int position) = 0;
    virtual void AddOpponentDamagedPosition(int position) = 0;
    virtual void AddPlayBgmPosition(int position, const SharedUtilsStringType &id) = 0;
    virtual void AddPlayBgmPermanentlyPosition(int position, const SharedUtilsStringType &id) = 0;
    virtual void AddStopBgmPosition(int position) = 0;
    virtual void AddStopBgmPermanentlyPosition(int position) = 0;
    virtual void AddStopBgmInstantlyPosition(int position) = 0;
    virtual void AddStopBgmInstantlyPermanentlyPosition(int position) = 0;
    virtual void AddZoomPosition(int position) = 0;
    virtual void AddEndZoomPosition(int position) = 0;
    virtual void AddBeginBreakdownPosition(int position) = 0;
    virtual void AddEndBreakdownPosition(int position) = 0;
};

extern Vector2 gameWindowSize;
extern RectangleWH dialogTextArea;
extern double dialogPadding;
#endif

double Min(double a, double b);
double Max(double a, double b);
SharedUtilsStringListType split(const SharedUtilsStringType &s, SharedUtilsCharType delim);

#ifndef LAUNCHER
void GetCharacterDirectionFromDirectionVector(Vector2 directionVector, CharacterDirection *pDirection, FieldCharacterDirection *pSpriteDirection);

SharedUtilsStringType ParseRawDialog(IDialogEventsOwner *pDialogEventsOwner, const SharedUtilsStringType &rawDialog, RectangleWH textAreaRect, double desiredPadding, SharedUtilsFontType dialogFont);
SharedUtilsStringType StripDialogEvents(const SharedUtilsStringType &s);
SharedUtilsStringType ParseDialogEvents(IDialogEventsOwner *pDialogEventsOwner, int lineOffset, const SharedUtilsStringType &stringToParse, SharedUtilsStringType *pStringToPrependOnNext);
#endif

#endif
