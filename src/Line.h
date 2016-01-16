/**
 * Basic header/include file for Line.cpp.
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

#ifndef LINE_H
#define LINE_H

#include "Vector2.h"
#include "XmlStorableObject.h"

class Line : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(Line)
        XML_STORABLE_CUSTOM_OBJECT(pointInLine, Vector2::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT(directionVector, Vector2::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    Line() {}

    Line(const Line &other)
    {
        this->pointInLine = other.pointInLine;
        this->directionVector = other.directionVector;
    }

    Line(Vector2 pointInLine, Vector2 directionVector)
    {
        this->pointInLine = pointInLine;
        this->directionVector = directionVector.Normalize();
    }

    virtual ~Line() { }

    Vector2 GetMinimalDisplacementToPoint(Vector2 point) const;
    Vector2 GetIntersectionPointWith(Line otherLine) const;

    Line & operator=(const Line &rhs)
    {
        this->pointInLine = rhs.pointInLine;
        this->directionVector = rhs.directionVector;

        return *this;
    }

private:
    Vector2 pointInLine;
    Vector2 directionVector;
};

#endif
