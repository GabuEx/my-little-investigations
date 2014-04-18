/**
 * Basic header/include file for Collisions.cpp.
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

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "Rectangle.h"
#include "Vector2.h"
#include <vector>

using namespace std;

class CollidableObject;
class CollisionParameter;
class XmlReader;

class HitBox
{
public:
    HitBox() {}
    HitBox(XmlReader *pReader);
    ~HitBox();

    bool ContainsPoint(Vector2 offset, Vector2 point) const;
    bool IsCollision(Vector2 offset, HitBox *pHitBox, Vector2 hitBoxOffset, CollisionParameter *pParam) const;
    RectangleWH GetBoundingBox() const;
    void Draw(Vector2 topLeftCornerPosition) const;
    HitBox * Clone();

private:
    vector<CollidableObject *> collidableObjectList;
    RectangleWH areaBoundsRectangle;
};

class CollidableObject
{
public:
    CollidableObject(XmlReader *pReader);

    static CollidableObject * CreateRectangle(Vector2 position, Vector2 size);

    vector<Vector2> * GetVertices() { return &this->vertices; }
    vector<Vector2> * GetNormals() { return &this->normals; }

    Vector2 GetPosition() const { return this->position; }
    void SetPosition(Vector2 position) { this->position = position; }

    void Draw(Vector2 topLeftCornerPosition) const;
    CollidableObject * Clone();

private:
    CollidableObject()
    {
    }

    vector<Vector2> vertices;
    vector<Vector2> normals;
    Vector2 position;
};

class OverlapEntry
{
public:
    double OverlapDistance;
    Vector2 OverlapAxis;
    CollidableObject *PCollidableObject1;
    CollidableObject *PCollidableObject2;

    OverlapEntry(double overlapDistance, Vector2 overlapAxis, CollidableObject *pCollidableObject1, CollidableObject *pCollidableObject2)
        : OverlapDistance(overlapDistance)
        , OverlapAxis(overlapAxis)
        , PCollidableObject1(pCollidableObject1)
        , PCollidableObject2(pCollidableObject2)
    {
    }

    bool operator==(const OverlapEntry &other) const
    {
        return
            (PCollidableObject1 == other.PCollidableObject1 && PCollidableObject2 == other.PCollidableObject2) ||
            (PCollidableObject1 == other.PCollidableObject2 && PCollidableObject2 == other.PCollidableObject1);
    }

    bool operator!=(const OverlapEntry &other) const
    {
        return !(*this == other);
    }
};

class CollisionParameter
{
public:
    double OverlapDistance;
    Vector2 OverlapAxis;

    vector<OverlapEntry> OverlapEntryList;

    void AddOverlapEntry(double overlapDistance, Vector2 overlapAxis, CollidableObject *pCollidableObject1, CollidableObject *pCollidableObject2)
    {
        OverlapEntryList.push_back(OverlapEntry(overlapDistance, overlapAxis, pCollidableObject1, pCollidableObject2));
    }
};

void CalculateInterval(vector<Vector2> *pVertices, Vector2 axis, double *pMinDistance, double *pMaxDistance);

// Determines whether there's an intersection between the vertices along the given axis,
// and returns how far they're overlapping if so.
bool IsIntervalIntersection(vector<Vector2> *pPolygon1Vertices, vector<Vector2> *pPolygon2Vertices, Vector2 offset, Vector2 axis, CollisionParameter *pParam);

// Determines whether there's an intersection between the given objects,
// and returns the distance they're overlapping and along which axis if so.
bool CollisionExists(CollidableObject *pCollisionObject1, Vector2 collisionObject1Offset, CollidableObject *pCollisionObject2, Vector2 collisionObject2Offset, CollisionParameter *pParam);

#endif
