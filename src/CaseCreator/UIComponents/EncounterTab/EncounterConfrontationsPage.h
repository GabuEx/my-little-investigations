#ifndef ENCOUNTERCONFRONTATIONSPAGE_H
#define ENCOUNTERCONFRONTATIONSPAGE_H

#include "../BaseTypes/Page.h"
#include "../TemplateHelpers/ListManipulator.h"

#include "CaseCreator/CaseContent/Location.h"

class EncounterConfrontationsPage : public Page<Encounter>
{
    Q_OBJECT

public:
    EncounterConfrontationsPage(QWidget *parent = 0);

    void Init(Encounter *pObject);
    void Reset();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    ListManipulator<Confrontation> *pConfrontationManipulator;
    bool isActive;
};

#endif // ENCOUNTERCONFRONTATIONSPAGE_H
