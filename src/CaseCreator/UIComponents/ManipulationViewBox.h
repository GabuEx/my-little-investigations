#ifndef MANIPULATIONVIEWBOX_H
#define MANIPULATIONVIEWBOX_H

#include "CaseCreator/Utilities/Interfaces.h"
#include "Rectangle.h"

#include <QGraphicsOpacityEffect>

class ManipulationViewBox : public IDrawable
{
public:
    ManipulationViewBox(IManipulatable *pParent);
    ~ManipulationViewBox();

    virtual RectangleWH GetBoundingRect()
    {
        return boxRect;
    }

    virtual bool GetFlipHorizontal()
    {
        return false;
    }

    void SetBoxRect(RectangleWH boxRect);

    bool GetIsMouseOver() { return this->isMouseOver; }
    void SetIsMouseOver(bool isMouseOver);
    bool GetIsSelected() { return this->isSelected; }
    void SetIsSelected(bool isSelected);
    bool GetIsBeingDragged() { return this->isBeingDragged; }
    void SetIsBeingDragged(bool isBeingDragged);

protected:
    void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
    void UpdateCore();

private:
    IManipulatable *pParentManipulatable;

    RectangleWH boxRect;

    bool isMouseOver;
    bool isSelected;
    bool isBeingDragged;

    QGraphicsItemGroup *pBoxGraphicsItemGroup;
    QGraphicsOpacityEffect *pBoxGraphicsOpacityEffect;
};

#endif // MANIPULATIONVIEWBOX_H
