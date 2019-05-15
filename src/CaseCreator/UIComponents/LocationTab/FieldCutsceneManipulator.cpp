#include "FieldCutsceneManipulator.h"

#include <QLabel>
#include <QSplitter>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "CaseCreator/UIComponents/TemplateHelpers/ObjectManipulator.h"

template<>
ObjectManipulator<FieldCutscene> * ObjectManipulator<FieldCutscene>::Create(QWidget *parent)
{
    return new FieldCutsceneManipulator(parent);
}

FieldCutsceneManipulator::FieldCutsceneManipulator(QWidget *parent) :
    ObjectManipulator<FieldCutscene>(parent)
{
    pMainLayout = new QVBoxLayout();

    pCutsceneView = new ManipulatableGraphicsView();
    pCutsceneView->setMouseTracking(true);
    pCutsceneScene = new ManipulatableGraphicsScene();
    pCutsceneView->SetScene(pCutsceneScene);

    QObject::connect(pCutsceneView, SIGNAL(SelectionChanged(IManipulatable*,IManipulatableObject*)), this, SLOT(SceneSelectionChanged(IManipulatable*,IManipulatableObject*)));

    pEditingWidget = new QWidget();
    pEditingLayout = new QVBoxLayout();

    pCutsceneEditor = new FieldCutsceneEditor();
    pEditingLayout->addWidget(pCutsceneEditor);

    pEditingWidget->setLayout(pEditingLayout);

    pContentSplitter = new QSplitter();
    pContentSplitter->addWidget(pCutsceneView);
    pContentSplitter->addWidget(pEditingWidget);
    pContentSplitter->setStretchFactor(0, 1);
    pContentSplitter->setStretchFactor(1, 0);

    pMainLayout->addWidget(pContentSplitter);
    setLayout(pMainLayout);

    pManipulationWidget = NULL;
    pLocationCutsceneView = NULL;

    pSelectedPhase = NULL;
    pSelectedManipulatable = NULL;
    pSelectedObject = NULL;

    QObject::connect(pCutsceneEditor, SIGNAL(FieldCutscenePhaseSelected(FieldCutscene::FieldCutscenePhase*)), this, SLOT(FieldCutsceneEditorPhaseSelected(FieldCutscene::FieldCutscenePhase*)));
}

FieldCutsceneManipulator::~FieldCutsceneManipulator()
{
    Reset();
}

void FieldCutsceneManipulator::Init(FieldCutscene *pObject)
{
    if (pObject == NULL)
    {
        return;
    }

    ObjectManipulator<FieldCutscene>::Init(pObject);
    pCutsceneEditor->Init(pObject);

    previousCameraCenterPosition = Vector2(-1, -1);
    processingPhaseSelection = false;

    if (pLocationCutsceneView == NULL)
    {
        pLocationCutsceneView = pObject->GetOwningLocation()->GetCutsceneView();

        QObject::connect(pLocationCutsceneView, SIGNAL(CameraRectChanged(RectangleWH)), this, SLOT(LocationCutsceneViewCameraRectChanged(RectangleWH)));
        QObject::connect(pLocationCutsceneView, SIGNAL(CharacterPositionChanged(Vector2)), this, SLOT(LocationCutsceneViewCharacterPositionChanged(Vector2)));

        pCutsceneScene->SetManipulationSurface(pLocationCutsceneView);
    }

    pLocationCutsceneView->SetToCutscene(pObject->GetId());

    pCutsceneScene->clear();
    pLocationCutsceneView->Draw(pCutsceneScene);
    pCutsceneScene->update();

    pObject->InitializeForDrawable(pLocationCutsceneView);

    if (pSelectedPhase != NULL)
    {
        FieldCutsceneEditorPhaseSelected(pSelectedPhase);
    }

    pCutsceneScene->setSceneRect(pLocationCutsceneView->GetSceneRect());

    UpdateScene();
    SceneSelectionChanged(NULL, NULL);
}

void FieldCutsceneManipulator::Reset()
{
    pCutsceneEditor->Reset();

    if (pLocationCutsceneView != NULL)
    {
        QObject::disconnect(pLocationCutsceneView, SIGNAL(CameraRectChanged(RectangleWH)), this, SLOT(LocationCutsceneViewCameraRectChanged(RectangleWH)));
        QObject::disconnect(pLocationCutsceneView, SIGNAL(CharacterPositionChanged(Vector2)), this, SLOT(LocationCutsceneViewCharacterPositionChanged(Vector2)));

        pLocationCutsceneView->Reset();
        pLocationCutsceneView->deleteLater();
        pLocationCutsceneView = NULL;
    }

    if (pCutsceneScene != NULL)
    {
        pCutsceneScene->SetManipulationSurface(NULL);
    }
}

bool FieldCutsceneManipulator::Undo()
{
    return pLocationCutsceneView->Undo();
}

bool FieldCutsceneManipulator::Redo()
{
    return pLocationCutsceneView->Redo();
}

void FieldCutsceneManipulator::SceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject)
{
    if (pSelectedObject != pNewSelectedObject)
    {
        pSelectedObject = pNewSelectedObject;
        pSelectedManipulatable = pNewSelectedManipulatable;
    }
}

void FieldCutsceneManipulator::FieldCutsceneEditorPhaseSelected(FieldCutscene::FieldCutscenePhase *pSelectedPhase)
{
    this->pSelectedPhase = pSelectedPhase;

    if (pLocationCutsceneView != NULL)
    {
        Vector2 cameraCenterPosition = pSelectedPhase->GetStateDuringObject().GetCameraPosition();

        if (previousCameraCenterPosition != cameraCenterPosition)
        {
            pCutsceneView->centerOn(cameraCenterPosition.ToQPointF());
            previousCameraCenterPosition = cameraCenterPosition;
        }

        processingPhaseSelection = true;
        pLocationCutsceneView->GoToState(pSelectedPhase->GetStateDuringObject());
        processingPhaseSelection = false;
    }
}

void FieldCutsceneManipulator::LocationCutsceneViewCameraRectChanged(RectangleWH newCameraRect)
{
    if (processingPhaseSelection)
    {
        return;
    }

    pSelectedPhase->UpdateCameraPosition(
                Vector2(
                    newCameraRect.GetX() + newCameraRect.GetWidth() / 2,
                    newCameraRect.GetY() + newCameraRect.GetHeight() / 2));

    UpdateAfterDrag();
}

void FieldCutsceneManipulator::LocationCutsceneViewCharacterPositionChanged(Vector2 newCharacterPosition)
{
    if (processingPhaseSelection)
    {
        return;
    }

    pSelectedPhase->UpdateCharacterPosition(newCharacterPosition);
    UpdateAfterDrag();
}

void FieldCutsceneManipulator::UpdateScene()
{
    if (pCutsceneScene != NULL && pLocationCutsceneView != NULL)
    {
        pLocationCutsceneView->Update();
    }
}

void FieldCutsceneManipulator::UpdateAfterDrag()
{
    pCutsceneEditor->Update(false /* reselectPhase */);
    pLocationCutsceneView->UpdateState(pSelectedPhase->GetStateDuringObject());
}
