#ifndef FIELDCHARACTERMANIPULATOR_H
#define FIELDCHARACTERMANIPULATOR_H

#include "LocationElementManipulator.h"
#include "CaseCreator/CaseContent/Character.h"
#include "CaseCreator/CaseContent/Encounter.h"

#include "../EncounterTab/ConversationEditor.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

class FieldCharacterManipulator : public LocationElementManipulator<Character::FieldInstance>
{
    Q_OBJECT
public:
    static FieldCharacterManipulator * Create(QWidget *parent = 0);

protected:
    explicit FieldCharacterManipulator(QWidget *parent = 0);

    Encounter * GetEncounter() override;
    bool SupportsFullEncounters() override;
};

/*class FieldCharacterManipulator : public ObjectManipulator<Character::FieldInstance>
{
    Q_OBJECT
public:
    explicit FieldCharacterManipulator(QWidget *parent = 0);

    void Init(Character::FieldInstance *pObject);
    void Reset();

    virtual void PullChanges();

signals:

public slots:
    void NameEditingFinished();
    void PositionEditingFinished();
    void AssociatedActionComboBoxCurrentIndexChanged(int newIndex);
    void EditConversationButtonClicked();
    void FinishEditingConversationsButtonClicked();

protected:
    virtual bool TryChangeFields();

    QWidget *pMainEditorWidget;
    QWidget *pConversationEditorWidget;

    QLabel *pDisplayNameLabel;
    QLineEdit *pNameLineEdit;
    QLineEdit *pXCoordinateLineEdit;
    QLineEdit *pYCoordinateLineEdit;

    QComboBox *pAssociatedActionComboBox;

    QWidget *pBeginEncounterWidget;
    QWidget *pBeginConversationWidget;
    QWidget *pBeginCutsceneWidget;

    EncounterSelector *pEncounterSelector;

    ConversationEditor *pConversationEditor;
};*/

#endif // FIELDCHARACTERMANIPULATOR_H
