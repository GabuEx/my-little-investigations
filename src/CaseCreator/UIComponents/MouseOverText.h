#ifndef MOUSEOVERTEXT_H
#define MOUSEOVERTEXT_H

#include <QString>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsOpacityEffect>

#include "Vector2.h"

class MouseOverText
{
public:
    MouseOverText();
    ~MouseOverText();

    QString GetText() { return text; }
    void SetText(QString text);

    void SetPositionFromMousePosition(Vector2 mousePosition);

    void Draw(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
    void Update();

private:
    void UpdateBoundingRect();

    QString text;
    Vector2 mousePosition;
    QPointF position;
    QRectF fullBoundingRect;

    QGraphicsItemGroup *pTextGraphicsItemGroup;
    QGraphicsSimpleTextItem *pTextItem;
    QGraphicsRectItem *pWhiteBorderItem;
    QGraphicsRectItem *pBlackBorderItem;
    QGraphicsOpacityEffect *pTextGraphicsOpacityEffect;
};

#endif // MOUSEOVERTEXT_H
