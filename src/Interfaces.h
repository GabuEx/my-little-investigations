/**
 * Interfaces to be used in the game.
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

#ifndef INTERFACES_H
#define INTERFACES_H

#include "Line.h"
#include "Rectangle.h"
#include "Vector2.h"
#include <vector>

class Location;

class ZOrderableObject
{
public:
    virtual bool IsVisible() = 0;
    virtual int GetZOrder() = 0;
    virtual Line * GetZOrderLine() = 0;
    virtual Vector2 GetZOrderPoint() = 0;
    virtual void Draw() = 0;
    virtual void Draw(Vector2 offsetVector) = 0;
};

class InteractiveElement
{
public:
    virtual bool GetCanInteractFromAnywhere() { return this->interactFromAnywhere; }

    virtual Vector2 GetCenterPoint() = 0;
    virtual RectangleWH GetBoundsForInteraction() = 0;
    virtual bool IsInteractionPointExact() = 0;
    virtual void BeginInteraction(Location *pLocation) = 0;

protected:
    bool interactFromAnywhere;
};

#endif
