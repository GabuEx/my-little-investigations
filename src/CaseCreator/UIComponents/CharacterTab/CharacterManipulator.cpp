#include "CharacterManipulator.h"

#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QVBoxLayout>

#include <QTimer>

#include "CaseCreator/CaseContent/Character.h"
#include "CharacterDialogSpritesPage.h"
#include "CharacterFieldSpritesPage.h"

template<>
ObjectManipulator<Character> * ObjectManipulator<Character>::Create(QWidget *parent)
{
    return new CharacterManipulator(parent);
}

CharacterManipulator::CharacterManipulator(QWidget *parent) :
    ObjectManipulator<Character>(parent)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();
    pTabWidget = new PageTabWidget<Character>();

    pTabWidget->AddPage(new CharacterDialogSpritesPage(), "Dialog Sprites");
    pTabWidget->AddPage(new CharacterFieldSpritesPage(), "Field Sprites");

    pMainLayout->addWidget(pTabWidget);
    setLayout(pMainLayout);
}

CharacterManipulator::~CharacterManipulator()
{
}

void CharacterManipulator::Init(Character *pObject)
{
    ObjectManipulator<Character>::Init(pObject);

    pTabWidget->Init(pObject);
}

void CharacterManipulator::Reset()
{
    pTabWidget->Reset();
}

bool CharacterManipulator::Undo()
{
    return pTabWidget->Undo();
}

bool CharacterManipulator::Redo()
{
    return pTabWidget->Redo();
}

bool CharacterManipulator::GetIsActive()
{
    return isActive;
}

void CharacterManipulator::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pTabWidget->SetIsActive(isActive);
    }
}
