/**
 * Basic header/include file for FieldCutscene.cpp.
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

#ifndef FIELDCUTSCENE_H
#define FIELDCUTSCENE_H

#include "Encounter.h"
#include "FieldCharacter.h"
#include "../EasingFunctions.h"
#include "../TransitionRequest.h"
#include "../CaseInformation/SpriteManager.h"
#include <map>
#include <vector>

using namespace std;

class XmlReader;
class XmlWriter;

class FieldCutscene
{
    class FieldCutscenePhase;
    class FieldCutsceneConcurrentMovements;

public:
    FieldCutscene();
    FieldCutscene(XmlReader *pReader);
    ~FieldCutscene();

    string GetId() const { return this->id; }
    void SetId(const string &id) { this->id = id; }

    bool GetIsEnabled() const { return this->isEnabled; }
    void SetIsEnabled(bool isEnabled) { this->isEnabled = isEnabled; }

    bool GetIsFinished() const { return this->isFinished; }
    void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

    bool GetHasCompleted() const { return this->hasCompleted; }
    void SetHasCompleted(bool hasCompleted) { this->hasCompleted = hasCompleted; }

    bool GetPreventsInitialBgm() const { return this->preventsInitialBgm; }
    void SetPreventsInitialBgm(bool preventsInitialBgm) { this->preventsInitialBgm = preventsInitialBgm; }

    string GetInitialBgmReplacement() const { return this->initialBgmReplacement; }
    void SetInitialBgmReplacement(const string &initialBgmReplacement) { this->initialBgmReplacement = initialBgmReplacement; }

    bool GetPreventsInitialAmbiance() const { return this->preventsInitialAmbiance; }
    void SetPreventsInitialAmbiance(bool preventsInitialAmbiance) { this->preventsInitialAmbiance = preventsInitialAmbiance; }

    string GetInitialAmbianceReplacement() const { return this->initialAmbianceReplacement; }
    void SetInitialAmbianceReplacement(const string &initialAmbianceReplacement) { this->initialAmbianceReplacement = initialAmbianceReplacement; }

    void SetBackgroundSprite(Sprite *pBackgroundSprite) { this->pBackgroundSprite = pBackgroundSprite; }
    void SetBackgroundSpriteOpacity(double backgroundSpriteOpacity) { this->backgroundSpriteOpacity = backgroundSpriteOpacity; }
    void SetReplacementBackgroundSprite(Sprite *pReplacementBackgroundSprite) { this->pReplacementBackgroundSprite = pReplacementBackgroundSprite; }
    void SetReplacementBackgroundSpriteOpacity(double replacementBackgroundSpriteOpacity) { this->replacementBackgroundSpriteOpacity = replacementBackgroundSpriteOpacity; }

    Vector2 GetCurrentCameraPosition() const { return this->currentCameraPosition; }

    bool GetHasBegun() const { return this->hasBegun; }
    void SetHasBegun(bool hasBegun) { this->hasBegun = hasBegun; }

    TransitionRequest GetTransitionRequest() const { return this->transitionRequest; }
    void SetTransitionRequest(TransitionRequest transitionRequest) { this->transitionRequest = transitionRequest; }

    void Begin(FieldCharacter *pPartnerCharacter);
    void UpdateCharacters(int delta, vector<HeightMap *> *pHeightMapList);
    void UpdatePhase(int delta);
    void Draw(Vector2 offsetVector, list<ZOrderableObject *> *pObjectsFromLocation);
    void Reset();

private:
    void StartNextPhase();
    FieldCutscenePhase * GetPhaseForNextElement(XmlReader *pReader);

    static bool CompareByZOrder(ZOrderableObject *pObject1, ZOrderableObject *pObject2)
    {
        return pObject1->GetZOrder() < pObject2->GetZOrder();
    }

    vector<FieldCutscenePhase *> phaseList;
    map<string, FieldCharacter *> idToCharacterMap;
    FieldCharacter *pActualPlayerCharacter;
    FieldCharacter *pActualPartnerCharacter;

    map<FieldCharacter *, CharacterDirection> characterToOriginalCharacterDirectionMap;
    map<FieldCharacter *, FieldCharacterDirection> characterToOriginalFieldCharacterDirectionMap;
    map<FieldCharacter *, Vector2> characterToOriginalPositionMap;
    map<FieldCharacter *, FieldCharacterState> characterToOriginalStateMap;

    FieldCutscenePhase *pCurrentPhase;
    unsigned int currentPhaseId;

    FieldCutsceneConcurrentMovements *pCurrentConcurrentMovements;

    Sprite *pBackgroundSprite;
    double backgroundSpriteOpacity;
    Sprite *pReplacementBackgroundSprite;
    double replacementBackgroundSpriteOpacity;

    Vector2 initialCameraPosition;
    Vector2 currentCameraPosition;

    string id;
    bool isEnabled;
    bool isFinished;
    bool hasBegun;
    bool hasCompleted;
    bool preventsInitialBgm;
    string initialBgmReplacement;
    bool preventsInitialAmbiance;
    string initialAmbianceReplacement;

    TransitionRequest transitionRequest;

    class FieldCutscenePhase
    {
    public:
        FieldCutscenePhase()
        {
            pParentCutscene = NULL;
            millisecondDelayBeforeBegin = 0;
            isFinished = false;
        }

        virtual ~FieldCutscenePhase() {}

        bool GetIsFinished() const { return this->isFinished; }
        int GetMillisecondDelayBeforeBegin() const { return this->millisecondDelayBeforeBegin; }
        virtual bool GetAllowsCharacterUpdates() { return true; }
        virtual bool GetShouldUpdateImmediately() { return true; }

        virtual void Begin(FieldCutscene *pParentCutscene)
        {
            SetIsFinished(false);
            this->pParentCutscene = pParentCutscene;
        }

        virtual void Update(int delta) = 0;
        virtual void Draw() = 0;

    protected:
        void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }
        void SetMillisecondDelayBeforeBegin(int millisecondDelayBeforeBegin) { this->millisecondDelayBeforeBegin = millisecondDelayBeforeBegin; }

        FieldCutscene *pParentCutscene;
        int millisecondDelayBeforeBegin;

    private:
        bool isFinished;
    };

    class FieldCutsceneConversation : public FieldCutscenePhase
    {
        friend class FieldCutscene;
        friend class FieldCutsceneConcurrentMovements;

    public:
        FieldCutsceneConversation()
        {
            pEncounter = NULL;
        }

        ~FieldCutsceneConversation()
        {
            delete pEncounter;
            pEncounter = NULL;
        }

        virtual bool GetAllowsCharacterUpdates() { return false; }

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneConversation(XmlReader *pReader);

        Encounter *pEncounter;
    };

    class FieldCutsceneConcurrentMovements : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutsceneConcurrentMovements()
        {
            millisecondsElapsed = 0;
        }

        ~FieldCutsceneConcurrentMovements();

        virtual bool GetShouldUpdateImmediately() { return false; }

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneConcurrentMovements(XmlReader *pReader);
        FieldCutscenePhase * GetPhaseForNextElement(XmlReader *pReader);

        vector<FieldCutscenePhase *> movementList;
        double millisecondsElapsed;
    };

    class FieldCutsceneMovement : public FieldCutscenePhase
    {
        friend class FieldCutscene;
        friend class FieldCutsceneConcurrentMovements;

    public:
        FieldCutsceneMovement()
        {
            movementState = FieldCharacterStateStanding;
            pMovingCharacter = NULL;
        }

        virtual bool GetShouldUpdateImmediately() { return false; }

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneMovement(XmlReader *pReader);

        string characterId;
        Vector2 targetPosition;
        FieldCharacterState movementState;

        FieldCharacter *pMovingCharacter;
        Vector2 normalizedMovementVector;
        Vector2 lastPosition;
    };

    class FieldCutsceneOrient : public FieldCutscenePhase
    {
        friend class FieldCutscene;
        friend class FieldCutsceneConcurrentMovements;

    public:
        FieldCutsceneOrient()
        {
            pCharacter = NULL;
            direction = CharacterDirectionNone;
            spriteDirection = FieldCharacterDirectionNone;
        }

        virtual bool GetShouldUpdateImmediately() { return false; }

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneOrient(XmlReader *pReader);

        string characterId;
        FieldCharacter *pCharacter;
        CharacterDirection direction;
        FieldCharacterDirection spriteDirection;
    };

    class FieldCutsceneSetBackground : public FieldCutscenePhase
    {
        friend class FieldCutscene;
        friend class FieldCutsceneConcurrentMovements;

    public:
        FieldCutsceneSetBackground()
        {
            msFadeDuration = 0;
            pBackgroundSprite = NULL;
            pBackgroundSpriteOpacityEase = NULL;
        }

        ~FieldCutsceneSetBackground();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneSetBackground(XmlReader *pReader);

        string backgroundSpriteId;
        int msFadeDuration;

        Sprite *pBackgroundSprite;
        EasingFunction *pBackgroundSpriteOpacityEase;
    };

    class FieldCutsceneCenterCamera : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutsceneCenterCamera()
        {
            pCameraMovementXEase = NULL;
            pCameraMovementYEase = NULL;
        }

        ~FieldCutsceneCenterCamera();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneCenterCamera(XmlReader *pReader);

        string characterId;
        Vector2 cameraCenterPosition;
        EasingFunction *pCameraMovementXEase;
        EasingFunction *pCameraMovementYEase;
    };

    class FieldCutscenePause : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutscenePause()
        {
            msPauseDuration = 0;
            msElapsedTime = 0;
        }

        ~FieldCutscenePause();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutscenePause(XmlReader *pReader);

        int msPauseDuration;
        int msElapsedTime;
    };

    class FieldCutscenePlayBgm : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutscenePlayBgm()
        {
            bgmId = "";
        }

        ~FieldCutscenePlayBgm();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutscenePlayBgm(XmlReader *pReader);

        string bgmId;
    };

    class FieldCutsceneStopBgm : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutsceneStopBgm()
        {
            isInstant = false;
        }

        ~FieldCutsceneStopBgm();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneStopBgm(XmlReader *pReader);

        bool isInstant;
    };

    class FieldCutscenePlayAmbiance : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutscenePlayAmbiance()
        {
            ambianceSfxId = "";
        }

        ~FieldCutscenePlayAmbiance();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutscenePlayAmbiance(XmlReader *pReader);

        string ambianceSfxId;
    };

    class FieldCutsceneStopAmbiance : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutsceneStopAmbiance()
        {
            isInstant = false;
        }

        ~FieldCutsceneStopAmbiance();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneStopAmbiance(XmlReader *pReader);

        bool isInstant;
    };

    class FieldCutsceneEndCase : public FieldCutscenePhase
    {
        friend class FieldCutscene;

    public:
        FieldCutsceneEndCase()
        {
        }

        ~FieldCutsceneEndCase();

        virtual void Begin(FieldCutscene *pParentCutscene);
        virtual void Update(int delta);
        virtual void Draw();

    private:
        FieldCutsceneEndCase(XmlReader *pReader);

        bool completesCase;
    };
};

#endif
