/**
 * Handles an array of buttons in character encounters.
 * Also contains definitions of the buttons themselves.
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

#include "ButtonArray.h"
#include "../mli_audio.h"
#include "../MouseHelper.h"
#include "../ResourceLoader.h"
#include "../CaseInformation/Case.h"
#include "../CaseInformation/CommonCaseResources.h"

#include <algorithm>

const int ButtonCascadeDelay = 125;

MLIFont *Button::pTextFont = NULL;
Image *Button::pCheckMarkImage = NULL;
Sprite *Button::pLockSprite = NULL;
Animation *Button::pUnlockingAnimation = NULL;

MLIFont *ButtonArray::pTextFont = NULL;

const Color NormalTextColor = Color(1.0, 1.0, 1.0, 1.0);
const Color MouseOverTextColor = Color(1.0, 1.0, 1.0, 0.0);
const Color MouseDownTextColor = Color(1.0, 1.0, 0.0, 0.0);
const Color DisabledTextColor = Color(1.0, 0.5, 0.5, 0.5);

const string lockSpriteSheetId = "__LockSpriteSheet";

const string lockAnimationSpriteId01 = "__LockAnimationSprite01";
const string lockAnimationSpriteId02 = "__LockAnimationSprite02";
const string lockAnimationSpriteId03 = "__LockAnimationSprite03";
const string lockAnimationSpriteId04 = "__LockAnimationSprite04";
const string lockAnimationSpriteId05 = "__LockAnimationSprite05";
const string lockAnimationSpriteId06 = "__LockAnimationSprite06";
const string lockAnimationSpriteId07 = "__LockAnimationSprite07";
const string lockAnimationSpriteId08 = "__LockAnimationSprite08";
const string lockAnimationSpriteId09 = "__LockAnimationSprite09";
const string lockAnimationSpriteId10 = "__LockAnimationSprite10";
const string lockAnimationSpriteId11 = "__LockAnimationSprite11";
const string lockAnimationSpriteId12 = "__LockAnimationSprite12";
const string lockAnimationSpriteId13 = "__LockAnimationSprite13";
const string lockAnimationSpriteId14 = "__LockAnimationSprite14";
const string lockAnimationSpriteId15 = "__LockAnimationSprite15";
const string lockAnimationSpriteId16 = "__LockAnimationSprite16";
const string lockAnimationSpriteId17 = "__LockAnimationSprite17";
const string lockAnimationSpriteId18 = "__LockAnimationSprite18";
const string lockAnimationSpriteId19 = "__LockAnimationSprite19";
const string lockAnimationSpriteId20 = "__LockAnimationSprite20";

const string lockAnimationId = "__LockAnimation";

void Button::Initialize(MLIFont *pTextFont, Image *pCheckMarkImage)
{
    Button::pTextFont = pTextFont;
    Button::pCheckMarkImage = pCheckMarkImage;

    Image *pLockSpriteSheet = ResourceLoader::GetInstance()->LoadImage("image/LockSpriteSheet.png");

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddImage(lockSpriteSheetId, pLockSpriteSheet);

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId01, lockSpriteSheetId, RectangleWH(0,   0,   40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId02, lockSpriteSheetId, RectangleWH(40,  0,   40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId03, lockSpriteSheetId, RectangleWH(80,  0,   40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId04, lockSpriteSheetId, RectangleWH(120, 0,   40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId05, lockSpriteSheetId, RectangleWH(160, 0,   40, 36));

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId06, lockSpriteSheetId, RectangleWH(0,   36,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId07, lockSpriteSheetId, RectangleWH(40,  36,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId08, lockSpriteSheetId, RectangleWH(80,  36,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId09, lockSpriteSheetId, RectangleWH(120, 36,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId10, lockSpriteSheetId, RectangleWH(160, 36,  40, 36));

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId11, lockSpriteSheetId, RectangleWH(0,   72,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId12, lockSpriteSheetId, RectangleWH(40,  72,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId13, lockSpriteSheetId, RectangleWH(80,  72,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId14, lockSpriteSheetId, RectangleWH(120, 72,  40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId15, lockSpriteSheetId, RectangleWH(160, 72,  40, 36));

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId16, lockSpriteSheetId, RectangleWH(0,   108, 40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId17, lockSpriteSheetId, RectangleWH(40,  108, 40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId18, lockSpriteSheetId, RectangleWH(80,  108, 40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId19, lockSpriteSheetId, RectangleWH(120, 108, 40, 36));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lockAnimationSpriteId20, lockSpriteSheetId, RectangleWH(160, 108, 40, 36));

    pLockSprite = CommonCaseResources::GetInstance()->GetSpriteManager()->GetSpriteFromId(lockAnimationSpriteId01);

    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(lockAnimationId, &pUnlockingAnimation);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId01);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId02);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId03);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId04);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId05);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId06);
    pUnlockingAnimation->AddSound("Unlocking");
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId07);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId08);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId09);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId10);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId11);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId12);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId13);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId14);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId15);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId16);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId17);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId18);
    pUnlockingAnimation->AddFrame(42, lockAnimationSpriteId19);
    pUnlockingAnimation->AddFrame(0, lockAnimationSpriteId20);
}

void Button::SetCustomIconId(string customIconId)
{
    this->customIconId = customIconId;

    if (customIconId.length() > 0)
    {
        pCustomIconSprite = Case::GetInstance()->GetSpriteManager()->GetSpriteFromId(customIconId);
    }
    else
    {
        pCustomIconSprite = NULL;
    }
}

bool Button::GetIsReady()
{
    return
        this->pInEase->GetIsFinished() &&
        !this->pOutEase->GetIsStarted() &&
        (GetUnlockedLockCount() == 0 || pUnlockingAnimation->IsFinished());
}

void Button::Show()
{
    Reset();
    SetIsHidden(false);

    if (GetUnlockedLockCount() > 0)
    {
        pUnlockingAnimation->Begin();
    }

    pInEase->Begin();
}

void Button::ShowInstantly()
{
    Reset();
    SetIsHidden(false);

    if (GetUnlockedLockCount() > 0)
    {
        pUnlockingAnimation->Finish();
    }

    pInEase->Finish();
    animationOffset = (int)pInEase->GetCurrentValue();
}

void Button::Hide()
{
    pOutEase->Begin();
}

void Button::Update(int delta)
{
    isMouseOver = false;
    isMouseDown = false;

    if (pOutEase->GetIsStarted() && !pOutEase->GetIsFinished())
    {
        pOutEase->Update(delta);
        animationOffset = (int)pOutEase->GetCurrentValue();
    }
    else if (pOutEase->GetIsFinished())
    {
        SetIsHidden(true);
    }
    else if (pInEase->GetIsStarted() && !pInEase->GetIsFinished())
    {
        pInEase->Update(delta);
        animationOffset = (int)pInEase->GetCurrentValue();
    }
    else
    {
        if (GetUnlockedLockCount() > 0 && !pUnlockingAnimation->IsFinished())
        {
            pUnlockingAnimation->Update(delta);

            AnimationSound * pSoundToPlay = pUnlockingAnimation->GetSoundToPlay();

            if (pSoundToPlay != NULL)
            {
                pSoundToPlay->Play(gSoundEffectsVolume);
            }
        }
        else if (!GetIsDisabled())
        {
            RectangleWH positionRect = RectangleWH(GetXPosition(), GetYPosition(), pTextFont->GetWidth(GetText()), TextHeight);
            bool isPressed = MouseHelper::PressedAndHeldAnywhere() || MouseHelper::DoublePressedAndHeldAnywhere();

            if (MouseHelper::ClickedOnRect(positionRect))
            {
                OnClicked();
            }

            if (MouseHelper::MouseDownOnRect(positionRect) && !isPressed)
            {
                isMouseDown = true;
            }
            else if (MouseHelper::MouseOverRect(positionRect) && !isPressed)
            {
                isMouseOver = true;
            }
        }
    }
}

void Button::Draw()
{
    Draw(0, 0);
}

void Button::Draw(double xOffset, double yOffset)
{
    Color textColor;

    if (GetIsDisabled())
    {
        textColor = DisabledTextColor;
    }
    else if (isMouseDown)
    {
        textColor = MouseDownTextColor;
    }
    else if (isMouseOver)
    {
        textColor = MouseOverTextColor;
    }
    else
    {
        textColor = NormalTextColor;
    }

    Vector2 textPosition = Vector2(
            GetXPosition() + animationOffset + xOffset,
            GetYPosition() + yOffset);

    pTextFont->Draw(GetText(), textPosition, textColor);

    if (pCustomIconSprite != NULL)
    {
        Vector2 spritePosition =
            Vector2(
                textPosition.GetX() - pCustomIconSprite->GetWidth() - 4,
                textPosition.GetY() - 4 + (36 - pCustomIconSprite->GetHeight()) / 2);

        pCustomIconSprite->Draw(spritePosition);
    }
    else if (GetLockCount() > 0 || GetUnlockedLockCount() > 0)
    {
        int lockPositionOffset = 0;

        for (int i = 0; i < GetLockCount(); i++)
        {
            lockPositionOffset += pLockSprite->GetWidth() + 2;

            Vector2 lockPosition =
                Vector2(
                    textPosition.GetX() - lockPositionOffset,
                    textPosition.GetY() - 9);

            pLockSprite->Draw(lockPosition);
        }

        for (int i = 0; i < GetUnlockedLockCount(); i++)
        {
            lockPositionOffset += pUnlockingAnimation->GetSize().GetX() + 2;

            Vector2 lockPosition =
                Vector2(
                    textPosition.GetX() - lockPositionOffset,
                    textPosition.GetY() - 9);

            pUnlockingAnimation->Draw(lockPosition);
        }
    }
    else if (GetShowCheckMark())
    {
        Vector2 checkMarkPosition =
            Vector2(
                textPosition.GetX() - pCheckMarkImage->width,
                textPosition.GetY() - 4);

        pCheckMarkImage->Draw(checkMarkPosition);
    }
}

void Button::Reset()
{
    SetIsHidden(true);
    animationOffset = gScreenWidth;
    pInEase->Reset();
    pOutEase->Reset();
    isMouseOver = false;
    isMouseDown = false;
}


void Button::OnClicked()
{
    EventProviders::GetButtonEventProvider()->RaiseButtonClicked(this, id);
    playSound(GetClickSoundEffect());
}

void ButtonArray::Initialize(MLIFont *pTextFont)
{
    ButtonArray::pTextFont = pTextFont;
}

ButtonArray::ButtonArray()
    : dialogRect(RectangleWH(3, 370, 954, 167))
{
    this->buttonClicked = false;
    this->topButtonIndex = 0;
    this->maxVisibleButtonCount = 2;
    this->visibleButtonCount = 0;
    this->desiredPadding = 30;
    ppVisibleButtons = new Button*[maxVisibleButtonCount];
    pUpArrow = new Arrow((int)dialogRect.GetX() + (int)dialogRect.GetWidth() / 2 - 12, (int)dialogRect.GetY() + 10, ArrowDirectionUp, 10 /* bounceDistance */, true /* isClickable */);
    pDownArrow = new Arrow((int)dialogRect.GetX() + (int)dialogRect.GetWidth() / 2 - 12, (int)dialogRect.GetY() + (int)dialogRect.GetHeight() - 20 - 10, ArrowDirectionDown, 10 /* bounceDistance */, true /* isClickable */);
    pBackTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, "BACK", true /* isCancel */);
    isCancelable = false;
}

