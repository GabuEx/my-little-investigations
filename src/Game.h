/**
 * Basic header/include file for Game.cpp.
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

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include "Image.h"
#include "Screens/MLIScreen.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

class Game
{
public:
    static bool CreateAndInit();
    static Game * GetInstance() { return pInstance; }

#ifdef GAME_EXECUTABLE
    void PrepareGameMode();
    void PrepareMenuMode();
    void SetOverlayById(const string &overlayId);
#endif

    void Update(int delta);
    void Draw();
    bool GetIsFinished() const { return isFinished; }

#ifdef GAME_EXECUTABLE
    bool GetShowCursor();
#endif

    static void Finish();

private:
    Game()
    {
        pCurrentScreen = NULL;
    #ifdef GAME_EXECUTABLE
        pOverlayScreen = NULL;
    #endif
    }

    ~Game();

    void Init();

    static Game *pInstance;

    map<string, MLIScreen *> screenFromIdMap;
    MLIScreen *pCurrentScreen;

#ifdef GAME_EXECUTABLE
    MLIScreen *pOverlayScreen;
#endif

    bool isFinished;
};

#endif
