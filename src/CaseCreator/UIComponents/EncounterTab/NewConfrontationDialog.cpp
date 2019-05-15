#include "NewConfrontationDialog.h"

#include "CaseCreator/CaseContent/Conversation.h"

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
NewObjectDialog<Confrontation> * NewObjectDialog<Confrontation>::Create(QWidget *parent, Qt::WindowFlags flags)
{
    return new NewConfrontationDialog(parent, flags);
}

NewConfrontationDialog::NewConfrontationDialog(QWidget *parent, Qt::WindowFlags flags) :
    NewObjectDialog<Confrontation>(parent, flags)
{
    QVBoxLayout *pRootLayout = new QVBoxLayout();
    QScrollArea *pRootScrollArea = new QScrollArea();

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    QGridLayout *pIdLayout = new QGridLayout();

    pIdLayout->setColumnStretch(0, 0);
    pIdLayout->setColumnStretch(1, 1);

    pIdPromptLabel = new QLabel("Confrontation ID: ");
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

    pRootScrollArea->setLayout(pMainLayout);
    pRootLayout->addWidget(pRootScrollArea);

    setLayout(pRootLayout);
    setWindowTitle("Confrontation creation");
}

Confrontation * NewConfrontationDialog::GetNewObject()
{
    return new Confrontation(id);
}

void NewConfrontationDialog::InitFields()
{
    id = "";

    pIdLineEdit->setText("");

    pIdPromptLabel->setStyleSheet("");
    pIdErrorLabel->setText(" ");

    pIdLineEdit->setFocus();
}

bool NewConfrontationDialog::ValidateFields()
{
    bool valid = ValidateId();

    return valid;
}

bool NewConfrontationDialog::ValidateId()
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
        pIdErrorLabel->setText("Error: Confrontation ID cannot be blank.");
        return false;
    }
}
