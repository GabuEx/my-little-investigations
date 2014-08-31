/**
 * Basic header/include file for Color.cpp.
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

#ifndef COLOR_H
#define COLOR_H

#include "SDL2/SDL_stdinc.h"
class XmlReader;

class Color
{
public:
    static Color White;

    Color();
    Color(double a, double r, double g, double b);

    double GetA() const { return this->a; }
    double GetIntA() const { return this->intA; }
    void SetA(double a) { this->a = a; this->intA = (Uint8)(a * 255); }
    double GetR() const { return this->r; }
    double GetIntR() const { return this->intR; }
    void SetR(double r) { this->r = r; this->intR = (Uint8)(r * 255); }
    double GetG() const { return this->g; }
    double GetIntG() const { return this->intG; }
    void SetG(double g) { this->g = g; this->intG = (Uint8)(g * 255); }
    double GetB() const { return this->b; }
    double GetIntB() const { return this->intB; }
    void SetB(double b) { this->b = b; this->intB = (Uint8)(b * 255); }

    Color & operator=(const Color &rhs);

    bool operator==(const Color &other) const;
    bool operator!=(const Color &other) const;

    Color(XmlReader *pReader);

private:
    double a;
    double r;
    double g;
    double b;

    Uint8 intA;
    Uint8 intR;
    Uint8 intG;
    Uint8 intB;
};

#endif
