/**
 * A vector in two dimensions.
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

#include "Vector2.h"
#include "XmlReader.h"
#include <math.h>

Vector2::Vector2()
{
    this->x = 0;
    this->y = 0;
}

Vector2::Vector2(double x, double y)
{
    this->x = x;
    this->y = y;
}

Vector2 & Vector2::operator=(const Vector2 &rhs)
{
    x = rhs.x;
    y = rhs.y;

    return *this;
}

double Vector2::Length() const
{
    return sqrt(GetX() * GetX() + GetY() * GetY());
}

Vector2 Vector2::Normalize() const
{
    return Vector2(GetX() / Length(), GetY() / Length());
}

Vector2 Vector2::Rotate(double radians) const
{
    return Vector2(
        GetX() * cos(radians) - GetY() * sin(radians),
        GetX() * sin(radians) + GetY() * cos(radians));
}

Vector2 & Vector2::operator+=(const Vector2 &rhs)
{
    x += rhs.x;
    y += rhs.y;

    return *this;
}

Vector2 & Vector2::operator-=(const Vector2 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;

    return *this;
}

Vector2 & Vector2::operator*=(const double &scalar)
{
    x *= scalar;
    y *= scalar;

    return *this;
}

const Vector2 Vector2::operator+(const Vector2 &other) const
{
    return Vector2(*this) += other;
}

const Vector2 Vector2::operator-(const Vector2 &other) const
{
    return Vector2(*this) -= other;
}

const Vector2 Vector2::operator*(const double &scalar) const
{
    return Vector2(*this) *= scalar;
}

double Vector2::operator*(const Vector2 &other) const
{
    return x * other.x + y * other.y;
}

bool Vector2::operator==(const Vector2 &other) const
{
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2 &other) const
{
    return !(*this == other);
}

bool Vector2::operator<(const Vector2 &other) const
{
    if (y != other.y)
    {
        return y < other.y;
    }
    else
    {
        return x < other.x;
    }
}

Vector2::Vector2(XmlReader *pReader)
{
    pReader->StartElement("Vector2");
    x = pReader->ReadDoubleElement("X");
    y = pReader->ReadDoubleElement("Y");
    pReader->EndElement();
}

#ifdef CASE_CREATOR
QPoint Vector2::ToQPoint()
{
    return QPoint((int)x, (int)y);
}

QSize Vector2::ToQSize()
{
    return QSize((int)x, (int)y);
}
#endif
