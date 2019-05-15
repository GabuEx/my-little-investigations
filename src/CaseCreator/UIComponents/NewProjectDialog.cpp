#include "NewProjectDialog.h"

#include <QFileDialog>
#include <QStandardPaths>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QRegExpValidator>

NewProjectDialog::NewProjectDialog(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags)
{
    QVBoxLayout *pRootLayout = new QVBoxLayout();
    QScrollArea *pRootScrollArea = new QScrollArea();

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    QGridLayout *pProjectNameLayout = new QGridLayout();

    pProjectNamePromptLabel = new QLabel("Project name: ");
    pProjectNameLayout->addWidget(pProjectNamePromptLabel, 0, 0);

    pProjectNameLineEdit = new QLineEdit();
    pProjectNameLineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9 ]+")));
    QObject::connect(pProjectNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(DoValidateProjectName()));

    pProjectNameLayout->addWidget(pProjectNameLineEdit, 0, 1);
    pMainLayout->addLayout(pProjectNameLayout);

    pProjectNameErrorLabel = new QLabel(" ");
    pProjectNameErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pProjectNameErrorLabel);

    QPushButton *pBaseProjectDirectoryPathSelectionButton = new QPushButton("Select base project directory");
    pBaseProjectDirectoryPathSelectionButton->setFixedWidth(400);
    QObject::connect(pBaseProjectDirectoryPathSelectionButton, SIGNAL(clicked()), this, SLOT(RetrieveBaseProjectDirectoryPath()));

    pMainLayout->addWidget(pBaseProjectDirectoryPathSelectionButton);

    QGridLayout *pBaseProjectDirectoryPathLayout = new QGridLayout();

    pBaseProjectDirectoryPathPromptLabel = new QLabel("Project directory path: ");
    pBaseProjectDirectoryPathLayout->addWidget(pBaseProjectDirectoryPathPromptLabel, 0, 0);

    pBaseProjectDirectoryPathLabel = new QLabel();
    pBaseProjectDirectoryPathLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pBaseProjectDirectoryPathLayout->addWidget(pBaseProjectDirectoryPathLabel, 0, 1);

    pMainLayout->addLayout(pBaseProjectDirectoryPathLayout);

    pBaseProjectDirectoryPathErrorLabel = new QLabel(" ");
    pBaseProjectDirectoryPathErrorLabel->setStyleSheet("QLabel { color: red; }");

    pMainLayout->addWidget(pBaseProjectDirectoryPathErrorLabel);

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
    setWindowTitle("New project creation");
}

void NewProjectDialog::RetrieveBaseProjectDirectoryPath()
{
    QString filePath =
        QFileDialog::getExistingDirectory(
            this,
            QString("Select base project directory"),
            baseProjectDirectoryPath);

    if (filePath.length() == 0)
    {
        return;
    }

    baseProjectDirectoryPath = filePath;

    SetProjectDirectoryPath();
}

void NewProjectDialog::InitFields()
{
    projectName = "Case 1";
    baseProjectDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QString("/My Little Investigations/Case Creator Projects");

    QDir baseProjectDir(baseProjectDirectoryPath);
    baseProjectDir.mkpath(".");

    pProjectNameLineEdit->setText(projectName);
    SetProjectDirectoryPath();

    pProjectNamePromptLabel->setStyleSheet("");
    pProjectNameErrorLabel->setText(" ");
    pBaseProjectDirectoryPathPromptLabel->setStyleSheet("");
    pBaseProjectDirectoryPathErrorLabel->setText(" ");

    pProjectNameLineEdit->setFocus();
}

bool NewProjectDialog::ValidateFields()
{
    bool valid = ValidateProjectName();

    return valid;
}


bool NewProjectDialog::ValidateProjectName()
{
    if (pProjectNameLineEdit->hasAcceptableInput())
    {
        projectName = pProjectNameLineEdit->text();

        SetProjectDirectoryPath();

        pProjectNamePromptLabel->setStyleSheet("");
        pProjectNameErrorLabel->setText(" ");
        return true;
    }
    else
    {
        pProjectNamePromptLabel->setStyleSheet("QLabel { color: red; }");
        pProjectNameErrorLabel->setText("Error: Project name cannot be blank.");
        return false;
    }
}

void NewProjectDialog::SetProjectDirectoryPath()
{
    projectDirectoryPath = baseProjectDirectoryPath + "/" + projectName;

    pBaseProjectDirectoryPathLabel->setText(QFontMetrics(pBaseProjectDirectoryPathLabel->font()).elidedText(projectDirectoryPath, Qt::ElideMiddle, pBaseProjectDirectoryPathLabel->width()));
    pBaseProjectDirectoryPathLabel->update();
}
