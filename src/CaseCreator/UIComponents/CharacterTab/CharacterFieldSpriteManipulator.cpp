#include "CharacterFieldSpriteManipulator.h"

#include <QGridLayout>
#include <QScrollArea>
#include <QFileDialog>

template<>
ObjectManipulator<Character::FieldSprite> * ObjectManipulator<Character::FieldSprite>::Create(QWidget *parent)
{
    return new CharacterFieldSpriteManipulator(parent);
}

CharacterFieldSpriteManipulator::CharacterFieldSpriteManipulator(QWidget *parent) :
    ObjectManipulator<Character::FieldSprite>(parent)
{
    pAnimationDrawingView = NULL;

    QGridLayout *pRootLayout = new QGridLayout();
    pRootLayout->setRowStretch(0, 0);
    pRootLayout->setRowStretch(1, 1);

    pSpriteGraphicsView = new QGraphicsView();
    pSpriteGraphicsScene = new QGraphicsScene();

    pSpriteGraphicsScene->setSceneRect(QRectF(0, 0, 200, 200));
    pSpriteGraphicsView->setScene(pSpriteGraphicsScene);
    pSpriteGraphicsView->setSceneRect(pSpriteGraphicsScene->sceneRect());
    pSpriteGraphicsView->setStyleSheet("background: transparent; border: transparent;");

    pRootLayout->addWidget(pSpriteGraphicsView, 0, 0);

    QScrollArea *pRootScrollArea = new QScrollArea();
    pRootScrollArea->setWidgetResizable(true);

    QWidget *pLayoutWidget = new QWidget();
    QVBoxLayout *pLayout = new QVBoxLayout();

    QHBoxLayout *pDownSpriteHeaderLayout = new QHBoxLayout();

    pDownSpriteHeaderLayout->addWidget(new QLabel("Down-facing sprites"));

    QPushButton *pDownSpriteFilePathSelectionButton = new QPushButton("Select file paths...");
    pDownSpriteHeaderLayout->addWidget(pDownSpriteFilePathSelectionButton);

    pDownSpriteHeaderLayout->addStretch(1);

    pLayout->addLayout(pDownSpriteHeaderLayout);

    pDownFrameSpriteFilePathSelector = new MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget>();
    pLayout->addWidget(pDownFrameSpriteFilePathSelector);

    QHBoxLayout *pDiagonalDownSpriteHeaderLayout = new QHBoxLayout();

    pDiagonalDownSpriteHeaderLayout->addWidget(new QLabel("Diagonal down-facing sprites"));

    QPushButton *pDiagonalDownSpriteFilePathSelectionButton = new QPushButton("Select file paths...");
    pDiagonalDownSpriteHeaderLayout->addWidget(pDiagonalDownSpriteFilePathSelectionButton);

    pDiagonalDownSpriteHeaderLayout->addStretch(1);

    pLayout->addLayout(pDiagonalDownSpriteHeaderLayout);

    pDiagonalDownFrameSpriteFilePathSelector = new MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget>();
    pLayout->addWidget(pDiagonalDownFrameSpriteFilePathSelector);

    QHBoxLayout *pSideSpriteHeaderLayout = new QHBoxLayout();

    pSideSpriteHeaderLayout->addWidget(new QLabel("Side-facing sprites"));

    QPushButton *pSideSpriteFilePathSelectionButton = new QPushButton("Select file paths...");
    pSideSpriteHeaderLayout->addWidget(pSideSpriteFilePathSelectionButton);

    pSideSpriteHeaderLayout->addStretch(1);

    pLayout->addLayout(pSideSpriteHeaderLayout);

    pSideFrameSpriteFilePathSelector = new MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget>();
    pLayout->addWidget(pSideFrameSpriteFilePathSelector);

    QHBoxLayout *pDiagonalUpSpriteHeaderLayout = new QHBoxLayout();

    pDiagonalUpSpriteHeaderLayout->addWidget(new QLabel("Diagonal up-facing sprites"));

    QPushButton *pDiagonalUpSpriteFilePathSelectionButton = new QPushButton("Select file paths...");
    pDiagonalUpSpriteHeaderLayout->addWidget(pDiagonalUpSpriteFilePathSelectionButton);

    pDiagonalUpSpriteHeaderLayout->addStretch(1);

    pLayout->addLayout(pDiagonalUpSpriteHeaderLayout);

    pDiagonalUpFrameSpriteFilePathSelector = new MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget>();
    pLayout->addWidget(pDiagonalUpFrameSpriteFilePathSelector);

    QHBoxLayout *pUpSpriteHeaderLayout = new QHBoxLayout();

    pUpSpriteHeaderLayout->addWidget(new QLabel("Up-facing sprites"));

    QPushButton *pUpSpriteFilePathSelectionButton = new QPushButton("Select file paths...");
    pUpSpriteHeaderLayout->addWidget(pUpSpriteFilePathSelectionButton);

    pUpSpriteHeaderLayout->addStretch(1);

    pLayout->addLayout(pUpSpriteHeaderLayout);

    pUpFrameSpriteFilePathSelector = new MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget>();
    pLayout->addWidget(pUpFrameSpriteFilePathSelector);

    pLayout->addStretch(1);

    pLayoutWidget->setLayout(pLayout);
    pRootScrollArea->setWidget(pLayoutWidget);
    pRootLayout->addWidget(pRootScrollArea, 1, 0);

    setLayout(pRootLayout);

    QObject::connect(pDownSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(DownSpriteFilePathsSelectionButtonClicked()));
    QObject::connect(pDiagonalDownSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(DiagonalDownSpriteFilePathsSelectionButtonClicked()));
    QObject::connect(pSideSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(SideSpriteFilePathsSelectionButtonClicked()));
    QObject::connect(pDiagonalUpSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(DiagonalUpSpriteFilePathsSelectionButtonClicked()));
    QObject::connect(pUpSpriteFilePathSelectionButton, SIGNAL(clicked()), this, SLOT(UpSpriteFilePathsSelectionButtonClicked()));

    QObject::connect(pDownFrameSpriteFilePathSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(DownFrameSpriteFilePathSelectorStringChanged(int,QString)));
    QObject::connect(pDownFrameSpriteFilePathSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(DownFrameSpriteFilePathSelectorStringAdded(int,QString)));
    QObject::connect(pDownFrameSpriteFilePathSelector, SIGNAL(StringRemoved(int)), this, SLOT(DownFrameSpriteFilePathSelectorStringRemoved(int)));
    QObject::connect(pDiagonalDownFrameSpriteFilePathSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(DiagonalDownFrameSpriteFilePathSelectorStringChanged(int,QString)));
    QObject::connect(pDiagonalDownFrameSpriteFilePathSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(DiagonalDownFrameSpriteFilePathSelectorStringAdded(int,QString)));
    QObject::connect(pDiagonalDownFrameSpriteFilePathSelector, SIGNAL(StringRemoved(int)), this, SLOT(DiagonalDownFrameSpriteFilePathSelectorStringRemoved(int)));
    QObject::connect(pSideFrameSpriteFilePathSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(SideFrameSpriteFilePathSelectorStringChanged(int,QString)));
    QObject::connect(pSideFrameSpriteFilePathSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(SideFrameSpriteFilePathSelectorStringAdded(int,QString)));
    QObject::connect(pSideFrameSpriteFilePathSelector, SIGNAL(StringRemoved(int)), this, SLOT(SideFrameSpriteFilePathSelectorStringRemoved(int)));
    QObject::connect(pDiagonalUpFrameSpriteFilePathSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(DiagonalUpFrameSpriteFilePathSelectorStringChanged(int,QString)));
    QObject::connect(pDiagonalUpFrameSpriteFilePathSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(DiagonalUpFrameSpriteFilePathSelectorStringAdded(int,QString)));
    QObject::connect(pDiagonalUpFrameSpriteFilePathSelector, SIGNAL(StringRemoved(int)), this, SLOT(DiagonalUpFrameSpriteFilePathSelectorStringRemoved(int)));
    QObject::connect(pUpFrameSpriteFilePathSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(UpFrameSpriteFilePathSelectorStringChanged(int,QString)));
    QObject::connect(pUpFrameSpriteFilePathSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(UpFrameSpriteFilePathSelectorStringAdded(int,QString)));
    QObject::connect(pUpFrameSpriteFilePathSelector, SIGNAL(StringRemoved(int)), this, SLOT(UpFrameSpriteFilePathSelectorStringRemoved(int)));
}

