/**
 * Basic header/include file for Encounter.cpp.
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

#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include "../EasingFunctions.h"
#include "../Image.h"
#include "../State.h"
#include "../Events/ButtonArrayEventProvider.h"
#include "../Events/EvidenceSelectorEventProvider.h"
#include "../UserInterface/ButtonArray.h"
#include "../UserInterface/EvidenceSelector.h"
#include "../UserInterface/Tab.h"
#include <map>
#include <vector>

using namespace std;

class Conversation;
class Interrogation;
class Confrontation;
class XmlReader;

class Encounter : public ButtonArrayEventListener, public EvidenceSelectorEventListener
{
public:
    static void Initialize(Image *pDialogBackgroundImage, Image *pBackgroundDarkeningImage);

    Encounter();
    Encounter(XmlReader *pReader);
    virtual ~Encounter();

    string GetId() const { return this->id; }
    void SetId(const string &id) { this->id = id; }

    string GetInitialLeftCharacterId() const { return this->initialLeftCharacterId; }
    void SetInitialLeftCharacterId(const string &initialLeftCharacterId) { this->initialLeftCharacterId = initialLeftCharacterId; }

    string GetInitialLeftCharacterEmotionId() const { return this->initialLeftCharacterEmotionId; }
    void SetInitialLeftCharacterEmotionId(const string &initialLeftCharacterEmotionId) { this->initialLeftCharacterEmotionId = initialLeftCharacterEmotionId; }

    string GetInitialRightCharacterId() const { return this->initialRightCharacterId; }
    void SetInitialRightCharacterId(const string &initialRightCharacterId) { this->initialRightCharacterId = initialRightCharacterId; }

    string GetInitialRightCharacterEmotionId() const { return this->initialRightCharacterEmotionId; }
    void SetInitialRightCharacterEmotionId(const string &initialRightCharacterEmotionId) { this->initialRightCharacterEmotionId = initialRightCharacterEmotionId; }

    Conversation * GetOneShotConversation() { return this->pOneShotConversation; }
    void SetOneShotConversation(Conversation *pOneShotConversation) { this->pOneShotConversation = pOneShotConversation; }

    bool GetOwnsOneShotConversation() const { return this->ownsOneShotConversation; }
    void SetOwnsOneShotConversation(bool ownsOneShotConversation) { this->ownsOneShotConversation = ownsOneShotConversation; }

    Conversation * GetPresentWrongEvidenceConversation() { return this->pPresentWrongEvidenceConversation; }
    void SetPresentWrongEvidenceConversation(Conversation *pPresentWrongEvidenceConversation) { this->pPresentWrongEvidenceConversation = pPresentWrongEvidenceConversation; }

    Conversation * GetPresentWrongProfileConversation() { return this->pPresentWrongProfileConversation; }
    void SetPresentWrongProfileConversation(Conversation *pPresentWrongProfileConversation) { this->pPresentWrongProfileConversation = pPresentWrongProfileConversation; }

    bool GetIsFinished() const { return this->isFinished; }
    void SetIsFinished(bool isFinished) { this->isFinished = isFinished; }

    TransitionRequest GetTransitionRequest() const { return this->transitionRequest; }
    void SetTransitionRequest(TransitionRequest transitionRequest) { this->transitionRequest = transitionRequest; }

    void Begin();
    void Update(int delta);
    void Draw();
    void Reset();

    void OnButtonArrayButtonClicked(ButtonArray *pSender, int id);
    void OnButtonArrayCanceled(ButtonArray *pSender);
    void OnEvidenceSelectorEvidencePresented(EvidenceSelector *pSender, const string &evidenceId);
    void OnEvidenceSelectorClosing(EvidenceSelector *pSender) { }

private:
    void RefreshButtonArrays();
    void RefreshButtonArrayContents();
    void PrepNextButtonArray(ButtonArray *pNextButtonArray, int buttonClickedId);
    void SwapButtonArrays();
    void BeginConversation(Conversation *pConversation);
    void PrepNextConversation(Conversation *pNextConversation, int buttonClickedId);
    void BeginNextConversation();
    void OnMainMenuButtonClicked(int id);
    void InitializeEasingFunctions();
    void Close();

    static Image *pDialogBackgroundImage;
    static Image *pBackgroundDarkeningImage;

    vector<Conversation *> conversationList;
    vector<Interrogation *> interrogationList;
    vector<Confrontation *> confrontationList;
    vector<Conversation *> allConversationList;
    map<string, Conversation *> presentEvidenceConversationDictionary;

    ButtonArray *pMainMenuButtonArray;

    ButtonArray *pNextButtonArray;
    ButtonArray *pCurrentButtonArray;
    ButtonArray *pLastButtonArrayBeforeConversation;
    bool shouldCloseButtonArray;

    State state;
    Conversation *pNextConversation;
    Conversation *pCurrentConversation;

    Tab *pCharacterNameTab;
    Tab *pExitTab;
    Tab *pPresentEvidenceTab;
    EvidenceSelector *pEvidenceSelector;

    EasingFunction *pBackgroundOpacityInEase;
    EasingFunction *pDialogBackgroundInEase;
    EasingFunction *pLeftCharacterInEase;
    EasingFunction *pRightCharacterInEase;
    EasingFunction *pBackgroundOpacityOutEase;
    EasingFunction *pDialogBackgroundOutEase;
    EasingFunction *pLeftCharacterOutEase;
    EasingFunction *pRightCharacterOutEase;

    int backgroundOpacityAlphaValue;
    int dialogBackgroundYPosition;

    string id;
    string initialLeftCharacterId;
    string initialLeftCharacterEmotionId;
    string initialRightCharacterId;
    string initialRightCharacterEmotionId;
    Conversation *pInitialConversation;
    Conversation *pOneShotConversation;
    bool ownsOneShotConversation;
    Conversation *pPresentWrongEvidenceConversation;
    Conversation *pPresentWrongProfileConversation;
    bool isFinished;

    TransitionRequest transitionRequest;

    string lastPresentedEvidenceId;
};

#endif
