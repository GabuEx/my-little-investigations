/**
 * Basic header/include file for globals.cpp.
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

#ifndef GLOBALS_H
#define GLOBALS_H

#include "LocalizableContent.h"
#include "Version.h"

#include <SDL2/SDL.h>

#ifndef GAME_EXECUTABLE
extern "C"
{
    #include <curl/curl.h>
}
#endif

#include <vector>
#include <map>

using namespace std;

extern SDL_Window *gpWindow;
extern SDL_Renderer *gpRenderer;
extern Uint16 gScreenWidth;
extern Uint16 gScreenHeight;

extern int gMaxTextureWidth;
extern int gMaxTextureHeight;

#ifdef GAME_EXECUTABLE
extern bool gIsFullscreen;
extern double gScreenScale;
extern Uint16 gHorizontalOffset;
extern Uint16 gVerticalOffset;

extern int gTexturesRecreatedCount;

extern bool gIsSavingScreenshot;
extern Uint16 gScreenshotWidth;
extern Uint16 gScreenshotHeight;
#endif

extern double gFramerate;
extern string gTitle;

#ifdef GAME_EXECUTABLE
extern SDL_threadID gUiThreadId;

extern string gCaseFilePath;
extern string gSaveFilePath;

// Option menu items.
extern bool gEnableTutorials;
extern bool gEnableHints;
extern bool gEnableFullscreen;
extern bool gEnableSkippingUnseenDialog;
#ifdef ENABLE_DEBUG_MODE
extern bool gEnableDebugMode;
#endif
extern double gBackgroundMusicVolume;
extern double gSoundEffectsVolume;
extern double gVoiceVolume;
#endif

extern string gLocalizedResourcesFileName;

#ifdef GAME_EXECUTABLE
extern bool gEnableTutorialsDefault;
extern bool gEnableHintsDefault;
extern bool gEnableFullscreenDefault;
extern bool gEnableSkippingUnseenDialogDefault;
#ifdef ENABLE_DEBUG_MODE
extern bool gEnableDebugModeDefault;
#endif
extern double gBackgroundMusicVolumeDefault;
extern double gSoundEffectsVolumeDefault;
extern double gVoiceVolumeDefault;

extern vector<string> gCompletedCaseGuidList;
extern map<string, bool> gCaseIsSignedByFilePathMap;
extern vector<string> gDialogsSeenList;

extern bool gToggleFullscreen;
#else
extern CURL *gpCurlHandle;
#endif

extern bool gIsQuitting;

extern Version gVersion;

#ifdef UPDATER
extern string gVersionsXmlFilePath;
extern string gUpdateScriptFilePath;

class MLIFont;

extern MLIFont *gpUpdatingFont;
#endif

class LocalizableContent;
extern LocalizableContent *gpLocalizableContent;

void EnsureUIThread();

#endif
