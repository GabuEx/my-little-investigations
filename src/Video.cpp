/**
 * Represents a single video in the game.
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

#include "Video.h"
#include "globals.h"
#include "ResourceLoader.h"
#include "CaseInformation/Case.h"
#include "XmlReader.h"
#include <math.h>

#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(51,42,0)
#define AVPixelFormat PixelFormat
#define AV_PIX_FMT_YUVJ420P PIX_FMT_YUVJ420P
#define AV_PIX_FMT_YUV420P PIX_FMT_YUV420P
#define AV_PIX_FMT_YUV444P PIX_FMT_YUV444P
#define AV_PIX_FMT_YUVJ420P PIX_FMT_YUVJ420P
#define AV_PIX_FMT_BGRA PIX_FMT_BGRA
#define AV_PIX_FMT_ARGB PIX_FMT_ARGB
#endif
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

const string CommonFilesId = "CommonFiles";

bool IsYUVFormat(AVPixelFormat pixelFormat)
{
    return pixelFormat == AV_PIX_FMT_YUVJ420P || pixelFormat == AV_PIX_FMT_YUV420P || pixelFormat == AV_PIX_FMT_YUV444P;
}

Video::Video(bool shouldLoop)
{
    pCurFrame = NULL;
    curFrameIndex = 0;
    this->shouldLoop = shouldLoop;
    this->isReady = false;

    pRWOpsIOContext = NULL;
    pFormatContext = NULL;
    videoStream = -1;
    pCodecContext = NULL;
    pCodec = NULL;
    pFrame = NULL;
    pMemToFree = NULL;
    pImageConvertContext = NULL;
    pCachedTexturePixels = NULL;
    pTexture = NULL;

    this->texturesRecreatedCount = gTexturesRecreatedCount;
}

Video::Video(XmlReader *pReader)
{
    pCurFrame = NULL;
    curFrameIndex = 0;
    this->isReady = false;

    pRWOpsIOContext = NULL;
    pFormatContext = NULL;
    videoStream = -1;
    pCodecContext = NULL;
    pCodec = NULL;
    pFrame = NULL;
    pMemToFree = NULL;
    pImageConvertContext = NULL;
    pCachedTexturePixels = NULL;
    pTexture = NULL;

    this->texturesRecreatedCount = gTexturesRecreatedCount;

    pReader->StartElement("Video");
    id = pReader->ReadTextElement("Id");
    shouldLoop = pReader->ReadBooleanElement("ShouldLoop");
    videoRelativeFilePath = pReader->ReadTextElement("VideoRelativeFilePath");
    width = pReader->ReadIntElement("Width");
    height = pReader->ReadIntElement("Height");

    pReader->StartList("Frame");

    while (pReader->MoveToNextListItem())
    {
        frameList.push_back(new Video::Frame(pReader));
    }

    pReader->EndElement();
}

Video::~Video()
{
    UnloadFile();

    for (unsigned int i = 0; i < frameList.size(); i++)
    {
        delete frameList[i];
    }

    frameList.clear();
}

void Video::SetVideoAttributes(const string &videoRelativeFilePath, unsigned int frameCount, int msFrameDuration, unsigned int width, unsigned int height)
{
    this->videoRelativeFilePath = videoRelativeFilePath;
    this->width = width;
    this->height = height;

    for (unsigned int i = 0; i < frameCount; i++)
    {
        frameList.push_back(new Video::Frame(msFrameDuration, i));
    }
}

void Video::AddSound(unsigned int frameIndex, const string &sfxId)
{
    if (frameIndex >= frameList.size())
    {
        throw MLIException("Invalid frame index!");
    }

    frameList[frameIndex]->pSound = new SpecifiedSound(sfxId);
}

void Video::Begin()
{
    Reset();
    pCurFrame->Begin();
}

void Video::Update(int delta)
{
    if (IsFinished())
    {
        return;
    }

    pCurFrame->Update(delta);

    while (!IsFinished() && pCurFrame->GetIsFinished())
    {
        MoveToNextFrame();
    }
}

void Video::Draw(Vector2 position)
{
    Draw(position, false /* flipHorizontally */, Color::White);
}

void Video::Draw(Vector2 position, bool flipHorizontally)
{
    Draw(position, flipHorizontally, Color::White);
}

void Video::Draw(Vector2 position, Color color)
{
    Draw(position, false /* flipHorizontally */, color);
}

void Video::Draw(Vector2 position, bool flipHorizontally, Color color)
{
    Draw(position, flipHorizontally, color, RectangleWH(0, 0, width, height));
}

void Video::Draw(Vector2 position, bool flipHorizontally, Color color, RectangleWH clipRect)
{
    if (IsFinished())
    {
        return;
    }

    Image::Draw(pTexture, position, clipRect, flipHorizontally, false /* flipVertically */, 1.0 /* scale */, color);
}

