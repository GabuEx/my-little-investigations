#ifndef LOCATIONZOOMEDVIEWPAGE_H
#define LOCATIONZOOMEDVIEWPAGE_H

#include "../BaseTypes/Page.h"
#include "../TemplateHelpers/ListManipulator.h"

#include "CaseCreator/CaseContent/Location.h"
#include "CaseCreator/UIComponents/ManipulatableGraphicsView.h"
#include "CaseCreator/UIComponents/ManipulatableGraphicsScene.h"

#include <QCheckBox>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

class IManipulatableObject;
class ObjectManipulatorSlots;

class LocationZoomedViewsPage : public Page<Location>
{
    Q_OBJECT

public:
    LocationZoomedViewsPage(QWidget *parent = 0);

    void Init(Location *pObject);
    void Reset();
    bool Undo();
    bool Redo();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    ListManipulator<ZoomedView> *pZoomedViewManipulator;
    bool isActive;
};

#endif // LOCATIONZOOMEDVIEWPAGE_H
