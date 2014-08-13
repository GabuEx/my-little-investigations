/**
 * Basic header/include file for AudioManager.cpp.
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

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "../EasingFunctions.h"
#include "../enums.h"
#include "../mli_audio.h"
#include <map>
#include <vector>

using namespace std;

class XmlReader;

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    void PlayBgmWithId(const string &id);
    void PauseCurrentBgm();
    void ResumeCurrentBgm();
    void StopCurrentBgm(bool isInstant);

    void PlayAmbianceWithId(const string &id);
    void PauseCurrentAmbiance();
    void ResumeCurrentAmbiance();
    void StopCurrentAmbiance(bool isInstant);

    void PlayRandomHoofstepSound(const string &textureName, double volume);

    void Update(int delta);

    void SetMusicVolume(double vol);
    void SetAmbianceVolume(double vol);

    void LowerMusicVolumeForDialog();
    void RestoreMusicVolumeForDialog();

    void LowerAmbianceVolumeForDialog();
    void RestoreAmbianceVolumeForDialog();

    void LoadFromXml(XmlReader *pReader);

private:
    void InitializeBgmFadeEase(int msDuration, bool fadeOut);
    void InitializeAmbianceFadeEase(int msDuration, bool fadeOut);

    vector<string> bgmIdList;
    vector<string> sfxIdList;
    map<string, vector<string> > hoofstepSoundIdListByTexture;

    FadeAction bgmFadeAction;
    EasingFunction *pBgmFadeEase;
    EasingFunction *pMusicVolumeChangeForDialogEase;

    FadeAction ambianceFadeAction;
    EasingFunction *pAmbianceFadeEase;
    EasingFunction *pAmbianceVolumeChangeForDialogEase;

    double musicVolMaster;
    double musicVolReductionPercentage;
    double ambianceVolMaster;
    double ambianceVolReductionPercentage;

    string cachedAmbianceSfxId;
};

#endif
