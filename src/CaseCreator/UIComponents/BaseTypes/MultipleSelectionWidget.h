#ifndef MULTIPLESELECTIONWIDGET_H
#define MULTIPLESELECTIONWIDGET_H

#include <QStringList>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "CaseCreator/Utilities/SizeAwareElidingLabel.h"

class MultipleSelectionSelectorWidgetSlots : public QWidget
{
    Q_OBJECT

public:
    MultipleSelectionSelectorWidgetSlots(QWidget *parent = 0, Qt::WindowFlags flags = 0) : QWidget(parent, flags) { }

    virtual void SetIsSingleton(bool isSingleton) = 0;

    virtual QString GetString() = 0;
    virtual void SetToString(const QString &string) = 0;
    virtual QWidget * CreateSelector() = 0;

signals:
    void StringSelected(MultipleSelectionSelectorWidgetSlots *pSender, const QString &string);
    void AddRequested(MultipleSelectionSelectorWidgetSlots *pSender);
    void DeleteRequested(MultipleSelectionSelectorWidgetSlots *pSender);

public slots:
    void StringSelected(const QString &string)
    {
        StringSelectedImpl(string);
    }

    void AddButtonClicked()
    {
        AddButtonClickedImpl();
    }

    void DeleteButtonClicked()
    {
        DeleteButtonClickedImpl();
    }

protected:
    virtual void StringSelectedImpl(const QString &string) = 0;
    virtual void AddButtonClickedImpl() = 0;
    virtual void DeleteButtonClickedImpl() = 0;
};

class MultipleSelectionSelectorWidget : public MultipleSelectionSelectorWidgetSlots
{
public:
    MultipleSelectionSelectorWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    void Initialize();

    virtual void SetIsSingleton(bool isSingleton) override;

protected:
    virtual void StringSelectedImpl(const QString &string) override;
    virtual void AddButtonClickedImpl() override;
    virtual void DeleteButtonClickedImpl() override;

private:
    QPushButton *pDeleteButton;
};

class MultipleSelectionWidgetBase : public QWidget
{
    Q_OBJECT

public:
    MultipleSelectionWidgetBase(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    QStringList GetSelections();
    void SetSelections(const QStringList &stringList);

    virtual MultipleSelectionSelectorWidgetSlots * CreateSelector() = 0;

signals:
    void StringChanged(int index, const QString &string);
    void StringAdded(int index, const QString &string);
    void StringRemoved(int index);

public slots:
    void SelectorStringSelected(MultipleSelectionSelectorWidgetSlots *pSender, const QString &string);
    void SelectorAddRequested(MultipleSelectionSelectorWidgetSlots *pSender);
    void SelectorDeleteRequested(MultipleSelectionSelectorWidgetSlots *pSender);

private:
    void UpdateSelectorSingletonness();

    QVBoxLayout *pSelectorLayout;
    QList<MultipleSelectionSelectorWidgetSlots *> selectorList;
};

template <class TSelectorClass>
class MultipleSelectionWidget : public MultipleSelectionWidgetBase
{
public:
    MultipleSelectionWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0) : MultipleSelectionWidgetBase(parent, flags) { }

    virtual MultipleSelectionSelectorWidgetSlots * CreateSelector() override
    {
        TSelectorClass *pSelector = new TSelectorClass();
        pSelector->Initialize();

        QObject::connect(pSelector, SIGNAL(StringSelected(MultipleSelectionSelectorWidgetSlots*,QString)), this, SLOT(SelectorStringSelected(MultipleSelectionSelectorWidgetSlots*,QString)));
        QObject::connect(pSelector, SIGNAL(AddRequested(MultipleSelectionSelectorWidgetSlots*)), this, SLOT(SelectorAddRequested(MultipleSelectionSelectorWidgetSlots*)));
        QObject::connect(pSelector, SIGNAL(DeleteRequested(MultipleSelectionSelectorWidgetSlots*)), this, SLOT(SelectorDeleteRequested(MultipleSelectionSelectorWidgetSlots*)));

        return pSelector;
    }
};

class FilePathMultipleSelectionSelectorWidget : public MultipleSelectionSelectorWidget
{
    Q_OBJECT

public:
    FilePathMultipleSelectionSelectorWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0) : MultipleSelectionSelectorWidget(parent, flags) { }

    virtual QString GetString() override;
    virtual void SetToString(const QString &string) override;
    virtual QWidget * CreateSelector() override;

public slots:
    void SelectFilePathButtonClicked();

private:
    QString filePath;

    QPushButton *pSelectFilePathButton;
    SizeAwareElidingLabel *pFilePathLabel;
};

#endif // MULTIPLESELECTIONWIDGET_H
