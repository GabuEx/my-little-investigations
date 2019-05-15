#include "InteractionLocationManipulator.h"

template<>
ObjectManipulator<InteractionLocation> * ObjectManipulator<InteractionLocation>::Create(QWidget *parent)
{
    return InteractionLocationManipulator::Create(parent);
}

InteractionLocationManipulator * InteractionLocationManipulator::Create(QWidget *parent)
{
    InteractionLocationManipulator *pManipulator = new InteractionLocationManipulator(parent);
    pManipulator->CreateVisualTree();
    return pManipulator;
}

InteractionLocationManipulator::InteractionLocationManipulator(QWidget *parent) :
    LocationElementManipulator<InteractionLocation>(parent)
{
}

Encounter * InteractionLocationManipulator::GetEncounter()
{
    return pObject->GetEncounter();
}

bool InteractionLocationManipulator::SupportsFullEncounters()
{
    return false;
}
