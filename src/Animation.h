/**
 * Basic header/include file for Animation.cpp.
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

#ifndef ANIMATION_H
#define ANIMATION_H

#include "AnimationSound.h"
#include "Sprite.h"
#include "Vector2.h"
#include <vector>

class XmlReader;

class Animation
{
public:
    Animation(ManagerSource managerSource)
    {
        pCurFrame = NULL;
        curFrameIndex = -1;
        this->managerSource = managerSource;
    }

    Animation(XmlReader *pReader, ManagerSource managerSource);
    ~Animation();

    void SetManagerSource(ManagerSource managerSource) { this->managerSource = managerSource; }

    void AddFrame(int msDuration, const string &spriteId);
    void AddSound(const string &sfxId);
    Vector2 GetSize();

    void Begin();
    void Update(int delta);
    void Draw(Vector2 position);
    void Draw(Vector2 position, bool flipHorizontally, double scale);
    void Draw(Vector2 position, Color color);
    void Reset();
    void Finish();

    Animation * Clone();

    bool IsReady();
    bool IsStarted();
    bool IsFinished();

    Sprite * GetFrameSprite();

    class Frame
    {
        friend class Animation;

    public:
        Frame()
            : msDuration(0)
            , spriteId("")
            , pSprite(NULL)
            , pSound(NULL)
            , wasSoundPlayed(false)
            , elapsedDuration(0)
            , managerSource(ManagerSourceCommonResources)
        {
        }

        Frame(int msDuration, const string &spriteId)
            : msDuration(msDuration)
            , spriteId(spriteId)
            , pSprite(NULL)
            , pSound(NULL)
            , wasSoundPlayed(false)
            , elapsedDuration(0)
            , managerSource(ManagerSourceCommonResources)
        {
        }

        Frame(XmlReader *pReader);
        ~Frame();

        void SetManagerSource(ManagerSource managerSource) { this->managerSource = managerSource; }

        double GetOverflowDuration() const;
        bool GetIsFinished() const;
        void Begin();
        void Begin(double overflowDuration);
        void Update(int delta);
        void Draw(Vector2 position, bool flipHorizontally, double scale);
        void Draw(Vector2 position, Color color);

        bool GetIsForever() const { return msDuration == 0; }

        AnimationSound * GetSoundToPlay();

    private:
        Sprite * GetSprite();

        int msDuration;
        string spriteId;
        Sprite *pSprite;
        AnimationSound *pSound;
        bool wasSoundPlayed;

        double elapsedDuration;

        ManagerSource managerSource;
    };

    AnimationSound * GetSoundToPlay();

private:
    vector<Frame *> frameList;
    Frame *pCurFrame;
    unsigned int curFrameIndex;

    ManagerSource managerSource;
};

#endif
