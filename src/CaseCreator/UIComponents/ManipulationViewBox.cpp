#include "ManipulationViewBox.h"

ManipulationViewBox::ManipulationViewBox(IManipulatable *pParent)
    : IDrawable(pParent)
{
    pParentManipulatable = pParent;

    isSelected = false;
    isMouseOver = false;
    isBeingDragged = false;

    pBoxGraphicsItemGroup = NULL;
    pBoxGraphicsOpacityEffect = NULL;
}

ManipulationViewBox::~ManipulationViewBox()
{
    pParentManipulatable = NULL;
    pBoxGraphicsItemGroup = NULL;
    pBoxGraphicsOpacityEffect = NULL;
}

void ManipulationViewBox::SetBoxRect(RectangleWH boxRect)
{
    if (this->boxRect != boxRect)
    {
        this->boxRect = boxRect;
        Update();
    }
}

void ManipulationViewBox::SetIsMouseOver(bool isMouseOver)
{
    if (isMouseOver != this->isMouseOver)
    {
        this->isMouseOver = isMouseOver;
        Update();
    }
}

void ManipulationViewBox::SetIsSelected(bool isSelected)
{
    if (isSelected != this->isSelected)
    {
        this->isSelected = isSelected;
        Update();
    }
}

void ManipulationViewBox::SetIsBeingDragged(bool isBeingDragged)
{
    if (isBeingDragged != this->isBeingDragged)
    {
        this->isBeingDragged = isBeingDragged;
        Update();
    }
}

void ManipulationViewBox::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    QGraphicsRectItem *pWhiteBoundingBoxItem = new QGraphicsRectItem();
    QGraphicsRectItem *pBlackBoundingBoxItem = new QGraphicsRectItem();
    QPen whitePen(QBrush(qRgba(255, 255, 255, 255)), 2);
    QPen blackPen(QBrush(qRgba(0, 0, 0, 255)), 4);

    whitePen.setCapStyle(Qt::SquareCap);
    blackPen.setCapStyle(Qt::SquareCap);

    pBoxGraphicsItemGroup = new QGraphicsItemGroup();

    QRectF boundingRect = boxRect.ToQRectF();

    pWhiteBoundingBoxItem->setRect(boundingRect);
    pBlackBoundingBoxItem->setRect(boundingRect);
    pWhiteBoundingBoxItem->setPen(whitePen);
    pBlackBoundingBoxItem->setPen(blackPen);
    pBoxGraphicsItemGroup->addToGroup(pWhiteBoundingBoxItem);
    pBoxGraphicsItemGroup->addToGroup(pBlackBoundingBoxItem);
    pWhiteBoundingBoxItem->setZValue(1);
    pBlackBoundingBoxItem->setZValue(0);

    pBoxGraphicsOpacityEffect = new QGraphicsOpacityEffect();
    pBoxGraphicsOpacityEffect->setOpacity(1.0);

    pBoxGraphicsItemGroup->setGraphicsEffect(pBoxGraphicsOpacityEffect);

    pScene->addItem(pBoxGraphicsItemGroup);
    addedItems.push_back(pBoxGraphicsItemGroup);
}

void ManipulationViewBox::UpdateCore()
{
    if (pBoxGraphicsItemGroup != NULL)
    {
        pBoxGraphicsOpacityEffect->setOpacity(!pParentManipulatable->IsEnabled() ? 0.0 : (isSelected ? 1.0 : (isMouseOver ? 2.0 / 3.0 : 1.0 / 3.0)));

        // We'll set the z-value of the bounding box for selected items
        // to a very large value to ensure that it always appears
        // on top of everything else, except mouse-over text.
        // For non-selected bounding boxes, we'll set it to one less than that
        // so these appear underneath selected bounding boxes, but
        // above everything else.
        pBoxGraphicsItemGroup->setZValue(100 * 1000000 - (isSelected ? 1 : 2));
    }
}
