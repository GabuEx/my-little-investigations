/**
 * Basic header/include file for Video.cpp.
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

#ifndef VIDEO_H
#define VIDEO_H

#include "AnimationSound.h"
#include "Color.h"
#include "Rectangle.h"
#include "Vector2.h"
#include "SDL2/SDL.h"
#include <vector>

class XmlReader;

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

class Image;
class RWOpsIOContext;

class Video
{
public:
    Video(bool shouldLoop);
    Video(XmlReader *pReader);
    ~Video();

    void SetVideoAttributes(const string &videoRelativeFilePath, unsigned int frameCount, int msFrameDuration, unsigned int width, unsigned int height);
    void AddSound(unsigned int frameIndex, const string &sfxId);

    unsigned int GetWidth() { return this->width; }
    unsigned int GetHeight() { return this->height; }

    void Begin();
    void Update(int delta);
    void Draw(Vector2 position);
    void Draw(Vector2 position, bool flipHorizontally);
    void Draw(Vector2 position, Color color);
    void Draw(Vector2 position, bool flipHorizontally, Color color);
    void Draw(Vector2 position, bool flipHorizontally, Color color, RectangleWH clipRect);
    void Reset();
    void Finish();

    void LoadFile();
    void UnloadFile();
    bool IsReady();
    bool IsFinished() const;

    void UpdateReadiness(const string &newLocationId, bool *pLoadFile, bool *pDeleteFile);
    bool IsAnimationReady();

    class Frame
    {
        friend class Video;

    public:
        Frame(int msDuration, unsigned int frameIndex);
        Frame(XmlReader *pReader);
        ~Frame();

        double GetOverflowDuration() const;
        bool GetIsFinished() const;
        void Begin();
        void Begin(double overflowDuration);
        void Update(int delta);

        bool GetIsForever() const { return msDuration == 0; }

        AnimationSound * GetSoundToPlay();

    private:
        int msDuration;
        unsigned int frameIndex;

        AnimationSound *pSound;
        bool wasSoundPlayed;

        double elapsedDuration;
    };

    AnimationSound * GetSoundToPlay();

private:
    void MoveToNextFrame();
    void WriteNextFrame();

    string id;
    string videoRelativeFilePath;
    unsigned int width;
    unsigned int height;
    bool isReady;

    vector<Frame *> frameList;
    Frame *pCurFrame;
    unsigned int curFrameIndex;

    bool shouldLoop;

    RWOpsIOContext *pRWOpsIOContext;
    AVFormatContext *pFormatContext;
    int videoStream;
    AVCodecContext *pCodecContext;
    AVCodec *pCodec;
    AVFrame *pFrame;
    void *pMemToFree;
    SwsContext *pImageConvertContext;

    unsigned char *pCachedTexturePixels;
    SDL_Texture *pTexture;

    int texturesRecreatedCount;
};

#endif
