/**
 * Provides global variables for use in the game's source code.
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

#include "globals.h"

#ifdef GAME_EXECUTABLE
#include "MLIException.h"
#include <SDL2/SDL.h>
#endif

#ifdef UPDATER
#include "MLIFont.h"
#endif

SDL_Window *gpWindow = NULL;
SDL_Renderer *gpRenderer = NULL;
Uint16 gScreenWidth = 0;
Uint16 gScreenHeight = 0;

int gMaxTextureWidth = 0;
int gMaxTextureHeight = 0;

#ifdef GAME_EXECUTABLE
bool gIsFullscreen = false;
double gScreenScale = 0.0;
Uint16 gHorizontalOffset = 0;
Uint16 gVerticalOffset = 0;

int gTexturesRecreatedCount = 0;

bool gIsSavingScreenshot = false;
Uint16 gScreenshotWidth = 0;
Uint16 gScreenshotHeight = 0;
#endif

double gFramerate = 0.0;
string gTitle = "";

#ifdef GAME_EXECUTABLE
SDL_threadID gUiThreadId = 0;

string gCaseFilePath = "";
string gSaveFilePath = "";

bool gEnableTutorials = true;
bool gEnableHints = true;
bool gEnableFullscreen = false;
bool gEnableSkippingUnseenDialog = false;
#ifdef ENABLE_DEBUG_MODE
bool gEnableDebugMode = false;
#endif
double gBackgroundMusicVolume = 0.2;
double gSoundEffectsVolume = 0.67;
double gVoiceVolume = 0.5;
#endif

string gLocalizedResourcesFileName = "common_en-US.dat";

#ifdef GAME_EXECUTABLE
bool gEnableTutorialsDefault = gEnableTutorials;
bool gEnableHintsDefault = gEnableHints;
bool gEnableFullscreenDefault = gEnableFullscreen;
bool gEnableSkippingUnseenDialogDefault = gEnableSkippingUnseenDialog;
#ifdef ENABLE_DEBUG_MODE
bool gEnableDebugModeDefault = gEnableDebugMode;
#endif
double gBackgroundMusicVolumeDefault = gBackgroundMusicVolume;
double gSoundEffectsVolumeDefault = gSoundEffectsVolume;
double gVoiceVolumeDefault = gVoiceVolume;

vector<string> gCompletedCaseGuidList;
map<string, bool> gCaseIsSignedByFilePathMap;
vector<string> gDialogsSeenList;

bool gToggleFullscreen = false;
#else
CURL *gpCurlHandle = NULL;
#endif

bool gIsQuitting = false;

Version gVersion(1, 0, 0);

#ifdef UPDATER
string gVersionsXmlFilePath = "";
string gUpdateScriptFilePath = "";
MLIFont *gpUpdatingFont = NULL;
#endif

LocalizableContent *gpLocalizableContent = NULL;

void EnsureUIThread()
{
#ifdef GAME_EXECUTABLE
    SDL_threadID currentThreadId = SDL_ThreadID();

    if (currentThreadId != gUiThreadId)
    {
        ThrowException("This method can only be called on the UI thread.");
    }
#endif
}
