/**
 * Basic header/include file for DialogCharacterManager.cpp.
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

#ifndef DIALOGCHARACTERMANAGER_H
#define DIALOGCHARACTERMANAGER_H

#include "../State.h"

#include <deque>
#include <vector>

using namespace std;

class DialogCharacter;
class XmlReader;

class DialogCharacterManager
{
public:
    DialogCharacterManager()
    {
        pBreakdownVideo = NULL;
    }

    ~DialogCharacterManager();

    static void Initialize(Image *pBreakdownFlashSprite);

    void UpdateForState(State *pState, int delta);
    void DrawForState(State *pState);
    static void DrawInterjectionForState(State *pState);
    void BeginAnimations(const string &characterId, const string &emotionId);
    void Update(const string &characterId, string &emotionId, int delta, bool finishOneTimeEmotions, bool isInBackground = false);
    void Draw(const string &characterId, const string &emotionId, bool isTalking, bool shouldChangeMouth, bool isRightSide, EasingFunction *pCharacterXOffsetEasingFunction);
    string GetCharacterNameFromId(const string &characterId);
    DialogCharacter * GetCharacterFromId(const string &characterId);
    void LoadFromXml(XmlReader *pReader);

private:
    static Image *pBreakdownFlashSprite;
    static EasingFunction *pFlashSpriteOpacityEaseIn;
    static EasingFunction *pFlashSpriteOpacityEaseOut;
    static double flashSpriteOpacity;

    map<string, DialogCharacter *> characterByIdMap;
    Video *pBreakdownVideo;

    Video *pSpeedLinesVideo;
    bool isShowingSpeedLines;
};

class DialogCharacter
{
private:
    class OneTimeEmotion
    {
    public:
        OneTimeEmotion(XmlReader *pReader);

        string GetTransitionToEmotion() { return this->transitionToEmotion; }
        int GetLeadInTime() { return this->leadInTime; }
        string GetVideoId() { return this->videoId; }

        Video * GetVideo();

    private:
        string transitionToEmotion;
        int leadInTime;
        string videoId;

        Video *pVideo;
    };

public:
    DialogCharacter()
    {
        currentEyeFrame = 0;
        msElapsedCurrentEyeFrame = 0;
        currentMouthIndex = 0;

        pPresentCorrectEvidenceSprite = NULL;
        pPresentIncorrectEvidenceSprite = NULL;
        pPressStatementSprite = NULL;
        pInterjectionSprite = NULL;
        pBreakdownVideo = NULL;
        pHealthBackgroundSprite = NULL;
        pEntranceForegroundSprite = NULL;
        pEntranceForegroundWhiteSprite = NULL;
    }

    DialogCharacter(XmlReader *pReader);
    ~DialogCharacter();

    string GetName() const { return this->name; }
    void SetName(const string &name) { this->name = name; }

    string GetDefaultEmotionId() const { return this->defaultEmotionId; }
    void SetDefaultEmotionId(const string &defaultEmotionId) { this->defaultEmotionId = defaultEmotionId; }

    string GetPresentCorrectEvidenceSpriteId() const { return this->presentCorrectEvidenceSpriteId; }
    void SetPresentCorrectEvidenceSpriteId(const string &presentCorrectEvidenceSpriteId) { this->presentCorrectEvidenceSpriteId = presentCorrectEvidenceSpriteId; }

    Sprite * GetPresentCorrectEvidenceSprite();

    string GetPresentCorrectEvidenceSoundId() const { return this->presentCorrectEvidenceSoundId; }
    void SetPresentCorrectEvidenceSoundId(const string &presentCorrectEvidenceSoundId) { this->presentCorrectEvidenceSoundId = presentCorrectEvidenceSoundId; }

    string GetPresentCorrectEvidencePromptedSoundId() const { return this->presentCorrectEvidencePromptedSoundId; }
    void SetPresentCorrectEvidencePromptedSoundId(const string &presentCorrectEvidencePromptedSoundId) { this->presentCorrectEvidencePromptedSoundId = presentCorrectEvidencePromptedSoundId; }

    string GetPresentIncorrectEvidenceSpriteId() const { return this->presentIncorrectEvidenceSpriteId; }
    void SetPresentIncorrectEvidenceSpriteId(const string &presentIncorrectEvidenceSpriteId) { this->presentIncorrectEvidenceSpriteId = presentIncorrectEvidenceSpriteId; }

    Sprite * GetPresentIncorrectEvidenceSprite();

    string GetPresentIncorrectEvidenceSoundId() const { return this->presentIncorrectEvidenceSoundId; }
    void SetPresentIncorrectEvidenceSoundId(const string &presentIncorrectEvidenceSoundId) { this->presentIncorrectEvidenceSoundId = presentIncorrectEvidenceSoundId; }

    string GetPressStatementSpriteId() const { return this->pressStatementSpriteId; }
    void SetPressStatementSpriteId(const string &pressStatementSpriteId) { this->pressStatementSpriteId = pressStatementSpriteId; }

    Sprite * GetPressStatementSprite();

    string GetPressStatementSoundId() const { return this->pressStatementSoundId; }
    void SetPressStatementSoundId(const string &pressStatementSoundId) { this->pressStatementSoundId = pressStatementSoundId; }

    string GetInterjectionSpriteId() const { return this->interjectionSpriteId; }
    void SetInterjectionSpriteId(const string &interjectionSpriteId) { this->interjectionSpriteId = interjectionSpriteId; }

    Sprite * GetInterjectionSprite();

    string GetInterjectionSoundId() const { return this->interjectionSoundId; }
    void SetInterjectionSoundId(const string &interjectionSoundId) { this->interjectionSoundId = interjectionSoundId; }

    void BeginAnimations(string emotionId);
    void Update(int delta, string &emotionId, bool finishOneTimeEmotions, bool isInBackground = false);
    void Draw(string emotionId, bool isTalking, bool shouldChangeMouth, bool isRightSide, int xOffset);
    Sprite * GetBaseSpriteForEmotion(const string &emotionId);
    Sprite * GetEyeSpriteForEmotion(const string &emotionId);
    Sprite * GetMouthSpriteForEmotion(const string &emotionId, bool isTalking, bool shouldChangeMouth);
    vector<Animation *> * GetForegroundLayersForEmotion(const string &emotionId);
    Video * GetBreakdownVideo();

    string GetHealthBackgroundSpriteId() const { return this->healthBackgroundSpriteId; }
    void SetHealthBackgroundSpriteId(const string &healthBackgroundSpriteId) { this->healthBackgroundSpriteId = healthBackgroundSpriteId; }

    Sprite * GetHealthBackgroundSprite();

    string GetEntranceForegroundSpriteId() const { return this->entranceForegroundSpriteId; }
    void SetEntranceForegroundSpriteId(const string &entranceForegroundSpriteId) { this->entranceForegroundSpriteId = entranceForegroundSpriteId; }

    Sprite * GetEntranceForegroundSprite();
    Sprite * GetEntranceForegroundWhiteSprite();

    Color GetBackgroundColor();

    int GetLeadInTimeForEmotion(const string &emotionId);

private:
    void PopulateEyeFrameDurationList(const string &emotionId);

    map<string, string> characterEmotionBaseSpriteIds;
    map<string, vector<string> > characterEmotionEyeSpriteIds;
    map<string, vector<string> > characterEmotionMouthSpriteIds;
    map<string, vector<string> > characterEmotionForegroundLayerIds;
    map<string, OneTimeEmotion *> characterOneTimeEmotions;

    map<string, Sprite *> characterEmotionBaseSprites;
    map<string, vector<Sprite *> > characterEmotionEyeSprites;
    map<string, vector<Sprite *> > characterEmotionMouthSprites;
    map<string, vector<Animation *> > characterEmotionForegroundLayers;

    string name;
    string defaultEmotionId;

    string presentCorrectEvidenceSpriteId;
    string presentCorrectEvidenceSoundId;
    string presentCorrectEvidencePromptedSoundId;
    string presentIncorrectEvidenceSpriteId;
    string presentIncorrectEvidenceSoundId;
    string pressStatementSpriteId;
    string pressStatementSoundId;
    string interjectionSpriteId;
    string interjectionSoundId;

    string breakdownVideoId;

    string healthBackgroundSpriteId;
    string entranceForegroundSpriteId;
    Color backgroundColor;

    Sprite *pPresentCorrectEvidenceSprite;
    Sprite *pPresentIncorrectEvidenceSprite;
    Sprite *pPressStatementSprite;
    Sprite *pInterjectionSprite;

    Video *pBreakdownVideo;

    Sprite *pHealthBackgroundSprite;
    Sprite *pEntranceForegroundSprite;
    Sprite *pEntranceForegroundWhiteSprite;

    unsigned int currentEyeFrame;
    unsigned int msElapsedCurrentEyeFrame;
    vector<unsigned int> eyeFrameDurationList;

    unsigned int currentMouthIndex;
    deque<unsigned int> mouthIndexOrder;
};

#endif