void Video::Reset()
{
    bool shouldRewriteFrame = curFrameIndex > 0 || texturesRecreatedCount != gTexturesRecreatedCount;

    curFrameIndex = 0;
    pCurFrame = frameList[0];

    if (pFormatContext != NULL)
    {
        av_seek_frame(pFormatContext, videoStream, 0, AVSEEK_FLAG_ANY);

        if (shouldRewriteFrame)
        {
            WriteNextFrame();
        }
    }

    texturesRecreatedCount = gTexturesRecreatedCount;
}

void Video::Finish()
{
    unsigned int lastFrameIndex = curFrameIndex;

    while (!IsFinished() && !pCurFrame->GetIsForever())
    {
        MoveToNextFrame();

        // If we've looped all the way around, then we know that finishing is impossible,
        // so we'll exit.
        if (lastFrameIndex == curFrameIndex)
        {
            break;
        }
    }
}

void Video::LoadFile()
{
    if (!isReady)
    {
        ResourceLoader::GetInstance()->LoadVideo(
            videoRelativeFilePath,
            &pRWOpsIOContext,
            &pFormatContext,
            &videoStream,
            &pCodecContext,
            &pCodec,
            &pMemToFree);

        pFrame = av_frame_alloc();

        pImageConvertContext =
            sws_getContext(
                width,
                height,
                pCodecContext->pix_fmt == AV_PIX_FMT_BGRA ? AV_PIX_FMT_ARGB : pCodecContext->pix_fmt,
                width,
                height,
                IsYUVFormat(pCodecContext->pix_fmt) ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_ARGB,
                SWS_BICUBIC,
                NULL,
                NULL,
                NULL);

        pTexture =
            SDL_CreateTexture(
                gpRenderer,
                IsYUVFormat(pCodecContext->pix_fmt) ? SDL_PIXELFORMAT_YV12 : SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height);
        SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);

        pCachedTexturePixels = new unsigned char[width * height * 4];

        WriteNextFrame();

        isReady = true;
    }
}

void Video::UnloadFile()
{
    if (isReady)
    {
        isReady = false;

        delete[] pCachedTexturePixels;
        pCachedTexturePixels = NULL;
        SDL_DestroyTexture(pTexture);
        pTexture = NULL;
        sws_freeContext(pImageConvertContext);
        pImageConvertContext = NULL;
        av_frame_free(&pFrame);
        avcodec_close(pCodecContext);
        pCodecContext = NULL;
        avformat_close_input(&pFormatContext);
        delete pRWOpsIOContext;
        pRWOpsIOContext = NULL;
        free(pMemToFree);
        pMemToFree = NULL;
    }
}

bool Video::IsReady()
{
    return pCurFrame != NULL;
}

bool Video::IsFinished() const
{
    return curFrameIndex == frameList.size();
}

void Video::UpdateReadiness(const string &newLocationId, bool *pLoadFrames, bool *pDeleteFrames)
{
    vector<string> parentLocationList = Case::GetInstance()->GetParentLocationListForVideoId(id);
    bool isNeeded = false;

    *pLoadFrames = false;
    *pDeleteFrames = false;

    for (unsigned int i = 0; i < parentLocationList.size(); i++)
    {
        if (parentLocationList[i] == newLocationId ||
            parentLocationList[i] == CommonFilesId ||
            parentLocationList[i] == Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId())
        {
            isNeeded = true;
        }
    }

    bool isReady = IsAnimationReady();

    if (isNeeded && !isReady)
    {
        *pLoadFrames = true;
    }
    else if (!isNeeded && isReady)
    {
        *pDeleteFrames = true;
    }
}

bool Video::IsAnimationReady()
{
    return isReady;
}

Video::Frame::Frame(int msDuration, unsigned int frameIndex)
{
    this->msDuration = msDuration;
    this->frameIndex = frameIndex;

    elapsedDuration = 0;
    pSound = NULL;
    wasSoundPlayed = false;
}

Video::Frame::Frame(XmlReader *pReader)
{
    elapsedDuration = 0;
    pSound = NULL;
    wasSoundPlayed = false;

    this->msDuration = pReader->ReadIntElement("MsDuration");

    if (pReader->ElementExists("Sound"))
    {
        pReader->StartElement("Sound");
        pSound = AnimationSound::LoadSoundFromXml(pReader);
        pReader->EndElement();
    }
}

Video::Frame::~Frame()
{
    delete pSound;
    pSound = NULL;
}

double Video::Frame::GetOverflowDuration() const
{
    double overflowDuration = elapsedDuration - msDuration;

    if (overflowDuration > 0)
    {
        return overflowDuration;
    }
    else
    {
        return 0;
    }
}

bool Video::Frame::GetIsFinished() const
{
    return msDuration > 0 && elapsedDuration >= msDuration;
}

void Video::Frame::Begin()
{
    Begin(0);
}