void CharacterFieldSpriteManipulator::Init(Character::FieldSprite *pObject)
{
    ObjectManipulator<Character::FieldSprite>::Init(pObject);

    if (pAnimationDrawingView != NULL)
    {
        pAnimationDrawingView->Reset();
        delete pAnimationDrawingView;
    }

    pAnimationDrawingView = new Animation::CompositeDrawingView(NULL);

    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDown), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown), true /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionSide), true /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp), true /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionUp), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionSide), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown), false /* flipHorizontal */);

    pDownFrameSpriteFilePathSelector->SetSelections(pObject->GetAnimationForDirection(FieldCharacterDirectionDown)->GetFramePaths());
    pDiagonalDownFrameSpriteFilePathSelector->SetSelections(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown)->GetFramePaths());
    pSideFrameSpriteFilePathSelector->SetSelections(pObject->GetAnimationForDirection(FieldCharacterDirectionSide)->GetFramePaths());
    pDiagonalUpFrameSpriteFilePathSelector->SetSelections(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp)->GetFramePaths());
    pUpFrameSpriteFilePathSelector->SetSelections(pObject->GetAnimationForDirection(FieldCharacterDirectionUp)->GetFramePaths());

    pAnimationDrawingView->Draw(pSpriteGraphicsScene);

    PullChanges();
}

