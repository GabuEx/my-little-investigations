#include "Flag.h"

#include "CaseCreator/CaseContent/CaseContent.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

Flag::Flag(const QString &id)
{
    this->id = id;
}

Flag::Flag(Staging::Flag *pStagingFlag)
{
    this->id = pStagingFlag->Id;
}

NewFlagDialog::NewFlagDialog(QWidget *parent, Qt::WindowFlags flags)
    : NewObjectDialog<Flag>(parent, flags)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();

    QGridLayout *pIdLayout = new QGridLayout();

    pIdLayout->setColumnStretch(0, 0);
    pIdLayout->setColumnStretch(1, 1);

    pIdPromptLabel = new QLabel("Flag ID: ");
    pIdLayout->addWidget(pIdPromptLabel, 0, 0);

    pIdLineEdit = new QLineEdit();
    pIdLineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+")));

    pIdLayout->addWidget(pIdLineEdit, 0, 1);

    pMainLayout->addLayout(pIdLayout);

    pIdErrorLabel = new QLabel(" ");
    pIdErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pIdErrorLabel);

    QHBoxLayout *pButtonLayout = new QHBoxLayout();

    QPushButton *pOkButton = new QPushButton("OK");
    QPushButton *pCancelButton = new QPushButton("Cancel");

    QObject::connect(pOkButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    pButtonLayout->addWidget(pOkButton);
    pButtonLayout->addWidget(pCancelButton);

    pMainLayout->addLayout(pButtonLayout);
    pMainLayout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(pMainLayout);
    setWindowTitle("Flag creation");
}

Flag * NewFlagDialog::GetNewObject()
{
    return new Flag(id);
}

void NewFlagDialog::InitFields()
{
    id = "";
}

bool NewFlagDialog::ValidateFields()
{
    return ValidateId();
}

bool NewFlagDialog::ValidateId()
{
    if (pIdLineEdit->hasAcceptableInput())
    {
        id = pIdLineEdit->text();

        pIdPromptLabel->setStyleSheet("");
        pIdErrorLabel->setText(" ");
        return true;
    }
    else
    {
        pIdPromptLabel->setStyleSheet("QLabel { color: red; }");
        pIdErrorLabel->setText("Error: Character ID cannot be blank.");
        return false;
    }
}

FlagEditor::FlagEditor(QWidget *parent)
    : QComboBox(parent)
{
    flagIds = CaseContent::GetInstance()->GetIds<Flag>();
    previousIndex = -1;

    addItems(flagIds);
    addItem("New flag...");

    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString FlagEditor::GetId()
{
    return flagIds[currentIndex()];
}

void FlagEditor::SetToId(const QString &id)
{
    int indexOfCurrentFlag = flagIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCurrentFlag >= 0)
    {
        setCurrentIndex(indexOfCurrentFlag);
    }
}

void FlagEditor::CurrentIndexChanged(int currentIndex)
{
    if (currentIndex < flagIds.length())
    {
        previousIndex = currentIndex;
        emit FlagSelected(flagIds[currentIndex]);
    }
    else if (!creatingNewFlag)
    {
        NewFlagDialog *pNewFlagDialog = new NewFlagDialog();

        if (pNewFlagDialog->exec() == QDialog::Accepted)
        {
            creatingNewFlag = true;

            Flag *pNewFlag = pNewFlagDialog->GetNewObject();
            CaseContent::GetInstance()->Add<Flag>(pNewFlag->GetId(), pNewFlag);

            int insertionPosition = 0;

            for (QString id : flagIds)
            {
                if (id.compare(pNewFlag->GetId()) >= 0)
                {
                    break;
                }

                insertionPosition++;
            }

            flagIds.insert(insertionPosition, pNewFlag->GetId());
            insertItem(insertionPosition, pNewFlag->GetId());
            setCurrentIndex(insertionPosition);

            creatingNewFlag = false;
        }
        else
        {
            setCurrentIndex(previousIndex);
        }
    }
}