ButtonArray::ButtonArray(int maxVisibleButtonCount, int x, int y, int width, int height, int desiredPadding)
    : dialogRect(RectangleWH(x, y, width, height))
{
    this->buttonClicked = false;
    this->topButtonIndex = 0;
    this->maxVisibleButtonCount = maxVisibleButtonCount;
    this->visibleButtonCount = 0;
    this->desiredPadding = desiredPadding;
    ppVisibleButtons = new Button*[maxVisibleButtonCount];
    pUpArrow = new Arrow((int)dialogRect.GetX() + (int)dialogRect.GetWidth() / 2 - 12, (int)dialogRect.GetY() + 10, ArrowDirectionUp, 10 /* bounceDistance */, true /* isClickable */);
    pDownArrow = new Arrow((int)dialogRect.GetX() + (int)dialogRect.GetWidth() / 2 - 12, (int)dialogRect.GetY() + (int)dialogRect.GetHeight() - 20 - 10, ArrowDirectionDown, 10 /* bounceDistance */, true /* isClickable */);
    pBackTab = new Tab(gScreenWidth - TabWidth, true /* isClickable */, "BACK", true /* isCancel */);
    isCancelable = false;
}

ButtonArray::~ButtonArray()
{
    for (unsigned int i = 0; i < buttonList.size(); i++)
    {
        delete buttonList[i];
    }

    delete pUpArrow;
    pUpArrow = NULL;
    delete pDownArrow;
    pDownArrow = NULL;
    delete pBackTab;
    pBackTab = NULL;

    delete [] ppVisibleButtons;

    EventProviders::GetButtonEventProvider()->ClearListener(this);
}

