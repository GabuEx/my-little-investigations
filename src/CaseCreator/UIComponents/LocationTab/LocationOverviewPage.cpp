#include "LocationOverviewPage.h"

#include <QLabel>
#include <QSplitter>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "CaseCreator/UIComponents/TemplateHelpers/ObjectManipulator.h"

LocationOverviewPage::LocationOverviewPage(QWidget *parent) :
    Page<Location>(parent)
{
    pMainLayout = new QVBoxLayout();

    QHBoxLayout *pLayersLayout = new QHBoxLayout();
    pLayersLayout->setSpacing(10);

    QLabel *pLayersLabel = new QLabel("Layers:");
    pCharactersCheckBox = new QCheckBox("Characters");
    pForegroundElementsCheckBox = new QCheckBox("Foreground Elements");
    pInteractionLocationsCheckBox = new QCheckBox("Interaction Locations");
    pHitBoxesCheckBox = new QCheckBox("Hitboxes");

    pCharactersCheckBox->setCheckState(Qt::Checked);
    pForegroundElementsCheckBox->setCheckState(Qt::Checked);
    pInteractionLocationsCheckBox->setCheckState(Qt::Checked);
    pHitBoxesCheckBox->setCheckState(Qt::Checked);

    showCharacters = true;
    showForegroundElements = true;
    showInteractionLocations = true;
    showHitBoxes = true;

    QObject::connect(pCharactersCheckBox, SIGNAL(stateChanged(int)), this, SLOT(CharactersCheckBoxStateChanged(int)));
    QObject::connect(pForegroundElementsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(ForegroundElementsCheckBoxStateChanged(int)));
    QObject::connect(pInteractionLocationsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(InteractionLocationsCheckBoxStateChanged(int)));
    QObject::connect(pHitBoxesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(HitBoxesCheckBoxStateChanged(int)));

    pLayersLayout->addWidget(pLayersLabel);
    pLayersLayout->addWidget(pCharactersCheckBox);
    pLayersLayout->addWidget(pForegroundElementsCheckBox);
    pLayersLayout->addWidget(pInteractionLocationsCheckBox);
    pLayersLayout->addWidget(pHitBoxesCheckBox);

    pMainLayout->addItem(pLayersLayout);
    pMainLayout->setAlignment(pLayersLayout, Qt::AlignLeft);

    pOverviewView = new ManipulatableGraphicsView();
    pOverviewView->setMouseTracking(true);
    pOverviewScene = new ManipulatableGraphicsScene();
    pOverviewView->SetScene(pOverviewScene);

    QObject::connect(pOverviewView, SIGNAL(SelectionChanged(IManipulatable*,IManipulatableObject*)), this, SLOT(SceneSelectionChanged(IManipulatable*,IManipulatableObject*)));

    pEditingWidget = new QWidget();
    pEditingLayout = new QVBoxLayout();

    pEditingWidget->setLayout(pEditingLayout);

    pContentSplitter = new QSplitter();
    pContentSplitter->addWidget(pOverviewView);
    pContentSplitter->addWidget(pEditingWidget);
    pContentSplitter->setStretchFactor(0, 1);
    pContentSplitter->setStretchFactor(1, 0);

    pEditingWidget->hide();

    pMainLayout->addWidget(pContentSplitter);
    setLayout(pMainLayout);

    pManipulationWidget = NULL;
    pLocationOverviewView = NULL;
    pSelectedObject = NULL;
}

LocationOverviewPage::~LocationOverviewPage()
{
    Reset();
}

void LocationOverviewPage::Init(Location *pObject)
{
    if (pObject == NULL)
    {
        return;
    }

    Page<Location>::Init(pObject);

    pLocationOverviewView = pObject->GetOverviewView();
    pOverviewScene->SetManipulationSurface(pLocationOverviewView);

    pOverviewScene->clear();
    pLocationOverviewView->Draw(pOverviewScene);
    pOverviewScene->update();

    pOverviewScene->setSceneRect(pLocationOverviewView->GetSceneRect());

    UpdateScene();
    SceneSelectionChanged(NULL, NULL);
}

void LocationOverviewPage::Reset()
{
    if (pLocationOverviewView != NULL)
    {
        pLocationOverviewView->Reset();
        pLocationOverviewView->deleteLater();
        pLocationOverviewView = NULL;
    }

    if (pOverviewScene != NULL)
    {
        pOverviewScene->SetManipulationSurface(NULL);
    }
}

bool LocationOverviewPage::Undo()
{
    return pLocationOverviewView->Undo();
}

bool LocationOverviewPage::Redo()
{
    return pLocationOverviewView->Redo();
}

void LocationOverviewPage::SceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject)
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

void LocationOverviewPage::SelectedManipulatableValueChanged()
{
    pManipulationWidget->PullChanges();
}

void LocationOverviewPage::ManipulationWidgetLargeSizeRequested()
{
    pContentSplitter->hide();

    pEditingLayout->removeWidget(pManipulationWidget);
    pMainLayout->addWidget(pManipulationWidget);
}

void LocationOverviewPage::ManipulationWidgetSmallSizeRequested()
{
    pMainLayout->removeWidget(pManipulationWidget);
    pEditingLayout->addWidget(pManipulationWidget);
    pEditingLayout->setAlignment(pManipulationWidget, Qt::AlignTop);

    pContentSplitter->show();
}

void LocationOverviewPage::ManipulationWidgetValueChanged()
{
    if (pSelectedManipulatable != NULL)
    {
        pSelectedManipulatable->Update();
    }
}

void LocationOverviewPage::CharactersCheckBoxStateChanged(int newState)
{
    showCharacters = (Qt::CheckState)newState == Qt::Checked;
    UpdateScene();
}

void LocationOverviewPage::ForegroundElementsCheckBoxStateChanged(int newState)
{
    showForegroundElements = (Qt::CheckState)newState == Qt::Checked;
    UpdateScene();
}

void LocationOverviewPage::InteractionLocationsCheckBoxStateChanged(int newState)
{
    showInteractionLocations = (Qt::CheckState)newState == Qt::Checked;
    UpdateScene();
}

void LocationOverviewPage::HitBoxesCheckBoxStateChanged(int newState)
{
    showHitBoxes = (Qt::CheckState)newState == Qt::Checked;
    UpdateScene();
}

void LocationOverviewPage::UpdateScene()
{
    if (pOverviewScene != NULL && pLocationOverviewView != NULL)
    {
        pLocationOverviewView->SetAreCharactersEnabled(showCharacters);
        pLocationOverviewView->SetAreForegroundElementsEnabled(showForegroundElements);
        pLocationOverviewView->SetAreInteractionLocationsEnabled(showInteractionLocations);
        pLocationOverviewView->SetAreHitBoxesEnabled(showHitBoxes);
        pLocationOverviewView->Update();
    }
}
