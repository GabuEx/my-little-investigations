#ifndef NEWOBJECTDIALOG_H
#define NEWOBJECTDIALOG_H

#include <QDialog>

#include <QApplication>

class NewObjectDialogSlots : public QDialog
{
    Q_OBJECT

public:
    explicit NewObjectDialogSlots(QWidget *parent = 0, Qt::WindowFlags flags = 0) :
        QDialog(parent, flags)
    {
    }

public slots:
    virtual int exec() { return Exec(); }
    virtual void accept() { return Accept(); }

    // This really should not be here, but thanks to inheritance and namescope wackiness,
    // it needs to be.  See EditorDialog in ObjectListWidget.h for why.
    void ObjectTypeComboBoxCurrentIndexChanged(const QString &selectedText)
    {
        ObjectTypeComboBoxCurrentIndexChangedImpl(selectedText);
    }

protected:
    virtual int Exec() = 0;
    virtual void Accept() = 0;

    virtual void ObjectTypeComboBoxCurrentIndexChangedImpl(const QString &/*selectedText*/) { }
};

template <class T>
class NewObjectDialog : public NewObjectDialogSlots
{
public:
    explicit NewObjectDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0) :
        NewObjectDialogSlots(parent, flags)
    {
    }

    virtual ~NewObjectDialog() { }

    static NewObjectDialog<T> * Create(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    virtual T * GetNewObject() = 0;

protected:
    int Exec()
    {
        InitFields();
        return QDialog::exec();
    }

    void Accept()
    {
        if (ValidateFields())
        {
            OnAccepted();
            QDialog::accept();
        }
        else
        {
            QApplication::beep();
        }
    }

    virtual void OnAccepted() { }

private:
    virtual void InitFields() = 0;
    virtual bool ValidateFields() = 0;
};

#endif // NEWOBJECTDIALOG_H
