#ifndef LOCATIONOVERVIEWPAGE_H
#define LOCATIONOVERVIEWPAGE_H

#include "../BaseTypes/Page.h"

#include "CaseCreator/CaseContent/Location.h"
#include "CaseCreator/UIComponents/ManipulatableGraphicsView.h"
#include "CaseCreator/UIComponents/ManipulatableGraphicsScene.h"

#include <QCheckBox>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

class IManipulatableObject;
class ObjectManipulatorSlots;

class LocationOverviewPage : public Page<Location>
{
    Q_OBJECT

public:
    LocationOverviewPage(QWidget *parent = 0);
    virtual ~LocationOverviewPage();

    void Init(Location *pObject);
    void Reset();
    bool Undo();
    bool Redo();

private slots:
    void SceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);
    void SelectedManipulatableValueChanged();
    void ManipulationWidgetValueChanged();
    void ManipulationWidgetLargeSizeRequested();
    void ManipulationWidgetSmallSizeRequested();

    void CharactersCheckBoxStateChanged(int newState);
    void ForegroundElementsCheckBoxStateChanged(int newState);
    void InteractionLocationsCheckBoxStateChanged(int newState);
    void HitBoxesCheckBoxStateChanged(int newState);

private:
    void UpdateScene();

    QVBoxLayout *pMainLayout;
    QSplitter *pContentSplitter;
    QWidget *pEditingWidget;
    QVBoxLayout *pEditingLayout;
    ObjectManipulatorSlots *pManipulationWidget;
    ManipulatableGraphicsView *pOverviewView;
    ManipulatableGraphicsScene *pOverviewScene;

    IManipulatable *pSelectedManipulatable;
    IManipulatableObject *pSelectedObject;

    Location::OverviewView *pLocationOverviewView;
    QCheckBox *pCharactersCheckBox;
    QCheckBox *pForegroundElementsCheckBox;
    QCheckBox *pInteractionLocationsCheckBox;
    QCheckBox *pHitBoxesCheckBox;

    bool showCharacters;
    bool showForegroundElements;
    bool showInteractionLocations;
    bool showHitBoxes;
};

#endif // LOCATIONOVERVIEWPAGE_H
