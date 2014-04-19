/**
 * Manager for all of the dialog characters in the game; provides their retrieval.
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

#include "DialogCharacterManager.h"
#include "Case.h"
#include "CommonCaseResources.h"
#include "../globals.h"
#include "../ResourceLoader.h"
#include "../XmlReader.h"
#include "../CaseContent/Dialog.h"
#include <algorithm>

Image *DialogCharacterManager::pBreakdownFlashSprite = NULL;
EasingFunction *DialogCharacterManager::pFlashSpriteOpacityEaseIn = NULL;
EasingFunction *DialogCharacterManager::pFlashSpriteOpacityEaseOut = NULL;
double DialogCharacterManager::flashSpriteOpacity = 0;

DialogCharacterManager::~DialogCharacterManager()
{
    for (map<string, DialogCharacter *>::iterator iter = characterByIdMap.begin(); iter != characterByIdMap.end(); ++iter)
    {
        delete iter->second;
    }
}

void DialogCharacterManager::Initialize(Image *pBreakdownFlashSprite)
{
    DialogCharacterManager::pBreakdownFlashSprite = pBreakdownFlashSprite;
    DialogCharacterManager::pFlashSpriteOpacityEaseIn = new LinearEase(0.0, 1.0, 1000);
    DialogCharacterManager::pFlashSpriteOpacityEaseOut = new LinearEase(1.0, 0.0, 1000);
    DialogCharacterManager::flashSpriteOpacity = 0;
}

void DialogCharacterManager::UpdateForState(State *pState, int delta)
{
    if (pState->GetBreakdownTransitionStarted() && !pFlashSpriteOpacityEaseIn->GetIsStarted())
    {
        pState->SetBreakdownTransitionStarted(false);
        pFlashSpriteOpacityEaseIn->Begin();
        pFlashSpriteOpacityEaseOut->Reset();
        playSound("BreakdownSwish");
    }

    if (pFlashSpriteOpacityEaseIn->GetIsStarted() && !pFlashSpriteOpacityEaseIn->GetIsFinished())
    {
        pFlashSpriteOpacityEaseIn->Update(delta);
        flashSpriteOpacity = pFlashSpriteOpacityEaseIn->GetCurrentValue();

        if (pFlashSpriteOpacityEaseIn->GetIsFinished())
        {
            if (pState->GetBreakdownActivePosition() != CharacterPositionNone)
            {
                string characterId =
                    pState->GetBreakdownActivePosition() == CharacterPositionLeft ?
                    pState->GetLeftCharacterId() :
                    pState->GetRightCharacterId();

                pBreakdownVideo = characterByIdMap[characterId]->GetBreakdownVideo();
                pBreakdownVideo->Begin();
                pFlashSpriteOpacityEaseOut->Begin();

                Confrontation *pConfrontation = pState->GetCurrentConfrontation();

                if (pConfrontation != NULL)
                {
                    pConfrontation->HideHealthIcons();
                }
            }
            else
            {
                pBreakdownVideo = NULL;
                pFlashSpriteOpacityEaseOut->Begin();
            }
        }
    }

    if (pFlashSpriteOpacityEaseOut->GetIsStarted() && !pFlashSpriteOpacityEaseOut->GetIsFinished())
    {
        pFlashSpriteOpacityEaseOut->Update(delta);
        flashSpriteOpacity = pFlashSpriteOpacityEaseOut->GetCurrentValue();

        if (pFlashSpriteOpacityEaseOut->GetIsFinished())
        {
            pFlashSpriteOpacityEaseIn->Reset();
            pState->SetBreakdownTransitionComplete(true);
        }
    }

    if (pBreakdownVideo == NULL)
    {
        string leftCharacterEmotionId = pState->GetLeftCharacterEmotionId();
        string leftReplacementCharacterEmotionId = pState->GetLeftReplacementCharacterEmotionId();
        string rightCharacterEmotionId = pState->GetRightCharacterEmotionId();
        string rightReplacementCharacterEmotionId = pState->GetRightReplacementCharacterEmotionId();

        if (pState->GetIsLeftCharacterZoomed() || pState->GetIsRightCharacterZoomed())
        {
            if (!isShowingSpeedLines)
            {
                isShowingSpeedLines = true;

                if (pSpeedLinesVideo == NULL)
                {
                    pSpeedLinesVideo = CommonCaseResources::GetInstance()->GetAnimationManager()->GetVideoFromId("SpeedLines");
                }

                pSpeedLinesVideo->Begin();
            }
            else
            {
                pSpeedLinesVideo->Update(delta);
            }
        }
        else
        {
            isShowingSpeedLines = false;
        }

        if (pState->GetIsLeftCharacterZoomed())
        {
            string zoomEmotion = "Zoom";

            Update(pState->GetLeftCharacterId(), leftCharacterEmotionId, delta, pState->GetIsFinishingDialog(), true /* isInBackground */);
            Update(pState->GetLeftCharacterId(), zoomEmotion, delta, pState->GetIsFinishingDialog());
        }
        else
        {
            Update(pState->GetLeftCharacterId(), leftCharacterEmotionId, delta, pState->GetIsFinishingDialog());
        }

        Update(pState->GetLeftReplacementCharacterId(), leftReplacementCharacterEmotionId, delta, pState->GetIsFinishingDialog());

        if (pState->GetIsRightCharacterZoomed())
        {
            string zoomEmotion = "Zoom";

            Update(pState->GetRightCharacterId(), rightCharacterEmotionId, delta, pState->GetIsFinishingDialog(), true /* isInBackground */);
            Update(pState->GetRightCharacterId(), zoomEmotion, delta, pState->GetIsFinishingDialog());
        }
        else
        {
            Update(pState->GetRightCharacterId(), rightCharacterEmotionId, delta, pState->GetIsFinishingDialog());
        }

        Update(pState->GetRightReplacementCharacterId(), rightReplacementCharacterEmotionId, delta, pState->GetIsFinishingDialog());

        if (leftCharacterEmotionId != pState->GetLeftCharacterEmotionId())
        {
            pState->SetLeftCharacterEmotionId(leftCharacterEmotionId);
        }

        if (leftReplacementCharacterEmotionId != pState->GetLeftReplacementCharacterEmotionId())
        {
            pState->SetLeftReplacementCharacterEmotionId(leftReplacementCharacterEmotionId);
        }

        if (rightCharacterEmotionId != pState->GetRightCharacterEmotionId())
        {
            pState->SetRightCharacterEmotionId(rightCharacterEmotionId);
        }

        if (rightReplacementCharacterEmotionId != pState->GetRightReplacementCharacterEmotionId())
        {
            pState->SetRightReplacementCharacterEmotionId(rightReplacementCharacterEmotionId);
        }

        if (pState->GetIsInterjectionOngoing())
        {
            pState->UpdateInterjection(delta);
        }
    }
    else if (pFlashSpriteOpacityEaseOut->GetIsFinished())
    {
        pBreakdownVideo->Update(delta);
    }
}

