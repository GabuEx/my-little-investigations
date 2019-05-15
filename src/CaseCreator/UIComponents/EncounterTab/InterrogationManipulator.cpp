#include "InterrogationManipulator.h"

#include "CaseCreator/CaseContent/Conversation.h"
#include "CaseCreator/CaseContent/Encounter.h"

#include "CaseCreator/UIComponents/EncounterTab/ConversationEditor.h"

#include <QHBoxLayout>

template<>
ObjectManipulator<Interrogation> * ObjectManipulator<Interrogation>::Create(QWidget *parent)
{
    return new InterrogationManipulator(parent);
}

InterrogationManipulator::InterrogationManipulator(QWidget *parent) :
    ObjectManipulator<Interrogation>(parent)
{
    QHBoxLayout *pMainLayout = new QHBoxLayout();

    pConversationEditor = new ConversationEditor();
    pMainLayout->addWidget(pConversationEditor);

    setLayout(pMainLayout);
}

void InterrogationManipulator::Init(Interrogation *pObject)
{
    ObjectManipulator<Interrogation>::Init(pObject);
    pConversationEditor->Init(pObject);
}

void InterrogationManipulator::Reset()
{
    pConversationEditor->Reset();
}

