#ifndef INTERACTIONLOCATIONMANIPULATOR_H
#define INTERACTIONLOCATIONMANIPULATOR_H

#include "LocationElementManipulator.h"
#include "CaseCreator/CaseContent/InteractionLocation.h"

class InteractionLocationManipulator : public LocationElementManipulator<InteractionLocation>
{
    Q_OBJECT
public:
    static InteractionLocationManipulator * Create(QWidget *parent = 0);

protected:
    explicit InteractionLocationManipulator(QWidget *parent = 0);

    Encounter * GetEncounter() override;
    bool SupportsFullEncounters() override;
};

#endif // INTERACTIONLOCATIONMANIPULATOR_H
