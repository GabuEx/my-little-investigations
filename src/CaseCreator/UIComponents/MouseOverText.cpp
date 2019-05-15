#include "MouseOverText.h"

MouseOverText::MouseOverText()
{
    text = "";

    pTextGraphicsItemGroup = NULL;
    pTextGraphicsOpacityEffect = NULL;

    pTextItem = NULL;
    pWhiteBorderItem = NULL;
    pBlackBorderItem = NULL;
}

MouseOverText::~MouseOverText()
{
    pTextGraphicsItemGroup = NULL;
    pTextGraphicsOpacityEffect = NULL;
}

void MouseOverText::SetText(QString text)
{
    if (this->text != text)
    {
        this->text = text;
        pTextItem->setText(text);

        UpdateBoundingRect();
        Update();
    }
}

void MouseOverText::SetPositionFromMousePosition(Vector2 mousePosition)
{
    if (this->mousePosition != mousePosition)
    {
        this->mousePosition = mousePosition;

        // +4 to account for the fact that the background rectangle is offset
        // from the position of the text.
        this->position =
                QPointF((qreal)mousePosition.GetX() - fullBoundingRect.width() / 2 + 4,
                        (qreal)mousePosition.GetY() - 10 - fullBoundingRect.height() + 4);

        Update();
    }
}

void MouseOverText::Draw(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    pTextItem = new QGraphicsSimpleTextItem();
    pWhiteBorderItem = new QGraphicsRectItem();
    pBlackBorderItem = new QGraphicsRectItem();
    QBrush blackBrush(qRgba(0, 0, 0, 255));

    pTextGraphicsItemGroup = new QGraphicsItemGroup();

    pTextItem->setBrush(blackBrush);
    pTextItem->setText(text);
    pTextItem->setZValue(2);

    // As is, for whatever reason, the bounding box we retrieve from the text item
    // seems to place it one pixel over from the box that we draw around it.
    // We'll move it over one pixel to account for that.
    pTextItem->setTransform(QTransform::fromTranslate(1, 0));

    pTextGraphicsItemGroup->addToGroup(pTextItem);

    UpdateBoundingRect();

    pWhiteBorderItem->setBrush(QBrush(qRgba(255, 255, 255, 255)));
    pBlackBorderItem->setBrush(blackBrush);
    pTextGraphicsItemGroup->addToGroup(pWhiteBorderItem);
    pTextGraphicsItemGroup->addToGroup(pBlackBorderItem);
    pWhiteBorderItem->setZValue(1);
    pBlackBorderItem->setZValue(0);

    pTextGraphicsOpacityEffect = new QGraphicsOpacityEffect();
    pTextGraphicsOpacityEffect->setOpacity(1.0);

    pTextGraphicsItemGroup->setGraphicsEffect(pTextGraphicsOpacityEffect);

    pScene->addItem(pTextGraphicsItemGroup);
    addedItems.push_back(pTextGraphicsItemGroup);
}

void MouseOverText::Update()
{
    if (pTextGraphicsItemGroup != NULL && pTextGraphicsOpacityEffect != NULL)
    {
        pTextGraphicsOpacityEffect->setOpacity(text.length() > 0 ? 1.0 : 0.0);
        pTextGraphicsItemGroup->setZValue(std::numeric_limits<qreal>::max());
        pTextGraphicsItemGroup->setTransform(QTransform::fromTranslate(position.x(), position.y()));
    }
}

void MouseOverText::UpdateBoundingRect()
{
    if (pTextItem != NULL && pWhiteBorderItem != NULL && pBlackBorderItem != NULL)
    {
        QRectF boundingRect = pTextItem->boundingRect();

        const int margin = 3;
        boundingRect.setLeft(boundingRect.left() - margin);
        boundingRect.setRight(boundingRect.right() + margin);
        boundingRect.setTop(boundingRect.top() - margin);
        boundingRect.setBottom(boundingRect.bottom() + margin);

        fullBoundingRect = boundingRect;
        fullBoundingRect.setLeft(fullBoundingRect.left() - 1);
        fullBoundingRect.setRight(fullBoundingRect.right() + 1);
        fullBoundingRect.setTop(fullBoundingRect.top() - 1);
        fullBoundingRect.setBottom(fullBoundingRect.bottom() + 1);

        pWhiteBorderItem->setRect(boundingRect);
        pBlackBorderItem->setRect(fullBoundingRect);
    }
}
