/**
 * Basic header/include file for mli_audio.cpp.
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

#ifndef MLI_AUDIO_H
#define MLI_AUDIO_H

#include <string>
#include <SDL2/SDL.h>
#ifdef __OSX
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#include <SDL2/SDL_thread.h>

using namespace std;

void initAudio();
void channelDone(int channel);

void musicToPartB();
bool preloadMusic(const string &id, SDL_RWops *pFileOpsA, SDL_RWops *pFileOpsB);
void unloadMusic(const string &id);
bool preloadSound(const string &id, SDL_RWops *pFileOps);
void unloadSound(const string &id);
bool preloadDialog(const string &id, SDL_RWops *pFileOps);
void unloadDialog(const string &id);
bool playMusic(const string &id);
string getPlayingMusic();
void clearPlayingMusic();
bool stopMusic();
bool pauseMusic();
bool resumeMusic();
bool playSound(const string &id);
bool playSound(const string &id, double volume);
bool playAmbiance(const string &id);
string getPlayingAmbiance();
void clearPlayingAmbiance();
bool stopAmbiance();
bool playPartnerAbilityLoop(const string &id);
bool setPartnerAbilityLoopVolume();
void stopPartnerAbilityLoop();
bool playLoopingSound(const string &id, int relativeChannel);
bool setLoopingSoundVolume(int relativeChannel, double volume);
void stopLoopingSounds();
bool playDialog(const string &id);
bool stopDialog();
string getPlayingDialog();
bool isAudioEnabled();
void setAudioEnabled(bool enabled);
double setVolumeMusic(double vol);
double setMusicDialogReductionPercentage(double percentage);
double setVolumeSound(double vol);
double setVolumeAmbiance(double vol);
double setAmbianceDialogReductionPercentage(double percentage);
double setVolumeDialog(double vol);
double getVolumeMusic();
double getVolumeSound();
double getVolumeAmbiance();
double getVolumeDialog();
void quitAudio();

#endif
