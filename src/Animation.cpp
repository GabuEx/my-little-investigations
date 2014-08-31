/**
 * Represents a single animation in the game.
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

#include "Animation.h"
#include "CaseInformation/Case.h"
#include "CaseInformation/CommonCaseResources.h"
#include "XmlReader.h"

Animation::Animation(XmlReader *pReader, ManagerSource managerSource)
{
    pCurFrame = NULL;
    curFrameIndex = -1;
    this->managerSource = managerSource;

    pReader->StartElement("Animation");

    pReader->StartList("Frame");

    while (pReader->MoveToNextListItem())
    {
        Frame *pFrame = new Frame(pReader);
        pFrame->SetManagerSource(managerSource);
        frameList.push_back(pFrame);
    }

    pReader->EndElement();
}

Animation::~Animation()
{
    for (unsigned int i = 0; i < frameList.size(); i++)
    {
        delete frameList[i];
    }
}

void Animation::AddFrame(int msDuration, const string &spriteId)
{
    Frame *pFrame = new Frame(msDuration, spriteId);
    pFrame->SetManagerSource(managerSource);
    frameList.push_back(pFrame);
}

void Animation::AddSound(const string &sfxId)
{
    frameList.back()->pSound = new SpecifiedSound(sfxId);
}

Vector2 Animation::GetSize()
{
    // To get the size of the animation, we'll just get the size of the first frame.
    // If other frames are different sizes, then the concept of a size for the animation itself
    // is meaningless, so we don't really care about that part.
    Sprite *pFirstFrameSprite = frameList[0]->GetSprite();
    return Vector2(pFirstFrameSprite->GetWidth(), pFirstFrameSprite->GetHeight());
}

void Animation::Begin()
{
    Reset();
    pCurFrame->Begin();
}

void Animation::Update(int delta)
{
    pCurFrame->Update(delta);

    if (pCurFrame->GetIsFinished())
    {
        double overflowDuration = pCurFrame->GetOverflowDuration();
        curFrameIndex++;

        if (curFrameIndex == frameList.size())
        {
            curFrameIndex = 0;
        }

        pCurFrame = frameList[curFrameIndex];
        pCurFrame->Begin(overflowDuration);
    }
}

void Animation::Draw(Vector2 position)
{
    pCurFrame->Draw(position, false /* flipHorizontally */, 1.0 /* scale */);
}

void Animation::Draw(Vector2 position, bool flipHorizontally, double scale)
{
    pCurFrame->Draw(position, flipHorizontally, scale);
}

void Animation::Draw(Vector2 position, Color color)
{
    pCurFrame->Draw(position, color);
}

void Animation::Reset()
{
    curFrameIndex = 0;
    pCurFrame = frameList[0];
}

void Animation::Finish()
{
    while (curFrameIndex < frameList.size() && !frameList[curFrameIndex]->GetIsForever())
    {
        curFrameIndex++;
    }
}

Animation * Animation::Clone()
{
    Animation *pCloneAnimation = new Animation(managerSource);

    for (unsigned int i = 0; i < frameList.size(); i++)
    {
        pCloneAnimation->frameList[i] = new Animation::Frame();

        pCloneAnimation->frameList[i]->msDuration = frameList[i]->msDuration;
        pCloneAnimation->frameList[i]->spriteId = frameList[i]->spriteId;
        pCloneAnimation->frameList[i]->pSprite = frameList[i]->pSprite;
        pCloneAnimation->frameList[i]->pSound = frameList[i]->pSound->Clone();

        pCloneAnimation->frameList[i]->elapsedDuration = frameList[i]->elapsedDuration;
    }

    pCloneAnimation->pCurFrame = frameList[curFrameIndex];
    pCloneAnimation->curFrameIndex = curFrameIndex;

    return pCloneAnimation;
}

bool Animation::IsReady()
{
    bool isReady = true;

    for (unsigned int i = 0; i < frameList.size(); i++)
    {
        if (!frameList[i]->GetSprite()->IsReady())
        {
            isReady = false;
            break;
        }
    }

    return isReady;
}

bool Animation::IsStarted()
{
    return pCurFrame != NULL;
}

bool Animation::IsFinished()
{
    return pCurFrame != NULL && pCurFrame->GetIsForever();
}

Sprite * Animation::GetFrameSprite()
{
    return pCurFrame != NULL ? pCurFrame->GetSprite() : NULL;
}

Animation::Frame::Frame(XmlReader *pReader)
{
    pSprite = NULL;
    pSound = NULL;
    wasSoundPlayed = false;

    msDuration = pReader->ReadIntElement("MsDuration");
    spriteId = pReader->ReadTextElement("SpriteId");

    if (pReader->ElementExists("Sound"))
    {
        pReader->StartElement("Sound");
        pSound = AnimationSound::LoadSoundFromXml(pReader);
        pReader->EndElement();
    }
}

Animation::Frame::~Frame()
{
    delete pSound;
    pSound = NULL;
}

double Animation::Frame::GetOverflowDuration() const
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

bool Animation::Frame::GetIsFinished() const
{
    return msDuration > 0 && elapsedDuration >= msDuration;
}

void Animation::Frame::Begin()
{
    Begin(0);
}

void Animation::Frame::Begin(double overflowDuration)
{
    elapsedDuration = overflowDuration;
    wasSoundPlayed = false;
}

void Animation::Frame::Update(int delta)
{
    elapsedDuration += delta;
}

void Animation::Frame::Draw(Vector2 position, bool flipHorizontally, double scale)
{
    GetSprite()->Draw(position, Color::White, scale, flipHorizontally);
}

void Animation::Frame::Draw(Vector2 position, Color color)
{
    GetSprite()->Draw(position, color);
}

AnimationSound * Animation::Frame::GetSoundToPlay()
{
    AnimationSound *pSoundToPlay = NULL;

    if (!wasSoundPlayed && pSound != NULL)
    {
        wasSoundPlayed = true;
        pSoundToPlay = pSound;
    }

    return pSoundToPlay;
}

Sprite * Animation::Frame::GetSprite()
{
    if (pSprite == NULL)
    {
        SpriteManager *pSpriteManager = NULL;

        switch (managerSource)
        {
            case ManagerSourceCaseFile:
                pSpriteManager = Case::GetInstance()->GetSpriteManager();
                break;

            case ManagerSourceCommonResources:
                pSpriteManager = CommonCaseResources::GetInstance()->GetSpriteManager();
                break;
        }

        pSprite = pSpriteManager->GetSpriteFromId(spriteId);
    }

    return pSprite;
}

AnimationSound * Animation::GetSoundToPlay()
{
    return pCurFrame != NULL ? pCurFrame->GetSoundToPlay() : NULL;
}
