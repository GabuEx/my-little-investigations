#include "ConversationManipulator.h"

#include "CaseCreator/CaseContent/Conversation.h"
#include "CaseCreator/CaseContent/Encounter.h"

#include "CaseCreator/UIComponents/EncounterTab/ConversationEditor.h"

#include <QHBoxLayout>

template<>
ObjectManipulator<Conversation> * ObjectManipulator<Conversation>::Create(QWidget *parent)
{
    return new ConversationManipulator(parent);
}

ConversationManipulator::ConversationManipulator(QWidget *parent) :
    ObjectManipulator<Conversation>(parent)
{
    QHBoxLayout *pMainLayout = new QHBoxLayout();

    pConversationEditor = new ConversationEditor();
    pMainLayout->addWidget(pConversationEditor);

    setLayout(pMainLayout);
}

void ConversationManipulator::Init(Conversation *pObject)
{
    ObjectManipulator<Conversation>::Init(pObject);
    pConversationEditor->Init(pObject);
}

void ConversationManipulator::Reset()
{
    pConversationEditor->Reset();
}
