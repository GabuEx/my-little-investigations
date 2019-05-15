#include "HitBox.Staging.h"

Staging::HitBoxPolygon::HitBoxPolygon(XmlReader *pReader)
{
    pReader->StartElement("CollidableObject");

    pReader->StartElement("Vertices");
    pReader->StartList("VertexEntry");

    while (pReader->MoveToNextListItem())
    {
        Vertices.push_back(Vector2(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("Normals");
    pReader->StartList("NormalEntry");

    while (pReader->MoveToNextListItem())
    {
        Normals.push_back(Vector2(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("Position");
    Position = Vector2(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Staging::HitBox::HitBox(XmlReader *pReader)
{
    pReader->StartElement("HitBox");
    pReader->StartElement("CollidableObjectList");

    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        PolygonList.push_back(new HitBoxPolygon(pReader));
    }

    pReader->EndElement();

    if (pReader->ElementExists("AreaBoundsRectangle"))
    {
        pReader->StartElement("AreaBoundsRectangle");
        AreaBoundsRectangle = RectangleWH(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}
