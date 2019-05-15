#ifndef ZOOMEDVIEWMANIPULATOR_H
#define ZOOMEDVIEWMANIPULATOR_H

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

class ZoomedViewManipulator : public ObjectManipulator<ZoomedView>
{
    Q_OBJECT

public:
    ZoomedViewManipulator(QWidget *parent = 0);
    virtual ~ZoomedViewManipulator();

    void Init(ZoomedView *pObject);
    void Reset();
    bool Undo();
    bool Redo();

private slots:
    void SceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);
    void SelectedManipulatableValueChanged();
    void ManipulationWidgetValueChanged();
    void ManipulationWidgetLargeSizeRequested();
    void ManipulationWidgetSmallSizeRequested();

private:
    void UpdateScene();

    QVBoxLayout *pMainLayout;
    QSplitter *pContentSplitter;
    QWidget *pEditingWidget;
    QVBoxLayout *pEditingLayout;
    ObjectManipulatorSlots *pManipulationWidget;
    ManipulatableGraphicsView *pZoomedViewView;
    ManipulatableGraphicsScene *pZoomedViewScene;

    IManipulatable *pSelectedManipulatable;
    IManipulatableObject *pSelectedObject;

    ZoomedView::OverviewView *pZoomedViewOverviewView;
};

#endif // ZOOMEDVIEWMANIPULATOR_H
