#include "LocationElementManipulator.h"

#include "CaseCreator/CaseContent/Character.h"
#include "CaseCreator/CaseContent/InteractionLocation.h"
#include "CaseCreator/CaseContent/ForegroundElement.h"

template <class T>
LocationElementManipulator<T>::LocationElementManipulator(QWidget *parent) :
    ObjectManipulator<T>(parent)
{
}

template <class T>
void LocationElementManipulator<T>::CreateVisualTree()
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

    QLabel *pPerformActionLabel = new QLabel("Perform an action when clicking this element...");
    pPerformActionLabel->setContentsMargins(0, 20, 0, 0);

    pMainEditorLayout->addWidget(pPerformActionLabel);

    pAssociatedActionComboBox = new QComboBox();
    pAssociatedActionComboBox->addItem("(no action)");

    if (SupportsFullEncounters())
    {
        pAssociatedActionComboBox->addItem("Begin an encounter...");
    }

    pAssociatedActionComboBox->addItem("Begin a single conversation...");
    pAssociatedActionComboBox->addItem("Begin a cutscene...");
    pMainEditorLayout->addWidget(pAssociatedActionComboBox);

    pBeginEncounterWidget = new QWidget();
    QVBoxLayout *pBeginEncounterLayout = new QVBoxLayout();
    pBeginEncounterLayout->setContentsMargins(0, 0, 0, 0);
    //pEncounterSelector = new EncounterSelector();
    //pBeginEncounterLayout->addWidget(pEncounterSelector);
    QPushButton *pEditEncounterButton = new QPushButton("Click to edit the encounter");
    pBeginEncounterLayout->addWidget(pEditEncounterButton);
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

    pEncounterEditorWidget = new QWidget();
    QGridLayout *pEncounterEditorLayout = new QGridLayout();
    pEncounterEditorLayout->setColumnStretch(0, 1);
    pEncounterEditorLayout->setRowStretch(0, 0);
    pEncounterEditorLayout->setRowStretch(1, 1);

    QPushButton *pFinishEditingEncounterButton = new QPushButton("Finish editing encounter");
    pEncounterEditorLayout->addWidget(pFinishEditingEncounterButton, 0, 0);

    pEncounterManipulator = new EncounterManipulator();
    pEncounterEditorLayout->addWidget(pEncounterManipulator, 1, 0);

    pEncounterEditorWidget->setLayout(pEncounterEditorLayout);
    pEncounterEditorWidget->hide();

    pLayout->addWidget(pEncounterEditorWidget);

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
    this->setLayout(pLayout);

    this->AssociatedActionComboBoxCurrentIndexChanged(-1);

    QWidget::connect(pNameLineEdit, SIGNAL(editingFinished()), this, SLOT(NameEditingFinished()));
    QWidget::connect(pXCoordinateLineEdit, SIGNAL(editingFinished()), this, SLOT(PositionEditingFinished()));
    QWidget::connect(pYCoordinateLineEdit, SIGNAL(editingFinished()), this, SLOT(PositionEditingFinished()));
    QWidget::connect(pAssociatedActionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(AssociatedActionComboBoxCurrentIndexChanged(int)));
    QWidget::connect(pEditEncounterButton, SIGNAL(clicked()), this, SLOT(EditEncounterButtonClicked()));
    QWidget::connect(pEditConversationButton, SIGNAL(clicked()), this, SLOT(EditConversationButtonClicked()));
    QWidget::connect(pFinishEditingEncounterButton, SIGNAL(clicked()), this, SLOT(FinishEditingEncounterButtonClicked()));
    QWidget::connect(pFinishEditingConversationsButton, SIGNAL(clicked()), this, SLOT(FinishEditingConversationButtonClicked()));
}

template <class T>
void LocationElementManipulator<T>::Init(T *pObject)
{
    ObjectManipulator<T>::Init(pObject);

    PullChanges();
}

template <class T>
void LocationElementManipulator<T>::Reset()
{
    //pEncounterSelector->Reset();
}

