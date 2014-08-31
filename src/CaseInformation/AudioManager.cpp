/**
 * Manager for all of the audio in the game; is used as an interface to
 * play sound, music, and dialog.
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

#include "AudioManager.h"
#include "../globals.h"
#include "../ResourceLoader.h"
#include "../XmlReader.h"

const int FadeDurationMs = 1000;
const double VolumePercentageDuringDialog = 0.4;

AudioManager::AudioManager()
{
    bgmFadeAction = FadeActionStop;
    pBgmFadeEase = NULL;
    pMusicVolumeChangeForDialogEase = NULL;
    ambianceFadeAction = FadeActionStop;
    pAmbianceFadeEase = NULL;
    pAmbianceVolumeChangeForDialogEase = NULL;

    if (isAudioEnabled())
    {
        SetMusicVolume(gBackgroundMusicVolume);
        SetAmbianceVolume(gSoundEffectsVolume);
    }

    musicVolReductionPercentage = 1;
}

AudioManager::~AudioManager()
{
    if (isAudioEnabled())
    {
        stopMusic();
        stopLoopingSounds();
        stopDialog();

        for (unsigned int i = 0; i < bgmIdList.size(); i++)
        {
            ResourceLoader::GetInstance()->UnloadMusic(bgmIdList[i]);
        }

        for (unsigned int i = 0; i < sfxIdList.size(); i++)
        {
            ResourceLoader::GetInstance()->UnloadSound(sfxIdList[i]);
        }

        bgmIdList.clear();
        sfxIdList.clear();
    }

    delete pBgmFadeEase;
    pBgmFadeEase = NULL;
    delete pMusicVolumeChangeForDialogEase;
    pMusicVolumeChangeForDialogEase = NULL;
    delete pAmbianceFadeEase;
    pAmbianceFadeEase = NULL;
    delete pAmbianceVolumeChangeForDialogEase;
    pAmbianceVolumeChangeForDialogEase = NULL;
}

void AudioManager::PlayBgmWithId(const string &id)
{
    if (id.length() == 0)
    {
        // If we've got no ID, that's a cue to stop the
        // currently playing music.
        stopMusic();
        return;
    }
    else if (id == getPlayingMusic() && pBgmFadeEase == NULL)
    {
        // This song is already playing and we're not fading out, so nothing to do.
        return;
    }

    delete pBgmFadeEase;
    pBgmFadeEase = NULL;

    setVolumeMusic(musicVolMaster);

    if (id != getPlayingMusic())
    {
        playMusic(id);
    }
}

void AudioManager::PauseCurrentBgm()
{
    if (getPlayingMusic().length() > 0)
    {
        InitializeBgmFadeEase(FadeDurationMs, true);
        bgmFadeAction = FadeActionPause;
    }
}

void AudioManager::ResumeCurrentBgm()
{
    if (getPlayingMusic().length() > 0)
    {
        resumeMusic();
        InitializeBgmFadeEase(FadeDurationMs, false);
        bgmFadeAction = FadeActionResume;
    }
}

void AudioManager::StopCurrentBgm(bool isInstant)
{
    if (getPlayingMusic().length() > 0)
    {
        if (isInstant)
        {
            stopMusic();
        }
        else
        {
            clearPlayingMusic();
            InitializeBgmFadeEase(FadeDurationMs, true);
            bgmFadeAction = FadeActionStop;
        }
    }
}

void AudioManager::PlayAmbianceWithId(const string &id)
{
    if (id.length() == 0)
    {
        // If we've got no ID, that's a cue to stop the
        // currently playing ambiance.
        stopAmbiance();
        return;
    }
    else if (id == getPlayingAmbiance() && pAmbianceFadeEase == NULL)
    {
        // This sound is already playing and we're not fading out, so nothing to do.
        return;
    }

    delete pAmbianceFadeEase;
    pAmbianceFadeEase = NULL;

    setVolumeAmbiance(ambianceVolMaster);

    if (id != getPlayingAmbiance())
    {
        playAmbiance(id);
    }
}

void AudioManager::PauseCurrentAmbiance()
{
    StopCurrentAmbiance(false /* isInstant */);
}

void AudioManager::ResumeCurrentAmbiance()
{
    PlayAmbianceWithId(cachedAmbianceSfxId);
}

void AudioManager::StopCurrentAmbiance(bool isInstant)
{
    if (getPlayingAmbiance().length() > 0)
    {
        if (isInstant)
        {
            stopAmbiance();
        }
        else
        {
            clearPlayingAmbiance();
            InitializeAmbianceFadeEase(FadeDurationMs, true);
            ambianceFadeAction = FadeActionStop;
        }
    }
}

void AudioManager::PlayRandomHoofstepSound(const string &textureName, double volume)
{
    string randomSoundId = hoofstepSoundIdListByTexture[textureName][rand() % hoofstepSoundIdListByTexture[textureName].size()];
    playSound(randomSoundId, volume);
}

