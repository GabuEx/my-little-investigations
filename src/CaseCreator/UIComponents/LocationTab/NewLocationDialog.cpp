#include "NewLocationDialog.h"

#include "CaseCreator/CaseContent/Location.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QFileDialog>

template<>
NewObjectDialog<Location> * NewObjectDialog<Location>::Create(QWidget *parent, Qt::WindowFlags flags)
{
    return new NewLocationDialog(parent, flags);
}

NewLocationDialog::NewLocationDialog(QWidget *parent, Qt::WindowFlags flags) :
    NewObjectDialog<Location>(parent, flags)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();

    QGridLayout *pIdLayout = new QGridLayout();

    pIdLayout->setColumnStretch(0, 0);
    pIdLayout->setColumnStretch(1, 1);

    pIdPromptLabel = new QLabel("Location ID: ");
    pIdLayout->addWidget(pIdPromptLabel, 0, 0);

    pIdLineEdit = new QLineEdit();
    pIdLineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+")));

    pIdLayout->addWidget(pIdLineEdit, 0, 1);

    pMainLayout->addLayout(pIdLayout);

    pIdErrorLabel = new QLabel(" ");
    pIdErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pIdErrorLabel);

    QPushButton *pBackgroundFilePathSelectionButton = new QPushButton("Select background image");
    pBackgroundFilePathSelectionButton->setFixedWidth(400);
    QObject::connect(pBackgroundFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(RetrieveBackgroundFilePath()));

    pMainLayout->addWidget(pBackgroundFilePathSelectionButton);

    QGridLayout *pBackgroundFilePathLayout = new QGridLayout();

    pBackgroundFilePathLayout->setColumnStretch(0, 0);
    pBackgroundFilePathLayout->setColumnStretch(1, 1);

    pBackgroundFilePathPromptLabel = new QLabel("Background file path: ");
    pBackgroundFilePathLayout->addWidget(pBackgroundFilePathPromptLabel, 0, 0);

    pBackgroundFilePathLabel = new QLabel();
    pBackgroundFilePathLayout->addWidget(pBackgroundFilePathLabel, 0, 1);

    pMainLayout->addLayout(pBackgroundFilePathLayout);

    pBackgroundFilePathErrorLabel = new QLabel(" ");
    pBackgroundFilePathErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pBackgroundFilePathErrorLabel);

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
    setWindowTitle("Location creation");
}

Location * NewLocationDialog::GetNewObject()
{
    return new Location(id, backgroundFilePath);
}

void NewLocationDialog::RetrieveBackgroundFilePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select background image"), QString(), tr("PNG image files (*.png)"));

    if (filePath.length() == 0)
    {
        return;
    }

    if (ValidateBackgroundFilePath(filePath))
    {
        pBackgroundFilePathLabel->setText(QFontMetrics(pBackgroundFilePathLabel->font()).elidedText(backgroundFilePath, Qt::ElideMiddle, pBackgroundFilePathLabel->width()));
        pBackgroundFilePathLabel->update();
    }
}

void NewLocationDialog::InitFields()
{
    id = "";
    backgroundFilePath = "";

    pIdLineEdit->setText("");
    pBackgroundFilePathLabel->setText("");

    pIdPromptLabel->setStyleSheet("");
    pIdErrorLabel->setText(" ");
    pBackgroundFilePathPromptLabel->setStyleSheet("");
    pBackgroundFilePathErrorLabel->setText(" ");

    pIdLineEdit->setFocus();
}

bool NewLocationDialog::ValidateFields()
{
    bool valid = ValidateId();
    valid = ValidateBackgroundFilePath(backgroundFilePath) && valid;

    return valid;
}

bool NewLocationDialog::ValidateId()
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
        pIdErrorLabel->setText("Error: Location ID cannot be blank.");
        return false;
    }
}

bool NewLocationDialog::ValidateBackgroundFilePath(QString backgroundFilePath)
{
    if (QFile(backgroundFilePath).exists())
    {
        this->backgroundFilePath = backgroundFilePath;

        pBackgroundFilePathPromptLabel->setStyleSheet("");
        pBackgroundFilePathErrorLabel->setText(" ");
        return true;
    }
    else
    {
        pBackgroundFilePathPromptLabel->setStyleSheet("QLabel { color: red; }");
        pBackgroundFilePathErrorLabel->setText("Error: Must select an image for the location background.");
        return false;
    }
}
