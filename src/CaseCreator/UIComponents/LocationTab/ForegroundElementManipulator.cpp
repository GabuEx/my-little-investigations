#include "ForegroundElementManipulator.h"

template<>
ObjectManipulator<ForegroundElement> * ObjectManipulator<ForegroundElement>::Create(QWidget *parent)
{
    return ForegroundElementManipulator::Create(parent);
}

ForegroundElementManipulator * ForegroundElementManipulator::Create(QWidget *parent)
{
    ForegroundElementManipulator *pManipulator = new ForegroundElementManipulator(parent);
    pManipulator->CreateVisualTree();
    return pManipulator;
}

ForegroundElementManipulator::ForegroundElementManipulator(QWidget *parent) :
    LocationElementManipulator<ForegroundElement>(parent)
{
}

Encounter * ForegroundElementManipulator::GetEncounter()
{
    return pObject->GetEncounter();
}

bool ForegroundElementManipulator::SupportsFullEncounters()
{
    return false;
}
