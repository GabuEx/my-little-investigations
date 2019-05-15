#ifndef FOREGROUNDELEMENTMANIPULATOR_H
#define FOREGROUNDELEMENTMANIPULATOR_H

#include "LocationElementManipulator.h"
#include "CaseCreator/CaseContent/ForegroundElement.h"

class ForegroundElementManipulator : public LocationElementManipulator<ForegroundElement>
{
    Q_OBJECT
public:
    static ForegroundElementManipulator * Create(QWidget *parent = 0);

protected:
    explicit ForegroundElementManipulator(QWidget *parent = 0);

    Encounter * GetEncounter() override;
    bool SupportsFullEncounters() override;
};

#endif // FOREGROUNDELEMENTMANIPULATOR_H
