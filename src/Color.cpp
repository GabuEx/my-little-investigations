/**
 * An ARGB color.
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

#include "Color.h"
#include "XmlReader.h"
#include <math.h>

Color Color::White = Color(1.0, 1.0, 1.0, 1.0);

Color::Color()
{
    SetA(0.0);
    SetR(0.0);
    SetG(0.0);
    SetB(0.0);
}

Color::Color(const Color &other)
{
    SetA(other.GetA());
    SetR(other.GetR());
    SetG(other.GetG());
    SetB(other.GetB());
}

Color::Color(double a, double r, double g, double b)
{
    SetA(std::max(std::min(a, 1.0), 0.0));
    SetR(std::max(std::min(r, 1.0), 0.0));
    SetG(std::max(std::min(g, 1.0), 0.0));
    SetB(std::max(std::min(b, 1.0), 0.0));
}

void Color::LoadElementsFromXml(XmlReader *pReader)
{
    switch (pReader->GetFormattingVersion())
    {
    case 1:
        SetA(pReader->ReadDoubleElement("A") / 255.0);
        SetR(pReader->ReadDoubleElement("R") / 255.0);
        SetG(pReader->ReadDoubleElement("G") / 255.0);
        SetB(pReader->ReadDoubleElement("B") / 255.0);
        break;

    case 2:
        XmlStorableObject::LoadElementsFromXml(pReader);
        break;

    default:
        ThrowException("Unknown XML formatting version.");
    }
}

Color & Color::operator=(const Color &rhs)
{
    a = rhs.a;
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;

    intA = rhs.intA;
    intR = rhs.intR;
    intG = rhs.intG;
    intB = rhs.intB;

    return *this;
}

bool Color::operator==(const Color &other) const
{
    return
        a == other.a &&
        r == other.r &&
        g == other.g &&
        b == other.b;
}

bool Color::operator!=(const Color &other) const
{
    return !(*this == other);
}