template <class T>
void LocationElementManipulator<T>::PullChanges()
{
    QLocale locale;

    pDisplayNameLabel->setText(this->pObject->GetDisplayName());
    pNameLineEdit->setText(this->pObject->GetName());
    pXCoordinateLineEdit->setText(locale.toString(this->pObject->GetPosition().GetX()));
    pYCoordinateLineEdit->setText(locale.toString(this->pObject->GetPosition().GetY()));

    //pEncounterSelector->Reset();

    Encounter *pEncounter = GetEncounter();

    if (pEncounter != NULL)
    {
        if (pEncounter->GetOneShotConversation() != NULL)
        {
            pAssociatedActionComboBox->setCurrentIndex(GetBeginSingleConversationIndex());
        }
        else
        {
            //pEncounterSelector->SetToId(pEncounter->GetId());
            pAssociatedActionComboBox->setCurrentIndex(GetBeginEncounterIndex());
        }
    }
}

template <class T>
void LocationElementManipulator<T>::NameEditingFinishedImpl()
{
    this->PushChanges();
}

template <class T>
void LocationElementManipulator<T>::PositionEditingFinishedImpl()
{
    this->PushChanges();
}

template <class T>
void LocationElementManipulator<T>::AssociatedActionComboBoxCurrentIndexChangedImpl(int newIndex)
{
    if (newIndex >= 0)
    {
        if (newIndex == GetBeginEncounterIndex())
        {
            pBeginEncounterWidget->show();
            pBeginConversationWidget->hide();
            pBeginCutsceneWidget->hide();
        }
        else if (newIndex == GetBeginSingleConversationIndex())
        {
            pBeginEncounterWidget->hide();
            pBeginConversationWidget->show();
            pBeginCutsceneWidget->hide();
        }
        else if (newIndex == GetBeginCutsceneIndex())
        {
            pBeginEncounterWidget->hide();
            pBeginConversationWidget->hide();
            pBeginCutsceneWidget->show();
        }
        else
        {
            pBeginEncounterWidget->hide();
            pBeginConversationWidget->hide();
            pBeginCutsceneWidget->hide();
        }
    }
    else
    {
        pBeginEncounterWidget->hide();
        pBeginConversationWidget->hide();
        pBeginCutsceneWidget->hide();
    }
}

template <class T>
void LocationElementManipulator<T>::EditEncounterButtonClickedImpl()
{
    Encounter *pSingleEncounter = GetEncounter();

    if (pSingleEncounter != NULL)
    {
        pMainEditorWidget->hide();
        pEncounterManipulator->Reset();
        pEncounterManipulator->Init(pSingleEncounter);
        pEncounterEditorWidget->show();

        emit this->LargeSizeRequested();
    }
}

template <class T>
void LocationElementManipulator<T>::EditConversationButtonClickedImpl()
{
    Encounter *pSingleEncounter = GetEncounter();

    if (pSingleEncounter != NULL && pSingleEncounter->GetOneShotConversation() != NULL)
    {
        pMainEditorWidget->hide();
        pConversationEditor->Reset();
        pConversationEditor->Init(pSingleEncounter->GetOneShotConversation());
        pConversationEditorWidget->show();

        emit this->LargeSizeRequested();
    }
}

template <class T>
void LocationElementManipulator<T>::FinishEditingEncounterButtonClickedImpl()
{
    pMainEditorWidget->show();
    pEncounterEditorWidget->hide();

    emit this->SmallSizeRequested();
}

template <class T>
void LocationElementManipulator<T>::FinishEditingConversationButtonClickedImpl()
{
    pMainEditorWidget->show();
    pConversationEditorWidget->hide();

    emit this->SmallSizeRequested();
}

template <class T>
bool LocationElementManipulator<T>::TryChangeFields()
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

    if (this->pObject->GetName() == pNameLineEdit->text() &&
            this->pObject->GetPosition().GetX() == xCoordinate &&
            this->pObject->GetPosition().GetY() == yCoordinate)
    {
        return false;
    }

    this->pObject->SetName(pNameLineEdit->text());
    pDisplayNameLabel->setText(this->pObject->GetDisplayName());

    this->pObject->SetPosition(Vector2(xCoordinate, yCoordinate));

    return true;
}

template class LocationElementManipulator<Character::FieldInstance>;
template class LocationElementManipulator<InteractionLocation>;
template class LocationElementManipulator<ForegroundElement>;
