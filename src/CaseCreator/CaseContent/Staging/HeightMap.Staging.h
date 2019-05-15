#ifndef HEIGHTMAP_STAGING_H
#define HEIGHTMAP_STAGING_H

#include "Vector2.h"
#include "Polygon.h"
#include "Line.h"
#include "XmlReader.h"

namespace Staging
{

class HeightMap
{
public:
    virtual ~HeightMap() {}

    static HeightMap * LoadFromXml(XmlReader *pReader);

    Vector2 DirectionVector;
    GeometricPolygon BoundingPolygon;

protected:
    void LoadFromXmlCore(XmlReader *pReader);
};

class ParabolicHeightMap : public HeightMap
{
public:
    ParabolicHeightMap(XmlReader *pReader);

    class HeightLine : public Line
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(HeightLine, Line)
            XML_STORABLE_INT(HeightAtLine)
        END_XML_STORABLE_OBJECT()

    public:
        HeightLine()
        {
            HeightAtLine = 0;
        }

        int HeightAtLine;
    };

    HeightLine HeightLine1;
    HeightLine HeightLine2;
    HeightLine HeightLine3;
};

}

#endif // HEIGHTMAP_STAGING_H
