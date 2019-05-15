#ifndef LOCATIONCUTSCENESPAGE_H
#define LOCATIONCUTSCENESPAGE_H

#include "../BaseTypes/Page.h"
#include "../TemplateHelpers/ListManipulator.h"

#include "CaseCreator/CaseContent/Location.h"

class LocationCutscenesPage : public Page<Location>
{
    Q_OBJECT

public:
    LocationCutscenesPage(QWidget *parent = 0);

    void Init(Location *pObject);
    void Reset();
    bool Undo();
    bool Redo();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    ListManipulator<FieldCutscene> *pCutsceneManipulator;
    bool isActive;
};

#endif // LOCATIONCUTSCENESPAGE_H