bool ButtonArray::GetIsClosed()
{
    bool allButtonsHidden = true;

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        if (!ppVisibleButtons[i]->GetIsHidden())
        {
            allButtonsHidden = false;
            break;
        }
    }

    return allButtonsHidden;
}

bool ButtonArray::GetIsReady()
{
    bool allButtonsReady = true;

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        if (!ppVisibleButtons[i]->GetIsReady())
        {
            allButtonsReady = false;
            break;
        }
    }

    return allButtonsReady;
}

void ButtonArray::Load(vector<ButtonArrayLoadParameters> loadParametersList)
{
    Reset();

    if (loadParametersList.size() == 0)
    {
        // If we have no actual buttons, then load a dummy button.
        Button *pButton = new Button(0, "(no conversation options)");
        pButton->SetIsDisabled(true);
        buttonList.push_back(pButton);
    }
    else
    {
        for (unsigned int i = 0; i < loadParametersList.size(); i++)
        {
            ButtonArrayLoadParameters loadParameters = loadParametersList[i];

            Button *pButton = new Button(i, loadParameters.text);

            if (loadParameters.isDisabled)
            {
                pButton->SetIsDisabled(true);
            }
            else
            {
                pButton->SetShowCheckMark(loadParameters.showCheckBox);
                pButton->SetLockCount(loadParameters.lockCount);
                pButton->SetUnlockedLockCount(loadParameters.unlockedLockCount);
                pButton->SetCustomIconId(loadParameters.customIconId);
            }

            buttonList.push_back(pButton);
        }
    }
}

