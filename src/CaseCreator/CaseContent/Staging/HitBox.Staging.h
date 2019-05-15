#ifndef HITBOX_STAGING_H
#define HITBOX_STAGING_H

#include "Rectangle.h"
#include "Vector2.h"
#include "XmlReader.h"

#include <QList>

namespace Staging
{

class HitBoxPolygon
{
public:
    HitBoxPolygon(XmlReader *pReader);

    QList<Vector2> Vertices;
    QList<Vector2> Normals;
    Vector2 Position;
};

class HitBox
{
public:
    HitBox(XmlReader *pReader);

    QList<HitBoxPolygon *> PolygonList;
    RectangleWH AreaBoundsRectangle;
};

}

#endif // STAGINGHITBOX_H