void DialogCharacterManager::DrawForState(State *pState)
{
    if (pBreakdownVideo == NULL)
    {
        if (pState->GetIsLeftCharacterZoomed())
        {
            pSpeedLinesVideo->Draw(Vector2(0, 0), characterByIdMap[pState->GetLeftCharacterId()]->GetBackgroundColor());
            Draw(pState->GetLeftCharacterId(), "Zoom", pState->GetIsLeftCharacterTalking(), pState->GetShouldLeftCharacterChangeMouth(), false /* isRightSide */, pState->GetLeftCharacterXOffsetEasingFunction());
        }
        else if (pState->GetIsRightCharacterZoomed())
        {
            pSpeedLinesVideo->Draw(Vector2(0, 0), true /* flipHorizontally */, characterByIdMap[pState->GetRightCharacterId()]->GetBackgroundColor());
            Draw(pState->GetRightCharacterId(), "Zoom", pState->GetIsRightCharacterTalking(), pState->GetShouldRightCharacterChangeMouth(), true /* isRightSide */, pState->GetRightCharacterXOffsetEasingFunction());
        }
        else
        {
            Draw(pState->GetLeftCharacterId(), pState->GetLeftCharacterEmotionId(), pState->GetIsLeftCharacterTalking(), pState->GetShouldLeftCharacterChangeMouth(), false /* isRightSide */, pState->GetLeftCharacterXOffsetEasingFunction());
            Draw(pState->GetLeftReplacementCharacterId(), pState->GetLeftReplacementCharacterEmotionId(), false /* isTalking */, false /* shouldChangeMouth */, false /* isRightSide */, pState->GetLeftReplacementCharacterXOffsetEasingFunction());
            Draw(pState->GetRightCharacterId(), pState->GetRightCharacterEmotionId(), pState->GetIsRightCharacterTalking(), pState->GetShouldRightCharacterChangeMouth(), true /* isRightSide */, pState->GetRightCharacterXOffsetEasingFunction());
            Draw(pState->GetRightReplacementCharacterId(), pState->GetRightReplacementCharacterEmotionId(), false /* isTalking */, false /* shouldChangeMouth */, true /* isRightSide */, pState->GetRightReplacementCharacterXOffsetEasingFunction());
        }
    }
    else
    {
        pBreakdownVideo->Draw(Vector2(0, 0));
    }

    if (flashSpriteOpacity > 0)
    {
        pBreakdownFlashSprite->Draw(Vector2(0, 0), Color(flashSpriteOpacity, 1.0, 1.0, 1.0));
    }
}

