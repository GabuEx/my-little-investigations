#ifndef LOCATIONELEMENTMANIPULATOR_H
#define LOCATIONELEMENTMANIPULATOR_H

#include "../TemplateHelpers/ObjectManipulator.h"
#include "CaseCreator/CaseContent/Encounter.h"

#include "../EncounterTab/EncounterManipulator.h"
#include "../EncounterTab/ConversationEditor.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

#include <QGridLayout>
#include <QVBoxLayout>

#include <QLocale>

template <class T>
class LocationElementManipulator : public ObjectManipulator<T>
{
protected:
    explicit LocationElementManipulator(QWidget *parent = 0);
    void CreateVisualTree();

public:
    void Init(T *pObject);
    void Reset();

    virtual void PullChanges();

protected:
    void NameEditingFinishedImpl() override;
    void PositionEditingFinishedImpl() override;
    void AssociatedActionComboBoxCurrentIndexChangedImpl(int newIndex) override;
    void EditEncounterButtonClickedImpl() override;
    void EditConversationButtonClickedImpl() override;
    void FinishEditingEncounterButtonClickedImpl() override;
    void FinishEditingConversationButtonClickedImpl() override;

    virtual bool TryChangeFields();
    virtual Encounter * GetEncounter() = 0;
    virtual bool SupportsFullEncounters() = 0;

    QWidget *pMainEditorWidget;
    QWidget *pEncounterEditorWidget;
    QWidget *pConversationEditorWidget;

    QLabel *pDisplayNameLabel;
    QLineEdit *pNameLineEdit;
    QLineEdit *pXCoordinateLineEdit;
    QLineEdit *pYCoordinateLineEdit;

    QComboBox *pAssociatedActionComboBox;

    QWidget *pBeginEncounterWidget;
    QWidget *pBeginConversationWidget;
    QWidget *pBeginCutsceneWidget;

    EncounterManipulator *pEncounterManipulator;

    EncounterSelector *pEncounterSelector;

    ConversationEditor *pConversationEditor;

private:
    int GetBeginEncounterIndex() { return SupportsFullEncounters() ? 1 : -1; }
    int GetBeginSingleConversationIndex() { return SupportsFullEncounters() ? 2 : 1; }
    int GetBeginCutsceneIndex() { return SupportsFullEncounters() ? 3 : 2; }
};

#endif // LOCATIONELEMENTMANIPULATOR_H