void AudioManager::Update(int delta)
{
    if (pBgmFadeEase != NULL)
    {
        pBgmFadeEase->Update(delta);

        if (pBgmFadeEase->GetIsFinished())
        {
            switch (bgmFadeAction)
            {
                case FadeActionPause:
                    pauseMusic();
                    break;

                case FadeActionResume:
                    break;

                case FadeActionStop:
                    stopMusic();
                    break;
            }

            delete pBgmFadeEase;
            pBgmFadeEase = NULL;
        }
        else
        {
            setVolumeMusic(musicVolMaster * pBgmFadeEase->GetCurrentValue());
        }
    }

    if (pMusicVolumeChangeForDialogEase != NULL)
    {
        pMusicVolumeChangeForDialogEase->Update(delta);
        musicVolReductionPercentage = pMusicVolumeChangeForDialogEase->GetCurrentValue();
        setMusicDialogReductionPercentage(musicVolReductionPercentage);

        if (pMusicVolumeChangeForDialogEase->GetIsFinished())
        {
            delete pMusicVolumeChangeForDialogEase;
            pMusicVolumeChangeForDialogEase = NULL;
        }
    }

    if (pAmbianceFadeEase != NULL)
    {
        pAmbianceFadeEase->Update(delta);

        if (pAmbianceFadeEase->GetIsFinished())
        {
            switch (ambianceFadeAction)
            {
                case FadeActionPause:
                    stopAmbiance();
                    break;

                case FadeActionResume:
                    break;

                case FadeActionStop:
                    stopAmbiance();
                    break;
            }

            delete pAmbianceFadeEase;
            pAmbianceFadeEase = NULL;
        }
        else
        {
            setVolumeAmbiance(ambianceVolMaster * pAmbianceFadeEase->GetCurrentValue());
        }
    }

    if (pAmbianceVolumeChangeForDialogEase != NULL)
    {
        pAmbianceVolumeChangeForDialogEase->Update(delta);
        ambianceVolReductionPercentage = pAmbianceVolumeChangeForDialogEase->GetCurrentValue();
        setAmbianceDialogReductionPercentage(musicVolReductionPercentage);

        if (pAmbianceVolumeChangeForDialogEase->GetIsFinished())
        {
            delete pAmbianceVolumeChangeForDialogEase;
            pAmbianceVolumeChangeForDialogEase = NULL;
        }
    }
}

void AudioManager::SetMusicVolume(double vol)
{
    musicVolMaster = vol;

    if (pBgmFadeEase != NULL)
    {
        setVolumeMusic(musicVolMaster * pBgmFadeEase->GetCurrentValue());
    }
    else
    {
        setVolumeMusic(musicVolMaster);
    }
}

void AudioManager::SetAmbianceVolume(double vol)
{
    ambianceVolMaster = vol;

    if (pAmbianceFadeEase != NULL)
    {
        setVolumeAmbiance(ambianceVolMaster * pAmbianceFadeEase->GetCurrentValue());
    }
    else
    {
        setVolumeAmbiance(ambianceVolMaster);
    }
}

void AudioManager::LowerMusicVolumeForDialog()
{
    delete pMusicVolumeChangeForDialogEase;
    pMusicVolumeChangeForDialogEase = new LinearStepEase(1, VolumePercentageDuringDialog, FadeDurationMs, 10);
    pMusicVolumeChangeForDialogEase->Begin();
}

void AudioManager::RestoreMusicVolumeForDialog()
{
    delete pMusicVolumeChangeForDialogEase;
    pMusicVolumeChangeForDialogEase = new LinearStepEase(musicVolReductionPercentage, 1, FadeDurationMs, 10);
    pMusicVolumeChangeForDialogEase->Begin();
}

void AudioManager::LowerAmbianceVolumeForDialog()
{
    delete pAmbianceVolumeChangeForDialogEase;
    pAmbianceVolumeChangeForDialogEase = new LinearStepEase(1, VolumePercentageDuringDialog, FadeDurationMs, 10);
    pAmbianceVolumeChangeForDialogEase->Begin();
}

void AudioManager::RestoreAmbianceVolumeForDialog()
{
    delete pAmbianceVolumeChangeForDialogEase;
    pAmbianceVolumeChangeForDialogEase = new LinearStepEase(ambianceVolReductionPercentage, 1, FadeDurationMs, 10);
    pAmbianceVolumeChangeForDialogEase->Begin();
}

void AudioManager::LoadFromXml(XmlReader *pReader)
{
    // If audio isn't enabled, then there isn't much point
    // in loading audio files.
    if (!isAudioEnabled())
    {
        return;
    }

    bgmIdList.clear();
    sfxIdList.clear();

    pReader->StartElement("AudioManager");

    pReader->StartElement("BgmIdToFilePathDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string bgmId = pReader->ReadTextElement("BgmId");
        string filePath = pReader->ReadTextElement("FilePath");
        ResourceLoader::GetInstance()->PreloadMusic(bgmId, filePath);
        bgmIdList.push_back(bgmId);
    }

    pReader->EndElement();

    pReader->StartElement("SfxIdToFilePathDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string sfxId = pReader->ReadTextElement("SfxId");
        string filePath = pReader->ReadTextElement("FilePath");
        ResourceLoader::GetInstance()->PreloadSound(sfxId, filePath);
        sfxIdList.push_back(sfxId);
    }

    pReader->EndElement();

    pReader->StartElement("HoofstepSoundIdListByTexture");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string textureName = pReader->ReadTextElement("TextureName");

        pReader->StartElement("HoofstepSoundIdList");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            string soundId = pReader->ReadTextElement("SoundId");

            hoofstepSoundIdListByTexture[textureName].push_back(soundId);
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->EndElement();
}

void AudioManager::InitializeBgmFadeEase(int msDuration, bool fadeOut)
{
    delete pBgmFadeEase;
    pBgmFadeEase = new LinearStepEase(fadeOut ? 1 : 0, fadeOut ? 0 : 1, msDuration, 10);
    pBgmFadeEase->Begin();
}

void AudioManager::InitializeAmbianceFadeEase(int msDuration, bool fadeOut)
{
    delete pAmbianceFadeEase;
    pAmbianceFadeEase = new LinearStepEase(fadeOut ? 1 : 0, fadeOut ? 0 : 1, msDuration, 10);
    pAmbianceFadeEase->Begin();
}
