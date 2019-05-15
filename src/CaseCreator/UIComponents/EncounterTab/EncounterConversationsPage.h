#ifndef ENCOUNTERCONVERSATIONSPAGE_H
#define ENCOUNTERCONVERSATIONSPAGE_H

#include "../BaseTypes/Page.h"
#include "../TemplateHelpers/ListManipulator.h"

#include "CaseCreator/CaseContent/Location.h"

class EncounterConversationsPage : public Page<Encounter>
{
    Q_OBJECT

public:
    EncounterConversationsPage(QWidget *parent = 0);

    void Init(Encounter *pObject);
    void Reset();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    ListManipulator<Conversation> *pConversationManipulator;
    bool isActive;
};

#endif // ENCOUNTERCONVERSATIONSPAGE_H
