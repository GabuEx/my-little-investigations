/**
 * Basic header/include file for DialogCutsceneManager.cpp.
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

#ifndef DIALOGCUTSCENEMANAGER_H
#define DIALOGCUTSCENEMANAGER_H

#include "../Color.h"
#include "../EasingFunctions.h"
#include "../Vector2.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

class Sprite;
class DialogCutsceneAnimation;
class DialogCutsceneAnimationElement;
class DialogCutsceneAnimationFrame;
class XmlReader;

class DialogCutsceneManager
{
public:
    DialogCutsceneManager() {}
    ~DialogCutsceneManager();

    DialogCutsceneAnimation * GetAnimationFromId(const string &id)
    {
        return idToAnimationMap[id];
    }

    DialogCutsceneAnimationElement * GetElementFromId(const string &id)
    {
        return idToElementMap[id];
    }

    void LoadFromXml(XmlReader *pReader);

private:
    map<string, DialogCutsceneAnimation *> idToAnimationMap;
    map<string, DialogCutsceneAnimationElement *> idToElementMap;
};

class DialogCutsceneAnimation
{
public:
    DialogCutsceneAnimation()
    {
        pBackgroundSprite = NULL;
        frameIndex = 0;
        pReplacementBackgroundSprite = NULL;
        pReplacementBackgroundSpriteOpacityEase = NULL;
        replacementBackgroundSpriteOpacity = 0;
        changeBackgroundCount = 0;
        isFinished = false;
    }

    DialogCutsceneAnimation(XmlReader *pReader);
    ~DialogCutsceneAnimation();

    string GetId() const { return this->id; }
    void setId(const string &id) { this->id = id; }

    int GetChangeBackgroundCount() const { return this->changeBackgroundCount; }
    void IncrementChangeBackgroundCount() { this->changeBackgroundCount++; }

    string GetBackgroundSpriteId() const { return this->backgroundSpriteId; }
    void SetBackgroundSpriteId(const string &backgroundSpriteId) { this->backgroundSpriteId = backgroundSpriteId; }

    Sprite * GetBackgroundSprite();
    vector<DialogCutsceneAnimationElement *> * GetElementList();

    bool GetIsFinished() const { return this->isFinished; }
    void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

    void Begin();
    void Update(int delta);
    void Draw();
    void Reset();
    void FinishCurrentFrame();
    void MoveToNextFrame();

private:
    Sprite *pBackgroundSprite;
    vector<string> elementIdList;
    vector<DialogCutsceneAnimationElement *> elementList;
    vector<DialogCutsceneAnimationFrame> frameList;
    unsigned int frameIndex;

    Sprite *pReplacementBackgroundSprite;
    double replacementBackgroundSpriteOpacity;
    EasingFunction *pReplacementBackgroundSpriteOpacityEase;

    string id;

    unsigned int changeBackgroundCount;
    string backgroundSpriteId;
    bool isFinished;
};

class DialogCutsceneAnimationElement
{
public:
    DialogCutsceneAnimationElement()
        : spriteId("")
    {
        pSprite = NULL;

        originalXPosition = 0;
        originalYPosition = 0;

        originalATint = 0;
        originalRTint = 0;
        originalGTint = 0;
        originalBTint = 0;

        pXPositionEase = NULL;
        pYPositionEase = NULL;

        pATintEase = NULL;
        pRTintEase = NULL;
        pGTintEase = NULL;
        pBTintEase = NULL;

        xPosition = 0;
        yPosition = 0;

        aTint = 0;
        rTint = 0;
        gTint = 0;
        bTint = 0;
    }

    DialogCutsceneAnimationElement(const string &spriteId, int x, int y, int a, int r, int g, int b);
    DialogCutsceneAnimationElement(XmlReader *pReader);

    Sprite * GetSprite();

    void Update(int delta);
    void Draw();
    void Finish();

    void SetTargetPosition(int x, int y, int msDuration);
    void SetTargetTint(int a, int r, int g, int b, int msDuration);
    void Reset();
    void ClearEasingFunctions();

private:
    string spriteId;
    Sprite *pSprite;

    int originalXPosition;
    int originalYPosition;

    int originalATint;
    int originalRTint;
    int originalGTint;
    int originalBTint;

    EasingFunction *pXPositionEase;
    EasingFunction *pYPositionEase;

    EasingFunction *pATintEase;
    EasingFunction *pRTintEase;
    EasingFunction *pGTintEase;
    EasingFunction *pBTintEase;

    int xPosition;
    int yPosition;

    int aTint;
    int rTint;
    int gTint;
    int bTint;
};

class DialogCutsceneAnimationFrame
{
public:
    DialogCutsceneAnimationFrame(XmlReader *pReader);

    int GetMsDuration() const { return this->msDuration; }
    string GetNewBackgroundSpriteId() const { return this->newBackgroundSpriteId; }

    void Begin();

private:
    DialogCutsceneAnimationFrame()
        : msDuration(0)
        , newBackgroundSpriteId("")
    {
    }

    int msDuration;
    string newBackgroundSpriteId;
    map<string, Vector2> positionChangeHashMap;
    map<string, Color> tintChangeHashMap;
};

#endif
