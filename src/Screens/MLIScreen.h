/**
 * Abstract base class for all the screens in the game.
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

#ifndef SCREEN_H
#define SCREEN_H

#include <string>

using namespace std;

class MLIScreen
{
public:
    static string GetCurrentScreenId() { return MLIScreen::currentScreenId; }
    static void SetCurrentScreenId(const string &currentScreenId) { MLIScreen::currentScreenId = currentScreenId; }

    static string GetLastScreenId() { return MLIScreen::lastScreenId; }
    static void SetLastScreenId(const string &lastScreenId) { MLIScreen::lastScreenId = lastScreenId; }

    virtual ~MLIScreen() {}

    bool GetIsFinished() const { return this->isFinished; }
    string GetNextScreenId() const { return this->nextScreenId; }

    virtual void LoadResources() { }
    virtual void UnloadResources() { }

    virtual void Init()
    {
        isFinished = false;
        nextScreenId = "";
    }

    virtual void Update(int delta) = 0;
    virtual void UpdateAudio(int delta) { }
    virtual void Draw() = 0;

    virtual bool GetShowCursor() { return true; }

protected:
    static string currentScreenId;
    static string lastScreenId;

    bool isFinished;
    string nextScreenId;
};

#ifdef GAME_EXECUTABLE
#define LOGO_SCREEN_ID "LogoScreen"
#define TITLE_SCREEN_ID "TitleScreen"
#define GAME_SCREEN_ID "GameScreen"
#define OPTIONS_SCREEN_ID "OptionsScreen"
#define LANGUAGE_SCREEN_ID "LanguageScreen"
#define CASE_SELECTION_SCREEN_ID "CaseSelectionScreen"
#define LOAD_SCREEN_ID "LoadScreen"
#define SAVE_SCREEN_ID "SaveScreen"
#endif
#ifdef UPDATER
#define CHECK_FOR_UPDATES_SCREEN_ID "CheckForUpdatesScreen"
#endif

#endif
