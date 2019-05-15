#include "NewFieldCutsceneDialog.h"

#include "CaseCreator/CaseContent/FieldCutscene.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

template<>
NewObjectDialog<FieldCutscene> * NewObjectDialog<FieldCutscene>::Create(QWidget *parent, Qt::WindowFlags flags)
{
    return new NewFieldCutsceneDialog(parent, flags);
}

NewFieldCutsceneDialog::NewFieldCutsceneDialog(QWidget *parent, Qt::WindowFlags flags) :
    NewObjectDialog<FieldCutscene>(parent, flags)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();

    QGridLayout *pIdLayout = new QGridLayout();

    pIdLayout->setColumnStretch(0, 0);
    pIdLayout->setColumnStretch(1, 1);

    pIdPromptLabel = new QLabel("Field cutscene ID: ");
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
    setWindowTitle("Field cutscene creation");
}

FieldCutscene * NewFieldCutsceneDialog::GetNewObject()
{
    return new FieldCutscene(id);
}

void NewFieldCutsceneDialog::InitFields()
{
    id = "";

    pIdLineEdit->setText("");

    pIdPromptLabel->setStyleSheet("");
    pIdErrorLabel->setText(" ");

    pIdLineEdit->setFocus();
}

bool NewFieldCutsceneDialog::ValidateFields()
{
    bool valid = ValidateId();

    return valid;
}

bool NewFieldCutsceneDialog::ValidateId()
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
        pIdErrorLabel->setText("Error: Foreground element ID cannot be blank.");
        return false;
    }
}