void ButtonArray::Show()
{
    buttonClicked = false;

    // If there's a button with a lock being opened,
    // show that one initially.
    // Otherwise, if there's a button with no icon next to it,
    // show that one initially instead.
    bool foundUnlockedLock = false;
    topButtonIndex = 0;

    for (unsigned int i = 0; i < buttonList.size(); i++)
    {
        if (buttonList[i]->GetUnlockedLockCount() > 0)
        {
            topButtonIndex = i;
            foundUnlockedLock = true;
            break;
        }
    }

    if (!foundUnlockedLock)
    {
        for (unsigned int i = 0; i < buttonList.size(); i++)
        {
            if (!buttonList[i]->GetShowCheckMark() && buttonList[i]->GetLockCount() == 0)
            {
                topButtonIndex = i;
                break;
            }
        }
    }

    UpdateButtons();
    EventProviders::GetButtonEventProvider()->ClearListener(this);
    EventProviders::GetButtonEventProvider()->RegisterListener(this);

    for (unsigned int i = 0; i < buttonList.size(); i++)
    {
        Button *pButton = buttonList[i];

        if (i >= topButtonIndex && i < topButtonIndex + visibleButtonCount)
        {
            pButton->SetInAnimationDelay(ButtonCascadeDelay * (i - topButtonIndex));
            pButton->SetOutAnimationDelay(ButtonCascadeDelay * (i - topButtonIndex));
            pButton->Show();
        }
        else
        {
            pButton->ShowInstantly();
        }
    }
}

void ButtonArray::Close()
{
    EventProviders::GetButtonEventProvider()->ClearListener(this);

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        ppVisibleButtons[i]->Hide();
    }
}

