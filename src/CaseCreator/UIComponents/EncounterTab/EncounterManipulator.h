#ifndef ENCOUNTERMANIPULATOR_H
#define ENCOUNTERMANIPULATOR_H

#include <QWidget>

#include <QGraphicsScene>
#include <QTimer>

#include "../TemplateHelpers/ObjectManipulator.h"
#include "CaseCreator/UIComponents/BaseTypes/PageTabWidget.h"

class Encounter;

class EncounterManipulator : public ObjectManipulator<Encounter>
{
    Q_OBJECT
public:
    explicit EncounterManipulator(QWidget *parent = 0);

    void Init(Encounter *pObject);
    void Reset();

    bool GetIsActive();
    void SetIsActive(bool isActive);

    bool ShouldIncludeItem(Encounter *pEncounter);

signals:

public slots:

private:
    PageTabWidget<Encounter> *pTabWidget;

    bool isActive;
};

#endif // ENCOUNTERMANIPULATOR_H
