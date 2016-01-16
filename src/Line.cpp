/**
 * A definition of a geometric line.
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

#include "Line.h"
#include "XmlReader.h"
#include <math.h>

Vector2 Line::GetMinimalDisplacementToPoint(Vector2 point) const
{
    Line perpendicularLine = Line(point, Vector2(-directionVector.GetY(), directionVector.GetX()));
    return GetIntersectionPointWith(perpendicularLine) - point;
}

Vector2 Line::GetIntersectionPointWith(Line otherLine) const
{
    double x = 0;
    double y = 0;

    if (directionVector.GetX() != 0 && otherLine.directionVector.GetX() != 0)
    {
        double directionVectorSlope = directionVector.GetY() / directionVector.GetX();
        double otherDirectionVectorSlope = otherLine.directionVector.GetY() / otherLine.directionVector.GetX();

        x = (otherLine.pointInLine.GetY() - otherDirectionVectorSlope * otherLine.pointInLine.GetX() - pointInLine.GetY() + directionVectorSlope * pointInLine.GetX()) / (directionVectorSlope - otherDirectionVectorSlope);
        y = directionVectorSlope * x + pointInLine.GetY() - directionVectorSlope * pointInLine.GetX();
    }
    else if (directionVector.GetX() != 0)
    {
        double directionVectorSlope = directionVector.GetY() / directionVector.GetX();

        x = otherLine.pointInLine.GetX();
        y = directionVectorSlope * x + pointInLine.GetY() - directionVectorSlope * pointInLine.GetX();
    }
    else if (otherLine.directionVector.GetX() != 0)
    {
        double otherDirectionVectorSlope = otherLine.directionVector.GetY() / otherLine.directionVector.GetX();

        x = pointInLine.GetX();
        y = otherDirectionVectorSlope * x + otherLine.pointInLine.GetY() - otherDirectionVectorSlope * otherLine.pointInLine.GetX();
    }

    return Vector2(x, y);
}
