/**
 * Static class to aid in the handling of mouse operations.
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

#include "MouseHelper.h"
#include "globals.h"
#include "ResourceLoader.h"
#include "Image.h"
#include "CaseInformation/CommonCaseResources.h"
#include "CaseInformation/Case.h"
#include "UserInterface/Tab.h"

const Uint32 ClickTimeThresholdMs = 250;
const int ClickDistanceThresholdPx = 5;
const Uint32 DoubleClickTimeThresholdMs = 250;

const int MouseOverTextMarginPx = 5;
const int MouseOverTextEaseDurationMs = 250;
const int MouseOverTextMaxOffsetPx = 25;

const string cursorSpriteSheetId = "__CursorSpriteSheet";

const string normalCursorSpriteId01 = "__NormalCursor01";

const string normalCursorAnimationId = "__NormalCursorAnimation";

const string lookCursorSpriteId01 = "__LookCursor01";
const string lookCursorSpriteId02 = "__LookCursor02";
const string lookCursorSpriteId03 = "__LookCursor03";
const string lookCursorSpriteId04 = "__LookCursor04";
const string lookCursorSpriteId05 = "__LookCursor05";

const string lookCursorAnimationId = "__LookCursorAnimation";

const string talkCursorSpriteId01 = "__TalkCursor01";
const string talkCursorSpriteId02 = "__TalkCursor02";
const string talkCursorSpriteId03 = "__TalkCursor03";
const string talkCursorSpriteId04 = "__TalkCursor04";

const string talkCursorAnimationId = "__TalkCursorAnimation";

const string exitNorthCursorSpriteId01 = "__ExitNorthCursor01";
const string exitNorthCursorSpriteId02 = "__ExitNorthCursor02";
const string exitNorthCursorSpriteId03 = "__ExitNorthCursor03";
const string exitNorthCursorSpriteId04 = "__ExitNorthCursor04";
const string exitNorthCursorSpriteId05 = "__ExitNorthCursor05";
const string exitNorthCursorSpriteId06 = "__ExitNorthCursor06";
const string exitNorthCursorSpriteId07 = "__ExitNorthCursor07";
const string exitNorthCursorSpriteId08 = "__ExitNorthCursor08";

const string exitNorthCursorAnimationId = "__ExitNorthCursorAnimation";

const string exitNorthEastCursorSpriteId01 = "__ExitNorthEastCursor01";
const string exitNorthEastCursorSpriteId02 = "__ExitNorthEastCursor02";
const string exitNorthEastCursorSpriteId03 = "__ExitNorthEastCursor03";
const string exitNorthEastCursorSpriteId04 = "__ExitNorthEastCursor04";
const string exitNorthEastCursorSpriteId05 = "__ExitNorthEastCursor05";
const string exitNorthEastCursorSpriteId06 = "__ExitNorthEastCursor06";
const string exitNorthEastCursorSpriteId07 = "__ExitNorthEastCursor07";
const string exitNorthEastCursorSpriteId08 = "__ExitNorthEastCursor08";

const string exitNorthEastCursorAnimationId = "__ExitNorthEastCursorAnimation";

const string exitEastCursorSpriteId01 = "__ExitEastCursor01";
const string exitEastCursorSpriteId02 = "__ExitEastCursor02";
const string exitEastCursorSpriteId03 = "__ExitEastCursor03";
const string exitEastCursorSpriteId04 = "__ExitEastCursor04";
const string exitEastCursorSpriteId05 = "__ExitEastCursor05";
const string exitEastCursorSpriteId06 = "__ExitEastCursor06";
const string exitEastCursorSpriteId07 = "__ExitEastCursor07";
const string exitEastCursorSpriteId08 = "__ExitEastCursor08";

const string exitEastCursorAnimationId = "__ExitEastCursorAnimation";

const string exitSouthEastCursorSpriteId01 = "__ExitSouthEastCursor01";
const string exitSouthEastCursorSpriteId02 = "__ExitSouthEastCursor02";
const string exitSouthEastCursorSpriteId03 = "__ExitSouthEastCursor03";
const string exitSouthEastCursorSpriteId04 = "__ExitSouthEastCursor04";
const string exitSouthEastCursorSpriteId05 = "__ExitSouthEastCursor05";
const string exitSouthEastCursorSpriteId06 = "__ExitSouthEastCursor06";
const string exitSouthEastCursorSpriteId07 = "__ExitSouthEastCursor07";
const string exitSouthEastCursorSpriteId08 = "__ExitSouthEastCursor08";

const string exitSouthEastCursorAnimationId = "__ExitSouthEastCursorAnimation";

const string exitSouthCursorSpriteId01 = "__ExitSouthCursor01";
const string exitSouthCursorSpriteId02 = "__ExitSouthCursor02";
const string exitSouthCursorSpriteId03 = "__ExitSouthCursor03";
const string exitSouthCursorSpriteId04 = "__ExitSouthCursor04";
const string exitSouthCursorSpriteId05 = "__ExitSouthCursor05";
const string exitSouthCursorSpriteId06 = "__ExitSouthCursor06";
const string exitSouthCursorSpriteId07 = "__ExitSouthCursor07";
const string exitSouthCursorSpriteId08 = "__ExitSouthCursor08";

const string exitSouthCursorAnimationId = "__ExitSouthCursorAnimation";

const string exitSouthWestCursorSpriteId01 = "__ExitSouthWestCursor01";
const string exitSouthWestCursorSpriteId02 = "__ExitSouthWestCursor02";
const string exitSouthWestCursorSpriteId03 = "__ExitSouthWestCursor03";
const string exitSouthWestCursorSpriteId04 = "__ExitSouthWestCursor04";
const string exitSouthWestCursorSpriteId05 = "__ExitSouthWestCursor05";
const string exitSouthWestCursorSpriteId06 = "__ExitSouthWestCursor06";
const string exitSouthWestCursorSpriteId07 = "__ExitSouthWestCursor07";
const string exitSouthWestCursorSpriteId08 = "__ExitSouthWestCursor08";

const string exitSouthWestCursorAnimationId = "__ExitSouthWestCursorAnimation";

const string exitWestCursorSpriteId01 = "__ExitWestCursor01";
const string exitWestCursorSpriteId02 = "__ExitWestCursor02";
const string exitWestCursorSpriteId03 = "__ExitWestCursor03";
const string exitWestCursorSpriteId04 = "__ExitWestCursor04";
const string exitWestCursorSpriteId05 = "__ExitWestCursor05";
const string exitWestCursorSpriteId06 = "__ExitWestCursor06";
const string exitWestCursorSpriteId07 = "__ExitWestCursor07";
const string exitWestCursorSpriteId08 = "__ExitWestCursor08";

const string exitWestCursorAnimationId = "__ExitWestCursorAnimation";

const string exitNorthWestCursorSpriteId01 = "__ExitNorthWestCursor01";
const string exitNorthWestCursorSpriteId02 = "__ExitNorthWestCursor02";
const string exitNorthWestCursorSpriteId03 = "__ExitNorthWestCursor03";
const string exitNorthWestCursorSpriteId04 = "__ExitNorthWestCursor04";
const string exitNorthWestCursorSpriteId05 = "__ExitNorthWestCursor05";
const string exitNorthWestCursorSpriteId06 = "__ExitNorthWestCursor06";
const string exitNorthWestCursorSpriteId07 = "__ExitNorthWestCursor07";
const string exitNorthWestCursorSpriteId08 = "__ExitNorthWestCursor08";

const string exitNorthWestCursorAnimationId = "__ExitNorthWestCursorAnimation";

const string exitDoorCursorSpriteId01 = "__ExitDoorCursor01";
const string exitDoorCursorSpriteId02 = "__ExitDoorCursor02";
const string exitDoorCursorSpriteId03 = "__ExitDoorCursor03";
const string exitDoorCursorSpriteId04 = "__ExitDoorCursor04";
const string exitDoorCursorSpriteId05 = "__ExitDoorCursor05";
const string exitDoorCursorSpriteId06 = "__ExitDoorCursor06";
const string exitDoorCursorSpriteId07 = "__ExitDoorCursor07";
const string exitDoorCursorSpriteId08 = "__ExitDoorCursor08";

const string exitDoorCursorAnimationId = "__ExitDoorCursorAnimation";

const string noExitCursorSpriteId01 = "__NoExitCursor01";
const string noExitCursorSpriteId02 = "__NoExitCursor02";

const string noExitCursorAnimationId = "__NoExitCursorAnimation";

const int exitAnimationNormalFrameDurationMs = 126;
const int exitAnimationHoldFrameDurationMs = 630;

bool MouseHelper::clickPossible = false;
bool MouseHelper::clicked = false;
bool MouseHelper::doubleClickPossible = false;
bool MouseHelper::doubleClickWasPossible = false;
bool MouseHelper::doubleClicked = false;
bool MouseHelper::previousWasLeftButtonDown = false;
Vector2 MouseHelper::previousMousePosition = Vector2(0, 0);
bool MouseHelper::drawCursor = false;

Uint32 MouseHelper::initialLeftButtonDownTime = 0;
Vector2 MouseHelper::initialLeftButtonDownPosition = Vector2(0, 0);
Uint32 MouseHelper::initialClickTime = 0;

map<CursorType, Animation *> MouseHelper::animationByCursorTypeMap;
Animation *MouseHelper::pCurrentCursorAnimation = NULL;
CursorType MouseHelper::cursorTypeToUpdateTo = CursorTypeNormal;
CursorType MouseHelper::currentCursorType = CursorTypeNormal;

string MouseHelper::mouseOverText = "";
vector<MouseHelper::MouseOverText *> MouseHelper::currentMouseOverTextList;

void MouseHelper::Init()
{
    clickPossible = false;
    clicked = false;
    doubleClickPossible = false;
    doubleClickWasPossible = false;
    doubleClicked = false;
    previousWasLeftButtonDown = false;
    previousMousePosition = Vector2(0, 0);
    drawCursor = false;
    pCurrentCursorAnimation = NULL;
}

void MouseHelper::LoadCursors()
{
    Image *pCursorSpriteSheet = ResourceLoader::GetInstance()->LoadImage("image/CursorSpriteSheet.png");

    //// NORMAL CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddImage(cursorSpriteSheetId, pCursorSpriteSheet);

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(normalCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   0, 30, 30));

    Animation *pNormalCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(normalCursorAnimationId, &pNormalCursorAnimation);
    pNormalCursorAnimation->AddFrame(5000, normalCursorSpriteId01);

    animationByCursorTypeMap[CursorTypeNormal] = pNormalCursorAnimation;

    //// LOOK CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lookCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   30, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lookCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  30, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lookCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  30, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lookCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  30, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(lookCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 30, 30, 30));

    Animation *pLookCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(lookCursorAnimationId, &pLookCursorAnimation);
    pLookCursorAnimation->AddFrame(1932, lookCursorSpriteId01);
    pLookCursorAnimation->AddFrame(84, lookCursorSpriteId02);
    pLookCursorAnimation->AddFrame(84, lookCursorSpriteId03);
    pLookCursorAnimation->AddFrame(84, lookCursorSpriteId04);
    pLookCursorAnimation->AddFrame(84, lookCursorSpriteId05);

    animationByCursorTypeMap[CursorTypeLook] = pLookCursorAnimation;

    //// TALK CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(talkCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   60, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(talkCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  60, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(talkCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  60, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(talkCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  60, 30, 30));

    Animation *pTalkCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(talkCursorAnimationId, &pTalkCursorAnimation);
    pTalkCursorAnimation->AddFrame(210, talkCursorSpriteId01);
    pTalkCursorAnimation->AddFrame(210, talkCursorSpriteId02);
    pTalkCursorAnimation->AddFrame(210, talkCursorSpriteId03);
    pTalkCursorAnimation->AddFrame(500, talkCursorSpriteId04);

    animationByCursorTypeMap[CursorTypeTalk] = pTalkCursorAnimation;

    //// EXIT NORTH CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   90, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  90, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  90, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  90, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 90, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 90, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 90, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 90, 30, 30));

    Animation *pExitNorthCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitNorthCursorAnimationId, &pExitNorthCursorAnimation);
    pExitNorthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthCursorSpriteId01);
    pExitNorthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthCursorSpriteId02);
    pExitNorthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthCursorSpriteId03);
    pExitNorthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthCursorSpriteId04);
    pExitNorthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthCursorSpriteId05);
    pExitNorthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthCursorSpriteId06);
    pExitNorthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthCursorSpriteId07);
    pExitNorthCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitNorthCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitNorth] = pExitNorthCursorAnimation;

    //// EXIT NORTHEAST CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   120, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  120, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  120, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  120, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 120, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 120, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 120, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthEastCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 120, 30, 30));

    Animation *pExitNorthEastCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitNorthEastCursorAnimationId, &pExitNorthEastCursorAnimation);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthEastCursorSpriteId01);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthEastCursorSpriteId02);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthEastCursorSpriteId03);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthEastCursorSpriteId04);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthEastCursorSpriteId05);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthEastCursorSpriteId06);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthEastCursorSpriteId07);
    pExitNorthEastCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitNorthEastCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitNorthEast] = pExitNorthEastCursorAnimation;

    //// EXIT EAST CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   150, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  150, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  150, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  150, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 150, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 150, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 150, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitEastCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 150, 30, 30));

    Animation *pExitEastCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitEastCursorAnimationId, &pExitEastCursorAnimation);
    pExitEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitEastCursorSpriteId01);
    pExitEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitEastCursorSpriteId02);
    pExitEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitEastCursorSpriteId03);
    pExitEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitEastCursorSpriteId04);
    pExitEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitEastCursorSpriteId05);
    pExitEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitEastCursorSpriteId06);
    pExitEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitEastCursorSpriteId07);
    pExitEastCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitEastCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitEast] = pExitEastCursorAnimation;

    //// EXIT SOUTHEAST CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   180, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  180, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  180, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  180, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 180, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 180, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 180, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthEastCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 180, 30, 30));

    Animation *pExitSouthEastCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitSouthEastCursorAnimationId, &pExitSouthEastCursorAnimation);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthEastCursorSpriteId01);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthEastCursorSpriteId02);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthEastCursorSpriteId03);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthEastCursorSpriteId04);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthEastCursorSpriteId05);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthEastCursorSpriteId06);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthEastCursorSpriteId07);
    pExitSouthEastCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitSouthEastCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitSouthEast] = pExitSouthEastCursorAnimation;

    //// EXIT SOUTH CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   210, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  210, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  210, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  210, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 210, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 210, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 210, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 210, 30, 30));

    Animation *pExitSouthCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitSouthCursorAnimationId, &pExitSouthCursorAnimation);
    pExitSouthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthCursorSpriteId01);
    pExitSouthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthCursorSpriteId02);
    pExitSouthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthCursorSpriteId03);
    pExitSouthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthCursorSpriteId04);
    pExitSouthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthCursorSpriteId05);
    pExitSouthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthCursorSpriteId06);
    pExitSouthCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthCursorSpriteId07);
    pExitSouthCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitSouthCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitSouth] = pExitSouthCursorAnimation;

    //// EXIT SOUTHWEST CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   240, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  240, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  240, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  240, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 240, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 240, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 240, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitSouthWestCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 240, 30, 30));

    Animation *pExitSouthWestCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitSouthWestCursorAnimationId, &pExitSouthWestCursorAnimation);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthWestCursorSpriteId01);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthWestCursorSpriteId02);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthWestCursorSpriteId03);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthWestCursorSpriteId04);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthWestCursorSpriteId05);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthWestCursorSpriteId06);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitSouthWestCursorSpriteId07);
    pExitSouthWestCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitSouthWestCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitSouthWest] = pExitSouthWestCursorAnimation;

    //// EXIT WEST CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   270, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  270, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  270, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  270, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 270, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 270, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 270, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitWestCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 270, 30, 30));

    Animation *pExitWestCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitWestCursorAnimationId, &pExitWestCursorAnimation);
    pExitWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitWestCursorSpriteId01);
    pExitWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitWestCursorSpriteId02);
    pExitWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitWestCursorSpriteId03);
    pExitWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitWestCursorSpriteId04);
    pExitWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitWestCursorSpriteId05);
    pExitWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitWestCursorSpriteId06);
    pExitWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitWestCursorSpriteId07);
    pExitWestCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitWestCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitWest] = pExitWestCursorAnimation;

    //// EXIT NORTHWEST CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   300, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  300, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId03, cursorSpriteSheetId, RectangleWH(60,  300, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId04, cursorSpriteSheetId, RectangleWH(90,  300, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId05, cursorSpriteSheetId, RectangleWH(120, 300, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId06, cursorSpriteSheetId, RectangleWH(150, 300, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId07, cursorSpriteSheetId, RectangleWH(180, 300, 30, 30));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitNorthWestCursorSpriteId08, cursorSpriteSheetId, RectangleWH(210, 300, 30, 30));

    Animation *pExitNorthWestCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitNorthWestCursorAnimationId, &pExitNorthWestCursorAnimation);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthWestCursorSpriteId01);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthWestCursorSpriteId02);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthWestCursorSpriteId03);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthWestCursorSpriteId04);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthWestCursorSpriteId05);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthWestCursorSpriteId06);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitNorthWestCursorSpriteId07);
    pExitNorthWestCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitNorthWestCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitNorthWest] = pExitNorthWestCursorAnimation;

    //// EXIT DOOR CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   330, 40, 40));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId02, cursorSpriteSheetId, RectangleWH(40,  330, 40, 40));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId03, cursorSpriteSheetId, RectangleWH(80,  330, 40, 40));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId04, cursorSpriteSheetId, RectangleWH(120, 330, 40, 40));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId05, cursorSpriteSheetId, RectangleWH(160, 330, 40, 40));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId06, cursorSpriteSheetId, RectangleWH(200, 330, 40, 40));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId07, cursorSpriteSheetId, RectangleWH(240, 330, 40, 40));
    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(exitDoorCursorSpriteId08, cursorSpriteSheetId, RectangleWH(280, 330, 40, 40));

    Animation *pExitDoorCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(exitDoorCursorAnimationId, &pExitDoorCursorAnimation);
    pExitDoorCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, exitDoorCursorSpriteId01);
    pExitDoorCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitDoorCursorSpriteId02);
    pExitDoorCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitDoorCursorSpriteId03);
    pExitDoorCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitDoorCursorSpriteId04);
    pExitDoorCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitDoorCursorSpriteId05);
    pExitDoorCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitDoorCursorSpriteId06);
    pExitDoorCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitDoorCursorSpriteId07);
    pExitDoorCursorAnimation->AddFrame(exitAnimationNormalFrameDurationMs, exitDoorCursorSpriteId08);

    animationByCursorTypeMap[CursorTypeExitDoor] = pExitDoorCursorAnimation;

    //// NO EXIT CURSOR ////

    CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(noExitCursorSpriteId01, cursorSpriteSheetId, RectangleWH(0,   370, 30, 30));
    //CommonCaseResources::GetInstance()->GetSpriteManager()->AddSprite(noExitCursorSpriteId02, cursorSpriteSheetId, RectangleWH(30,  370, 30, 30));

    Animation *pNoExitCursorAnimation = NULL;
    CommonCaseResources::GetInstance()->GetAnimationManager()->AddAnimation(noExitCursorAnimationId, &pNoExitCursorAnimation);
    pNoExitCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, noExitCursorSpriteId01);
    //pNoExitCursorAnimation->AddFrame(exitAnimationHoldFrameDurationMs, noExitCursorSpriteId02);

    animationByCursorTypeMap[CursorTypeNoExit] = pNoExitCursorAnimation;

    pCurrentCursorAnimation = animationByCursorTypeMap[CursorTypeNormal];
    pCurrentCursorAnimation->Begin();
}

void MouseHelper::UpdateState(bool isLeftButtonDown, int mouseX, int mouseY, bool drawCursor)
{
    bool currentWasLeftButtonDown = isLeftButtonDown;

    // If we're in full-screen mode, then we need to account for the screen scale.
    if (gIsFullscreen)
    {
        mouseX = (int)(mouseX / gScreenScale);
        mouseY = (int)(mouseY / gScreenScale);
    }

    previousMousePosition.SetX(mouseX);
    previousMousePosition.SetY(mouseY);

    UpdateTiming();

    if (!currentWasLeftButtonDown && previousWasLeftButtonDown)
    {
        // If a click is still possible now that the button has gone up,
        // and we weren't already expecting a double-click,
        // then we have a click, and we'll start considering the possibility
        // of a double-click. If on the other hand a double-click was expected,
        // then we now have a double-click.
        clicked = clickPossible && !doubleClickPossible;
        doubleClicked = clickPossible && doubleClickPossible;
        doubleClickPossible = clickPossible && !doubleClickPossible;
        clickPossible = false;

        if (doubleClickPossible)
        {
            initialClickTime = SDL_GetTicks();
        }
    }
    else if (currentWasLeftButtonDown && !previousWasLeftButtonDown)
    {
        initialLeftButtonDownTime = SDL_GetTicks();
        initialLeftButtonDownPosition = previousMousePosition;
        clickPossible = true;
        doubleClickWasPossible = doubleClickPossible;
    }

    previousWasLeftButtonDown = currentWasLeftButtonDown;
    MouseHelper::drawCursor = drawCursor;
}

void MouseHelper::UpdateTiming()
{
    if (clickPossible)
    {
        // We should only mark this as a click if the up
        // comes in fast enough following the down, and if the
        // cursor didn't move very far.
        clickPossible =
                SDL_GetTicks() - initialLeftButtonDownTime <= ClickTimeThresholdMs &&
                (previousMousePosition - initialLeftButtonDownPosition).Length() <= ClickDistanceThresholdPx;
    }

    if (doubleClickPossible)
    {
        // We should only mark this as a double-click if the
        // second click comes in fast enough following the first.
        doubleClickPossible = SDL_GetTicks() - initialClickTime <= DoubleClickTimeThresholdMs;
    }
}

void MouseHelper::UpdateCursor(int delta)
{
    if (pCurrentCursorAnimation != NULL)
    {
        pCurrentCursorAnimation->Update(delta);
    }
    else if (currentCursorType == CursorTypeCustom)
    {
        Case::GetInstance()->GetPartnerManager()->UpdateCursor(delta);
    }

    bool mouseOverTextFound = false;

    if (currentMouseOverTextList.size() > 0)
    {
        for (unsigned int i = 0; i < currentMouseOverTextList.size(); i++)
        {
            currentMouseOverTextList[i]->Update(delta);

            if (mouseOverText != currentMouseOverTextList[i]->GetText() && !currentMouseOverTextList[i]->GetIsFadingOut())
            {
                currentMouseOverTextList[i]->FadeOut();
            }
            else if (mouseOverText.length() > 0 && mouseOverText == currentMouseOverTextList[i]->GetText())
            {
                mouseOverTextFound = true;
            }
        }

        for (int i = (int)currentMouseOverTextList.size() - 1; i >= 0; i--)
        {
            if (currentMouseOverTextList[i]->GetIsFinished())
            {
                MouseOverText *pMouseOverText = currentMouseOverTextList[i];
                currentMouseOverTextList.erase(currentMouseOverTextList.begin() + i);
                delete pMouseOverText;
            }
        }
    }

    // Add new mouse over text if necessary.
    if (!mouseOverTextFound && mouseOverText.length() > 0)
    {
        currentMouseOverTextList.push_back(new MouseOverText(mouseOverText));
    }
}

void MouseHelper::DrawCursor()
{
    if (drawCursor && (pCurrentCursorAnimation != NULL || currentCursorType == CursorTypeCustom))
    {
        Vector2 cursorPosition = previousMousePosition;

        if (currentCursorType == CursorTypeCustom)
        {
            cursorPosition += Case::GetInstance()->GetPartnerManager()->GetCursorDrawingOffset();
        }

        if (pCurrentCursorAnimation != NULL)
        {
            pCurrentCursorAnimation->Draw(cursorPosition);
        }
        else if (currentCursorType == CursorTypeCustom)
        {
            Case::GetInstance()->GetPartnerManager()->DrawCursor(cursorPosition);
        }

        if (currentMouseOverTextList.size() > 0)
        {
            for (unsigned int i = 0; i < currentMouseOverTextList.size(); i++)
            {
                currentMouseOverTextList[i]->Draw(cursorPosition);
            }
        }
    }
}

CursorType MouseHelper::GetCursorType()
{
    return cursorTypeToUpdateTo;
}

void MouseHelper::SetCursorType(CursorType cursorType)
{
    cursorTypeToUpdateTo = cursorType;
}

void MouseHelper::ApplyCursorUpdate()
{
    if (cursorTypeToUpdateTo != currentCursorType)
    {
        currentCursorType = cursorTypeToUpdateTo;
        pCurrentCursorAnimation = currentCursorType != CursorTypeCustom ? animationByCursorTypeMap[currentCursorType] : NULL;

        if (pCurrentCursorAnimation != NULL)
        {
            pCurrentCursorAnimation->Begin();
        }
    }
}

bool MouseHelper::MouseOverRect(RectangleWH rectangle)
{
    return
        !clicked &&
        !previousWasLeftButtonDown &&
        IsMouseInRect(rectangle);
}

bool MouseHelper::MouseDownOnRect(RectangleWH rectangle)
{
    return
        previousWasLeftButtonDown &&
        IsMouseInRect(rectangle);
}

bool MouseHelper::MouseOverPolygon(GeometricPolygon polygon)
{
    return
        !clicked &&
        !previousWasLeftButtonDown &&
        IsMouseInPolygon(polygon);
}

bool MouseHelper::MouseDownOnPolygon(GeometricPolygon polygon)
{
    return
        previousWasLeftButtonDown &&
        IsMouseInPolygon(polygon);
}

bool MouseHelper::PressedAndHeldAnywhere()
{
    return
        previousWasLeftButtonDown &&
        !clickPossible &&
        !doubleClickWasPossible &&
        IsMouseInViewport();
}

bool MouseHelper::DoublePressedAndHeldAnywhere()
{
    return
        previousWasLeftButtonDown &&
        !clickPossible &&
        doubleClickWasPossible &&
        IsMouseInViewport();
}

bool MouseHelper::ClickedOnRect(RectangleWH rectangle)
{
    return
        clicked &&
        IsMouseInRect(rectangle);
}

bool MouseHelper::ClickedOnPolygon(GeometricPolygon polygon)
{
    return
        clicked &&
        IsMouseInPolygon(polygon);
}

bool MouseHelper::ClickedAnywhere()
{
    return
        clicked &&
        IsMouseInViewport();
}

bool MouseHelper::DoubleClickedOnRect(RectangleWH rectangle)
{
    return
        doubleClicked &&
        IsMouseInRect(rectangle);
}

bool MouseHelper::DoubleClickedOnPolygon(GeometricPolygon polygon)
{
    return
        doubleClicked &&
        IsMouseInPolygon(polygon);
}

bool MouseHelper::DoubleClickedAnywhere()
{
    return
        doubleClicked &&
        IsMouseInViewport();
}

Vector2 MouseHelper::GetMousePosition()
{
    return previousMousePosition;
}

Vector2 MouseHelper::GetCursorSize()
{
    if (pCurrentCursorAnimation != NULL)
    {
        return pCurrentCursorAnimation->GetSize();
    }
    else if (currentCursorType == CursorTypeCustom)
    {
        return Case::GetInstance()->GetPartnerManager()->GetCursorSize();
    }
    else
    {
        return Vector2(0, 0);
    }
}

void MouseHelper::HandleClick()
{
    clicked = false;
}

void MouseHelper::HandleDoubleClick()
{
    doubleClicked = false;
}

bool MouseHelper::IsMouseInRect(RectangleWH rectangle)
{
    return
        previousMousePosition.GetX() >= rectangle.GetX() &&
        previousMousePosition.GetX() <= rectangle.GetX() + rectangle.GetWidth() &&
        previousMousePosition.GetY() >= rectangle.GetY() &&
        previousMousePosition.GetY() <= rectangle.GetY() + rectangle.GetHeight();
}

bool MouseHelper::IsMouseInPolygon(GeometricPolygon polygon)
{
    return polygon.Contains(previousMousePosition);
}

bool MouseHelper::IsMouseInViewport()
{
    return IsMouseInRect(RectangleWH(0, 0, gScreenWidth, gScreenHeight));
}

MouseHelper::MouseOverText::MouseOverText(const string &text)
    : text(text)
    , fadeOutMousePosition(Vector2(0, 0))
{
    this->currentOffset = -MouseOverTextMaxOffsetPx;
    this->currentOpacity = 0;
    this->pOffsetInEase = new QuadraticEase(-MouseOverTextMaxOffsetPx, 0, MouseOverTextEaseDurationMs);
    this->pOffsetOutEase = NULL;
    this->pOpacityInEase = new LinearEase(0, 1, MouseOverTextEaseDurationMs);
    this->pOpacityOutEase = NULL;
    this->isFadingOut = false;

    this->pOffsetInEase->Begin();
    this->pOpacityInEase->Begin();
}

MouseHelper::MouseOverText::MouseOverText(const MouseOverText &other)
    : text(other.text)
    , fadeOutMousePosition(Vector2(0, 0))
{
    this->currentOffset = -MouseOverTextMaxOffsetPx;
    this->currentOpacity = 0;
    this->pOffsetInEase = new QuadraticEase(-MouseOverTextMaxOffsetPx, 0, MouseOverTextEaseDurationMs);
    this->pOffsetOutEase = NULL;
    this->pOpacityInEase = new LinearEase(0, 1, MouseOverTextEaseDurationMs);
    this->pOpacityOutEase = NULL;
    this->isFadingOut = false;

    this->pOffsetInEase->Begin();
    this->pOpacityInEase->Begin();
}

MouseHelper::MouseOverText::~MouseOverText()
{
    delete pOffsetInEase;
    pOffsetInEase = NULL;
    delete pOffsetOutEase;
    pOffsetOutEase = NULL;
    delete pOpacityInEase;
    pOpacityInEase = NULL;
    delete pOpacityOutEase;
    pOpacityOutEase = NULL;
}

void MouseHelper::MouseOverText::Update(int delta)
{
    if (pOpacityInEase != NULL && !pOpacityInEase->GetIsFinished())
    {
        pOffsetInEase->Update(delta);
        currentOffset = pOffsetInEase->GetCurrentValue();
        pOpacityInEase->Update(delta);
        currentOpacity = pOpacityInEase->GetCurrentValue();
    }
    else if (pOpacityOutEase != NULL && pOpacityOutEase->GetIsStarted() && !pOpacityOutEase->GetIsFinished())
    {
        pOffsetOutEase->Update(delta);
        currentOffset = pOffsetOutEase->GetCurrentValue();
        pOpacityOutEase->Update(delta);
        currentOpacity = pOpacityOutEase->GetCurrentValue();
    }
}

void MouseHelper::MouseOverText::Draw(Vector2 cursorPosition)
{
    MLIFont *pMouseOverFont = CommonCaseResources::GetInstance()->GetFontManager()->GetFontFromId("MouseOverFont");
    Vector2 cursorSize = GetCursorSize();
    Vector2 mouseOverTextPosition = cursorPosition + Vector2(cursorSize.GetX() / 2, cursorSize.GetY()) - Vector2(pMouseOverFont->GetWidth(text) / 2, 0);

    if (mouseOverTextPosition.GetX() < MouseOverTextMarginPx)
    {
        mouseOverTextPosition.SetX(MouseOverTextMarginPx);
    }
    else if (mouseOverTextPosition.GetX() + pMouseOverFont->GetWidth(text) > gScreenWidth - MouseOverTextMarginPx)
    {
        mouseOverTextPosition.SetX(gScreenWidth - MouseOverTextMarginPx - pMouseOverFont->GetWidth(text));
    }

    if (mouseOverTextPosition.GetY() + pMouseOverFont->GetHeight(text) > gScreenHeight - TabHeight - MouseOverTextMarginPx)
    {
        mouseOverTextPosition.SetY(mouseOverTextPosition.GetY() - cursorSize.GetY() - pMouseOverFont->GetHeight(text));
    }

    if (mouseOverTextPosition.GetY() + pMouseOverFont->GetHeight(text) > gScreenHeight - TabHeight - MouseOverTextMarginPx)
    {
        mouseOverTextPosition.SetY(gScreenHeight - TabHeight - MouseOverTextMarginPx - pMouseOverFont->GetHeight(text));
    }

    // We don't want the mouse over text to continue to follow the mouse cursor in the y-direction
    // after it starts fading out, so we'll set the current mouse over text position such that
    // we can retrieve the last y-position prior to fading out as its permanent y-position
    // until the fade-out has completed.
    if (!isFadingOut)
    {
        fadeOutMousePosition = mouseOverTextPosition;
    }
    else
    {
        mouseOverTextPosition.SetY(fadeOutMousePosition.GetY());
    }

    mouseOverTextPosition += Vector2(0, currentOffset);

    pMouseOverFont->Draw(text, mouseOverTextPosition, Color(currentOpacity, 1.0, 1.0, 1.0));
}

void MouseHelper::MouseOverText::FadeOut()
{
    double startOffset = pOffsetInEase->GetCurrentValue();
    double startOpacity = pOpacityInEase->GetCurrentValue();
    pOffsetOutEase = new QuadraticEase(startOffset, startOffset + startOpacity * MouseOverTextMaxOffsetPx, (int)(startOpacity * MouseOverTextEaseDurationMs));
    pOffsetOutEase->Begin();
    pOpacityOutEase = new LinearEase(startOpacity, 0, (int)(startOpacity * MouseOverTextEaseDurationMs));
    pOpacityOutEase->Begin();
    pOffsetInEase->Finish();
    pOpacityInEase->Finish();

    isFadingOut = true;
}

bool MouseHelper::MouseOverText::GetIsFinished()
{
    return pOpacityOutEase != NULL && pOpacityOutEase->GetIsFinished();
}
