#include "EncounterConversationsPage.h"

#include <QVBoxLayout>

EncounterConversationsPage::EncounterConversationsPage(QWidget *parent)
    : Page<Encounter>(parent)
{
    isActive = false;

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pConversationManipulator = new ListManipulator<Conversation>();
    pMainLayout->addWidget(pConversationManipulator);

    setLayout(pMainLayout);
}

void EncounterConversationsPage::Init(Encounter *pObject)
{
    Page<Encounter>::Init(pObject);
    pConversationManipulator->SetParentObject(pObject);
}

void EncounterConversationsPage::Reset()
{

}

bool EncounterConversationsPage::GetIsActive()
{
    return isActive;
}

void EncounterConversationsPage::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pConversationManipulator->SetIsActive(isActive);
    }
}
