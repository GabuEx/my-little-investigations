#include "FieldCharacterManipulator.h"

#include <QLabel>
#include <QScrollArea>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <QLocale>

#include "CaseCreator/CaseContent/CaseContent.h"
#include "CaseCreator/CaseContent/Character.h"
#include "CaseCreator/CaseContent/Encounter.h"

template<>
ObjectManipulator<Character::FieldInstance> * ObjectManipulator<Character::FieldInstance>::Create(QWidget *parent)
{
    return FieldCharacterManipulator::Create(parent);
}

FieldCharacterManipulator * FieldCharacterManipulator::Create(QWidget *parent)
{
    FieldCharacterManipulator *pManipulator = new FieldCharacterManipulator(parent);
    pManipulator->CreateVisualTree();
    return pManipulator;
}

FieldCharacterManipulator::FieldCharacterManipulator(QWidget *parent) :
    LocationElementManipulator<Character::FieldInstance>(parent)
{
}

Encounter * FieldCharacterManipulator::GetEncounter()
{
    if (pObject->GetClickEncounterId().length() > 0)
    {
        return CaseContent::GetInstance()->GetById<Encounter>(pObject->GetClickEncounterId());
    }
    else
    {
        return NULL;
    }
}

bool FieldCharacterManipulator::SupportsFullEncounters()
{
    return true;
}

