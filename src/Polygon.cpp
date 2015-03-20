/**
 * A definition of a geometric polygon.
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

#include "Polygon.h"
#include "Line.h"
#include "XmlReader.h"
#include <limits>

// vector<T>.size() is an unsigned int, whereas QList<T>.size() is a signed int.
// No idea why, but we need to do this in order not to get a compiler warning.
#ifndef CASE_CREATOR
typedef unsigned int PointsIntType;
#else
#include "XmlWriter.h"

typedef int PointsIntType;
#endif

GeometricPolygon::GeometricPolygon(XmlReader *pReader)
{
    pReader->StartElement("Polygon");
    pReader->StartList("Vertex");

    while (pReader->MoveToNextListItem())
    {
        points.push_back(Vector2(pReader->ReadDoubleElement("X"), pReader->ReadDoubleElement("Y")));
    }

    pReader->EndElement();
}

#ifdef CASE_CREATOR
void GeometricPolygon::SaveToProjectFile(XmlWriter *pWriter)
{
    pWriter->StartElement("Polygon");

    for (Vector2 point : points)
    {
        pWriter->StartElement("Vertex");
        pWriter->WriteDoubleElement("X", point.GetX());
        pWriter->WriteDoubleElement("Y", point.GetY());
        pWriter->EndElement();
    }

    pWriter->EndElement();
}
#endif

RectangleWH GeometricPolygon::GetBoundingBox()
{
    if (points.empty())
    {
        return RectangleWH();
    }

    double minX = points[0].GetX();
    double minY = points[0].GetY();
    double maxX = points[0].GetX();
    double maxY = points[0].GetY();

    for (PointsIntType i = 1; i < points.size(); i++)
    {
        if (points[i].GetX() < minX)
        {
            minX = points[i].GetX();
        }
        else if (points[i].GetX() > maxX)
        {
            maxX = points[i].GetX();
        }

        if (points[i].GetY() < minY)
        {
            minY = points[i].GetY();
        }
        else if (points[i].GetY() > maxY)
        {
            maxY = points[i].GetY();
        }
    }

    return RectangleWH(minX, minY, maxX - minX, maxY - minY);
}

bool GeometricPolygon::Contains(Vector2 point)
{
    // To find whether or not this polygon contains the given point,
    // we'll use the ray-casting algorithm:
    // we check to see how many times a ray from the left side passes
    // through an edge on its way to the point in question.
    // If it passes through an odd number of edges, then the point
    // is inside the polygon; otherwise, it's outside.
    bool oddNumberOfCrossings = false;
    int lastIndex = int(points.size() - 1);

    for (PointsIntType index = 0; index < points.size(); index++)
    {
        // For the edge that we're currently on, the ray can only
        // potentially pass through it if the y-coordinates of the
        // vertexes at each end of the edge are on either side of the
        // point that we're testing against.  We'll skip this edge if that
        // isn't the case.
        if ((points[index].GetY() < point.GetY() && points[lastIndex].GetY() >= point.GetY()) ||
            (points[index].GetY() >= point.GetY() && points[lastIndex].GetY() < point.GetY()))
        {
            // Now that we do have a potential edge to be crossed, we'll check for a crossing.
            // In order to do this, we'll create a geometric line corresponding to the
            // edge direction, plus another geometric line corresponding to the ray in question.
            // Since we already know that the y-coordinates of the edge are on either side
            // of the y-coordinate of the point we're testing against, we know that
            // an intersection between these two lines will occur.  However, we then test
            // whether this intersection occurs to the left of the point we're testing against.
            // Only if that is the case is this intersection relevant.
            Line rayLine(point, Vector2(1, 0));
            Line edgeLine(points[index], points[lastIndex] - points[index]);

            if (rayLine.GetIntersectionPointWith(edgeLine).GetX() < point.GetX())
            {
                oddNumberOfCrossings = !oddNumberOfCrossings;
            }
        }

        lastIndex = index;
    }

    return oddNumberOfCrossings;
}

const GeometricPolygon GeometricPolygon::operator-(const Vector2 &other) const
{
    GeometricPolygon newPolygon = *this;

    for (PointsIntType i = 0; i < newPolygon.points.size(); i++)
    {
        newPolygon.points[i] -= other;
    }

    return newPolygon;
}
