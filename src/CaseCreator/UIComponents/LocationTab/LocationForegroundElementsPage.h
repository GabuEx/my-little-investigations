#ifndef LOCATIONFOREGROUNDELEMENTSPAGE_H
#define LOCATIONFOREGROUNDELEMENTSPAGE_H

#include "../BaseTypes/Page.h"

#include "CaseCreator/CaseContent/Location.h"
#include "CaseCreator/UIComponents/TemplateHelpers/ListManipulator.h"

#include <QGraphicsScene>
#include <QMouseEvent>

class LocationForegroundElementsPage : public Page<Location>
{
    Q_OBJECT

public:
    LocationForegroundElementsPage(QWidget *parent = 0);

    void Init(Location *pObject);
    void Reset();

private:
    ListManipulator<ForegroundElement> *pForegroundElementManipulator;
};

#endif // LOCATIONFOREGROUNDELEMENTSPAGE_H
