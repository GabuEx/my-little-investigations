#include "ZoomedViewManipulator.h"

#include <QLabel>
#include <QSplitter>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "CaseCreator/UIComponents/TemplateHelpers/ObjectManipulator.h"

template<>
ObjectManipulator<ZoomedView> * ObjectManipulator<ZoomedView>::Create(QWidget *parent)
{
    return new ZoomedViewManipulator(parent);
}

ZoomedViewManipulator::ZoomedViewManipulator(QWidget *parent) :
    ObjectManipulator<ZoomedView>(parent)
{
    pMainLayout = new QVBoxLayout();

    // TODO: Finish this so we can display the background and foreground elements in this zoomed view.
    pZoomedViewView = new ManipulatableGraphicsView();
    pZoomedViewView->setMouseTracking(true);
    pZoomedViewScene = new ManipulatableGraphicsScene();
    pZoomedViewView->SetScene(pZoomedViewScene);

    QObject::connect(pZoomedViewScene, SIGNAL(SelectionChanged(IManipulatable*,IManipulatableObject*)), this, SLOT(SceneSelectionChanged(IManipulatable*,IManipulatableObject*)));

    pEditingWidget = new QWidget();
    pEditingLayout = new QVBoxLayout();

    pEditingWidget->setLayout(pEditingLayout);
    pEditingWidget->hide();

    pContentSplitter = new QSplitter();
    pContentSplitter->addWidget(pZoomedViewView);
    pContentSplitter->addWidget(pEditingWidget);
    pContentSplitter->setStretchFactor(0, 1);
    pContentSplitter->setStretchFactor(1, 0);

    pMainLayout->addWidget(pContentSplitter);
    setLayout(pMainLayout);

    pManipulationWidget = NULL;
    pSelectedManipulatable = NULL;
    pSelectedObject = NULL;
    pZoomedViewOverviewView = NULL;
}

ZoomedViewManipulator::~ZoomedViewManipulator()
{
    Reset();
}

void ZoomedViewManipulator::Init(ZoomedView *pObject)
{
    if (pObject == NULL)
    {
        return;
    }

    ObjectManipulator<ZoomedView>::Init(pObject);

    pZoomedViewOverviewView = pObject->GetOverviewView();
    pZoomedViewScene->SetManipulationSurface(pZoomedViewOverviewView);

    pZoomedViewScene->clear();
    pZoomedViewOverviewView->Draw(pZoomedViewScene);
    pZoomedViewScene->update();

    pZoomedViewScene->setSceneRect(pZoomedViewOverviewView->GetSceneRect());

    UpdateScene();
    SceneSelectionChanged(NULL, NULL);
}

void ZoomedViewManipulator::Reset()
{
}

bool ZoomedViewManipulator::Undo()
{
    return false;
}

bool ZoomedViewManipulator::Redo()
{
    return false;
}

void ZoomedViewManipulator::SceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject)
{
    if (pSelectedObject != pNewSelectedObject)
    {
        ObjectManipulatorSlots *pOldManipulationWidget = pManipulationWidget;

        if (pNewSelectedObject != NULL)
        {
            pManipulationWidget = pNewSelectedObject->GetManipulationWidget();
        }
        else
        {
            pManipulationWidget = NULL;
        }

        if (pManipulationWidget != pOldManipulationWidget)
        {
            if (pOldManipulationWidget != NULL)
            {
                QObject::disconnect(pOldManipulationWidget, SIGNAL(ValueChanged()), this, SLOT(ManipulationWidgetValueChanged()));
                QObject::disconnect(pOldManipulationWidget, SIGNAL(LargeSizeRequested()), this, SLOT(ManipulationWidgetLargeSizeRequested()));
                QObject::disconnect(pOldManipulationWidget, SIGNAL(SmallSizeRequested()), this, SLOT(ManipulationWidgetSmallSizeRequested()));
                QObject::disconnect(pSelectedObject, SIGNAL(PropertyChanged(QString)), this, SLOT(SelectedManipulatableValueChanged()));

                pEditingLayout->removeWidget(pOldManipulationWidget);
                pOldManipulationWidget->deleteLater();
            }

            if (pManipulationWidget == NULL)
            {
                pEditingWidget->hide();
            }
            else
            {
                QObject::connect(pManipulationWidget, SIGNAL(ValueChanged()), this, SLOT(ManipulationWidgetValueChanged()));
                QObject::connect(pManipulationWidget, SIGNAL(LargeSizeRequested()), this, SLOT(ManipulationWidgetLargeSizeRequested()));
                QObject::connect(pManipulationWidget, SIGNAL(SmallSizeRequested()), this, SLOT(ManipulationWidgetSmallSizeRequested()));
                QObject::connect(pNewSelectedObject, SIGNAL(PropertyChanged(QString)), this, SLOT(SelectedManipulatableValueChanged()));

                pEditingLayout->addWidget(pManipulationWidget);
                pEditingLayout->setAlignment(pManipulationWidget, Qt::AlignTop);
                pEditingWidget->show();
            }
        }

        pSelectedObject = pNewSelectedObject;
        pSelectedManipulatable = pNewSelectedManipulatable;
    }
}

void ZoomedViewManipulator::SelectedManipulatableValueChanged()
{
    pManipulationWidget->PullChanges();
}

void ZoomedViewManipulator::ManipulationWidgetLargeSizeRequested()
{
    pContentSplitter->hide();

    pEditingLayout->removeWidget(pManipulationWidget);
    pMainLayout->addWidget(pManipulationWidget);
}

void ZoomedViewManipulator::ManipulationWidgetSmallSizeRequested()
{
    pMainLayout->removeWidget(pManipulationWidget);
    pEditingLayout->addWidget(pManipulationWidget);
    pEditingLayout->setAlignment(pManipulationWidget, Qt::AlignTop);

    pContentSplitter->show();
}

void ZoomedViewManipulator::ManipulationWidgetValueChanged()
{
    if (pSelectedManipulatable != NULL)
    {
        pSelectedManipulatable->Update();
    }
}

void ZoomedViewManipulator::UpdateScene()
{
    if (pZoomedViewScene != NULL && pZoomedViewOverviewView != NULL)
    {
        pZoomedViewOverviewView->Update();
    }
}