/*FieldCharacterManipulator::FieldCharacterManipulator(QWidget *parent) :
    ObjectManipulator<Character::FieldInstance>(parent)
{
    QHBoxLayout *pLayout = new QHBoxLayout();
    pMainEditorWidget = new QWidget();

    QVBoxLayout *pMainEditorLayout = new QVBoxLayout();

    pDisplayNameLabel = new QLabel();
    pMainEditorLayout->addWidget(pDisplayNameLabel);

    QGridLayout *pNameLayout = new QGridLayout();
    pNameLayout->setColumnStretch(0, 0);
    pNameLayout->setRowStretch(0, 0);
    pNameLayout->setRowStretch(1, 1);

    QLabel *pNameLabel = new QLabel("Name:");
    pNameLineEdit = new QLineEdit();

    pNameLayout->addWidget(pNameLabel, 0, 0);
    pNameLayout->addWidget(pNameLineEdit, 0, 1);

    pMainEditorLayout->addLayout(pNameLayout);

    QGridLayout *pPositionLayout = new QGridLayout();
    pPositionLayout->setColumnStretch(0, 0);
    pPositionLayout->setRowStretch(0, 0);
    pPositionLayout->setRowStretch(1, 1);
    pPositionLayout->setRowStretch(2, 0);
    pPositionLayout->setRowStretch(3, 1);

    QLabel *pPositionLabel = new QLabel("Position");
    pPositionLabel->setContentsMargins(0, 20, 0, 0);

    pMainEditorLayout->addWidget(pPositionLabel);

    QLabel *pXLabel = new QLabel("X: ");
    pXCoordinateLineEdit = new QLineEdit();
    QLabel *pYLabel = new QLabel("Y: ");
    pYCoordinateLineEdit = new QLineEdit();

    pPositionLayout->addWidget(pXLabel, 0, 0);
    pPositionLayout->addWidget(pXCoordinateLineEdit, 0, 1);
    pPositionLayout->addWidget(pYLabel, 0, 2);
    pPositionLayout->addWidget(pYCoordinateLineEdit, 0, 3);

    pMainEditorLayout->addLayout(pPositionLayout);

    QLabel *pPerformActionLabel = new QLabel("Perform an action when clicking this character...");
    pPerformActionLabel->setContentsMargins(0, 20, 0, 0);

    pMainEditorLayout->addWidget(pPerformActionLabel);

    pAssociatedActionComboBox = new QComboBox();
    pAssociatedActionComboBox->addItem("(no action)");
    pAssociatedActionComboBox->addItem("Begin an encounter...");
    pAssociatedActionComboBox->addItem("Begin a single conversation...");
    pAssociatedActionComboBox->addItem("Begin a cutscene...");
    pMainEditorLayout->addWidget(pAssociatedActionComboBox);

    pBeginEncounterWidget = new QWidget();
    QVBoxLayout *pBeginEncounterLayout = new QVBoxLayout();
    pBeginEncounterLayout->setContentsMargins(0, 0, 0, 0);
    pEncounterSelector = new EncounterSelector();
    pBeginEncounterLayout->addWidget(pEncounterSelector);
    pBeginEncounterWidget->setLayout(pBeginEncounterLayout);
    pMainEditorLayout->addWidget(pBeginEncounterWidget);

    pBeginConversationWidget = new QWidget();
    QVBoxLayout *pBeginConversationLayout = new QVBoxLayout();
    pBeginConversationLayout->setContentsMargins(0, 0, 0, 0);
    QPushButton *pEditConversationButton = new QPushButton("Click to edit the conversation");
    pBeginConversationLayout->addWidget(pEditConversationButton);
    pBeginConversationWidget->setLayout(pBeginConversationLayout);
    pMainEditorLayout->addWidget(pBeginConversationWidget);

    pBeginCutsceneWidget = new QWidget();
    QVBoxLayout *pBeginCutsceneLayout = new QVBoxLayout();
    pBeginCutsceneLayout->setContentsMargins(0, 0, 0, 0);
    QPushButton *pEditCutsceneButton = new QPushButton("Click to edit the cutscene");
    pBeginCutsceneLayout->addWidget(pEditCutsceneButton);
    pBeginCutsceneWidget->setLayout(pBeginCutsceneLayout);
    pMainEditorLayout->addWidget(pBeginCutsceneWidget);

    pMainEditorWidget->setLayout(pMainEditorLayout);
    pLayout->addWidget(pMainEditorWidget);

    pConversationEditorWidget = new QWidget();
    QGridLayout *pConversationEditorLayout = new QGridLayout();
    pConversationEditorLayout->setColumnStretch(0, 1);
    pConversationEditorLayout->setRowStretch(0, 0);
    pConversationEditorLayout->setRowStretch(1, 1);

    QPushButton *pFinishEditingConversationsButton = new QPushButton("Finish editing conversation");
    pConversationEditorLayout->addWidget(pFinishEditingConversationsButton, 0, 0);

    pConversationEditor = new ConversationEditor();
    pConversationEditorLayout->addWidget(pConversationEditor, 1, 0);

    pConversationEditorWidget->setLayout(pConversationEditorLayout);
    pConversationEditorWidget->hide();

    pLayout->addWidget(pConversationEditorWidget);
    setLayout(pLayout);

    QWidget::connect(pNameLineEdit, SIGNAL(editingFinished()), this, SLOT(NameEditingFinished()));
    QWidget::connect(pXCoordinateLineEdit, SIGNAL(editingFinished()), this, SLOT(PositionEditingFinished()));
    QWidget::connect(pYCoordinateLineEdit, SIGNAL(editingFinished()), this, SLOT(PositionEditingFinished()));
    QWidget::connect(pAssociatedActionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(AssociatedActionComboBoxCurrentIndexChanged(int)));
    QWidget::connect(pEditConversationButton, SIGNAL(clicked()), this, SLOT(EditConversationButtonClicked()));
    QWidget::connect(pFinishEditingConversationsButton, SIGNAL(clicked()), this, SLOT(FinishEditingConversationsButtonClicked()));

    AssociatedActionComboBoxCurrentIndexChanged(0);
}

void FieldCharacterManipulator::Init(Character::FieldInstance *pObject)
{
    ObjectManipulator<Character::FieldInstance>::Init(pObject);

    PullChanges();
}

void FieldCharacterManipulator::Reset()
{
    pEncounterSelector->Reset();
}

void FieldCharacterManipulator::PullChanges()
{
    QLocale locale;

    pDisplayNameLabel->setText(pObject->GetDisplayName());
    pNameLineEdit->setText(pObject->GetName());
    pXCoordinateLineEdit->setText(locale.toString(pObject->GetPosition().GetX()));
    pYCoordinateLineEdit->setText(locale.toString(pObject->GetPosition().GetY()));

    pEncounterSelector->Reset();

    if (pObject->GetClickEncounterId().length() > 0)
    {
        Encounter *pEncounter = CaseContent::GetInstance()->GetById<Encounter>(pObject->GetClickEncounterId());

        if (pEncounter != NULL)
        {
            if (pEncounter->GetOneShotConversation() != NULL)
            {
                pAssociatedActionComboBox->setCurrentIndex(2);
            }
            else
            {
                pEncounterSelector->SetToId(pObject->GetClickEncounterId());
                pAssociatedActionComboBox->setCurrentIndex(1);
            }
        }
    }
}

void FieldCharacterManipulator::NameEditingFinished()
{
    PushChanges();
}

void FieldCharacterManipulator::PositionEditingFinished()
{
    PushChanges();
}

void FieldCharacterManipulator::AssociatedActionComboBoxCurrentIndexChanged(int newIndex)
{
    switch (newIndex)
    {
    case 1:
        pBeginEncounterWidget->show();
        pBeginConversationWidget->hide();
        pBeginCutsceneWidget->hide();
        break;

    case 2:
        pBeginEncounterWidget->hide();
        pBeginConversationWidget->show();
        pBeginCutsceneWidget->hide();
        break;

    case 3:
        pBeginEncounterWidget->hide();
        pBeginConversationWidget->hide();
        pBeginCutsceneWidget->show();
        break;

    default:
        pBeginEncounterWidget->hide();
        pBeginConversationWidget->hide();
        pBeginCutsceneWidget->hide();
        break;
    }
}

void FieldCharacterManipulator::EditConversationButtonClicked()
{
    Encounter *pEncounter = CaseContent::GetInstance()->GetById<Encounter>(pObject->GetClickEncounterId());

    if (pEncounter != NULL && pEncounter->GetOneShotConversation() != NULL)
    {
        pMainEditorWidget->hide();
        pConversationEditor->Reset();
        pConversationEditor->Init(pEncounter->GetOneShotConversation());
        pConversationEditorWidget->show();

        emit LargeSizeRequested();
    }
}

void FieldCharacterManipulator::FinishEditingConversationsButtonClicked()
{
    pMainEditorWidget->show();
    pConversationEditorWidget->hide();

    emit SmallSizeRequested();
}

bool FieldCharacterManipulator::TryChangeFields()
{
    QLocale locale;

    bool gotXCoordinate = false;
    bool gotYCoordinate = false;
    int xCoordinate = locale.toInt(pXCoordinateLineEdit->text(), &gotXCoordinate);
    int yCoordinate = locale.toInt(pYCoordinateLineEdit->text(), &gotYCoordinate);

    if (!gotXCoordinate || !gotYCoordinate)
    {
        return false;
    }

    if (pObject->GetName() == pNameLineEdit->text() &&
            pObject->GetPosition().GetX() == xCoordinate &&
            pObject->GetPosition().GetY() == yCoordinate)
    {
        return false;
    }

    pObject->SetName(pNameLineEdit->text());
    pDisplayNameLabel->setText(pObject->GetDisplayName());

    pObject->SetPosition(Vector2(xCoordinate, yCoordinate));

    return true;
}*/
