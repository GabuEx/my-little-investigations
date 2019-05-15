#include "NewCharacterDialog.h"

#include "CaseCreator/CaseContent/Character.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QValidator>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QFileDialog>

template<>
NewObjectDialog<Character> * NewObjectDialog<Character>::Create(QWidget *parent, Qt::WindowFlags flags)
{
    return new NewCharacterDialog(parent, flags);
}

NewCharacterDialog::NewCharacterDialog(QWidget *parent, Qt::WindowFlags flags)
    : NewObjectDialog<Character>(parent, flags)
{
    QVBoxLayout *pRootLayout = new QVBoxLayout();
    QScrollArea *pRootScrollArea = new QScrollArea();

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    QGridLayout *pIdLayout = new QGridLayout();

    pIdLayout->setColumnStretch(0, 0);
    pIdLayout->setColumnStretch(1, 1);

    pIdPromptLabel = new QLabel("Character ID: ");
    pIdLayout->addWidget(pIdPromptLabel, 0, 0);

    pIdLineEdit = new QLineEdit();
    pIdLineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+")));

    pIdLayout->addWidget(pIdLineEdit, 0, 1);

    pMainLayout->addLayout(pIdLayout);

    pIdErrorLabel = new QLabel(" ");
    pIdErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pIdErrorLabel);

    QGridLayout *pNameLayout = new QGridLayout();

    pNameLayout->setColumnStretch(0, 0);
    pNameLayout->setColumnStretch(1, 1);

    pNamePromptLabel = new QLabel("Character name: ");
    pNameLayout->addWidget(pNamePromptLabel, 0, 0);

    pNameLineEdit = new QLineEdit();
    pNameLineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9 ]+")));

    pNameLayout->addWidget(pNameLineEdit, 0, 1);

    pMainLayout->addLayout(pNameLayout);

    pNameErrorLabel = new QLabel(" ");
    pNameErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pNameErrorLabel);

    QHBoxLayout *pButtonLayout = new QHBoxLayout();

    QPushButton *pOkButton = new QPushButton("OK");
    QPushButton *pCancelButton = new QPushButton("Cancel");

    QObject::connect(pOkButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    pButtonLayout->addWidget(pOkButton);
    pButtonLayout->addWidget(pCancelButton);

    pMainLayout->addLayout(pButtonLayout);
    pMainLayout->setSizeConstraint(QLayout::SetFixedSize);

    pRootScrollArea->setLayout(pMainLayout);
    pRootLayout->addWidget(pRootScrollArea);

    setLayout(pRootLayout);
    setWindowTitle("Character creation");
}

Character * NewCharacterDialog::GetNewObject()
{
    return new Character(id, name, "Neutral");
}

void NewCharacterDialog::InitFields()
{
    id = "";
    name = "";

    pIdLineEdit->setText("");

    pIdPromptLabel->setStyleSheet("");
    pIdErrorLabel->setText(" ");

    pIdLineEdit->setFocus();
}

bool NewCharacterDialog::ValidateFields()
{
    bool valid = ValidateId();
    valid = ValidateName() && valid;

    return valid;
}

bool NewCharacterDialog::ValidateId()
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

bool NewCharacterDialog::ValidateName()
{
    if (pNameLineEdit->hasAcceptableInput())
    {
        name = pNameLineEdit->text();

        pNamePromptLabel->setStyleSheet("");
        pNameErrorLabel->setText(" ");
        return true;
    }
    else
    {
        pNamePromptLabel->setStyleSheet("QLabel { color: red; }");
        pNameErrorLabel->setText("Error: Character name cannot be blank.");
        return false;
    }
}
