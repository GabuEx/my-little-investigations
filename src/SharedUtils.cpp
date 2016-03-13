/**
 * Utility methods to handle various tasks in both the game executable and the case creator.
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

#include "SharedUtils.h"

#include "MLIException.h"

RectangleWH dialogTextArea = RectangleWH(3, 370, 954, 167);
double dialogPadding = 30;

const double FullStopMillisecondPause = 500;
const double HalfStopMillisecondPause = 250;
const double EllipsisMillisecondPause = 150;

#ifndef CASE_CREATOR
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <limits>
#else
#include <QtGlobal>
#endif

double Min(double a, double b)
{
#ifndef CASE_CREATOR
    return min(a, b);
#else
    return qMin(a, b);
#endif
}

double Max(double a, double b)
{
#ifndef CASE_CREATOR
    return max(a, b);
#else
    return qMax(a, b);
#endif
}

int StringIndexOf(const SharedUtilsStringType &s, SharedUtilsCharType c)
{
#ifndef CASE_CREATOR
    return static_cast<int>(s.find(c));
#else
    return s.indexOf(c);
#endif
}

int StringIndexNotFoundValue()
{
#ifndef CASE_CREATOR
    return static_cast<int>(string::npos);
#else
    return -1;
#endif
}

SharedUtilsStringType GetSubstring(const SharedUtilsStringType &s, int startPosition, int length = StringIndexNotFoundValue())
{
#ifndef CASE_CREATOR
    return s.substr(startPosition, length);
#else
    if (length > 0)
    {
        return s.right(s.length() - startPosition).left(length);
    }
    else if (length < 0)
    {
        return s.right(s.length() - startPosition);
    }
    else
    {
        return "";
    }
#endif
}

double GetStringWidth(const SharedUtilsStringType &s, SharedUtilsFontType font)
{
#ifndef CASE_CREATOR
    return font->GetWidth(s);
#else
    return QFontMetrics(font).width(s);
#endif
}

SharedUtilsStringType StringToLower(const SharedUtilsStringType &s)
{
#ifndef CASE_CREATOR
    SharedUtilsStringType stringCopy = s;
    transform(stringCopy.begin(), stringCopy.end(), stringCopy.begin(), ::tolower);
    return stringCopy;
#else
    return s.toLower();
#endif
}

int StringToInt(const SharedUtilsStringType &s)
{
#ifndef CASE_CREATOR
    return strtod(s.c_str(), NULL);
#else
    return s.toInt();
#endif
}

#ifndef CASE_CREATOR
#include <sstream>

SharedUtilsStringListType &split(const SharedUtilsStringType &s, SharedUtilsCharType delim, SharedUtilsStringListType &tokens)
{
    stringstream ss(s);
    SharedUtilsStringType item;

    while(getline(ss, item, delim))
    {
        tokens.push_back(item);
    }

    return tokens;
}
#endif

SharedUtilsStringListType split(const SharedUtilsStringType &s, SharedUtilsCharType delim)
{
#ifndef CASE_CREATOR
    SharedUtilsStringListType tokens;
    return split(s, delim, tokens);
#else
    return s.split(delim);
#endif
}

SharedUtilsStringType ParseRawDialog(IDialogEventsOwner *pDialogEventsOwner, const SharedUtilsStringType &rawDialog, RectangleWH textAreaRect, double desiredPadding, SharedUtilsFontType dialogFont)
{
    double allowedWidth = textAreaRect.GetWidth() - desiredPadding * 2;
    SharedUtilsStringType fullString = "";
    SharedUtilsStringListType wordList = split(rawDialog, ' ');

    while (!wordList.empty())
    {
        SharedUtilsStringType curstring = "";
        double curTextWidth = 0;
        bool lineDone = false;
        bool addSpace = false;

        if (fullString.length() > 0)
        {
            fullString += "\n";
        }

        while (!lineDone)
        {
            SharedUtilsStringType stringToTest = (addSpace ? " " : "") + wordList.front();
            double curStringWidth = GetStringWidth(StripDialogEvents(stringToTest), dialogFont);

            // If we've got a single word that takes up more than the entire length of the screen,
            // then we need to split it up.
            if (curTextWidth == 0 && curStringWidth > allowedWidth)
            {
                SharedUtilsStringType testString = "";
                SharedUtilsStringType lastTestString = "";
                curStringWidth = 0;

                while (curStringWidth <= allowedWidth)
                {
                    if (stringToTest[0] == '{')
                    {
                        testString += stringToTest[0];
                        stringToTest = GetSubstring(stringToTest, 1);

                        while (stringToTest[0] != '}')
                        {
                            testString += stringToTest[0];
                            stringToTest = GetSubstring(stringToTest, 1);
                        }

                        testString += stringToTest[0];
                        stringToTest = GetSubstring(stringToTest, 1);
                    }

                    lastTestString = testString;
                    testString += stringToTest[0];
                    double testCurStringWidth = GetStringWidth(StripDialogEvents(testString), dialogFont);

                    if (testCurStringWidth > allowedWidth)
                    {
                        break;
                    }

                    curStringWidth = testCurStringWidth;
                    stringToTest = GetSubstring(stringToTest, 1);
                }

                wordList.insert(wordList.begin() + 1, stringToTest);
                stringToTest = lastTestString;
            }

            if (curTextWidth + curStringWidth <= allowedWidth)
            {
                SharedUtilsStringType stringToPrependOnNext;
                stringToTest = ParseDialogEvents(pDialogEventsOwner, static_cast<int>(fullString.length() + curstring.length()), stringToTest, &stringToPrependOnNext);
                curstring += stringToTest;
                curTextWidth += curStringWidth;
                wordList.pop_front();
                addSpace = true;

                if (wordList.empty())
                {
                    lineDone = true;
                }
                else
                {
                    wordList[0] = stringToPrependOnNext + wordList.front();
                }
            }
            else
            {
                lineDone = true;
            }
        }

        fullString += curstring;
    }

    return fullString;
}

SharedUtilsStringType StripDialogEvents(const SharedUtilsStringType &s)
{
    SharedUtilsStringType strippedString = s;

    while (StringIndexOf(strippedString, '{') != StringIndexNotFoundValue() && StringIndexOf(strippedString, '}') != StringIndexNotFoundValue())
    {
        int eventStart = StringIndexOf(strippedString, '{');
        SharedUtilsStringListType eventComponents = split(GetSubstring(strippedString, eventStart + 1, StringIndexOf(strippedString, '}') - eventStart - 1), ':');
        SharedUtilsStringType replacementText = "";
        eventComponents[0] = StringToLower(eventComponents[0]);
        SharedUtilsStringType testString = eventComponents[0];

        if (testString == "fullstop")
        {
            if (eventComponents.size() > 1)
            {
                replacementText = eventComponents[1];
            }
            else
            {
                replacementText = ".";
            }
        }
        else if (testString == "halfstop")
        {
            if (eventComponents.size() > 1)
            {
                replacementText = eventComponents[1];
            }
            else
            {
                replacementText = ",";
            }
        }
        else if (testString == "ellipsis")
        {
            replacementText = "...";
        }

        strippedString = GetSubstring(strippedString, 0, StringIndexOf(strippedString, '{')) + replacementText + GetSubstring(strippedString, StringIndexOf(strippedString, '}') + 1);
    }

    return strippedString;
}

SharedUtilsStringType ParseDialogEvents(IDialogEventsOwner *pDialogEventsOwner, int lineOffset, const SharedUtilsStringType &stringToParse, SharedUtilsStringType *pStringToPrependOnNext)
{
    SharedUtilsStringType parsedString = stringToParse;
    *pStringToPrependOnNext = "";

    while (StringIndexOf(parsedString, '{') != StringIndexNotFoundValue() && StringIndexOf(parsedString, '}') != StringIndexNotFoundValue())
    {
        int eventStart = StringIndexOf(parsedString, '{');
        int eventEnd = StringIndexOf(parsedString, '}');

        SharedUtilsStringListType eventComponents = split(GetSubstring(parsedString, eventStart + 1, eventEnd - eventStart - 1), ':');
        SharedUtilsStringType replacementText = "";
        eventComponents[0] = StringToLower(eventComponents[0]);
        SharedUtilsStringType testString = eventComponents[0];

        if (pDialogEventsOwner != NULL)
        {
            if (testString == "speed")
            {
                double newMillisecondsPerCharacterUpdate = StringToInt(eventComponents[1]);
                pDialogEventsOwner->AddSpeedChangePosition(lineOffset + eventStart, newMillisecondsPerCharacterUpdate);
            }
            else if (testString == "emotion")
            {
                SharedUtilsStringType newEmotion = eventComponents[1];
                pDialogEventsOwner->AddEmotionChangePosition(lineOffset + eventStart, newEmotion);
            }
            else if (testString == "otheremotion")
            {
                SharedUtilsStringType newOtherEmotion = eventComponents[1];
                pDialogEventsOwner->AddEmotionOtherChangePosition(lineOffset + eventStart, newOtherEmotion);
            }
            else if (testString == "pause")
            {
                double millisecondDuration = StringToInt(eventComponents[1]);
                pDialogEventsOwner->AddPausePosition(lineOffset + eventStart, millisecondDuration);
            }
            else if (testString == "audiopause")
            {
                double millisecondDuration = StringToInt(eventComponents[1]);
                pDialogEventsOwner->AddAudioPausePosition(lineOffset + eventStart, millisecondDuration);
             }
            else if (testString == "mouth")
            {
                eventComponents[1] = StringToLower(eventComponents[1]);
                bool mouthIsOn = eventComponents[1] == "on";
                pDialogEventsOwner->AddMouthChangePosition(lineOffset + eventStart, mouthIsOn);
            }
            else if (testString == "fullstop")
            {
                if (eventComponents.size() > 1)
                {
                    replacementText = eventComponents[1];
                }
                else
                {
                    replacementText = ".";
                }

                pDialogEventsOwner->AddMouthChangePosition(lineOffset + eventStart, false /* mouthIsOn */);
                pDialogEventsOwner->AddPausePosition(lineOffset + eventStart + 1, FullStopMillisecondPause);

                if (eventEnd + 1 == (int)parsedString.length())
                {
                    *pStringToPrependOnNext = "{Mouth:On}";
                }
                else
                {
                    pDialogEventsOwner->AddMouthChangePosition(lineOffset + eventStart + 1, true /* mouthIsOn */);
                }
            }
            else if (testString == "halfstop")
            {
                if (eventComponents.size() > 1)
                {
                    replacementText = eventComponents[1];
                }
                else
                {
                    replacementText = ",";
                }

                pDialogEventsOwner->AddMouthChangePosition(lineOffset + eventStart, false /* mouthIsOn */);
                pDialogEventsOwner->AddPausePosition(lineOffset + eventStart + 1, HalfStopMillisecondPause);

                if (eventEnd + 1 == (int)parsedString.length())
                {
                    *pStringToPrependOnNext = "{Mouth:On}";
                }
                else
                {
                    pDialogEventsOwner->AddMouthChangePosition(lineOffset + eventStart + 1, true /* mouthIsOn */);
                }
            }
            else if (testString == "ellipsis")
            {
                replacementText = "...";

                pDialogEventsOwner->AddMouthChangePosition(lineOffset + eventStart, false /* mouthIsOn */);
                pDialogEventsOwner->AddPausePosition(lineOffset + eventStart + 1, EllipsisMillisecondPause);
                pDialogEventsOwner->AddPausePosition(lineOffset + eventStart + 2, EllipsisMillisecondPause);
                pDialogEventsOwner->AddPausePosition(lineOffset + eventStart + 3, EllipsisMillisecondPause);

                if (eventEnd + 1 == (int)parsedString.length())
                {
                    *pStringToPrependOnNext = "{Mouth:On}";
                }
                else
                {
                    pDialogEventsOwner->AddMouthChangePosition(lineOffset + eventStart + 3, true /* mouthIsOn */);
                }
            }
            else if (testString == "aside")
            {
                int currentIndex = lineOffset + eventStart;
                pDialogEventsOwner->StartAside(currentIndex);
            }
            else if (testString == "/aside")
            {
                int currentIndex = lineOffset + eventStart;
                pDialogEventsOwner->EndAside(currentIndex, eventEnd, static_cast<int>(parsedString.length()), pStringToPrependOnNext);
            }
            else if (testString == "emphasis")
            {
                int currentIndex = lineOffset + eventStart;
                pDialogEventsOwner->StartEmphasis(currentIndex);
            }
            else if (testString == "/emphasis")
            {
                int currentIndex = lineOffset + eventStart;
                pDialogEventsOwner->EndEmphasis(currentIndex);
            }
            else if (testString == "playsound")
            {
                SharedUtilsStringType soundId = eventComponents[1];
                pDialogEventsOwner->AddPlaySoundPosition(lineOffset + eventStart, soundId);
            }
            else if (testString == "shake")
            {
                pDialogEventsOwner->AddShakePosition(lineOffset + eventStart);
            }
            else if (testString == "screenshake")
            {
                double shakeIntensity = Min(Max(StringToInt(eventComponents[1]), 0.0), 100.0);
                pDialogEventsOwner->AddScreenShakePosition(lineOffset + eventStart, shakeIntensity);
            }
            else if (testString == "nextframe")
            {
                pDialogEventsOwner->AddNextFramePosition(lineOffset + eventStart);
            }
            else if (testString == "damageplayer")
            {
                pDialogEventsOwner->AddPlayerDamagedPosition(lineOffset + eventStart);
            }
            else if (testString == "damageopponent")
            {
                pDialogEventsOwner->AddOpponentDamagedPosition(lineOffset + eventStart);
            }
            else if (testString == "playbgm")
            {
                SharedUtilsStringType bgmId = eventComponents[1];
                pDialogEventsOwner->AddPlayBgmPosition(lineOffset + eventStart, bgmId);
            }
            else if (testString == "playbgmpermanently")
            {
                SharedUtilsStringType bgmId = eventComponents[1];
                pDialogEventsOwner->AddPlayBgmPermanentlyPosition(lineOffset + eventStart, bgmId);
            }
            else if (testString == "stopbgm")
            {
                pDialogEventsOwner->AddStopBgmPosition(lineOffset + eventStart);
            }
            else if (testString == "stopbgmpermanently")
            {
                pDialogEventsOwner->AddStopBgmPermanentlyPosition(lineOffset + eventStart);
            }
            else if (testString == "stopbgminstantly")
            {
                pDialogEventsOwner->AddStopBgmInstantlyPosition(lineOffset + eventStart);
            }
            else if (testString == "stopbgminstantlypermanently")
            {
                pDialogEventsOwner->AddStopBgmInstantlyPermanentlyPosition(lineOffset + eventStart);
            }
            else if (testString == "zoom")
            {
                pDialogEventsOwner->AddZoomPosition(lineOffset + eventStart);
            }
            else if (testString == "endzoom")
            {
                pDialogEventsOwner->AddEndZoomPosition(lineOffset + eventStart);
            }
            else if (testString == "beginbreakdown")
            {
                pDialogEventsOwner->AddBeginBreakdownPosition(lineOffset + eventStart);
            }
            else if (testString == "endbreakdown")
            {
                pDialogEventsOwner->AddEndBreakdownPosition(lineOffset + eventStart);
            }
            else
            {
                ThrowException("Unknown event.");
            }
        }
        else
        {
            if (testString == "fullstop")
            {
                if (eventComponents.size() > 1)
                {
                    replacementText = eventComponents[1];
                }
                else
                {
                    replacementText = ".";
                }

                if (eventEnd + 1 == (int)parsedString.length())
                {
                    *pStringToPrependOnNext = "{Mouth:On}";
                }
            }
            else if (testString == "halfstop")
            {
                if (eventComponents.size() > 1)
                {
                    replacementText = eventComponents[1];
                }
                else
                {
                    replacementText = ",";
                }

                if (eventEnd + 1 == (int)parsedString.length())
                {
                    *pStringToPrependOnNext = "{Mouth:On}";
                }
            }
            else if (testString == "ellipsis")
            {
                replacementText = "...";

                if (eventEnd + 1 == (int)parsedString.length())
                {
                    *pStringToPrependOnNext = "{Mouth:On}";
                }
            }
            else if (testString == "ellipsis")
            {
                if (eventEnd + 1 == (int)parsedString.length())
                {
                    *pStringToPrependOnNext = "{Mouth:On}";
                }
            }
        }

        parsedString = GetSubstring(parsedString, 0, eventStart) + replacementText + GetSubstring(parsedString, eventEnd + 1);
    }

    return parsedString;
}
