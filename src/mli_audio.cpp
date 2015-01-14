/**
 * Provides functions related to the sound system. (Music, SFX, dialog, etc.)
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

#include "mli_audio.h"
#include <iostream>
#include <map>

using namespace std;

map<string, Mix_Music*> music;
map<string, Mix_Chunk*> sfx;
map<string, Mix_Chunk*> dialog;
string currentMusic = "";
string currentMusicToReport = "";
string currentDialog = "";
string currentAmbiance = "";
string currentAmbianceToReport = "";

#define DIALOG_CHANNEL 0
#define AMBIENCE_CHANNEL 1
#define PARTNER_ABILITY_LOOP_CHANNEL 2
#define SOUND_LOOP_CHANNEL_START 3
#define NUM_SOUND_LOOP_CHANNELS 4
#define NUM_RESERVED_CHANNELS (SOUND_LOOP_CHANNEL_START + NUM_SOUND_LOOP_CHANNELS)

volatile bool audioEnabled = true;

SDL_Thread * fadeThread = NULL;

volatile int fadeTime = 0;
volatile int fadeDir = 0;
volatile double fadeMultiplier = 1.0;
volatile bool pauseWhenFadeDoneFadeOut = true;

volatile double musicVol = 1.0;
volatile double musicDialogReductionPercentage = 1.0;
volatile double soundVol = 1.0;
volatile double ambianceVol = 1.0;
volatile double ambianceDialogReductionPercentage = 1.0;
volatile double dialogVol = 1.0;

void initAudio()
{
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096) != 0)
    {
        // If the audio couldn't be started, just set audioEnabled to false.
        // Future preload/play/etc. calls will be ignored.
        // As much as no audio is a detriment to gameplay, the game /is/ playable without it...
        setAudioEnabled(false);
        cout << "ERROR: Unable to initialize audio: %s\n", Mix_GetError();
    }
    else
    {
        // We'll allocate the number of reserved channels needed plus 8 more - that should be more than enough.
        Mix_AllocateChannels(NUM_RESERVED_CHANNELS + 8);

        // If the audio was successfully initialized, reserve channel 0 (for dialog), 1 (for ambiance), and 2-5 (for looping sounds) and set up the channelDone callback.
        Mix_ReserveChannels(SOUND_LOOP_CHANNEL_START + NUM_SOUND_LOOP_CHANNELS);
        Mix_ChannelFinished(channelDone);
    }
}

void channelDone(int channel)
{
    // Called whenever a channel is stopped.
    // We only really care if it was the dialog channel.
    // If it was, there isn't any dialog playing anymore.
    if(channel == DIALOG_CHANNEL)
    {
        currentDialog = "";
    }
}

void musicToPartB()
{
    Mix_HookMusicFinished(NULL);
    Mix_PlayMusic(music[currentMusic + "_B"], -1);
}

bool preloadMusic(const string &id, SDL_RWops *pFileOpsA, SDL_RWops *pFileOpsB)
{
    if(!audioEnabled) return false;
    Mix_Music *pMusicA = Mix_LoadMUS_RW(pFileOpsA, true);
    if(pMusicA == NULL) return false;
    Mix_Music *pMusicB = Mix_LoadMUS_RW(pFileOpsB, true);
    if(pMusicB == NULL)
    {
        Mix_FreeMusic(pMusicA);
        return false;
    }
    music[id+"_A"] = pMusicA;
    music[id+"_B"] = pMusicB;
    return true;
}

void unloadMusic(const string &id)
{
    Mix_Music *pMusicA = music[id + "_A"];
    Mix_Music *pMusicB = music[id + "_B"];

    if (pMusicA != NULL)
    {
        Mix_FreeMusic(pMusicA);
    }

    if (pMusicB != NULL)
    {
        Mix_FreeMusic(pMusicB);
    }

    music.erase(id + "_A");
    music.erase(id + "_B");
}

bool preloadSound(const string &id, SDL_RWops *pFileOps)
{
    if(!audioEnabled) return false;
    Mix_Chunk *pSound = Mix_LoadWAV_RW(pFileOps, 1);
    if(pSound == NULL) return false;
    Mix_VolumeChunk(pSound, (int)(soundVol * MIX_MAX_VOLUME));
    sfx[id] = pSound;
    return true;
}

void unloadSound(const string &id)
{
    Mix_Chunk *pSound = sfx[id];

    if (pSound != NULL)
    {
        Mix_FreeChunk(pSound);
    }

    sfx.erase(id);
}

bool preloadDialog(const string &id,SDL_RWops *pFileOps)
{
    if (!audioEnabled) return false;
    Mix_Chunk *pSound = Mix_LoadWAV_RW(pFileOps, 1);
    if (pSound == NULL) return false;
    dialog[id] = pSound;
    return true;
}

void unloadDialog(const string &id)
{
    Mix_Chunk *pSound = dialog[id];

    if (pSound != NULL)
    {
        Mix_FreeChunk(pSound);
    }

    dialog.erase(id);
}

bool playMusic(const string &id)
{
    if (!audioEnabled) return false;
    Mix_Music *pMusicA = music[id + "_A"];
    if (!pMusicA) return false;
    Mix_Music *pMusicB = music[id + "_B"];
    if (!pMusicB) return false;
    if (currentMusic.length() > 0)
    {
        Mix_HookMusicFinished(NULL);
        Mix_HaltMusic();
    }
    currentMusic = id;
    currentMusicToReport = id;
    Mix_HookMusicFinished(musicToPartB);
    if (Mix_PlayMusic(pMusicA, 0) == -1)
    {
        currentMusic = "";
        currentMusicToReport = "";
        cout << "WARNING: Couldn't play music \"" << id.c_str() << "\"." << endl;
        return false;
    }
    return true;
}

string getPlayingMusic()
{
    return currentMusicToReport;
}

void clearPlayingMusic()
{
    currentMusicToReport = "";
}

bool stopMusic()
{
    if (!audioEnabled || currentMusic.length() == 0) return false;
    Mix_HookMusicFinished(NULL);
    Mix_HaltMusic();
    currentMusic = "";
    currentMusicToReport = "";
    return true;
}

bool pauseMusic()
{
    if (!audioEnabled || currentMusic.length() == 0) return false;
    Mix_PauseMusic();
    return true;
}

bool resumeMusic()
{
    if (!audioEnabled || currentMusic.length() == 0) return false;
    Mix_ResumeMusic();
    return true;
}

bool playSound(const string &id)
{
    return playSound(id, 1.0);
}

bool playSound(const string &id, double volume)
{
    if (!audioEnabled) return false;
    Mix_Chunk *pSound = sfx[id];
    if (!pSound) return false;

    int setVol = (int)(soundVol * volume * MIX_MAX_VOLUME);
    Mix_VolumeChunk(pSound, setVol);

    return Mix_PlayChannel(-1, pSound, 0) >= 0;
}

bool playAmbiance(const string &id)
{
    if (!audioEnabled) return false;
    Mix_Chunk *pSound = sfx[id];
    if (!pSound) return false;

    currentAmbiance = id;
    currentAmbianceToReport = id;

    Mix_HaltChannel(AMBIENCE_CHANNEL);
    return Mix_PlayChannel(AMBIENCE_CHANNEL, pSound, -1) == AMBIENCE_CHANNEL;
}

string getPlayingAmbiance()
{
    return currentAmbiance;
}

void clearPlayingAmbiance()
{
    currentAmbianceToReport = "";
}

bool stopAmbiance()
{
    if (!audioEnabled || currentAmbiance.length() == 0) return false;
    Mix_HaltChannel(AMBIENCE_CHANNEL);
    currentAmbiance = "";
    currentAmbianceToReport = "";
    return true;
}

bool playPartnerAbilityLoop(const string &id)
{
    if (!audioEnabled) return false;

    Mix_HaltChannel(PARTNER_ABILITY_LOOP_CHANNEL);

    if (id.length() == 0)
    {
        return false;
    }

    Mix_Chunk *pSound = sfx[id];
    if (!pSound) return false;

    Mix_HaltChannel(PARTNER_ABILITY_LOOP_CHANNEL);
    setPartnerAbilityLoopVolume();
    return Mix_PlayChannel(PARTNER_ABILITY_LOOP_CHANNEL, pSound, -1) == PARTNER_ABILITY_LOOP_CHANNEL;
}

bool setPartnerAbilityLoopVolume()
{
    int setVol = (int)(soundVol * MIX_MAX_VOLUME);
    return Mix_Volume(PARTNER_ABILITY_LOOP_CHANNEL, setVol) > 0;
}

void stopPartnerAbilityLoop()
{
    if (!audioEnabled) return;

    Mix_HaltChannel(PARTNER_ABILITY_LOOP_CHANNEL);
}

bool playLoopingSound(const string &id, int relativeChannel)
{
    if (!audioEnabled) return false;
    if (relativeChannel >= NUM_SOUND_LOOP_CHANNELS) return false;

    Mix_HaltChannel(SOUND_LOOP_CHANNEL_START + relativeChannel);

    if (id.length() == 0)
    {
        return false;
    }

    Mix_Chunk *pSound = sfx[id];
    if (!pSound) return false;

    Mix_HaltChannel(SOUND_LOOP_CHANNEL_START + relativeChannel);
    return Mix_PlayChannel(SOUND_LOOP_CHANNEL_START + relativeChannel, pSound, -1) == SOUND_LOOP_CHANNEL_START + relativeChannel;
}

bool setLoopingSoundVolume(int relativeChannel, double volume)
{
    int setVol = (int)(soundVol * volume * MIX_MAX_VOLUME);
    return Mix_Volume(SOUND_LOOP_CHANNEL_START + relativeChannel, setVol) > 0;
}

void stopLoopingSounds()
{
    if (!audioEnabled) return;

    for (int i = 0; i < NUM_SOUND_LOOP_CHANNELS; i++)
    {
        Mix_HaltChannel(SOUND_LOOP_CHANNEL_START + i);
    }
}

bool playDialog(const string &id)
{
    if (!audioEnabled) return false;
    if (currentDialog.length() > 0) Mix_HaltChannel(DIALOG_CHANNEL);
    Mix_Chunk *pSound = dialog[id];
    if (!pSound) return false;
    currentDialog = id;
    if (Mix_PlayChannel(DIALOG_CHANNEL, pSound, 0) != DIALOG_CHANNEL)
    {
        currentDialog = "";
        return false;
    }
    return true;
}

bool stopDialog()
{
    if (!audioEnabled || currentDialog.length() == 0) return false;
    Mix_HaltChannel(DIALOG_CHANNEL);
    return true;
}

string getPlayingDialog()
{
    return currentDialog;
}

bool isAudioEnabled()
{
    return audioEnabled;
}

void setAudioEnabled(bool enabled)
{
    audioEnabled = enabled;
}

double setVolumeMusic(double vol)
{
    if (!audioEnabled) return musicVol;
    Mix_VolumeMusic((int)(vol * musicDialogReductionPercentage * fadeMultiplier * MIX_MAX_VOLUME));
    musicVol = vol;
    return vol;
}

double setMusicDialogReductionPercentage(double percentage)
{
    if (!audioEnabled) return musicDialogReductionPercentage;
    musicDialogReductionPercentage = percentage;
    setVolumeMusic(musicVol);
    return percentage;
}

double setVolumeSound(double vol)
{
    if (!audioEnabled) return soundVol;
    soundVol = vol;
    return vol;
}

double setVolumeAmbiance(double vol)
{
    if (!audioEnabled) return ambianceVol;
    Mix_Volume(AMBIENCE_CHANNEL, (int)(vol * ambianceDialogReductionPercentage * MIX_MAX_VOLUME));
    ambianceVol = vol;
    return vol;
}

double setAmbianceDialogReductionPercentage(double percentage)
{
    if (!audioEnabled) return ambianceDialogReductionPercentage;
    ambianceDialogReductionPercentage = percentage;
    setVolumeAmbiance(ambianceVol);
    return percentage;
}

double setVolumeDialog(double vol)
{
    if (!audioEnabled) return dialogVol;
    Mix_Volume(DIALOG_CHANNEL, (int)(vol * MIX_MAX_VOLUME));
    dialogVol = vol;
    return vol;
}

double getVolumeMusic()
{
    return musicVol;
}

double getVolumeSound()
{
    return soundVol;
}

double getVolumeAmbiance()
{
    return ambianceVol;
}

double getVolumeDialog()
{
    return dialogVol;
}

void quitAudio()
{
    if(audioEnabled)
    {
        //Stop any currently-playing music/dialog/sfx, free all resources, and kill the audio thread.
        stopMusic();
        stopAmbiance();
        stopDialog();
        Mix_HaltChannel(-1);
        for(map<string,Mix_Music*>::const_iterator iter = music.begin(); iter != music.end(); ++iter) Mix_FreeMusic(iter->second);
        music.clear();
        for(map<string,Mix_Chunk*>::const_iterator iter = dialog.begin(); iter != dialog.end(); ++iter) Mix_FreeChunk(iter->second);
        dialog.clear();
        for(map<string,Mix_Chunk*>::const_iterator iter = sfx.begin(); iter != sfx.end(); ++iter) Mix_FreeChunk(iter->second);
        sfx.clear();
        Mix_CloseAudio();
    }
}
