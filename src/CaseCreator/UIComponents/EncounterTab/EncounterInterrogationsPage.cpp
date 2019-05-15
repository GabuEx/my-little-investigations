#include "EncounterInterrogationsPage.h"

#include <QVBoxLayout>

EncounterInterrogationsPage::EncounterInterrogationsPage(QWidget *parent)
    : Page<Encounter>(parent)
{
    isActive = false;

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pInterrogationManipulator = new ListManipulator<Interrogation>();
    pMainLayout->addWidget(pInterrogationManipulator);

    setLayout(pMainLayout);
}

void EncounterInterrogationsPage::Init(Encounter *pObject)
{
    Page<Encounter>::Init(pObject);
    pInterrogationManipulator->SetParentObject(pObject);
}

void EncounterInterrogationsPage::Reset()
{

}

bool EncounterInterrogationsPage::GetIsActive()
{
    return isActive;
}

void EncounterInterrogationsPage::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pInterrogationManipulator->SetIsActive(isActive);
    }
}
