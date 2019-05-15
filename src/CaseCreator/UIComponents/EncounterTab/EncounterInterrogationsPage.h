#ifndef ENCOUNTERINTERROGATIONSPAGE_H
#define ENCOUNTERINTERROGATIONSPAGE_H

#include "../BaseTypes/Page.h"
#include "../TemplateHelpers/ListManipulator.h"

#include "CaseCreator/CaseContent/Location.h"

class EncounterInterrogationsPage : public Page<Encounter>
{
    Q_OBJECT

public:
    EncounterInterrogationsPage(QWidget *parent = 0);

    void Init(Encounter *pObject);
    void Reset();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    ListManipulator<Interrogation> *pInterrogationManipulator;
    bool isActive;
};

#endif // ENCOUNTERINTERROGATIONSPAGE_H
