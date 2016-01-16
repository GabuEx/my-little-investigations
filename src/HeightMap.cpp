/**
 * A generalized way to represent a smooth, nonlinear height increase across a
 * patch of terrain.
 *
 * @author GabuEx, dawnmew
 * @since 1.0
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2014 Equestrian Dreamers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "HeightMap.h"
#include "Rectangle.h"
#include "XmlReader.h"
#include <math.h>

HeightMap * HeightMap::LoadFromXml(XmlReader *pReader)
{
    HeightMap *pHeightMap = NULL;

    if (pReader->ElementExists("ParabolicHeightMap"))
    {
        pHeightMap = new ParabolicHeightMap(pReader);
    }

    return pHeightMap;
}

Vector2 HeightMap::GetBasePointOffsetFromHeightenedPoint(Vector2 point)
{
    // Since the function assigning heightened points to base points can sometimes
    // assign the same heightened point to two or more base points, it's
    // a non-invertible function.  As such, to perform the reverse operation,
    // we just brute-force it: we start at the heightened point and then
    // iterate downwards until we find a base point whose assigned heightened point
    // matches the original point.
    Vector2 basePointOffset = Vector2(-1, -1);
    RectangleWH boundingBox = boundingPolygon.GetBoundingBox();

    // First we rule out the situations where this cannot possibly have
    // a base point associated with it: when it's below or to the side
    // of the bounds of the height map.
    if (point.GetX() < boundingBox.GetX() ||
            point.GetX() > boundingBox.GetX() + boundingBox.GetWidth() ||
            point.GetY() > boundingBox.GetY() + boundingBox.GetHeight())
    {
        return basePointOffset;
    }

    int highestHeight = GetHighestHeight();
    Vector2 originalPoint = point;

    for (int i = 0; i < highestHeight; i++)
    {
        point.SetY(point.GetY() + 1);

        if (point.GetY() > boundingBox.GetY() + boundingBox.GetHeight())
        {
            break;
        }
        else if (fabs(point.GetY() - GetHeightAtPoint(point) - originalPoint.GetY()) <= 1 && IsPointInBoundingPolygon(point))
        {
            basePointOffset = point - originalPoint;
            break;
        }
    }

    return basePointOffset;
}

void HeightMap::LoadFromXmlCore(XmlReader *pReader)
{
    pReader->StartElement("DirectionVector");
    directionVector = Vector2(pReader);
    pReader->EndElement();

    pReader->StartElement("BoundingPolygon");
    boundingPolygon = GeometricPolygon(pReader);
    pReader->EndElement();
}

ParabolicHeightMap::ParabolicHeightMap(XmlReader *pReader)
{
    pReader->StartElement("ParabolicHeightMap");

    LoadFromXmlCore(pReader);

    pReader->StartElement("HeightLine1");
    heightLine1 = HeightLine(pReader);
    pReader->EndElement();

    pReader->StartElement("HeightLine2");
    heightLine2 = HeightLine(pReader);
    pReader->EndElement();

    pReader->StartElement("HeightLine3");
    heightLine3 = HeightLine(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

int ParabolicHeightMap::GetHeightAtPoint(Vector2 point)
{
    Line characterDirectionLine(point, directionVector);

    Vector2 heightLine1IntersectionPoint = characterDirectionLine.GetIntersectionPointWith(heightLine1);
    Vector2 heightLine2IntersectionPoint = characterDirectionLine.GetIntersectionPointWith(heightLine2);
    Vector2 heightLine3IntersectionPoint = characterDirectionLine.GetIntersectionPointWith(heightLine3);

    double distanceToHeightLine1 = (heightLine1IntersectionPoint - point).Length();
    double distanceToHeightLine2 = (heightLine2IntersectionPoint - point).Length();
    double distanceToHeightLine3 = (heightLine3IntersectionPoint - point).Length();

    bool inFirstHalf =
        distanceToHeightLine3 > distanceToHeightLine1 &&
        distanceToHeightLine3 > distanceToHeightLine2;

    if (inFirstHalf)
    {
        double normalizedDistance = distanceToHeightLine1 / (distanceToHeightLine1 + distanceToHeightLine2);
        double weight = (1 - normalizedDistance) * (1 - normalizedDistance);

        return (int)floor(0.5 + heightLine1.GetHeightAtLine() * weight + heightLine2.GetHeightAtLine() * (1 - weight));
    }
    else
    {
        double normalizedDistance = distanceToHeightLine3 / (distanceToHeightLine3 + distanceToHeightLine2);
        double weight = (1 - normalizedDistance) * (1 - normalizedDistance);

        return (int)floor(0.5 + heightLine3.GetHeightAtLine() * weight + heightLine2.GetHeightAtLine() * (1 - weight));
    }
}

int ParabolicHeightMap::GetHighestHeight()
{
    return max(heightLine1.GetHeightAtLine(), max(heightLine2.GetHeightAtLine(), heightLine3.GetHeightAtLine()));
}
