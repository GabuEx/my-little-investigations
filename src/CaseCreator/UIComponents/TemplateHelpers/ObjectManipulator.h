#ifndef OBJECTMANIPULATOR_H
#define OBJECTMANIPULATOR_H

#include <QWidget>

class ObjectManipulatorSlots : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectManipulatorSlots(QWidget *parent = 0) :
        QWidget(parent)
    {
    }

    virtual void PullChanges() = 0;

signals:
    void ValueChanged();
    void LargeSizeRequested();
    void SmallSizeRequested();

public slots:
    // Needed for LocationElementManipulator<T>
    void NameEditingFinished()
    {
        NameEditingFinishedImpl();
    }

    void PositionEditingFinished()
    {
        PositionEditingFinishedImpl();
    }

    void AssociatedActionComboBoxCurrentIndexChanged(int newIndex)
    {
        AssociatedActionComboBoxCurrentIndexChangedImpl(newIndex);
    }

    void EditEncounterButtonClicked()
    {
        EditEncounterButtonClickedImpl();
    }

    void EditConversationButtonClicked()
    {
        EditConversationButtonClickedImpl();
    }

    void FinishEditingEncounterButtonClicked()
    {
        FinishEditingEncounterButtonClickedImpl();
    }

    void FinishEditingConversationButtonClicked()
    {
        FinishEditingConversationButtonClickedImpl();
    }

protected:
    virtual void NameEditingFinishedImpl() { }
    virtual void PositionEditingFinishedImpl() { }
    virtual void AssociatedActionComboBoxCurrentIndexChangedImpl(int /*newIndex*/) { }
    virtual void EditEncounterButtonClickedImpl() { }
    virtual void EditConversationButtonClickedImpl() { }
    virtual void FinishEditingEncounterButtonClickedImpl() { }
    virtual void FinishEditingConversationButtonClickedImpl() { }
};

template <class T>
class ObjectManipulator : public ObjectManipulatorSlots
{
public:
    explicit ObjectManipulator(QWidget *parent = 0) :
        ObjectManipulatorSlots(parent)
    {
        pObject = NULL;
    }

    virtual ~ObjectManipulator()
    {
        pObject = NULL;
    }

    static ObjectManipulator<T> * Create(QWidget *parent = 0);

    virtual void Init(T *pObject)
    {
        this->pObject = pObject;
    }

    virtual void Reset() = 0;

    virtual void PullChanges() { }

    virtual bool Undo() { return false; }
    virtual bool Redo() { return false; }

    virtual bool GetIsActive() { return true; }
    virtual void SetIsActive(bool) { }

    virtual bool ShouldIncludeItem(T */* pItem*/) { return true; }

protected:
    void PushChanges()
    {
        if (TryChangeFields())
        {
            emit ValueChanged();
        }
    }

    virtual bool TryChangeFields()
    {
        return true;
    }

    T *pObject;
};

#endif // OBJECTMANIPULATOR_H
