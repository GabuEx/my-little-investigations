#ifndef LOCATIONMANIPULATOR_H
#define LOCATIONMANIPULATOR_H

#include <QWidget>

#include <QGraphicsScene>

#include "../TemplateHelpers/ObjectManipulator.h"
#include "../BaseTypes/PageTabWidget.h"

class Location;

class LocationManipulator : public ObjectManipulator<Location>
{
    Q_OBJECT
public:
    explicit LocationManipulator(QWidget *parent = 0);

    void Init(Location *pObject);
    void Reset();
    bool Undo();
    bool Redo();

    bool GetIsActive();
    void SetIsActive(bool isActive);

signals:

public slots:

private:
    PageTabWidget<Location> *pTabWidget;

    bool isActive;
};

#endif // LOCATIONMANIPULATOR_H
