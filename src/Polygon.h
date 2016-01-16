/**
 * Basic header/include file for Polygon.cpp.
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

#ifndef POLYGON_H
#define POLYGON_H

#include "Rectangle.h"
#include "Vector2.h"
#include "XmlStorableObject.h"

#ifndef CASE_CREATOR
#include <vector>

#define PolygonListType vector
#else
#include <QList>

#define PolygonListType QList
#endif

using namespace std;

class GeometricPolygon : public XmlStorableObject
{
    BEGIN_NAMED_XML_STORABLE_OBJECT(GeometricPolygon, Polygon)
        XML_STORABLE_CUSTOM_OBJECT_LIST(points, Vector2::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    GeometricPolygon() {}
    GeometricPolygon(const GeometricPolygon &other);
    virtual ~GeometricPolygon() { }

    static GeometricPolygon CreateFromXml(XmlReader *pReader)
    {
        return GeometricPolygon(pReader);
    }

    void LoadElementsFromXml(XmlReader *pReader);

    bool Empty() const { return points.empty(); }

    bool Contains(Vector2 point);
    RectangleWH GetBoundingBox() const;

    GeometricPolygon & operator=(const GeometricPolygon &rhs);

    GeometricPolygon & operator+=(const Vector2 &rhs);
    GeometricPolygon & operator-=(const Vector2 &rhs);

    const GeometricPolygon operator+(const Vector2 &rhs) const;
    const GeometricPolygon operator-(const Vector2 &rhs) const;

private:
    PolygonListType<Vector2> points;
};

#endif
