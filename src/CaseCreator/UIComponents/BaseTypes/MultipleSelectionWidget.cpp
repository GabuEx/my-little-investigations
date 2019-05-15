#include "MultipleSelectionWidget.h"

#include <QGridLayout>
#include <QFileDialog>

#include "MLIException.h"
#include "CaseCreator/CaseContent/CaseContent.h"

MultipleSelectionWidgetBase::MultipleSelectionWidgetBase(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    pSelectorLayout = new QVBoxLayout();
    pSelectorLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pSelectorLayout);
}

QStringList MultipleSelectionWidgetBase::GetSelections()
{
    QStringList selections;

    for (MultipleSelectionSelectorWidgetSlots *pSelector : selectorList)
    {
        selections.append(pSelector->GetString());
    }

    return selections;
}

void MultipleSelectionWidgetBase::SetSelections(const QStringList &stringList)
{
    for (MultipleSelectionSelectorWidgetSlots *pSelector : selectorList)
    {
        pSelectorLayout->removeWidget(pSelector);
        delete pSelector;
    }

    selectorList.clear();

    for (QString string : stringList)
    {
        MultipleSelectionSelectorWidgetSlots *pSelector = CreateSelector();
        pSelector->SetToString(string);

        pSelectorLayout->addWidget(pSelector);
        selectorList.append(pSelector);
    }

    UpdateSelectorSingletonness();
}

void MultipleSelectionWidgetBase::SelectorStringSelected(MultipleSelectionSelectorWidgetSlots *pSender, const QString &string)
{
    for (int i = 0; i < selectorList.size(); i++)
    {
        if (selectorList[i] == pSender)
        {
            emit StringChanged(i, string);
            break;
        }
    }
}

void MultipleSelectionWidgetBase::SelectorAddRequested(MultipleSelectionSelectorWidgetSlots *pSender)
{
    for (int i = 0; i < selectorList.size(); i++)
    {
        if (selectorList[i] == pSender)
        {
            MultipleSelectionSelectorWidgetSlots *pSelector = CreateSelector();
            pSelectorLayout->insertWidget(i + 1, pSelector);
            selectorList.insert(i + 1, pSelector);
            emit StringAdded(i + 1, pSelector->GetString());
            break;
        }
    }

    UpdateSelectorSingletonness();
}

void MultipleSelectionWidgetBase::SelectorDeleteRequested(MultipleSelectionSelectorWidgetSlots *pSender)
{
    if (selectorList.size() < 2)
    {
        throw new MLIException("Should never be able to remove a selector when there's fewer than two.");
    }

    for (int i = 0; i < selectorList.size(); i++)
    {
        if (selectorList[i] == pSender)
        {
            pSelectorLayout->removeWidget(pSender);
            selectorList.removeAt(i);
            emit StringRemoved(i);
            break;
        }
    }

    UpdateSelectorSingletonness();
}

void MultipleSelectionWidgetBase::UpdateSelectorSingletonness()
{
    for (MultipleSelectionSelectorWidgetSlots *pSelector : selectorList)
    {
        pSelector->SetIsSingleton(selectorList.size() == 1);
    }
}

MultipleSelectionSelectorWidget::MultipleSelectionSelectorWidget(QWidget *parent, Qt::WindowFlags flags)
    : MultipleSelectionSelectorWidgetSlots(parent, flags)
{
}

void MultipleSelectionSelectorWidget::Initialize()
{
    QGridLayout *pLayout = new QGridLayout();
    pLayout->setColumnStretch(0, 1);
    pLayout->setColumnStretch(1, 0);
    pLayout->setColumnStretch(2, 0);

    pLayout->addWidget(CreateSelector(), 0, 0);

    QPushButton *pAddButton = new QPushButton();
    pAddButton->setText("+");
    QObject::connect(pAddButton, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
    pLayout->addWidget(pAddButton, 0, 1);

    pDeleteButton = new QPushButton();
    pDeleteButton->setText("x");
    pLayout->addWidget(pDeleteButton, 0, 2);
    QObject::connect(pDeleteButton, SIGNAL(clicked()), this, SLOT(DeleteButtonClicked()));

    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);
}

void MultipleSelectionSelectorWidget::SetIsSingleton(bool isSingleton)
{
    if (isSingleton)
    {
        pDeleteButton->hide();
        pDeleteButton->setEnabled(false);
    }
    else
    {
        pDeleteButton->show();
        pDeleteButton->setEnabled(true);
    }
}

void MultipleSelectionSelectorWidget::StringSelectedImpl(const QString &string)
{
    emit StringSelected(this, string);
}

void MultipleSelectionSelectorWidget::AddButtonClickedImpl()
{
    emit AddRequested(this);
}

void MultipleSelectionSelectorWidget::DeleteButtonClickedImpl()
{
    emit DeleteRequested(this);
}

QString FilePathMultipleSelectionSelectorWidget::GetString()
{
    return filePath;
}

void FilePathMultipleSelectionSelectorWidget::SetToString(const QString &string)
{
    filePath = string;
    pFilePathLabel->SetText(string);
}

QWidget * FilePathMultipleSelectionSelectorWidget::CreateSelector()
{
    QWidget *pSelectorWidget = new QWidget();
    QGridLayout *pSelectorLayout = new QGridLayout();

    pSelectorLayout->setColumnStretch(0, 0);
    pSelectorLayout->setColumnStretch(1, 1);

    pSelectFilePathButton = new QPushButton();
    pSelectFilePathButton->setText("Select file path...");

    pSelectorLayout->addWidget(pSelectFilePathButton, 0, 0);

    pFilePathLabel = new SizeAwareElidingLabel(Qt::ElideLeft);

    pSelectorLayout->addWidget(pFilePathLabel, 0, 1);
    pSelectorWidget->setLayout(pSelectorLayout);

    QObject::connect(pSelectFilePathButton, SIGNAL(clicked()), this, SLOT(SelectFilePathButtonClicked()));
    return pSelectorWidget;
}

void FilePathMultipleSelectionSelectorWidget::SelectFilePathButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select image file"), QString(), tr("PNG images (*.png)"));

    if (filePath.length() > 0)
    {
        SetToString(filePath);
        emit StringSelected(filePath);
    }
}