void CharacterFieldSpriteManipulator::Reset()
{
    //pEncounterSelector->Reset();
}

void CharacterFieldSpriteManipulator::PullChanges()
{
}

void CharacterFieldSpriteManipulator::DownSpriteFilePathsSelectionButtonClicked()
{
    QStringList filePaths = GetMultipleImages();

    if (filePaths.size() > 0)
    {
        Animation *pAnimation = pObject->GetAnimationForDirection(FieldCharacterDirectionDown);

        pDownFrameSpriteFilePathSelector->SetSelections(filePaths);
        pAnimation->SetFrames(filePaths);
        pAnimationDrawingView->ReplaceAnimation(0, pAnimation, false /* flipHorizontal */);
    }
}

void CharacterFieldSpriteManipulator::DiagonalDownSpriteFilePathsSelectionButtonClicked()
{
    QStringList filePaths = GetMultipleImages();

    if (filePaths.size() > 0)
    {
        Animation *pAnimation = pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown);

        pDiagonalDownFrameSpriteFilePathSelector->SetSelections(filePaths);
        pAnimation->SetFrames(filePaths);
        pAnimationDrawingView->ReplaceAnimation(1, pAnimation, true /* flipHorizontal */);
        pAnimationDrawingView->ReplaceAnimation(7, pAnimation, false /* flipHorizontal */);
    }
}

void CharacterFieldSpriteManipulator::SideSpriteFilePathsSelectionButtonClicked()
{
    QStringList filePaths = GetMultipleImages();

    if (filePaths.size() > 0)
    {
        Animation *pAnimation = pObject->GetAnimationForDirection(FieldCharacterDirectionSide);

        pSideFrameSpriteFilePathSelector->SetSelections(filePaths);
        pAnimation->SetFrames(filePaths);
        pAnimationDrawingView->ReplaceAnimation(2, pAnimation, true /* flipHorizontal */);
        pAnimationDrawingView->ReplaceAnimation(6, pAnimation, false /* flipHorizontal */);
    }
}

