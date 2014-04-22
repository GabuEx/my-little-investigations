/**
 * Basic header/include file for GameScreen.cpp.
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

#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include "MLIScreen.h"
#include "../CaseContent/Area.h"
#include "../Events/CaseParsingEventProvider.h"

class GameScreen : public MLIScreen, public CaseParsingEventListener
{
private:
    class LoadCaseParameters
    {
    public:
        LoadCaseParameters(const string &caseFilePath)
        {
            this->caseFilePath = caseFilePath;
        }

        string GetCaseFilePath() { return this->caseFilePath; }

    private:
        string caseFilePath;
    };

public:
    GameScreen();
    virtual ~GameScreen();

    void LoadResources();
    void UnloadResources();
    virtual void Init();
    static int LoadCaseStatic(void *pData);
    static int UnloadCaseStatic(void *pData);
    void Update(int delta);
    void UpdateAudio(int delta);
    void Draw();
    void OnCaseParsingComplete(const string &caseFileName);

private:
    int numLoadingDots;
    int timeSinceLastLoadingDotsUpdate;

    bool caseIsReady;
    bool caseNeedsReset;
    bool startedLoadingResources;
    bool isFinishing;

    Video *pConfrontationEntranceBackgroundVideo;
    Video *pConfrontationEntranceVfxVideo;

    Video *pSpeedLinesVideo;
};

#endif
