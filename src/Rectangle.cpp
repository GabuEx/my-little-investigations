/**
 * A rectangle with XY-coordinates and width and height.
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

#include "Rectangle.h"
#include "XmlReader.h"

#ifdef CASE_CREATOR
#include "XmlWriter.h"
#endif

RectangleWH::RectangleWH()
{
    this->x = 0;
    this->y = 0;
    this->width = 0;
    this->height = 0;
}

RectangleWH::RectangleWH(const RectangleWH &other)
{
    this->x = other.x;
    this->y = other.y;
    this->width = other.width;
    this->height = other.height;
}

RectangleWH::RectangleWH(double x, double y, double width, double height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

#ifdef CASE_CREATOR
RectangleWH::RectangleWH(QRectF rect)
{
    this->x = (double)rect.x();
    this->y = (double)rect.y();
    this->width = (double)rect.width();
    this->height = (double)rect.height();
}
#endif

RectangleWH & RectangleWH::operator=(const RectangleWH &rhs)
{
    x = rhs.x;
    y = rhs.y;
    width = rhs.width;
    height = rhs.height;

    return *this;
}

bool RectangleWH::operator==(const RectangleWH &other) const
{
    return
        x == other.x &&
        y == other.y &&
        width == other.width &&
        height == other.height;
}

bool RectangleWH::operator!=(const RectangleWH &other) const
{
    return !(*this == other);
}

#ifdef CASE_CREATOR
RectangleWH & RectangleWH::operator+=(const Vector2 &rhs)
{
    x += rhs.GetX();
    y += rhs.GetY();

    return *this;
}

RectangleWH & RectangleWH::operator-=(const Vector2 &rhs)
{
    x -= rhs.GetX();
    y -= rhs.GetY();

    return *this;
}

const RectangleWH RectangleWH::operator+(const Vector2 &rhs) const
{
    return RectangleWH(*this) += rhs;
}

const RectangleWH RectangleWH::operator-(const Vector2 &rhs) const
{
    return RectangleWH(*this) -= rhs;
}

QRect RectangleWH::ToQRect() const
{
    return QRect((int)x, (int)y, (int)width, (int)height);
}

QRectF RectangleWH::ToQRectF() const
{
    return QRectF(x, y, width, height);
}

bool RectangleWH::ContainsPoint(Vector2 point) const
{
    return
        point.GetX() >= x &&
        point.GetY() >= y &&
        point.GetX() <= x + width &&
        point.GetY() <= y + height;
}
#endif
