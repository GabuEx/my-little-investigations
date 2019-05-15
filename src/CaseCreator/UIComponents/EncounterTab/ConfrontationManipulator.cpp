#include "ConfrontationManipulator.h"

#include "CaseCreator/CaseContent/Conversation.h"
#include "CaseCreator/CaseContent/Encounter.h"

#include "CaseCreator/UIComponents/EncounterTab/ConversationEditor.h"

#include <QHBoxLayout>

template<>
ObjectManipulator<Confrontation> * ObjectManipulator<Confrontation>::Create(QWidget *parent)
{
    return new ConfrontationManipulator(parent);
}

ConfrontationManipulator::ConfrontationManipulator(QWidget *parent) :
    ObjectManipulator<Confrontation>(parent)
{
    QHBoxLayout *pMainLayout = new QHBoxLayout();

    pConversationEditor = new ConversationEditor();
    pMainLayout->addWidget(pConversationEditor);

    setLayout(pMainLayout);
}

void ConfrontationManipulator::Init(Confrontation *pObject)
{
    ObjectManipulator<Confrontation>::Init(pObject);
    pConversationEditor->Init(pObject);
}

void ConfrontationManipulator::Reset()
{
    pConversationEditor->Reset();
}
