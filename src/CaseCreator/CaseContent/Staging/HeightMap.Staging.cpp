#include "HeightMap.Staging.h"

#include "MLIException.h"

Staging::HeightMap * Staging::HeightMap::LoadFromXml(XmlReader *pReader)
{
    HeightMap *pHeightMap = NULL;

    if (pReader->ElementExists("ParabolicHeightMap"))
    {
        pHeightMap = new ParabolicHeightMap(pReader);
    }
    else
    {
        throw MLIException("Unexpected height map type.");
    }

    return pHeightMap;
}

void Staging::HeightMap::LoadFromXmlCore(XmlReader *pReader)
{
    pReader->StartElement("DirectionVector");
    DirectionVector = Vector2(pReader);
    pReader->EndElement();

    pReader->StartElement("BoundingPolygon");
    BoundingPolygon = GeometricPolygon(pReader);
    pReader->EndElement();
}

Staging::ParabolicHeightMap::ParabolicHeightMap(XmlReader *pReader)
{
    pReader->StartElement("ParabolicHeightMap");

    LoadFromXmlCore(pReader);

    pReader->StartElement("HeightLine1");
    HeightLine1 = HeightLine(pReader);
    pReader->EndElement();

    pReader->StartElement("HeightLine2");
    HeightLine2 = HeightLine(pReader);
    pReader->EndElement();

    pReader->StartElement("HeightLine3");
    HeightLine3 = HeightLine(pReader);
    pReader->EndElement();

    pReader->EndElement();
}
