#include "CharacterDialogSpriteManipulator.h"

#include <QScrollArea>
#include <QPushButton>
#include <QFileDialog>

#include "CaseCreator/CaseContent/CaseContent.h"

template<>
ObjectManipulator<Character::DialogEmotion> * ObjectManipulator<Character::DialogEmotion>::Create(QWidget *parent)
{
    return new CharacterDialogSpriteManipulator(parent);
}

CharacterDialogSpriteManipulator::CharacterDialogSpriteManipulator(QWidget *parent) :
    ObjectManipulator<Character::DialogEmotion>(parent)
{
    pSilentSpriteDrawingView = NULL;
    pTalkingSpriteDrawingView = NULL;

    QGridLayout *pRootLayout = new QGridLayout();
    pRootLayout->setRowStretch(0, 0);
    pRootLayout->setRowStretch(1, 1);

    QHBoxLayout *pSpriteLayout = new QHBoxLayout();

    pSpriteLayout->addStretch(1);

    pSilentSpriteGraphicsView = new QGraphicsView();
    pSilentSpriteGraphicsScene = new QGraphicsScene();

    pSilentSpriteGraphicsScene->setSceneRect(QRectF(0, 0, 480, 360));
    pSilentSpriteGraphicsView->setScene(pSilentSpriteGraphicsScene);
    pSilentSpriteGraphicsView->setSceneRect(pSilentSpriteGraphicsScene->sceneRect());
    pSilentSpriteGraphicsView->scale(0.5, 0.5);
    pSilentSpriteGraphicsView->setStyleSheet("background: transparent; border: transparent;");

    pSpriteLayout->addWidget(pSilentSpriteGraphicsView);

    pTalkingSpriteGraphicsView = new QGraphicsView();
    pTalkingSpriteGraphicsScene = new QGraphicsScene();

    pTalkingSpriteGraphicsScene->setSceneRect(QRectF(0, 0, 480, 360));
    pTalkingSpriteGraphicsView->setScene(pTalkingSpriteGraphicsScene);
    pTalkingSpriteGraphicsView->setSceneRect(pTalkingSpriteGraphicsScene->sceneRect());
    pTalkingSpriteGraphicsView->scale(0.5, 0.5);
    pTalkingSpriteGraphicsView->setStyleSheet("background: transparent; border: transparent;");

    pSpriteLayout->addWidget(pTalkingSpriteGraphicsView);

    pSpriteLayout->addStretch(1);

    pRootLayout->addLayout(pSpriteLayout, 0, 0);

    QScrollArea *pRootScrollArea = new QScrollArea();
    pRootScrollArea->setWidgetResizable(true);

    QWidget *pLayoutWidget = new QWidget();
    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Base sprite"));

    QGridLayout *pBaseSpriteLayout = new QGridLayout();
    pBaseSpriteLayout->setColumnStretch(0, 0);
    pBaseSpriteLayout->setColumnStretch(1, 1);

    QPushButton *pBaseSpriteFilePathSelectionButton = new QPushButton("Select file path...");
    pBaseSpriteLayout->addWidget(pBaseSpriteFilePathSelectionButton, 0, 0);

    pBaseSpriteFilePathLabel = new QLabel();
    pBaseSpriteLayout->addWidget(pBaseSpriteFilePathLabel, 0, 1);

    pLayout->addLayout(pBaseSpriteLayout);

    pEyeSpriteFilePathSelector = new MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget>();
    pMouthSpriteFilePathSelector = new MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget>();

    QHBoxLayout *pEyeSpriteHeaderLayout = new QHBoxLayout();

    pEyeSpriteHeaderLayout->addWidget(new QLabel("Eye sprites"));

    QPushButton *pEyeSpriteFilePathSelectionButton = new QPushButton("Select file paths...");
    pEyeSpriteHeaderLayout->addWidget(pEyeSpriteFilePathSelectionButton);

    pEyeSpriteHeaderLayout->addStretch(1);

    pLayout->addLayout(pEyeSpriteHeaderLayout);
    pLayout->addWidget(pEyeSpriteFilePathSelector);

    QHBoxLayout *pMouthSpriteHeaderLayout = new QHBoxLayout();

    pMouthSpriteHeaderLayout->addWidget(new QLabel("Mouth sprites"));

    QPushButton *pMouthSpriteFilePathSelectionButton = new QPushButton("Select file paths...");
    pMouthSpriteHeaderLayout->addWidget(pMouthSpriteFilePathSelectionButton);

    pMouthSpriteHeaderLayout->addStretch(1);

    pLayout->addLayout(pMouthSpriteHeaderLayout);
    pLayout->addWidget(pMouthSpriteFilePathSelector);

    pLayout->addStretch(1);

    pLayoutWidget->setLayout(pLayout);
    pRootScrollArea->setWidget(pLayoutWidget);
    pRootLayout->addWidget(pRootScrollArea, 1, 0);

    setLayout(pRootLayout);

    QObject::connect(pBaseSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(BaseSpriteFilePathSelectionButtonClicked()));
    QObject::connect(pEyeSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(EyeSpriteFilePathsSelectionButtonClicked()));
    QObject::connect(pMouthSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(MouthSpriteFilePathsSelectionButtonClicked()));

    QObject::connect(pEyeSpriteFilePathSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(EyeSpriteFilePathSelectorStringChanged(int,QString)));
    QObject::connect(pEyeSpriteFilePathSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(EyeSpriteFilePathSelectorStringAdded(int,QString)));
    QObject::connect(pEyeSpriteFilePathSelector, SIGNAL(StringRemoved(int)), this, SLOT(EyeSpriteFilePathSelectorStringRemoved(int)));

    QObject::connect(pMouthSpriteFilePathSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(MouthSpriteFilePathSelectorStringChanged(int,QString)));
    QObject::connect(pMouthSpriteFilePathSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(MouthSpriteFilePathSelectorStringAdded(int,QString)));
    QObject::connect(pMouthSpriteFilePathSelector, SIGNAL(StringRemoved(int)), this, SLOT(MouthSpriteFilePathSelectorStringRemoved(int)));
}