void DialogCharacterManager::DrawInterjectionForState(State *pState)
{
    if (!pState->GetIsInterjectionOngoing())
    {
        return;
    }

    double interjectionSpriteXOffset = 0;

    if (pState->GetInterjectionSpriteInEasingFunction()->GetIsStarted() && !pState->GetInterjectionSpriteOutEasingFunction()->GetIsStarted())
    {
        interjectionSpriteXOffset = pState->GetInterjectionSpriteInEasingFunction()->GetCurrentValue();
    }
    else if (pState->GetInterjectionSpriteOutEasingFunction()->GetIsStarted() && !pState->GetInterjectionSpriteOutEasingFunction()->GetIsFinished())
    {
        interjectionSpriteXOffset = pState->GetInterjectionSpriteOutEasingFunction()->GetCurrentValue();
    }

    pState->GetInterjectionSprite()->Draw(Vector2(interjectionSpriteXOffset, 0));
}

void DialogCharacterManager::BeginAnimations(string characterId, string emotionId)
{
    if (characterId.length() == 0)
    {
        return;
    }

    characterByIdMap[characterId]->BeginAnimations(emotionId);
}

void DialogCharacterManager::Update(string characterId, string &emotionId, int delta, bool finishOneTimeEmotions, bool isInBackground)
{
    if (characterId.length() == 0)
    {
        return;
    }

    characterByIdMap[characterId]->Update(delta, emotionId, finishOneTimeEmotions, isInBackground);
}

void DialogCharacterManager::Draw(string characterId, string emotionId, bool isTalking, bool shouldChangeMouth, bool isRightSide, EasingFunction *pCharacterXOffsetEasingFunction)
{
    int xOffset = (int)(pCharacterXOffsetEasingFunction == NULL ? 0 : pCharacterXOffsetEasingFunction->GetCurrentValue());

    if (characterId.length() == 0)
    {
        return;
    }

    characterByIdMap[characterId]->Draw(emotionId, isTalking, shouldChangeMouth, isRightSide, xOffset);
}

string DialogCharacterManager::GetCharacterNameFromId(string characterId)
{
    if (characterId.length() == 0 || characterByIdMap.count(characterId) == 0)
    {
        return "";
    }
    else
    {
        return characterByIdMap[characterId]->GetName();
    }
}

