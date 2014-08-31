/**
 * Classes and functions having to do with collision detection.
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

#include "Collisions.h"
#include "XmlReader.h"
#include <math.h>
#include <limits>

HitBox::HitBox(XmlReader *pReader)
{
    pReader->StartElement("HitBox");
    pReader->StartElement("CollidableObjectList");

    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        collidableObjectList.push_back(new CollidableObject(pReader));
    }

    pReader->EndElement();

    if (pReader->ElementExists("AreaBoundsRectangle"))
    {
        pReader->StartElement("AreaBoundsRectangle");
        areaBoundsRectangle = RectangleWH(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}

HitBox::~HitBox()
{
    for (unsigned int i = 0; i < collidableObjectList.size(); i++)
    {
        delete collidableObjectList[i];
    }
}

bool HitBox::ContainsPoint(Vector2 offset, Vector2 point) const
{
    bool isCollision = false;
    Vector2 overlapCompensation = Vector2(0, 0);
    CollidableObject *pCollidableObjectForPoint = CollidableObject::CreateRectangle(Vector2(0, 0), Vector2(1, 1));

    for (unsigned int i = 0; i < collidableObjectList.size(); i++)
    {
        CollidableObject *pCollidableObject = collidableObjectList[i];
        CollisionParameter tempParam;

        if (CollisionExists(pCollidableObject, offset + overlapCompensation, pCollidableObjectForPoint, point, &tempParam)
            && fabs(tempParam.OverlapDistance) > 0.0001)
        {
            overlapCompensation += tempParam.OverlapAxis * tempParam.OverlapDistance;
            isCollision = true;
        }
    }

    delete pCollidableObjectForPoint;
    return isCollision;
}

bool HitBox::IsCollision(Vector2 offset, HitBox *pHitBox, Vector2 hitBoxOffset, CollisionParameter *pParam) const
{
    bool isCollision = false;
    Vector2 overlapCompensation = Vector2(0, 0);

    if (pHitBox != NULL)
    {
        for (unsigned int i = 0; i < collidableObjectList.size(); i++)
        {
            CollidableObject *pCollidableObject1 = collidableObjectList[i];

            for (unsigned int j = 0; j < pHitBox->collidableObjectList.size(); j++)
            {
                CollidableObject *pCollidableObject2 = pHitBox->collidableObjectList[j];
                CollisionParameter tempParam;

                if (CollisionExists(pCollidableObject1, offset + overlapCompensation, pCollidableObject2, hitBoxOffset, &tempParam)
                    && fabs(tempParam.OverlapDistance) > 0.0001)
                {
                    for (unsigned int k = 0; k < tempParam.OverlapEntryList.size(); k++)
                    {
                        pParam->OverlapEntryList.push_back(tempParam.OverlapEntryList[k]);
                    }

                    overlapCompensation += tempParam.OverlapAxis * tempParam.OverlapDistance;
                    isCollision = true;
                }
            }
        }
    }

    pParam->OverlapDistance = overlapCompensation.Length();
    pParam->OverlapAxis = overlapCompensation.Normalize();
    return isCollision;
}

RectangleWH HitBox::GetBoundingBox() const
{
    double left = numeric_limits<double>::infinity();
    double top = numeric_limits<double>::infinity();
    double right = -numeric_limits<double>::infinity();
    double bottom = -numeric_limits<double>::infinity();

    for (unsigned int i = 0; i < collidableObjectList.size(); i++)
    {
        vector<Vector2> *pVertices = collidableObjectList[i]->GetVertices();

        for (unsigned int j = 0; j < pVertices->size(); j++)
        {
            Vector2 vertex = (*pVertices)[j];

            if (vertex.GetX() < left)
            {
                left = vertex.GetX();
            }

            if (vertex.GetX() > right)
            {
                right = vertex.GetX();
            }

            if (vertex.GetY() < top)
            {
                top = vertex.GetY();
            }

            if (vertex.GetY() > bottom)
            {
                bottom = vertex.GetY();
            }
        }
    }

    return RectangleWH(left, top, right - left, bottom - top);
}

void HitBox::Draw(Vector2 topLeftCornerPosition) const
{
    for (unsigned int i = 0; i < collidableObjectList.size(); i++)
    {
        collidableObjectList[i]->Draw(topLeftCornerPosition);
    }
}

HitBox * HitBox::Clone()
{
    HitBox *pCloneHitBox = new HitBox();

    for (unsigned int i = 0; i < collidableObjectList.size(); i++)
    {
        pCloneHitBox->collidableObjectList.push_back(collidableObjectList[i]->Clone());
    }

    pCloneHitBox->areaBoundsRectangle = areaBoundsRectangle;

    return pCloneHitBox;
}

CollidableObject::CollidableObject(XmlReader *pReader)
{
    pReader->StartElement("CollidableObject");

    pReader->StartElement("Vertices");
    pReader->StartList("VertexEntry");

    while (pReader->MoveToNextListItem())
    {
        vertices.push_back(Vector2(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("Normals");
    pReader->StartList("NormalEntry");

    while (pReader->MoveToNextListItem())
    {
        normals.push_back(Vector2(pReader));
    }

    pReader->EndElement();

    pReader->StartElement("Position");
    position = Vector2(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

CollidableObject * CollidableObject::CreateRectangle(Vector2 position, Vector2 size)
{
    CollidableObject *pRectangle = new CollidableObject();

    pRectangle->SetPosition(position + (size * 0.5));

    pRectangle->GetVertices()->push_back(position - pRectangle->GetPosition());
    pRectangle->GetVertices()->push_back(Vector2(position.GetX(), position.GetY() + size.GetY()) - pRectangle->GetPosition());
    pRectangle->GetVertices()->push_back(position + size - pRectangle->GetPosition());
    pRectangle->GetVertices()->push_back(Vector2(position.GetX() + size.GetX(), position.GetY()) - pRectangle->GetPosition());

    pRectangle->GetNormals()->push_back(Vector2(1, 0));
    pRectangle->GetNormals()->push_back(Vector2(0, 1));

    return pRectangle;
}

void CollidableObject::Draw(Vector2 topLeftCornerPosition) const
{
    // No reason to bother implementing this until we actually need it.
}

CollidableObject * CollidableObject::Clone()
{
    CollidableObject *pCloneCollidableObject = new CollidableObject();

    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        pCloneCollidableObject->vertices.push_back(vertices[i]);
    }

    for (unsigned int i = 0; i < normals.size(); i++)
    {
        pCloneCollidableObject->normals.push_back(normals[i]);
    }

    pCloneCollidableObject->position = position;

    return pCloneCollidableObject;
}

void CalculateInterval(vector<Vector2> *pVertices, Vector2 axis, double *pMinDistance, double *pMaxDistance)
{
    double minDistance = axis * (*pVertices)[0];
    double maxDistance = minDistance;

    for (unsigned int i = 1; i < pVertices->size(); i++)
    {
        double distance = axis * (*pVertices)[i];

        if (distance < minDistance)
        {
            minDistance = distance;
        }
        else if (distance > maxDistance)
        {
            maxDistance = distance;
        }
    }

    *pMinDistance = minDistance;
    *pMaxDistance = maxDistance;
}

bool IsIntervalIntersection(vector<Vector2> *pPolygon1Vertices, vector<Vector2> *pPolygon2Vertices, Vector2 offset, Vector2 axis, CollisionParameter *pParam)
{
    double minDistance1;
    double maxDistance1;
    double minDistance2;
    double maxDistance2;

    CalculateInterval(pPolygon1Vertices, axis, &minDistance1, &maxDistance1);
    CalculateInterval(pPolygon2Vertices, axis, &minDistance2, &maxDistance2);

    double h = offset * axis;
    minDistance1 += h;
    maxDistance1 += h;

    double distance1 = minDistance1 - maxDistance2;
    double distance2 = minDistance2 - maxDistance1;

    if (fabs(distance1) < 0.01)
    {
        distance1 = 0;
    }

    if (fabs(distance2) < 0.01)
    {
        distance2 = 0;
    }

    pParam->OverlapDistance = max(distance1, distance2);
    return distance1 <= 0 && distance2 <= 0;
}

bool CollisionExists(CollidableObject *pCollisionObject1, Vector2 collisionObject1Offset, CollidableObject *pCollisionObject2, Vector2 collisionObject2Offset, CollisionParameter *pParam)
{
    pParam->OverlapAxis = Vector2(0, 0);
    pParam->OverlapDistance = -numeric_limits<double>::infinity();

    int axesCount = 0;
    Vector2 axes[32];

    for (unsigned int i = 0; i < pCollisionObject1->GetNormals()->size(); i++)
    {
        axes[axesCount++] = (*pCollisionObject1->GetNormals())[i];
    }

    for (unsigned int i = 0; i < pCollisionObject2->GetNormals()->size(); i++)
    {
        axes[axesCount++] = (*pCollisionObject2->GetNormals())[i];
    }

    Vector2 collisionObjectsSeparation = pCollisionObject1->GetPosition() + collisionObject1Offset - (pCollisionObject2->GetPosition() + collisionObject2Offset);

    for (int i = 0; i < axesCount; i++)
    {
        CollisionParameter tempParam;

        if (!IsIntervalIntersection(pCollisionObject1->GetVertices(), pCollisionObject2->GetVertices(), collisionObjectsSeparation, axes[i], &tempParam))
        {
            return false;
        }

        if (tempParam.OverlapDistance < 0)
        {
            double overlapDistance = tempParam.OverlapDistance;
            Vector2 overlapAxis = axes[i];

            if ((overlapAxis * overlapDistance) * collisionObjectsSeparation < 0)
            {
                overlapAxis *= -1;
            }

            if (pParam->OverlapDistance < 0 && tempParam.OverlapDistance > pParam->OverlapDistance)
            {
                pParam->OverlapDistance = overlapDistance;
                pParam->OverlapAxis = overlapAxis;
            }

            pParam->AddOverlapEntry(overlapDistance, overlapAxis, pCollisionObject1, pCollisionObject2);
        }
        else
        {
            double overlapDistance = tempParam.OverlapDistance;
            Vector2 overlapAxis = axes[i];

            if (overlapDistance > pParam->OverlapDistance)
            {
                pParam->OverlapDistance = overlapDistance;
                pParam->OverlapAxis = overlapAxis;
            }

            pParam->AddOverlapEntry(overlapDistance, overlapAxis, pCollisionObject1, pCollisionObject2);
        }
    }

    return true;
}
