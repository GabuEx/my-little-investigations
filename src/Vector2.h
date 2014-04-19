/**
 * Basic header/include file for Vector2.cpp.
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

#ifndef VECTOR2_H
#define VECTOR2_H

class XmlReader;

class Vector2
{
public:
    Vector2();
    Vector2(double x, double y);

    double GetX() const { return this->x; }
    void SetX(double x) { this->x = x; }
    double GetY() const { return this->y; }
    void SetY(double y) { this->y = y; }

    Vector2 & operator=(const Vector2 &rhs);

    double Length() const;
    Vector2 Normalize() const;
    Vector2 Rotate(double radians) const;

    Vector2 & operator+=(const Vector2 &rhs);
    Vector2 & operator-=(const Vector2 &rhs);
    Vector2 & operator*=(const double &scalar);
    double & operator*=(const Vector2 &rhs);

    const Vector2 operator+(const Vector2 &rhs) const;
    const Vector2 operator-(const Vector2 &rhs) const;
    const Vector2 operator*(const double &scalar) const;
    const double operator*(const Vector2 &rhs) const;

    bool operator==(const Vector2 &other) const;
    bool operator!=(const Vector2 &other) const;

    bool operator<(const Vector2 &other) const;

    Vector2(XmlReader *pReader);

private:
    double x;
    double y;
};

#endif