DialogCharacter * DialogCharacterManager::GetCharacterFromId(string characterId)
{
    return characterByIdMap[characterId];
}

void DialogCharacterManager::LoadFromXml(XmlReader *pReader)
{
    pBreakdownVideo = NULL;
    pSpeedLinesVideo = NULL;
    isShowingSpeedLines = false;

    pReader->StartElement("DialogCharacterManager");

    pReader->StartElement("CharacterByIdHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        characterByIdMap[id] = new DialogCharacter(pReader);
    }

    pReader->EndElement();

    pReader->EndElement();
}

DialogCharacter::OneTimeEmotion::OneTimeEmotion(XmlReader *pReader)
{
    pVideo = NULL;

    pReader->StartElement("OneTimeEmotion");

    if (pReader->ElementExists("TransitionToEmotion"))
    {
        transitionToEmotion = pReader->ReadTextElement("TransitionToEmotion");
    }

    if (pReader->ElementExists("LeadInTime"))
    {
        leadInTime = pReader->ReadIntElement("LeadInTime");
    }
    else
    {
        leadInTime = 0;
    }

    videoId = pReader->ReadTextElement("VideoId");
    pReader->EndElement();
}

Video * DialogCharacter::OneTimeEmotion::GetVideo()
{
    if (pVideo == NULL)
    {
        pVideo = Case::GetInstance()->GetAnimationManager()->GetVideoFromId(videoId);
    }

    return pVideo;
}

