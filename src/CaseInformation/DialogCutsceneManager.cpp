/**
 * Manager for all of the dialog cutscenes in the game; provides their retrieval.
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

#include "DialogCutsceneManager.h"
#include "../CaseInformation/Case.h"
#include "../XmlReader.h"

DialogCutsceneManager::~DialogCutsceneManager()
{
    for (map<string, DialogCutsceneAnimation *>::iterator iter = idToAnimationMap.begin(); iter != idToAnimationMap.end(); ++iter)
    {
        delete iter->second;
    }

    for (map<string, DialogCutsceneAnimationElement *>::iterator iter = idToElementMap.begin(); iter != idToElementMap.end(); ++iter)
    {
        delete iter->second;
    }
}

void DialogCutsceneManager::LoadFromXml(XmlReader *pReader)
{
    pReader->StartElement("DialogCutsceneManager");

    pReader->StartElement("IdToAnimationHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        idToAnimationMap[id] = new DialogCutsceneAnimation(pReader);
    }

    pReader->EndElement();

    pReader->StartElement("IdToElementHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");
        idToElementMap[id] = new DialogCutsceneAnimationElement(pReader);
    }

    pReader->EndElement();

    pReader->EndElement();
}

DialogCutsceneAnimation::DialogCutsceneAnimation(XmlReader *pReader)
{
    pBackgroundSprite = NULL;
    frameIndex = 0;
    pReplacementBackgroundSprite = NULL;
    pReplacementBackgroundSpriteOpacityEase = NULL;
    replacementBackgroundSpriteOpacity = 0;
    changeBackgroundCount = 0;
    isFinished = false;

    pReader->StartElement("DialogCutsceneAnimation");

    id = pReader->ReadTextElement("Id");
    backgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");

    pReader->StartElement("ElementIdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        elementIdList.push_back(pReader->ReadTextElement("ElementId"));
    }

    pReader->EndElement();

    pReader->StartElement("FrameList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        frameList.push_back(DialogCutsceneAnimationFrame(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

DialogCutsceneAnimation::~DialogCutsceneAnimation()
{
    delete pReplacementBackgroundSpriteOpacityEase;
    pReplacementBackgroundSpriteOpacityEase = NULL;
}

Sprite * DialogCutsceneAnimation::GetBackgroundSprite()
{
    if (pBackgroundSprite == NULL)
    {
        pBackgroundSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(GetBackgroundSpriteId());
    }

    return pBackgroundSprite;
}

vector<DialogCutsceneAnimationElement *> * DialogCutsceneAnimation::GetElementList()
{
    if (elementList.size() == 0)
    {
        for (unsigned int i = 0; i < elementIdList.size(); i++)
        {
            elementList.push_back(Case::GetInstance()->GetDialogCutsceneManager()->GetElementFromId(elementIdList[i]));
        }
    }

    return &elementList;
}

void DialogCutsceneAnimation::Begin()
{
    SetIsFinished(false);
    Reset();
    frameIndex = -1;
    MoveToNextFrame();
}

void DialogCutsceneAnimation::Update(int delta)
{
    if (pReplacementBackgroundSprite != NULL)
    {
        pReplacementBackgroundSpriteOpacityEase->Update(delta);
        replacementBackgroundSpriteOpacity = pReplacementBackgroundSpriteOpacityEase->GetCurrentValue();

        if (pReplacementBackgroundSpriteOpacityEase->GetIsFinished())
        {
            pBackgroundSprite = pReplacementBackgroundSprite;
            pReplacementBackgroundSprite = NULL;
            replacementBackgroundSpriteOpacity = 0;

            delete pReplacementBackgroundSpriteOpacityEase;
            pReplacementBackgroundSpriteOpacityEase = NULL;
        }
    }

    vector<DialogCutsceneAnimationElement *> *pElementList = GetElementList();

    for (unsigned int i = 0; i < pElementList->size(); i++)
    {
        (*pElementList)[i]->Update(delta);
    }
}

void DialogCutsceneAnimation::Draw()
{
    GetBackgroundSprite()->Draw(Vector2(0, 0));

    if (pReplacementBackgroundSprite != NULL)
    {
        pReplacementBackgroundSprite->Draw(Vector2(0, 0), Color(replacementBackgroundSpriteOpacity, 1.0, 1.0, 1.0));
    }

    vector<DialogCutsceneAnimationElement *> *pElementList = GetElementList();

    for (unsigned int i = 0; i < pElementList->size(); i++)
    {
        (*pElementList)[i]->Draw();
    }
}

void DialogCutsceneAnimation::Reset()
{
    vector<DialogCutsceneAnimationElement  *> *pElementList = GetElementList();

    for (unsigned int i = 0; i < pElementList->size(); i++)
    {
        (*pElementList)[i]->Reset();
    }

    pBackgroundSprite = NULL;
    pReplacementBackgroundSprite = NULL;
    replacementBackgroundSpriteOpacity = 0;

    delete pReplacementBackgroundSpriteOpacityEase;
    pReplacementBackgroundSpriteOpacityEase = NULL;
}

void DialogCutsceneAnimation::FinishCurrentFrame()
{
    if (pReplacementBackgroundSprite != NULL)
    {
        pBackgroundSprite = pReplacementBackgroundSprite;
        pReplacementBackgroundSprite = NULL;
        replacementBackgroundSpriteOpacity = 0;

        delete pReplacementBackgroundSpriteOpacityEase;
        pReplacementBackgroundSpriteOpacityEase = NULL;
    }

    vector<DialogCutsceneAnimationElement *> *pElementList = GetElementList();

    for (unsigned int i = 0; i < pElementList->size(); i++)
    {
        (*pElementList)[i]->Finish();
    }
}

void DialogCutsceneAnimation::MoveToNextFrame()
{
    frameIndex++;

    if (pReplacementBackgroundSprite != NULL)
    {
        pBackgroundSprite = pReplacementBackgroundSprite;
        pReplacementBackgroundSprite = NULL;
        replacementBackgroundSpriteOpacity = 0;

        delete pReplacementBackgroundSpriteOpacityEase;
        pReplacementBackgroundSpriteOpacityEase = NULL;
    }

    vector<DialogCutsceneAnimationElement *> *pElementList = GetElementList();

    for (unsigned int i = 0; i < pElementList->size(); i++)
    {
        (*pElementList)[i]->Finish();
        (*pElementList)[i]->ClearEasingFunctions();
    }

    if (frameIndex < frameList.size())
    {
        frameList[frameIndex].Begin();

        if (frameList[frameIndex].GetNewBackgroundSpriteId().length() > 0)
        {
            pReplacementBackgroundSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(frameList[frameIndex].GetNewBackgroundSpriteId());

            delete pReplacementBackgroundSpriteOpacityEase;
            pReplacementBackgroundSpriteOpacityEase = new LinearEase(0.0, 1.0, frameList[frameIndex].GetMsDuration());
            pReplacementBackgroundSpriteOpacityEase->Begin();
        }
    }
    else
    {
        SetIsFinished(true);
    }
}

DialogCutsceneAnimationElement::DialogCutsceneAnimationElement(string spriteId, int x, int y, int a, int r, int g, int b)
    : spriteId(spriteId)
{
    this->pSprite = NULL;

    this->originalXPosition = x;
    this->originalYPosition = y;

    this->originalATint = a;
    this->originalRTint = r;
    this->originalGTint = g;
    this->originalBTint = b;

    this->pXPositionEase = NULL;
    this->pYPositionEase = NULL;

    this->pATintEase = NULL;
    this->pRTintEase = NULL;
    this->pGTintEase = NULL;
    this->pBTintEase = NULL;

    this->xPosition = 0;
    this->yPosition = 0;

    this->aTint = 0;
    this->rTint = 0;
    this->gTint = 0;
    this->bTint = 0;
}

DialogCutsceneAnimationElement::DialogCutsceneAnimationElement(XmlReader *pReader)
    : spriteId("")
{
    pReader->StartElement("DialogCutsceneAnimationElement");
    spriteId = pReader->ReadTextElement("SpriteId");
    pSprite = NULL;
    originalXPosition = pReader->ReadIntElement("OriginalXPosition");
    originalYPosition = pReader->ReadIntElement("OriginalYPosition");
    originalATint = pReader->ReadIntElement("OriginalATint");
    originalRTint = pReader->ReadIntElement("OriginalRTint");
    originalGTint = pReader->ReadIntElement("OriginalGTint");
    originalBTint = pReader->ReadIntElement("OriginalBTint");
    pReader->EndElement();

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

Sprite * DialogCutsceneAnimationElement::GetSprite()
{
    if (pSprite == NULL)
    {
        pSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(spriteId);
    }

    return pSprite;
}

void DialogCutsceneAnimationElement::Update(int delta)
{
    if (pXPositionEase != NULL)
    {
        pXPositionEase->Update(delta);
        xPosition = (int)pXPositionEase->GetCurrentValue();
    }

    if (pYPositionEase != NULL)
    {
        pYPositionEase->Update(delta);
        yPosition = (int)pYPositionEase->GetCurrentValue();
    }

    if (pATintEase != NULL)
    {
        pATintEase->Update(delta);
        aTint = (int)pATintEase->GetCurrentValue();
    }

    if (pRTintEase != NULL)
    {
        pRTintEase->Update(delta);
        rTint = (int)pRTintEase->GetCurrentValue();
    }

    if (pGTintEase != NULL)
    {
        pGTintEase->Update(delta);
        gTint = (int)pGTintEase->GetCurrentValue();
    }

    if (pBTintEase != NULL)
    {
        pBTintEase->Update(delta);
        bTint = (int)pBTintEase->GetCurrentValue();
    }
}

void DialogCutsceneAnimationElement::Draw()
{
    GetSprite()->Draw(Vector2(xPosition, yPosition), Color(rTint / 255.0, gTint / 255.0, bTint / 255.0, aTint / 255.0));
}

void DialogCutsceneAnimationElement::Finish()
{
    if (pXPositionEase != NULL)
    {
        pXPositionEase->Finish();
        xPosition = (int)pXPositionEase->GetCurrentValue();
    }

    if (pYPositionEase != NULL)
    {
        pYPositionEase->Finish();
        yPosition = (int)pYPositionEase->GetCurrentValue();
    }

    if (pATintEase != NULL)
    {
        pATintEase->Finish();
        aTint = (int)pATintEase->GetCurrentValue();
    }

    if (pRTintEase != NULL)
    {
        pRTintEase->Finish();
        rTint = (int)pRTintEase->GetCurrentValue();
    }

    if (pGTintEase != NULL)
    {
        pGTintEase->Finish();
        gTint = (int)pGTintEase->GetCurrentValue();
    }

    if (pBTintEase != NULL)
    {
        pBTintEase->Finish();
        bTint = (int)pBTintEase->GetCurrentValue();
    }
}

void DialogCutsceneAnimationElement::SetTargetPosition(int x, int y, int msDuration)
{
    delete pXPositionEase;
    pXPositionEase = new LinearEase(xPosition, x, msDuration);
    pXPositionEase->Begin();

    delete pYPositionEase;
    pYPositionEase = new LinearEase(yPosition, y, msDuration);
    pYPositionEase->Begin();
}

void DialogCutsceneAnimationElement::SetTargetTint(int a, int r, int g, int b, int msDuration)
{
    delete pATintEase;
    pATintEase = new LinearEase(aTint, a, msDuration);
    pATintEase->Begin();
    delete pRTintEase;
    pRTintEase = new LinearEase(rTint, r, msDuration);
    pRTintEase->Begin();
    delete pGTintEase;
    pGTintEase = new LinearEase(gTint, g, msDuration);
    pGTintEase->Begin();
    delete pBTintEase;
    pBTintEase = new LinearEase(bTint, b, msDuration);
    pBTintEase->Begin();
}

void DialogCutsceneAnimationElement::Reset()
{
    xPosition = originalXPosition;
    yPosition = originalYPosition;

    aTint = originalATint;
    rTint = originalRTint;
    gTint = originalGTint;
    bTint = originalBTint;

    ClearEasingFunctions();
}

void DialogCutsceneAnimationElement::ClearEasingFunctions()
{
    delete pXPositionEase;
    pXPositionEase = NULL;
    delete pYPositionEase;
    pYPositionEase = NULL;

    delete pATintEase;
    pATintEase = NULL;
    delete pRTintEase;
    pRTintEase = NULL;
    delete pGTintEase;
    pGTintEase = NULL;
    delete pBTintEase;
    pBTintEase = NULL;
}

DialogCutsceneAnimationFrame::DialogCutsceneAnimationFrame(XmlReader *pReader)
{
    pReader->StartElement("DialogCutsceneAnimationFrame");
    msDuration = pReader->ReadIntElement("MsDuration");

    pReader->StartElement("PositionChangeHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");

        pReader->StartElement("PositionChange");
        positionChangeHashMap[id] = Vector2(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("TintChangeHashMap");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        string id = pReader->ReadTextElement("Id");

        pReader->StartElement("TintChange");
        tintChangeHashMap[id] = Color(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    if (pReader->ElementExists("NewBackgroundSpriteId"))
    {
        newBackgroundSpriteId = pReader->ReadTextElement("NewBackgroundSpriteId");
    }

    pReader->EndElement();
}

void DialogCutsceneAnimationFrame::Begin()
{
    for (map<string, Vector2>::iterator iter = positionChangeHashMap.begin(); iter != positionChangeHashMap.end(); ++iter)
    {
        DialogCutsceneAnimationElement *pElement = Case::GetInstance()->GetDialogCutsceneManager()->GetElementFromId(iter->first);

        if (pElement != NULL)
        {
            pElement->SetTargetPosition((int)iter->second.GetX(), (int)iter->second.GetY(), msDuration);
        }
    }

    for (map<string, Color>::iterator iter = tintChangeHashMap.begin(); iter != tintChangeHashMap.end(); ++iter)
    {
        DialogCutsceneAnimationElement *pElement = Case::GetInstance()->GetDialogCutsceneManager()->GetElementFromId(iter->first);

        if (pElement != NULL)
        {
            pElement->SetTargetTint(iter->second.GetIntA(), iter->second.GetIntR(), iter->second.GetIntG(), iter->second.GetIntB(), msDuration);
        }
    }
}
