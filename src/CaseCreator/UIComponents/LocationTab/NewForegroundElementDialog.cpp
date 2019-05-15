#include "NewForegroundElementDialog.h"

#include "CaseCreator/CaseContent/ForegroundElement.h"

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
NewObjectDialog<ForegroundElement> * NewObjectDialog<ForegroundElement>::Create(QWidget *parent, Qt::WindowFlags flags)
{
    return new NewForegroundElementDialog(parent, flags);
}

NewForegroundElementDialog::NewForegroundElementDialog(QWidget *parent, Qt::WindowFlags flags) :
    NewObjectDialog<ForegroundElement>(parent, flags)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();

    QGridLayout *pNameLayout = new QGridLayout();

    pNameLayout->setColumnStretch(0, 0);
    pNameLayout->setColumnStretch(1, 1);

    pNamePromptLabel = new QLabel("Location ID: ");
    pNameLayout->addWidget(pNamePromptLabel, 0, 0);

    pNameLineEdit = new QLineEdit();
    pNameLineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+")));

    pNameLayout->addWidget(pNameLineEdit, 0, 1);

    pMainLayout->addLayout(pNameLayout);

    pNameErrorLabel = new QLabel(" ");
    pNameErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pNameErrorLabel);

    QPushButton *pSpriteFilePathSelectionButton = new QPushButton("Select sprite image");
    pSpriteFilePathSelectionButton->setFixedWidth(400);
    QObject::connect(pSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(RetrieveSpriteFilePath()));

    pMainLayout->addWidget(pSpriteFilePathSelectionButton);

    QGridLayout *pSpriteFilePathLayout = new QGridLayout();

    pSpriteFilePathLayout->setColumnStretch(0, 0);
    pSpriteFilePathLayout->setColumnStretch(1, 1);

    pSpriteFilePathPromptLabel = new QLabel("Sprite file path: ");
    pSpriteFilePathLayout->addWidget(pSpriteFilePathPromptLabel, 0, 0);

    pSpriteFilePathLabel = new QLabel();
    pSpriteFilePathLayout->addWidget(pSpriteFilePathLabel, 0, 1);

    pMainLayout->addLayout(pSpriteFilePathLayout);

    pSpriteFilePathErrorLabel = new QLabel(" ");
    pSpriteFilePathErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pSpriteFilePathErrorLabel);

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

ForegroundElement * NewForegroundElementDialog::GetNewObject()
{
    return new ForegroundElement(name, spriteFilePath);
}

void NewForegroundElementDialog::RetrieveSpriteFilePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select sprite image"), QString(), tr("PNG image files (*.png)"));

    if (filePath.length() == 0)
    {
        return;
    }

    if (ValidateSpriteFilePath(filePath))
    {
        pSpriteFilePathLabel->setText(QFontMetrics(pSpriteFilePathLabel->font()).elidedText(spriteFilePath, Qt::ElideMiddle, pSpriteFilePathLabel->width()));
        pSpriteFilePathLabel->update();
    }
}

void NewForegroundElementDialog::InitFields()
{
    name = "";
    spriteFilePath = "";

    pNameLineEdit->setText("");
    pSpriteFilePathLabel->setText("");

    pNamePromptLabel->setStyleSheet("");
    pNameErrorLabel->setText(" ");
    pSpriteFilePathPromptLabel->setStyleSheet("");
    pSpriteFilePathErrorLabel->setText(" ");

    pNameLineEdit->setFocus();
}

bool NewForegroundElementDialog::ValidateFields()
{
    bool valid = ValidateName();
    valid = ValidateSpriteFilePath(spriteFilePath) && valid;

    return valid;
}

bool NewForegroundElementDialog::ValidateName()
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
        pNameErrorLabel->setText("Error: Foreground element name cannot be blank.");
        return false;
    }
}

bool NewForegroundElementDialog::ValidateSpriteFilePath(QString spriteFilePath)
{
    if (QFile(spriteFilePath).exists())
    {
        this->spriteFilePath = spriteFilePath;

        pSpriteFilePathPromptLabel->setStyleSheet("");
        pSpriteFilePathErrorLabel->setText(" ");
        return true;
    }
    else
    {
        pSpriteFilePathPromptLabel->setStyleSheet("QLabel { color: red; }");
        pSpriteFilePathErrorLabel->setText("Error: Must select an image for the foreground element sprite.");
        return false;
    }
}