DialogCharacter::DialogCharacter(XmlReader *pReader)
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

    pReader->StartElement("DialogCharacter");
    name = pReader->ReadTextElement("Name");
    defaultEmotionId = pReader->ReadTextElement("DefaultEmotionId");
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);

    pReader->StartElement("CharacterEmotionBaseSpriteIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string emotionId = pReader->ReadTextElement("EmotionId");
        string spriteId = pReader->ReadTextElement("SpriteId");

        characterEmotionBaseSpriteIds[emotionId] = spriteId;
    }

    pReader->EndElement();

    pReader->StartElement("CharacterEmotionEyeSpriteIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string emotionId = pReader->ReadTextElement("EmotionId");
        characterEmotionEyeSpriteIds[emotionId] = vector<string>();

        pReader->StartElement("SpriteIds");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            characterEmotionEyeSpriteIds[emotionId].push_back(pReader->ReadTextElement("SpriteId"));
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("CharacterEmotionMouthSpriteIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string emotionId = pReader->ReadTextElement("EmotionId");
        characterEmotionMouthSpriteIds[emotionId] = vector<string>();

        pReader->StartElement("SpriteIds");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            characterEmotionMouthSpriteIds[emotionId].push_back(pReader->ReadTextElement("SpriteId"));
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("CharacterEmotionForegroundLayerIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string emotionId = pReader->ReadTextElement("EmotionId");
        characterEmotionForegroundLayerIds[emotionId] = vector<string>();

        pReader->StartElement("AnimationIds");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            characterEmotionForegroundLayerIds[emotionId].push_back(pReader->ReadTextElement("AnimationId"));
        }

        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("CharacterOneTimeEmotions");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string emotionId = pReader->ReadTextElement("EmotionId");

        pReader->StartElement("OneTimeEmotion");
        characterOneTimeEmotions[emotionId] = new OneTimeEmotion(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    if (pReader->ElementExists("PresentCorrectEvidenceSpriteId"))
    {
        presentCorrectEvidenceSpriteId = pReader->ReadTextElement("PresentCorrectEvidenceSpriteId");
    }

    if (pReader->ElementExists("PresentCorrectEvidenceSoundId"))
    {
        presentCorrectEvidenceSoundId = pReader->ReadTextElement("PresentCorrectEvidenceSoundId");
        ResourceLoader::GetInstance()->PreloadDialog(presentCorrectEvidenceSoundId, presentCorrectEvidenceSoundId);
    }

    if (pReader->ElementExists("PresentCorrectEvidencePromptedSoundId"))
    {
        presentCorrectEvidencePromptedSoundId = pReader->ReadTextElement("PresentCorrectEvidencePromptedSoundId");
        ResourceLoader::GetInstance()->PreloadDialog(presentCorrectEvidencePromptedSoundId, presentCorrectEvidencePromptedSoundId);
    }

    if (pReader->ElementExists("PresentIncorrectEvidenceSpriteId"))
    {
        presentIncorrectEvidenceSpriteId = pReader->ReadTextElement("PresentIncorrectEvidenceSpriteId");
    }

    if (pReader->ElementExists("PresentIncorrectEvidenceSoundId"))
    {
        presentIncorrectEvidenceSoundId = pReader->ReadTextElement("PresentIncorrectEvidenceSoundId");
        ResourceLoader::GetInstance()->PreloadDialog(presentIncorrectEvidenceSoundId, presentIncorrectEvidenceSoundId);
    }

    if (pReader->ElementExists("PressStatementSpriteId"))
    {
        pressStatementSpriteId = pReader->ReadTextElement("PressStatementSpriteId");
    }

    if (pReader->ElementExists("PressStatementSoundId"))
    {
        pressStatementSoundId = pReader->ReadTextElement("PressStatementSoundId");
        ResourceLoader::GetInstance()->PreloadDialog(pressStatementSoundId, pressStatementSoundId);
    }

    if (pReader->ElementExists("InterjectionSpriteId"))
    {
        interjectionSpriteId = pReader->ReadTextElement("InterjectionSpriteId");
    }

    if (pReader->ElementExists("InterjectionSpriteId"))
    {
        interjectionSoundId = pReader->ReadTextElement("InterjectionSoundId");
        ResourceLoader::GetInstance()->PreloadDialog(interjectionSoundId, interjectionSoundId);
    }

    if (pReader->ElementExists("BreakdownVideoId"))
    {
        breakdownVideoId = pReader->ReadTextElement("BreakdownVideoId");
    }

    if (pReader->ElementExists("HealthBackgroundSpriteId"))
    {
        healthBackgroundSpriteId = pReader->ReadTextElement("HealthBackgroundSpriteId");
    }

    if (pReader->ElementExists("EntranceForegroundSpriteId"))
    {
        entranceForegroundSpriteId = pReader->ReadTextElement("EntranceForegroundSpriteId");
    }

    if (pReader->ElementExists("BackgroundColor"))
    {
        pReader->StartElement("BackgroundColor");
        backgroundColor = Color(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}

DialogCharacter::~DialogCharacter()
{
    for (map<string, OneTimeEmotion *>::iterator iter = characterOneTimeEmotions.begin(); iter != characterOneTimeEmotions.end(); ++iter)
    {
        delete iter->second;
    }
}

Sprite * DialogCharacter::GetPresentCorrectEvidenceSprite()
{
    if (pPresentCorrectEvidenceSprite == NULL)
    {
        pPresentCorrectEvidenceSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(presentCorrectEvidenceSpriteId);
    }

    return pPresentCorrectEvidenceSprite;
}

Sprite * DialogCharacter::GetPresentIncorrectEvidenceSprite()
{
    if (pPresentIncorrectEvidenceSprite == NULL)
    {
        pPresentIncorrectEvidenceSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(presentIncorrectEvidenceSpriteId);
    }

    return pPresentIncorrectEvidenceSprite;
}

Sprite * DialogCharacter::GetPressStatementSprite()
{
    if (pPressStatementSprite == NULL)
    {
        pPressStatementSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(pressStatementSpriteId);
    }

    return pPressStatementSprite;
}

Sprite * DialogCharacter::GetInterjectionSprite()
{
    if (pInterjectionSprite == NULL)
    {
        pInterjectionSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(interjectionSpriteId);
    }

    return pInterjectionSprite;
}

void DialogCharacter::BeginAnimations(string emotionId)
{
    if (emotionId.length() == 0)
    {
        emotionId = defaultEmotionId;
    }

    if (characterOneTimeEmotions.count(emotionId) > 0)
    {
        Video *pVideo = characterOneTimeEmotions[emotionId]->GetVideo();
        pVideo->Begin();
    }

    vector<Animation *> *pAnimationList = GetForegroundLayersForEmotion(emotionId);

    if (pAnimationList != NULL)
    {
        for (unsigned int i = 0; i < pAnimationList->size(); i++)
        {
            Animation *pAnimation = (*pAnimationList)[i];
            pAnimation->Begin();
        }
    }
}

void DialogCharacter::Update(int delta, string &emotionId, bool finishOneTimeEmotions, bool isInBackground)
{
    if (emotionId.length() == 0)
    {
        emotionId = defaultEmotionId;
    }

    if (characterOneTimeEmotions.count(emotionId) > 0)
    {
        OneTimeEmotion *pOneTimeEmotion = characterOneTimeEmotions[emotionId];

        Video *pVideo = pOneTimeEmotion->GetVideo();

        if (finishOneTimeEmotions)
        {
            pVideo->Finish();
        }
        else
        {
            pVideo->Update(delta);
        }

        if (pVideo->IsFinished() && pOneTimeEmotion->GetTransitionToEmotion().length() > 0)
        {
            pVideo->Reset();
            emotionId = pOneTimeEmotion->GetTransitionToEmotion();
        }
        else
        {
            return;
        }
    }

    vector<Animation *> *pForegroundLayers = GetForegroundLayersForEmotion(emotionId);

    if (pForegroundLayers != NULL)
    {
        for (unsigned int i = 0; i < pForegroundLayers->size(); i++)
        {
            Animation *pAnimation = (*pForegroundLayers)[i];
            pAnimation->Update(delta);
        }
    }

    // If this emotion is currently in the background (e.g., if the character is currently zoomed),
    // then we won't bother updating the eye frame duration list.
    if (isInBackground || characterEmotionEyeSpriteIds.count(emotionId) == 0)
    {
        return;
    }

    if (eyeFrameDurationList.size() == 0 || eyeFrameDurationList.size() != characterEmotionEyeSpriteIds[emotionId].size())
    {
        PopulateEyeFrameDurationList(emotionId);
    }

    msElapsedCurrentEyeFrame += delta;

    while (msElapsedCurrentEyeFrame > eyeFrameDurationList[currentEyeFrame])
    {
        msElapsedCurrentEyeFrame -= eyeFrameDurationList[currentEyeFrame];
        currentEyeFrame++;

        // If we've reached the end, then we'll wrap back around and get a
        // new random number representing the time until the next eye blink.
        if (currentEyeFrame >= eyeFrameDurationList.size())
        {
            currentEyeFrame = 0;
            eyeFrameDurationList[0] = (int)(rand() % 2001) + 2000;
        }
    }
}

void DialogCharacter::Draw(string emotionId, bool isTalking, bool shouldChangeMouth, bool isRightSide, int xOffset)
{
    if (emotionId.length() == 0)
    {
        emotionId = defaultEmotionId;
    }

    if (characterOneTimeEmotions.count(emotionId) > 0)
    {
        Video *pVideo = characterOneTimeEmotions[emotionId]->GetVideo();
        Vector2 position((isRightSide ? (double)gScreenWidth - pVideo->GetWidth() : 0) + xOffset, (double)gScreenHeight - Dialog::Height - pVideo->GetHeight());
        pVideo->Draw(position, !isRightSide, Color::White);
        return;
    }

    Sprite *pBaseSprite = GetBaseSpriteForEmotion(emotionId);
    Sprite *pEyeSprite = GetEyeSpriteForEmotion(emotionId);
    Sprite *pMouthSprite = GetMouthSpriteForEmotion(emotionId, isTalking, shouldChangeMouth);
    vector<Animation *> *pForegroundLayers = GetForegroundLayersForEmotion(emotionId);

    if (pBaseSprite != NULL)
    {
        Vector2 position((isRightSide ? gScreenWidth / 2 : 0) + xOffset, 0);

        pBaseSprite->Draw(position, Color::White, 1.0, !isRightSide);

        if (pEyeSprite != NULL)
        {
            pEyeSprite->Draw(position, Color::White, 1.0, !isRightSide);
        }

        if (pMouthSprite != NULL)
        {
            pMouthSprite->Draw(position, Color::White, 1.0, !isRightSide);
        }

        if (pForegroundLayers != NULL)
        {
            for (unsigned int i = 0; i < pForegroundLayers->size(); i++)
            {
                (*pForegroundLayers)[i]->Draw(position, !isRightSide, 1.0);
            }
        }
    }
}

Sprite * DialogCharacter::GetBaseSpriteForEmotion(string emotionId)
{
    Sprite *pSprite = NULL;

    if (characterOneTimeEmotions.count(emotionId) == 0)
    {
        if (characterEmotionBaseSprites.count(emotionId) > 0)
        {
            pSprite = characterEmotionBaseSprites[emotionId];
        }
        else
        {
            pSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(characterEmotionBaseSpriteIds[emotionId]);
            characterEmotionBaseSprites[emotionId] = pSprite;
        }
    }

    return pSprite;
}

Sprite * DialogCharacter::GetEyeSpriteForEmotion(string emotionId)
{
    vector<Sprite *> *pEyeSpriteList = NULL;

    if (characterEmotionEyeSprites.count(emotionId) > 0)
    {
        pEyeSpriteList = &characterEmotionEyeSprites[emotionId];
    }
    else
    {
        if (characterEmotionEyeSpriteIds[emotionId].empty())
        {
            return NULL;
        }

        characterEmotionEyeSprites[emotionId] = vector<Sprite *>();

        for (unsigned int i = 0; i < characterEmotionEyeSpriteIds[emotionId].size(); i++)
        {
            Sprite *pSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(characterEmotionEyeSpriteIds[emotionId][i]);
            characterEmotionEyeSprites[emotionId].push_back(pSprite);
        }

        pEyeSpriteList = &characterEmotionEyeSprites[emotionId];
    }

    return (*pEyeSpriteList)[min(currentEyeFrame, (unsigned int)pEyeSpriteList->size() - 1)];
}

Sprite * DialogCharacter::GetMouthSpriteForEmotion(string emotionId, bool isTalking, bool shouldChangeMouth)
{
    vector<Sprite *> *pMouthSpriteList = NULL;

    if (characterEmotionMouthSprites.count(emotionId) > 0)
    {
        pMouthSpriteList = &characterEmotionMouthSprites[emotionId];
    }
    else
    {
        if (characterEmotionMouthSpriteIds[emotionId].empty())
        {
            return NULL;
        }

        characterEmotionMouthSprites[emotionId] = vector<Sprite *>();

        for (unsigned int i = 0; i < characterEmotionMouthSpriteIds[emotionId].size(); i++)
        {
            Sprite *pSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(characterEmotionMouthSpriteIds[emotionId][i]);
            characterEmotionMouthSprites[emotionId].push_back(pSprite);
        }

        pMouthSpriteList = &characterEmotionMouthSprites[emotionId];
    }

    // The closed mouth is always at position 0, so if we're not talking,
    // we want to return that one.  Otherwise, we want to return a random
    // frame in the sprite list.  This ensures that the talking animation
    // seems fluid rather than having it just be the same mouth positions
    // repeated over and over.
    int spriteIndex = 0;

    if (isTalking)
    {
        if (shouldChangeMouth)
        {
            if (mouthIndexOrder.empty())
            {
                for (unsigned int i = 0; i < pMouthSpriteList->size(); i++)
                {
                    mouthIndexOrder.push_back(i);
                }

                random_shuffle(mouthIndexOrder.begin(), mouthIndexOrder.end());

                // We never want to begin with a shut mouth, so if
                // a 0 is at the start, we'll move it to the second
                // position.
                if (mouthIndexOrder[0] == 0 && mouthIndexOrder.size() > 1)
                {
                    mouthIndexOrder[0] = mouthIndexOrder[1];
                    mouthIndexOrder[1] = 0;
                }
            }

            do
            {
                if (mouthIndexOrder.empty())
                {
                    currentMouthIndex = 0;
                }
                else
                {
                    currentMouthIndex = mouthIndexOrder[0];
                    mouthIndexOrder.pop_front();
                }
            }
            while (currentMouthIndex >= pMouthSpriteList->size());
        }

        spriteIndex = currentMouthIndex;
    }

    return (*pMouthSpriteList)[min(spriteIndex, (int)pMouthSpriteList->size() - 1)];
}

vector<Animation *> * DialogCharacter::GetForegroundLayersForEmotion(string emotionId)
{
    vector<Animation *> *pForegroundLayers = NULL;

    if (characterEmotionForegroundLayers.count(emotionId) > 0)
    {
        pForegroundLayers = &characterEmotionForegroundLayers[emotionId];
    }
    else
    {
        if (characterEmotionForegroundLayerIds[emotionId].empty())
        {
            return NULL;
        }

        vector<string> *pForegroundLayerIds = &characterEmotionForegroundLayerIds[emotionId];

        for (unsigned int i = 0; i < pForegroundLayerIds->size(); i++)
        {
            Animation *pAnimation = Case::GetInstance()->GetAnimationManager()->GetAnimationFromId((*pForegroundLayerIds)[i]);
            characterEmotionForegroundLayers[emotionId].push_back(pAnimation);
        }

        pForegroundLayers = &characterEmotionForegroundLayers[emotionId];
    }

    return pForegroundLayers;
}

Video * DialogCharacter::GetBreakdownVideo()
{
    if (pBreakdownVideo == NULL)
    {
        pBreakdownVideo = Case::GetInstance()->GetAnimationManager()->GetVideoFromId(breakdownVideoId);
    }

    return pBreakdownVideo;
}

Sprite * DialogCharacter::GetHealthBackgroundSprite()
{
    if (pHealthBackgroundSprite == NULL)
    {
        pHealthBackgroundSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(healthBackgroundSpriteId);
    }

    return pHealthBackgroundSprite;
}

Sprite * DialogCharacter::GetEntranceForegroundSprite()
{
    if (pEntranceForegroundSprite == NULL)
    {
        pEntranceForegroundSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(entranceForegroundSpriteId);
    }

    return pEntranceForegroundSprite;
}

Sprite * DialogCharacter::GetEntranceForegroundWhiteSprite()
{
    if (pEntranceForegroundWhiteSprite == NULL)
    {
        pEntranceForegroundWhiteSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(entranceForegroundSpriteId + "White");
    }

    return pEntranceForegroundWhiteSprite;
}

Color DialogCharacter::GetBackgroundColor()
{
    return backgroundColor;
}

int DialogCharacter::GetLeadInTimeForEmotion(string emotionId)
{
    return characterOneTimeEmotions.count(emotionId) > 0 ? characterOneTimeEmotions[emotionId]->GetLeadInTime() : 0;
}

void DialogCharacter::PopulateEyeFrameDurationList(string emotionId)
{
    eyeFrameDurationList.clear();

    // We want the time until the next eye blink to be random, in order
    // for the blinks to appear natural rather than mechanical.
    eyeFrameDurationList.push_back((int)(rand() % 2001) + 2000);

    for (unsigned int i = 1; i < characterEmotionEyeSpriteIds[emotionId].size(); i++)
    {
        eyeFrameDurationList.push_back(75);
    }
}
