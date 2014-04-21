/**
 * Basic header/include file for AnimationManager.cpp.
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

#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "../Animation.h"
#include "../Video.h"
#include <map>

using namespace std;

class Animation;
class XmlReader;

class AnimationManager
{
public:
    AnimationManager(ManagerSource managerSource)
    {
        this->managerSource = managerSource;
    }

    ~AnimationManager();
    void AddAnimation(const string &animationId, Animation **ppAnimation);
    void AddVideo(const string &animationId, Video **ppVideo, bool shouldLoop = false);
    void DeleteAnimation(const string &animationId);
    void DeleteVideo(const string &animationId);
    Animation * GetAnimationFromId(const string &animationId);
    Video * GetVideoFromId(const string &videoId);
    void LoadFromXml(XmlReader *pReader);
    void FinishUpdateLoadedTextures(const string &newLocationId);
    void UnloadResources();

private:
    map<string, Animation *> animationByIdMap;
    map<string, Video *> videoByIdMap;

    ManagerSource managerSource;
};

#endif