void Video::Frame::Begin(double overflowDuration)
{
    elapsedDuration = overflowDuration;
    wasSoundPlayed = false;
}

void Video::Frame::Update(int delta)
{
    AnimationSound *pSoundToPlay = GetSoundToPlay();

    if (pSoundToPlay != NULL)
    {
        pSoundToPlay->Play(gSoundEffectsVolume);
    }

    elapsedDuration += delta;
}

AnimationSound * Video::Frame::GetSoundToPlay()
{
    AnimationSound *pSoundToPlay = NULL;

    if (!wasSoundPlayed && pSound != NULL)
    {
        wasSoundPlayed = true;
        pSoundToPlay = pSound;
    }

    return pSoundToPlay;
}

AnimationSound * Video::GetSoundToPlay()
{
    return pCurFrame != NULL ? pCurFrame->GetSoundToPlay() : NULL;
}

void Video::MoveToNextFrame()
{
    double overflowDuration = pCurFrame->GetOverflowDuration();
    curFrameIndex++;

    if (IsFinished() && shouldLoop)
    {
        curFrameIndex = 0;
        av_seek_frame(pFormatContext, videoStream, 0, AVSEEK_FLAG_ANY);
    }

    if (!IsFinished())
    {
        pCurFrame = frameList[curFrameIndex];
        pCurFrame->Begin(overflowDuration);

        WriteNextFrame();
    }
}

void Video::WriteNextFrame()
{
    int frameFinished = 0;
    AVPacket packet;

    if (texturesRecreatedCount != gTexturesRecreatedCount)
    {
        texturesRecreatedCount = gTexturesRecreatedCount;

        SDL_DestroyTexture(pTexture);
        pTexture =
            SDL_CreateTexture(
                gpRenderer,
                IsYUVFormat(pCodecContext->pix_fmt) ? SDL_PIXELFORMAT_YV12 : SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height);
        SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);

        unsigned char *pPixels = NULL;
        int pitch = 0;

        SDL_LockTexture(pTexture, NULL, reinterpret_cast<void **>(&pPixels), &pitch);
        memcpy(pPixels, pCachedTexturePixels, pitch * height);
        SDL_UnlockTexture(pTexture);
    }

    while (!frameFinished)
    {
        if (av_read_frame(pFormatContext, &packet) != 0)
        {
            break;
        }

        if (packet.stream_index == videoStream && avcodec_decode_video2(pCodecContext, pFrame, &frameFinished, &packet) >= 0)
        {
            if (frameFinished)
            {
                AVPicture picture;
                unsigned char *pPixels = NULL;
                int pitch = 0;

                SDL_LockTexture(pTexture, NULL, reinterpret_cast<void **>(&pPixels), &pitch);

                if (IsYUVFormat(pCodecContext->pix_fmt))
                {
                    picture.data[0] = pFrame->data[0];
                    picture.data[1] = pFrame->data[1];
                    picture.data[2] = pFrame->data[2];
                    picture.linesize[0] = pFrame->linesize[0];
                    picture.linesize[1] = pFrame->linesize[1];
                    picture.linesize[2] = pFrame->linesize[2];
                }
                else
                {
                    picture.data[0] = pPixels;
                    picture.linesize[0] = pitch;
                }

                sws_scale(pImageConvertContext, pFrame->data, pFrame->linesize, 0, pFrame->height, picture.data, picture.linesize);

                if (IsYUVFormat(pCodecContext->pix_fmt))
                {
                    if (pitch == picture.linesize[0])
                    {
                        int size = pitch * pFrame->height;

                        memcpy(pPixels, picture.data[0], size);
                        memcpy(pPixels + size, picture.data[2], size / 4);
                        memcpy(pPixels + size * 5 / 4, picture.data[1], size / 4);
                    }
                    else
                    {
                        unsigned char *y1, *y2, *y3, *i1, *i2, *i3;
                        y1 = pPixels;
                        y3 = pPixels + pitch * pFrame->height;
                        y2 = pPixels + pitch * pFrame->height * 5 / 4;

                        i1 = picture.data[0];
                        i2 = picture.data[1];
                        i3 = picture.data[2];

                        for (int i = 0; i < pFrame->height / 2; i++)
                        {
                            memcpy(y1, i1, pitch);
                            i1 += picture.linesize[0];
                            y1 += pitch;
                            memcpy(y1, i1, pitch);

                            memcpy(y2, i2, pitch / 2);
                            memcpy(y3, i3, pitch / 2);

                            y1 += pitch;
                            y2 += pitch / 2;
                            y3 += pitch / 2;
                            i1 += picture.linesize[0];
                            i2 += picture.linesize[1];
                            i3 += picture.linesize[2];
                        }
                    }
                }

                memcpy(pCachedTexturePixels, pPixels, pitch * height);
                SDL_UnlockTexture(pTexture);
            }
        }

        av_free_packet(&packet);
    }
}