void ButtonArray::Update(int delta)
{
    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        ppVisibleButtons[i]->Update(delta);
    }

    if (GetIsReady())
    {
        if (GetIsCancelable() && !buttonClicked)
        {
            pBackTab->Update();

            if (pBackTab->GetIsClicked())
            {
                OnCanceled();
            }
        }

        if (buttonList.size() > visibleButtonCount && visibleButtonCount > 0)
        {
            pUpArrow->Update(delta);
            pDownArrow->Update(delta);
        }

        if (pUpArrow->GetIsClicked() || pDownArrow->GetIsClicked())
        {
            if (pUpArrow->GetIsClicked())
            {
                topButtonIndex--;
            }
            else
            {
                topButtonIndex++;
            }

            UpdateButtons();
        }
    }
}

void ButtonArray::Draw()
{
    Draw(0, 0);
}

void ButtonArray::Draw(double xOffset, double yOffset)
{
    if (GetIsCancelable())
    {
        pBackTab->Draw(xOffset, yOffset);
    }

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        ppVisibleButtons[i]->Draw(xOffset, yOffset);
    }

    bool allButtonsReady = true;

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        if (!ppVisibleButtons[i]->GetIsReady())
        {
            allButtonsReady = false;
            break;
        }
    }

    if (allButtonsReady && buttonList.size() > visibleButtonCount && visibleButtonCount > 0)
    {
        pUpArrow->Draw(xOffset, yOffset);
        pDownArrow->Draw(xOffset, yOffset);
    }
}

void ButtonArray::ReorderOutAnimations(int newFirstOutButtonId)
{
    // If we only have one single visible button,
    // no sense worrying about animation order.
    if (visibleButtonCount == 1)
    {
        ppVisibleButtons[0]->SetOutAnimationDelay(0);
        return;
    }

    int firstOutPosition = -1;

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        if (newFirstOutButtonId == ppVisibleButtons[i]->GetId())
        {
            firstOutPosition = i;
            break;
        }
    }

    if (firstOutPosition == -1)
    {
        throw Exception("Something very bad has happened - a button not being shown has been clicked.");
    }

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        ppVisibleButtons[(firstOutPosition + i) % visibleButtonCount]->SetOutAnimationDelay(ButtonCascadeDelay * i);
    }
}

void ButtonArray::Reset()
{
    pBackTab->Reset();

    for (unsigned int i = 0; i < buttonList.size(); i++)
    {
        delete buttonList[i];
    }

    buttonList.clear();
    buttonClicked = false;
    visibleButtonCount = 0;
}

void ButtonArray::OnButtonClicked(Button *pButton, int id)
{
    EventProviders::GetButtonArrayEventProvider()->RaiseButtonArrayButtonClicked(this, id);
    buttonClicked = true;
}

void ButtonArray::UpdateButtons()
{
    if (buttonList.size() == 1)
    {
        topButtonIndex = 0;
    }
    else if (topButtonIndex > (int)buttonList.size() - maxVisibleButtonCount)
    {
        topButtonIndex = (int)(buttonList.size() - maxVisibleButtonCount);
    }

    visibleButtonCount = min((unsigned int)buttonList.size(), maxVisibleButtonCount);

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        ppVisibleButtons[i] = buttonList[topButtonIndex + i];
    }

    for (unsigned int i = visibleButtonCount; i < maxVisibleButtonCount; i++)
    {
        ppVisibleButtons[i] = NULL;
    }

    int paddedX = (int)(dialogRect.GetX() + desiredPadding);
    int paddedY = (int)(dialogRect.GetY() + desiredPadding);
    int availableWidth = (int)(dialogRect.GetWidth() - desiredPadding * 2);
    int availableHeight = (int)(dialogRect.GetHeight() - desiredPadding * 2);

    for (unsigned int i = 0; i < visibleButtonCount; i++)
    {
        ppVisibleButtons[i]->SetXPosition(paddedX + availableWidth / 2 - (int)pTextFont->GetWidth(ppVisibleButtons[i]->GetText()) / 2);
        ppVisibleButtons[i]->SetYPosition(paddedY + availableHeight * (i + 1) / (visibleButtonCount + 1) - TextHeight / 2);
    }

    pUpArrow->SetIsEnabled(topButtonIndex > 0);
    pDownArrow->SetIsEnabled(topButtonIndex < (int)buttonList.size() - visibleButtonCount);
}

void ButtonArray::OnCanceled()
{
    EventProviders::GetButtonArrayEventProvider()->RaiseButtonArrayCanceled(this);
    buttonClicked = true;
}
