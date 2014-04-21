/**
 * Basic header/include file for Location.cpp.
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

#ifndef LOCATION_H
#define LOCATION_H

#include "Crowd.h"
#include "FieldCharacter.h"
#include "FieldCutscene.h"
#include "ForegroundElement.h"
#include "ZoomedView.h"
#include "../enums.h"
#include "../Vector2.h"
#include "../Events/PromptOverlayEventProvider.h"
#include "../UserInterface/PromptOverlay.h"
#include "../UserInterface/Tab.h"

#include <list>
#include <map>
#include <queue>
#include <stack>
#include <vector>

class FieldCharacter;
class HeightMap;
class XmlReader;
class XmlWriter;

class Location : public PromptOverlayEventListener
{
public:
    class PathfindingThreadParameters
    {
    public:
        PathfindingThreadParameters(Location *pLocation, FieldCharacter *pCharacter, Vector2 startPosition, Vector2 endPosition, FieldCharacterState characterStateIfMoving, int threadId);

        Location *pLocation;
        FieldCharacter *pCharacter;
        Vector2 startPosition;
        Vector2 endPosition;
        FieldCharacterState characterStateIfMoving;
        int threadId;
    };

    class Transition : public InteractiveElement, public ZOrderableObject
    {
        friend class Location;

    public:
        Transition(const string &targetLocationId, const string &targetLocationName)
        {
            this->targetLocationId = targetLocationId;
            this->targetLocationName = targetLocationName;
            this->pTargetLocation = NULL;
            this->pHitBox = NULL;
            this->transitionDirection = TransitionDirectionNorth;
            this->interactionLocation = Vector2(-1, -1);
            this->transitionStartSfxId = "";
            this->transitionEndSfxId = "";
            this->hideWhenLocked = false;
            this->pCondition = NULL;
            this->pEncounter = NULL;
        }

        virtual ~Transition()
        {
            delete pHitBox;
            pHitBox = NULL;
            delete pCondition;
            pCondition = NULL;
            delete pEncounter;
            pEncounter = NULL;
        }

        string GetTargetLocationName() const { return this->targetLocationName; }

        Location * GetTargetLocation();

        HitBox * GetHitBox() { return this->pHitBox; }
        void SetHitBox(HitBox *pHitBox) { this->pHitBox = pHitBox; }

        TransitionDirection GetTransitionDirection() const { return this->transitionDirection; }
        bool HasInteractionLocation() const { return this->interactionLocation.GetX() >= 0 && this->interactionLocation.GetY() >= 0; }
        Vector2 GetInteractionLocation() const { return this->interactionLocation; }
        bool GetHideWhenLocked() const { return this->hideWhenLocked; }
        Condition * GetCondition() { return this->pCondition; }
        Encounter * GetEncounter() { return this->pEncounter; }

        Vector2 GetCenterPoint() { return this->interactionLocation; }
        RectangleWH GetBoundsForInteraction() { return RectangleWH(interactionLocation.GetX(), interactionLocation.GetY(), 1, 1); }
        bool IsInteractionPointExact() { return HasInteractionLocation(); }
        void BeginInteraction(Location *pLocation);

        string GetTransitionStartSfxId() { return this->transitionStartSfxId; }
        string GetTransitionEndSfxId() { return this->transitionEndSfxId; }

        bool IsVisible() { return true; }
        int GetZOrder() { return -1; }
        Line * GetZOrderLine() { return NULL; }
        Vector2 GetZOrderPoint() { return Vector2(-1, -1); }
        void Draw() { }
        void Draw(Vector2 offsetVector) { }

    private:
        Transition(XmlReader *pReader);
        Transition(const Transition &other);

        string targetLocationId;
        string targetLocationName;
        Location *pTargetLocation;
        HitBox *pHitBox;
        TransitionDirection transitionDirection;
        Vector2 interactionLocation;
        string transitionStartSfxId;
        string transitionEndSfxId;
        bool hideWhenLocked;
        Condition *pCondition;
        Encounter *pEncounter;
    };

    class StartPosition
    {
        friend class Location;

    public:
        StartPosition()
        {
            direction = CharacterDirectionNone;
            fieldDirection = FieldCharacterDirectionNone;
        }

        StartPosition(Vector2 position, CharacterDirection direction, FieldCharacterDirection fieldDirection)
        {
            this->position = position;
            this->direction = direction;
            this->fieldDirection = fieldDirection;
        }

        Vector2 GetPosition() const { return this->position; }
        CharacterDirection GetDirection() const { return this->direction; }
        FieldCharacterDirection GetFieldDirection() const { return this->fieldDirection; }

    private:
        StartPosition(XmlReader *pReader);

        void setPosition(Vector2 position) { this->position = position; }
        void setDirection(CharacterDirection direction) { this->direction = direction; }
        void setFieldDirection(FieldCharacterDirection fieldDirection) { this->fieldDirection = fieldDirection; }

        Vector2 position;
        CharacterDirection direction;
        FieldCharacterDirection fieldDirection;
    };

    class LoopingSound
    {
    public:
        LoopingSound(XmlReader *pReader);

        string soundId;
        Vector2 origin;
    };

    Location(XmlReader *pReader);
    Location(const Location &other);
    virtual ~Location();

    static void Initialize(Image *pFadeSprite);

    string GetId() const { return this->id; }
    void SetId(const string &id) { this->id = id; }

    string GetBackgroundSpriteId() const { return this->backgroundSpriteId; }
    void SetBackgroundSpriteId(const string &backgroundSpriteId) { this->backgroundSpriteId = backgroundSpriteId; }

    string GetBgm();
    void SetBgm(const string &bgm) { this->bgm = bgm; }

    string GetAmbianceSfxId();
    void SetAmbianceSfxId(const string &ambianceSfxId) { this->ambianceSfxId = ambianceSfxId; }

    HitBox * GetAreaHitBox() { return this->pAreaHitBox; }
    void SetAreaHitBox(HitBox *pAreaHitBox) { this->pAreaHitBox = pAreaHitBox; }

    vector<FieldCutscene *> * GetCutsceneList();

    vector<string> * GetCutsceneIdList() { return &cutsceneIdList; }
    void SetCutsceneIdList(vector<string> *pCutsceneIdList) { cutsceneIdList = *pCutsceneIdList; }

    vector<HiddenForegroundElement *> * GetHiddenForegroundElementList() { return &hiddenForegroundElementList; }

    bool GetAcceptsUserInput();

    void UpdateLoadedTextures(bool waitUntilLoaded = true);

    void Begin(const string &transitionId);
    void Update(int delta);
    void UpdateTabPositions(int delta);
    void Draw();
    void DrawForScreenshot();
    void Reset();

    void BeginTransition(Location *pTargetLocation, const string &transitionId);
    void CheckForTransitionUnderPlayer();

    void SaveToSaveFile(XmlWriter *pWriter);
    void LoadFromSaveFile(XmlReader *pReader);

    void BeginCharacterInteraction(FieldCharacter *pCharacter);
    void BeginForegroundElementInteraction(ForegroundElement *pForegroundElement);
    void BeginCrowdInteraction(Crowd *pCrowd);

    void OnPromptOverlayValueReturned(PromptOverlay *pSender, const string &value);

private:
    void SetLoopingSoundLevels();
    Sprite * GetBackgroundSprite();
    RectangleWH GetBounds();
    StartPosition GetStartPositionFromTransitionId(const string &transitionId);
    StartPosition * GetPartnerStartPositionFromTransitionId(const string &transitionId);
    void SetTargetInteractiveElement(InteractiveElement *pInteractiveElement, FieldCharacterState characterStateIfMoving);
    bool GetIsPlayerCharacterCloseToInteractiveElement();
    void StartCharacterOnPath(FieldCharacter *pCharacter, Vector2 endPosition, FieldCharacterState characterStateIfMoving, bool doAsync = true);
    void OnExited(Location *pLocation, const string &transitionId);
    static int PerformPathfindingStatic(void *pData);
    void PerformPathfinding(FieldCharacter *pCharacter, Vector2 startPosition, Vector2 endPosition, FieldCharacterState characterStateIfMoving, int threadId);

    static bool CompareByZOrder(ZOrderableObject *pObject1, ZOrderableObject *pObject2)
    {
        return pObject1->GetZOrder() < pObject2->GetZOrder() || (pObject1->GetZOrder() == pObject2->GetZOrder() && pObject2 == pCurrentPlayerCharacter);
    }

    static bool CompareByZOrderDescending(ZOrderableObject *pObject1, ZOrderableObject *pObject2)
    {
        return CompareByZOrder(pObject2, pObject1);
    }

    queue<Vector2> RemoveUnnecessaryStepsFromPath(FieldCharacter *pCharacter, Vector2 startPosition, queue<Vector2> pathPositionQueue);
    bool IsCollisionBetweenTwoPositions(FieldCharacter *pCharacter, Vector2 startPosition, Vector2 endPosition);
    Vector2 FindClosestPassablePositionForCharacter(FieldCharacter *pCharacter, Vector2 position);
    void FindClosestPassablePositionForCharacter(FieldCharacter *pCharacter, Vector2 position, deque<OverlapEntry> *pOverlapEntriesThusFar, stack<Vector2> *pPositionsThusFar, list<Vector2> *pPossiblePositions);

    // Implements the A* search algorithm.
    queue<Vector2> GetPathForCharacterBetweenPoints(FieldCharacter *pCharacter, Vector2 start, Vector2 goal);
    double HeuristicCostEstimate(Vector2 start, Vector2 end);
    bool IsPointInTileAtPoint(Vector2 point, Vector2 tilePoint, double tileSize);
    Vector2 GetVectorWithLowestFScore(list<Vector2> openSet, map<Vector2, double> fScore);
    list<Vector2> GetNeighbors(FieldCharacter *pCharacter, Vector2 position, Vector2 goal, double tileSize);
    bool TestCollisionWithLocationElements(FieldCharacter *pCharacter, Vector2 position);
    bool TestCollisionWithLocationElements(FieldCharacter *pCharacter, Vector2 position, CollisionParameter *pParam);
    queue<Vector2> * ReconstructPath(map<Vector2, Vector2> *pCameFrom, Vector2 currentNode, Vector2 goalNode);
    queue<Vector2> * ReconstructPath(map<Vector2, Vector2> *pCameFrom, Vector2 currentNode);

    static Image *pFadeSprite;
    static FieldCharacter *pCurrentPlayerCharacter;
    static string pendingTransitionEndSfxId;

    RectangleWH bounds;
    Sprite *pBackgroundSprite;
    FieldCharacter *pPlayerCharacter;
    string previousPartnerCharacterId;
    FieldCharacter *pPartnerCharacter;
    vector<FieldCharacter *> characterList;
    vector<Crowd *> crowdList;
    vector<ForegroundElement *> foregroundElementList;
    vector<HiddenForegroundElement *> hiddenForegroundElementList;
    map<string, ZoomedView *> zoomedViewsByIdMap;
    vector<Transition *> transitionList;
    vector<LoopingSound *> loopingSoundList;
    vector<FieldCutscene *> cutsceneList;
    vector<HeightMap *> heightMapList;

    Transition *pTransitionAtPlayer;

    StartPosition startPositionFromMap;
    map<string, StartPosition> transitionIdToStartPositionMap;
    map<string, StartPosition> transitionIdToPartnerStartPositionMap;

    Encounter *pCurrentEncounter;
    Encounter *pEvidenceCombinationEncounter;
    InteractiveElement *pCurrentInteractiveElement;
    ForegroundElement *pCurrentInteractiveForegroundElement;
    Crowd *pCurrentInteractiveCrowd;
    ZoomedView *pCurrentZoomedView;
    FieldCutscene *pCurrentCutscene;
    FieldCutscene *pNextCutscene;

    Tab *pEvidenceTab;
    EvidenceSelector *pEvidenceSelector;

    Tab *pPartnerTab;
    PartnerInformation partnerInformation;
    int animationOffsetPartner;
    EasingFunction *pInEasePartner;
    EasingFunction *pOutEasePartner;
    Tab *pPartnerAbilityTab;

    EasingFunction *pFadeInEase;
    EasingFunction *pFadeOutEase;
    double fadeOpacity;
    Location *pTargetLocation;
    string transitionId;
    bool isTransitioning;
    bool isFinishing;
    string overlayId;
    bool wasInOverlay;

    Tab *pOkTab;

    Tab *pSaveTab;
    Tab *pLoadTab;

    Tab *pOptionsTab;
    Tab *pQuitTab;

    PromptOverlay *pQuitConfirmOverlay;

    InteractiveElement *pTargetInteractiveElement;
    Vector2 clickPoint;

    map<FieldCharacter *, FieldCharacterState> characterStateMap;
    map<FieldCharacter *, queue<Vector2> > characterTargetPositionQueueMap;
    map<FieldCharacter *, Vector2> characterTargetPositionMap;

    bool movingDirectly;

    SDL_Thread *pPathfindingThread;
    SDL_sem *pPathfindingValuesSemaphore;
    int lastPathfindingThreadId;

    Vector2 drawingOffsetVector;

    string id;
    string backgroundSpriteId;
    string bgm;
    string pendingBgm;
    string ambianceSfxId;
    string pendingAmbianceSfxId;

    HitBox *pAreaHitBox;
    vector<string> cutsceneIdList;

    bool shouldAutosave;
};

#endif
