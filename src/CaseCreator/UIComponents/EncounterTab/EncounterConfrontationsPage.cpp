#include "EncounterConfrontationsPage.h"

#include <QVBoxLayout>

EncounterConfrontationsPage::EncounterConfrontationsPage(QWidget *parent)
    : Page<Encounter>(parent)
{
    isActive = false;

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pConfrontationManipulator = new ListManipulator<Confrontation>();
    pMainLayout->addWidget(pConfrontationManipulator);

    setLayout(pMainLayout);
}

void EncounterConfrontationsPage::Init(Encounter *pObject)
{
    Page<Encounter>::Init(pObject);
    pConfrontationManipulator->SetParentObject(pObject);
}

void EncounterConfrontationsPage::Reset()
{

}

bool EncounterConfrontationsPage::GetIsActive()
{
    return isActive;
}

void EncounterConfrontationsPage::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pConfrontationManipulator->SetIsActive(isActive);
    }
}
