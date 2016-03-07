/**
 * Represents a closeup view of something in the field.
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

#include "ZoomedView.h"
#include "../globals.h"
#include "../MouseHelper.h"
#include "../XmlReader.h"
#include "../CaseInformation/Case.h"

ZoomedView::ZoomedView(XmlReader *pReader)
{
    pBackgroundSprite = NULL;
    pCurrentInteractiveForegroundElement = NULL;
    isFinished = false;

    pExitTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, gpLocalizableContent->GetText("ZoomedView/ExitText"), true /* useCancelClickSoundEffect */, TabRowBottom);

    pReader->StartElement("ZoomedView");
    id = pReader->ReadTextElement("Id");
    backgroundSpriteId = pReader->ReadTextElement("BackgroundSpriteId");

    pReader->StartElement("ForegroundElementList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        foregroundElementList.push_back(new ForegroundElement(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

ZoomedView::ZoomedView(const ZoomedView &other)
    : id(other.id)
    , backgroundSpriteId(other.backgroundSpriteId)
{
    pBackgroundSprite = NULL;
    pCurrentInteractiveForegroundElement = NULL;
    isFinished = false;

    pExitTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, gpLocalizableContent->GetText("ZoomedView/ExitText"), true /* useCancelClickSoundEffect */, TabRowBottom);
}

ZoomedView::~ZoomedView()
{
    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        delete foregroundElementList[i];
    }

    delete pExitTab;
    pExitTab = NULL;
}

void ZoomedView::Begin()
{
    SetIsFinished(false);
}

void ZoomedView::Update(int delta)
{
    if (pCurrentInteractiveForegroundElement != NULL)
    {
        pCurrentInteractiveForegroundElement->UpdateInteraction(delta);

        if (pCurrentInteractiveForegroundElement->GetIsInteractionFinished())
        {
            pCurrentInteractiveForegroundElement = NULL;
        }
        else
        {
            return;
        }
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        foregroundElementList[i]->Update(delta);
    }

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        ForegroundElement *pForegroundElement = foregroundElementList[i];

        if (pForegroundElement->GetIsClicked() && pForegroundElement->GetIsInteractive())
        {
            pForegroundElement->BeginInteraction();
            pCurrentInteractiveForegroundElement = pForegroundElement;
            return;
        }
    }

    pExitTab->Update();

    if (pExitTab->GetIsClicked())
    {
        SetIsFinished(true);
    }
}

void ZoomedView::Draw()
{
    GetBackgroundSprite()->Draw(Vector2(0, 0));

    for (unsigned int i = 0; i < foregroundElementList.size(); i++)
    {
        foregroundElementList[i]->Draw();
    }

    if (pCurrentInteractiveForegroundElement == NULL)
    {
        pExitTab->Draw();
    }
    else
    {
        pCurrentInteractiveForegroundElement->DrawInteraction();
    }
}

Sprite * ZoomedView::GetBackgroundSprite()
{
    if (pBackgroundSprite == NULL)
    {
        pBackgroundSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(GetBackgroundSpriteId());
    }

    return pBackgroundSprite;
}
