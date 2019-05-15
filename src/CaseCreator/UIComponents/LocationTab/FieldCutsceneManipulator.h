#ifndef FIELDCUTSCENEMANIPULATOR_H
#define FIELDCUTSCENEMANIPULATOR_H

#include "../TemplateHelpers/ObjectManipulator.h"

#include "CaseCreator/CaseContent/Location.h"
#include "CaseCreator/CaseContent/FieldCutscene.h"
#include "CaseCreator/UIComponents/ManipulatableGraphicsView.h"
#include "CaseCreator/UIComponents/ManipulatableGraphicsScene.h"

#include "FieldCutsceneEditor.h"

#include <QCheckBox>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

class FieldCutscene;

class IManipulatableObject;
class ObjectManipulatorSlots;

class FieldCutsceneManipulator : public ObjectManipulator<FieldCutscene>
{
    Q_OBJECT

public:
    FieldCutsceneManipulator(QWidget *parent = 0);
    virtual ~FieldCutsceneManipulator();

    void Init(FieldCutscene *pObject);
    void Reset();
    bool Undo();
    bool Redo();

private slots:
    void SceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);
    void FieldCutsceneEditorPhaseSelected(FieldCutscene::FieldCutscenePhase *pSelectedPhase);

    void LocationCutsceneViewCameraRectChanged(RectangleWH newCameraRect);
    void LocationCutsceneViewCharacterPositionChanged(Vector2 newCharacterPosition);

private:
    void UpdateScene();
    void UpdateAfterDrag();

    QVBoxLayout *pMainLayout;
    QSplitter *pContentSplitter;
    QWidget *pEditingWidget;
    QVBoxLayout *pEditingLayout;
    ObjectManipulatorSlots *pManipulationWidget;
    ManipulatableGraphicsView *pCutsceneView;
    ManipulatableGraphicsScene *pCutsceneScene;

    FieldCutscene::FieldCutscenePhase *pSelectedPhase;
    IManipulatable *pSelectedManipulatable;
    IManipulatableObject *pSelectedObject;

    Location::CutsceneView *pLocationCutsceneView;
    bool processingPhaseSelection;

    FieldCutsceneEditor *pCutsceneEditor;

    Vector2 previousCameraCenterPosition;
};

#endif // FIELDCUTSCENEMANIPULATOR_H
