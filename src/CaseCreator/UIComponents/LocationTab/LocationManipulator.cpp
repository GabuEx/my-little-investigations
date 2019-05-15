#include "LocationManipulator.h"

#include <QLabel>
#include <QScrollArea>

#include <QVBoxLayout>

#include "CaseCreator/CaseContent/Location.h"
#include "LocationOverviewPage.h"
#include "LocationCutscenesPage.h"
#include "LocationZoomedViewsPage.h"
#include "LocationForegroundElementsPage.h"

template<>
ObjectManipulator<Location> * ObjectManipulator<Location>::Create(QWidget *parent)
{
    return new LocationManipulator(parent);
}

LocationManipulator::LocationManipulator(QWidget *parent) :
    ObjectManipulator<Location>(parent)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();
    pTabWidget = new PageTabWidget<Location>();

    pTabWidget->AddPage(new LocationOverviewPage(), "Overview");
    pTabWidget->AddPage(new LocationCutscenesPage(), "Cutscenes");
    pTabWidget->AddPage(new LocationZoomedViewsPage(), "Zoomed Views");

    pMainLayout->addWidget(pTabWidget);
    setLayout(pMainLayout);
}

void LocationManipulator::Init(Location *pObject)
{
    ObjectManipulator<Location>::Init(pObject);

    pTabWidget->Init(pObject);
}

void LocationManipulator::Reset()
{
    pTabWidget->Reset();
}

bool LocationManipulator::Undo()
{
    return pTabWidget->Undo();
}

bool LocationManipulator::Redo()
{
    return pTabWidget->Redo();
}

bool LocationManipulator::GetIsActive()
{
    return isActive;
}

void LocationManipulator::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pTabWidget->SetIsActive(isActive);
    }
}
