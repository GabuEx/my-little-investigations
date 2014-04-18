/**
 * Easing functions that faciliate smooth UI animations in-game.
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

#include "EasingFunctions.h"
#include <algorithm>
using namespace std;

void EasingFunction::Begin()
{
    Reset();
    SetIsStarted(true);
}

void EasingFunction::Update(int delta)
{
    if (GetIsStarted() && !GetIsFinished())
    {
        msElapsedDuration = std::min(msElapsedDuration + delta, msTotalDuration + GetAnimationStartDelay());

        if (msElapsedDuration - GetAnimationStartDelay() == msTotalDuration)
        {
            SetIsFinished(true);
        }
    }
}

void EasingFunction::Reset()
{
    msElapsedDuration = 0;
    SetIsStarted(false);
    SetIsFinished(false);
}

void EasingFunction::Finish()
{
    msElapsedDuration = msTotalDuration + GetAnimationStartDelay();
    SetIsFinished(true);
}

double EasingFunction::GetCurrentValue()
{
    if (msElapsedDuration < GetAnimationStartDelay())
    {
        return startValue;
    }
    else if (msTotalDuration == 0)
    {
        return endValue;
    }
    else
    {
        return (startValue + (endValue - startValue) * GetNormalizedValue((double)(msElapsedDuration - GetAnimationStartDelay()) / msTotalDuration));
    }
}

double ConfrontationEntranceCharacterEase::GetNormalizedValue(double normalizedTime)
{
    // We want 108 discrete frames, so we'll convert normalized time into a frame number.
    int frameNumber = (int)(normalizedTime * 108.0);

    if (frameNumber < 12)
    {
        return 0;
    }
    else if (frameNumber < 21)
    {
        double stepNormalizedTime = (double)(frameNumber - 12) / (21 - 12);
        return (stepNormalizedTime * stepNormalizedTime);
    }
    else if (frameNumber < 31)
    {
        double stepNormalizedTime = (double)(frameNumber - 21) / (31 - 21) ;
        return 1 + stepNormalizedTime * 0.1;
    }
    else if (frameNumber < 34)
    {
        double stepNormalizedTime = (double)(frameNumber - 31) / (34 - 31);
        return 1.1 - (stepNormalizedTime * stepNormalizedTime) * 0.1;
    }
    else if (frameNumber < 90)
    {
        return 1;
    }
    else if (frameNumber < 100)
    {
        double stepNormalizedTime = (double)(frameNumber - 90) / (100 - 90);
        return 1 - stepNormalizedTime * stepNormalizedTime;
    }
    else
    {
        return 0;
    }
}

double ConfrontationEntranceBlockEase::GetNormalizedValue(double normalizedTime)
{
    // We want 108 discrete frames, so we'll convert normalized time into a frame number.
    int frameNumber = (int)(normalizedTime * 108.0);

    if (frameNumber < 12)
    {
        return 0;
    }
    else if (frameNumber < 21)
    {
        double stepNormalizedTime = (double)(frameNumber - 12) / (21 - 12);
        return (stepNormalizedTime * stepNormalizedTime);
    }
    else if (frameNumber < 22)
    {
        return 0.95;
    }
    else if (frameNumber < 23)
    {
        return 1;
    }
    else if (frameNumber < 24)
    {
        return 0.98125;
    }
    else if (frameNumber < 25)
    {
        return 1;
    }
    else if (frameNumber < 26)
    {
        return 0.99583;
    }
    else if (frameNumber < 27)
    {
        return 1;
    }
    else if (frameNumber < 28)
    {
        return 0.997917;
    }
    else if (frameNumber < 86)
    {
        return 1;
    }
    else if (frameNumber < 100)
    {
        double stepNormalizedTime = (double)(frameNumber - 86) / (100 - 86);
        return 1 - stepNormalizedTime * stepNormalizedTime;
    }
    else
    {
        return 0;
    }
}

double ConfrontationEntranceFlashEase::GetNormalizedValue(double normalizedTime)
{
    // We want 108 discrete frames, so we'll convert normalized time into a frame number.
    int frameNumber = (int)(normalizedTime * 108.0);

    if (frameNumber < 34)
    {
        return 0;
    }
    else if (frameNumber < 38)
    {
        double stepNormalizedTime = (double)(frameNumber - 34) / (38 - 34);
        return 1.0 - stepNormalizedTime;
    }
    else
    {
        return 0;
    }
}