void CharacterDialogSpriteManipulator::Init(Character::DialogEmotion *pObject)
{
    ObjectManipulator<Character::DialogEmotion>::Init(pObject);

    ReloadDrawingViews();

    pBaseSpriteFilePathLabel->setText(pObject->GetBaseFilePath());
    pEyeSpriteFilePathSelector->SetSelections(pObject->GetEyeFilePaths());
    pMouthSpriteFilePathSelector->SetSelections(pObject->GetMouthFilePaths());

    PullChanges();
}

void CharacterDialogSpriteManipulator::Reset()
{
    //pEncounterSelector->Reset();
}

void CharacterDialogSpriteManipulator::PullChanges()
{
}

void CharacterDialogSpriteManipulator::BaseSpriteFilePathSelectionButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select image file"), QString(), tr("PNG images (*.png)"));

    if (filePath.length() > 0)
    {
        pObject->SetBaseFilePath(filePath);
        pBaseSpriteFilePathLabel->setText(filePath);
        ReloadDrawingViews();
    }
}

void CharacterDialogSpriteManipulator::EyeSpriteFilePathsSelectionButtonClicked()
{
    QStringList filePaths = GetMultipleImages();

    if (filePaths.size() > 0)
    {
        pEyeSpriteFilePathSelector->SetSelections(filePaths);
        pObject->SetEyeFrames(filePaths);
        ReloadDrawingViews();
    }
}

void CharacterDialogSpriteManipulator::MouthSpriteFilePathsSelectionButtonClicked()
{
    QStringList filePaths = GetMultipleImages();

    if (filePaths.size() > 0)
    {
        pMouthSpriteFilePathSelector->SetSelections(filePaths);
        pObject->SetMouthFrames(filePaths);
        ReloadDrawingViews();
    }
}

void CharacterDialogSpriteManipulator::EyeSpriteFilePathSelectorStringChanged(int index, const QString &newString)
{
    pObject->SetEyeFrame(index, newString);
    ReloadDrawingViews();
}

void CharacterDialogSpriteManipulator::EyeSpriteFilePathSelectorStringAdded(int index, const QString &newString)
{
    pObject->InsertEyeFrame(index, newString);
    ReloadDrawingViews();
}

void CharacterDialogSpriteManipulator::EyeSpriteFilePathSelectorStringRemoved(int index)
{
    pObject->RemoveEyeFrame(index);
    ReloadDrawingViews();
}

void CharacterDialogSpriteManipulator::MouthSpriteFilePathSelectorStringChanged(int index, const QString &newString)
{
    pObject->SetMouthFrame(index, newString);
    ReloadDrawingViews();
}

void CharacterDialogSpriteManipulator::MouthSpriteFilePathSelectorStringAdded(int index, const QString &newString)
{
    pObject->InsertMouthFrame(index, newString);
    ReloadDrawingViews();
}

void CharacterDialogSpriteManipulator::MouthSpriteFilePathSelectorStringRemoved(int index)
{
    pObject->RemoveMouthFrame(index);
    ReloadDrawingViews();
}

QStringList CharacterDialogSpriteManipulator::GetMultipleImages()
{
    return QFileDialog::getOpenFileNames(this, tr("Select image files"), QString(), tr("PNG images (*.png)"));
}

void CharacterDialogSpriteManipulator::ReloadDrawingViews()
{
    if (pSilentSpriteDrawingView != NULL)
    {
        pSilentSpriteDrawingView->Reset();
        delete pSilentSpriteDrawingView;
    }

    if (pTalkingSpriteDrawingView != NULL)
    {
        pTalkingSpriteDrawingView->Reset();
        delete pTalkingSpriteDrawingView;
    }

    pSilentSpriteDrawingView = pObject->GetDrawingView(NULL);
    pTalkingSpriteDrawingView = pObject->GetDrawingView(NULL);
    pTalkingSpriteDrawingView->SetIsSpeaking(true);

    pSilentSpriteDrawingView->Draw(pSilentSpriteGraphicsScene);
    pTalkingSpriteDrawingView->Draw(pTalkingSpriteGraphicsScene);
}
