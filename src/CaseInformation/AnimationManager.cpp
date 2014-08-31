/**
 * Manager for all of the animations in the game; provides their retrieval.
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

#include "AnimationManager.h"
#include "Case.h"
#include "../ResourceLoader.h"
#include "../XmlReader.h"

AnimationManager::~AnimationManager()
{
    for (map<string, Animation *>::iterator iter = animationByIdMap.begin(); iter != animationByIdMap.end(); ++iter)
    {
        delete iter->second;
    }

    for (map<string, Video *>::iterator iter = videoByIdMap.begin(); iter != videoByIdMap.end(); ++iter)
    {
        delete iter->second;
    }
}

void AnimationManager::AddAnimation(const string &animationId, Animation **ppAnimation)
{
    Animation *pAnimation = new Animation(managerSource);
    animationByIdMap[animationId] = pAnimation;

    *ppAnimation = pAnimation;
}

void AnimationManager::AddVideo(const string &videoId, Video **ppVideo, bool shouldLoop)
{
    Video *pVideo = new Video(shouldLoop);
    videoByIdMap[videoId] = pVideo;

    *ppVideo = pVideo;
}

void AnimationManager::DeleteAnimation(const string &animationId)
{
    Animation *pAnimation = animationByIdMap[animationId];
    animationByIdMap.erase(animationId);
    delete pAnimation;
}

void AnimationManager::DeleteVideo(const string &videoId)
{
    Video *pVideo = videoByIdMap[videoId];
    videoByIdMap.erase(videoId);
    delete pVideo;
}

Animation * AnimationManager::GetAnimationFromId(const string &animationId)
{
    return animationByIdMap[animationId];
}

Video * AnimationManager::GetVideoFromId(const string &videoId)
{
    return videoByIdMap[videoId];
}

void AnimationManager::LoadFromXml(XmlReader *pReader)
{
    pReader->StartElement("AnimationManager");
    pReader->StartElement("AnimationByIdHashMap");

    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        animationByIdMap[id] = new Animation(pReader, managerSource);
    }

    pReader->EndElement();
    pReader->StartElement("VideoByIdHashMap");

    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        videoByIdMap[id] = new Video(pReader);
    }

    pReader->EndElement();
    pReader->EndElement();
}

void AnimationManager::FinishUpdateLoadedTextures(const string &newLocationId)
{
    for (map<string, Video *>::iterator iter = videoByIdMap.begin(); iter != videoByIdMap.end(); ++iter)
    {
        bool loadSprite = false;
        bool deleteSprite = false;
        string videoId = iter->first;
        Video *pVideo = iter->second;

        if (pVideo == NULL)
        {
            continue;
        }

        pVideo->UpdateReadiness(newLocationId, &loadSprite, &deleteSprite);

        if (loadSprite)
        {
            ResourceLoader::GetInstance()->AddVideoToLoadList(pVideo);
        }
        else if (deleteSprite)
        {
            ResourceLoader::GetInstance()->AddVideoToDeleteList(pVideo);
        }
    }
}

void AnimationManager::UnloadResources()
{
    for (map<string, Video *>::iterator iter = videoByIdMap.begin(); iter != videoByIdMap.end(); ++iter)
    {
        ResourceLoader::GetInstance()->AddVideoToDeleteList(iter->second);
    }

    videoByIdMap.clear();
}
