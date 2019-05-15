#include "EncounterManipulator.h"

#include "EncounterConversationsPage.h"
#include "EncounterInterrogationsPage.h"
#include "EncounterConfrontationsPage.h"
#include "CaseCreator/UIComponents/BaseTypes/PageTabWidget.h"

#include <QVBoxLayout>

template<>
ObjectManipulator<Encounter> * ObjectManipulator<Encounter>::Create(QWidget *parent)
{
    return new EncounterManipulator(parent);
}

EncounterManipulator::EncounterManipulator(QWidget *parent) :
    ObjectManipulator<Encounter>(parent)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();
    pTabWidget = new PageTabWidget<Encounter>();

    pTabWidget->AddPage(new EncounterConversationsPage(), "Conversations");
    pTabWidget->AddPage(new EncounterInterrogationsPage(), "Interrogations");
    pTabWidget->AddPage(new EncounterConfrontationsPage(), "Confrontations");

    pMainLayout->addWidget(pTabWidget);
    setLayout(pMainLayout);

    isActive = false;
}

void EncounterManipulator::Init(Encounter *pObject)
{
    ObjectManipulator<Encounter>::Init(pObject);

    pTabWidget->Init(pObject);
}

void EncounterManipulator::Reset()
{
    pTabWidget->Reset();
}

bool EncounterManipulator::GetIsActive()
{
    return isActive;
}

void EncounterManipulator::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pTabWidget->SetIsActive(isActive);
    }
}

bool EncounterManipulator::ShouldIncludeItem(Encounter *pEncounter)
{
    return pEncounter->GetOneShotConversation() == NULL;
}
