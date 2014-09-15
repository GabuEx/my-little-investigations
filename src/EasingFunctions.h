/**
 * Basic header/include file for EasingFunctions.cpp.
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

#ifndef EASINGFUNCTIONS_H
#define EASINGFUNCTIONS_H

#include <SDL2/SDL_stdinc.h>
#include <math.h>

class EasingFunction
{
public:
    EasingFunction(double startValue, double endValue, int msDuration)
    {
        this->startValue = startValue;
        this->endValue = endValue;
        this->msTotalDuration = msDuration;
        animationStartDelay = 0;

        Reset();
    }

    virtual ~EasingFunction() {}

    int GetAnimationStartDelay() const { return this->animationStartDelay; }
    void SetAnimationStartDelay(int animationStartDelay) { this->animationStartDelay = animationStartDelay; }
    bool GetIsStarted() const { return this->isStarted; }
    bool GetIsFinished() const { return this->isFinished; }

    void Begin();
    void Update(int delta);
    void Reset();
    void Finish();

    bool IsRunning() const { return GetIsStarted() && !GetIsFinished(); }

    double GetCurrentValue();

protected:
    virtual double GetNormalizedValue(double normalizedTime) = 0;

    int msTotalDuration;
    int msElapsedDuration;

private:
    void SetIsStarted(bool isStarted) { this->isStarted = isStarted; }
    void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

    int animationStartDelay;
    bool isStarted;
    bool isFinished;

    double startValue;
    double endValue;
};

class LinearEase : public EasingFunction
{
public:
    LinearEase(double startValue, double endValue, int msDuration)
        : EasingFunction(startValue, endValue, msDuration)
    {
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime)
    {
        return normalizedTime;
    }
};

class LinearStepEase : public EasingFunction
{
public:
    LinearStepEase(double startValue, double endValue, int msDuration, int numSteps)
        : EasingFunction(startValue, endValue, msDuration)
    {
        this->numSteps = numSteps;
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime)
    {
        return floor(numSteps * normalizedTime) / numSteps;
    }

private:
    int numSteps;
};

class QuadraticEase : public EasingFunction
{
public:
    QuadraticEase(double startValue, double endValue, int msDuration)
        : EasingFunction(startValue, endValue, msDuration)
    {
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime)
    {
        return 1 - (1 - normalizedTime) * (1 - normalizedTime);
    }
};

class EarthquakeEase : public EasingFunction
{
public:
    EarthquakeEase(int startValue, int endValue, int msDuration, int oscillations = 14)
        : EasingFunction(startValue, endValue, msDuration)
    {
        this->oscillations = oscillations;
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime)
    {
        double dampeningFactor = 1 - normalizedTime;
        return dampeningFactor * (0.5 + sin(M_PI + normalizedTime * oscillations * M_PI) / 2);
    }

private:
    int oscillations;
};

class CenteredEarthquakeEase : public EasingFunction
{
public:
    CenteredEarthquakeEase(int startValue, int endValue, int msDuration, int oscillations = 14)
        : EasingFunction(startValue, endValue, msDuration)
    {
        this->oscillations = oscillations;
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime)
    {
        double dampeningFactor = 1 - normalizedTime;
        return dampeningFactor * sin(M_PI + normalizedTime * oscillations * M_PI);
    }

private:
    int oscillations;
};

class GravityBounceEase : public EasingFunction
{
public:
    GravityBounceEase(double startValue, double endValue, int msDuration)
        : EasingFunction(startValue, endValue, msDuration)
    {
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime)
    {
        if (normalizedTime < 0.5)
        {
            return 4 * normalizedTime * normalizedTime;
        }
        else
        {
            return 4 * (normalizedTime - 1) * (normalizedTime - 1);
        }
    }
};

class SCurveEase : public EasingFunction
{
public:
    SCurveEase(double startValue, double endValue, int msDuration)
        : EasingFunction(startValue, endValue, msDuration)
    {
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime)
    {
        if (normalizedTime < 0.5)
        {
            return 2 * normalizedTime * normalizedTime;
        }
        else
        {
            normalizedTime -= 0.5;
            return 0.5 + (1 - (1 - normalizedTime * 2) * (1 - normalizedTime * 2)) / 2;
        }
    }
};

class ConfrontationEntranceCharacterEase : public EasingFunction
{
public:
    ConfrontationEntranceCharacterEase(double startValue, double endValue, int msDuration)
        : EasingFunction(startValue, endValue, msDuration)
    {
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime);
};

class ConfrontationEntranceBlockEase : public EasingFunction
{
public:
    ConfrontationEntranceBlockEase(double startValue, double endValue, int msDuration)
        : EasingFunction(startValue, endValue, msDuration)
    {
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime);
};

class ConfrontationEntranceFlashEase : public EasingFunction
{
public:
    ConfrontationEntranceFlashEase(double startValue, double endValue, int msDuration)
        : EasingFunction(startValue, endValue, msDuration)
    {
    }

protected:
    virtual double GetNormalizedValue(double normalizedTime);
};

#endif