void CharacterFieldSpriteManipulator::DiagonalUpSpriteFilePathsSelectionButtonClicked()
{
    QStringList filePaths = GetMultipleImages();

    if (filePaths.size() > 0)
    {
        Animation *pAnimation = pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp);

        pDiagonalUpFrameSpriteFilePathSelector->SetSelections(filePaths);
        pAnimation->SetFrames(filePaths);
        pAnimationDrawingView->ReplaceAnimation(3, pAnimation, true /* flipHorizontal */);
        pAnimationDrawingView->ReplaceAnimation(5, pAnimation, false /* flipHorizontal */);
    }
}

void CharacterFieldSpriteManipulator::UpSpriteFilePathsSelectionButtonClicked()
{
    QStringList filePaths = GetMultipleImages();

    if (filePaths.size() > 0)
    {
        Animation *pAnimation = pObject->GetAnimationForDirection(FieldCharacterDirectionUp);

        pUpFrameSpriteFilePathSelector->SetSelections(filePaths);
        pAnimation->SetFrames(filePaths);
        pAnimationDrawingView->ReplaceAnimation(4, pAnimation, false /* flipHorizontal */);
    }
}

QStringList CharacterFieldSpriteManipulator::GetMultipleImages()
{
    return QFileDialog::getOpenFileNames(this, tr("Select image files"), QString(), tr("PNG images (*.png)"));
}

void CharacterFieldSpriteManipulator::UpFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionUp)->SetFrame(index, newString);
}

void CharacterFieldSpriteManipulator::UpFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionUp)->InsertFrame(index, newString);
}

void CharacterFieldSpriteManipulator::UpFrameSpriteFilePathSelectorStringRemoved(int index)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionUp)->RemoveFrame(index);
}

void CharacterFieldSpriteManipulator::DiagonalUpFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp)->SetFrame(index, newString);
}

void CharacterFieldSpriteManipulator::DiagonalUpFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp)->InsertFrame(index, newString);
}

void CharacterFieldSpriteManipulator::DiagonalUpFrameSpriteFilePathSelectorStringRemoved(int index)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp)->RemoveFrame(index);
}

void CharacterFieldSpriteManipulator::SideFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionSide)->SetFrame(index, newString);
}

void CharacterFieldSpriteManipulator::SideFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionSide)->InsertFrame(index, newString);
}

void CharacterFieldSpriteManipulator::SideFrameSpriteFilePathSelectorStringRemoved(int index)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionSide)->RemoveFrame(index);
}

void CharacterFieldSpriteManipulator::DiagonalDownFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown)->SetFrame(index, newString);
}

void CharacterFieldSpriteManipulator::DiagonalDownFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown)->InsertFrame(index, newString);
}

void CharacterFieldSpriteManipulator::DiagonalDownFrameSpriteFilePathSelectorStringRemoved(int index)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown)->RemoveFrame(index);
}

void CharacterFieldSpriteManipulator::DownFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDown)->SetFrame(index, newString);
}

void CharacterFieldSpriteManipulator::DownFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDown)->InsertFrame(index, newString);
}

void CharacterFieldSpriteManipulator::DownFrameSpriteFilePathSelectorStringRemoved(int index)
{
    pObject->GetAnimationForDirection(FieldCharacterDirectionDown)->RemoveFrame(index);
}

void CharacterFieldSpriteManipulator::ReloadDrawingViews()
{
    if (pAnimationDrawingView != NULL)
    {
        pAnimationDrawingView->Reset();
        delete pAnimationDrawingView;
    }

    pAnimationDrawingView = new Animation::CompositeDrawingView(NULL);

    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDown), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown), true /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionSide), true /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp), true /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionUp), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalUp), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionSide), false /* flipHorizontal */);
    pAnimationDrawingView->AddAnimation(pObject->GetAnimationForDirection(FieldCharacterDirectionDiagonalDown), false /* flipHorizontal */);

    pAnimationDrawingView->Draw(pSpriteGraphicsScene);
}
