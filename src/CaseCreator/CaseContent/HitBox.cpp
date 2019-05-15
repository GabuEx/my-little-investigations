#include "HitBox.h"

#include "XmlReader.h"
#include "XmlWriter.h"
#include "CaseCreator/CaseContent/Staging/HitBox.Staging.h"

#include <math.h>
#include <limits>


HitBox::HitBox(Staging::HitBox *pStagingHitBox)
{
    for (int i = 0; i < pStagingHitBox->PolygonList.count(); i++)
    {
        polygonList.push_back(new HitBox::Polygon(pStagingHitBox->PolygonList[i]));
    }
}

HitBox::~HitBox()
{
    for (int i = 0; i < polygonList.size(); i++)
    {
        delete polygonList[i];
    }
}

void HitBox::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("HitBox");
    pWriter->StartElement("CollidableObjectList");

    for (Polygon *pPolygon : this->polygonList)
    {
        pWriter->StartElement("Entry");
        pPolygon->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();
    pWriter->EndElement();
}

RectangleWH HitBox::GetBoundingBox() const
{
    double left = numeric_limits<double>::infinity();
    double top = numeric_limits<double>::infinity();
    double right = -numeric_limits<double>::infinity();
    double bottom = -numeric_limits<double>::infinity();

    for (int i = 0; i < polygonList.size(); i++)
    {
        QList<Vector2> *pVertices = polygonList[i]->GetVertices();
        Vector2 position = polygonList[i]->GetPosition();

        for (int j = 0; j < pVertices->size(); j++)
        {
            Vector2 vertex = (*pVertices)[j] + position;

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

HitBox::Polygon::Polygon(Staging::HitBoxPolygon *pStagingHitBoxPolygon)
{
    for (int i = 0; i < pStagingHitBoxPolygon->Vertices.count(); i++)
    {
        vertices.push_back(pStagingHitBoxPolygon->Vertices[i]);
    }

    position = pStagingHitBoxPolygon->Position;
}

HitBox::Polygon::~Polygon()
{

}

void HitBox::Polygon::WriteToCaseXml(XmlWriter *pWriter)
{
    QList<Vector2> normals;

    for (int i = 0; i < vertices.length(); i++)
    {
        if (i > 0)
        {
            Vector2 edgeVector = vertices[i] - vertices[i - 1];
            normals.append((Vector2(-edgeVector.GetY(), edgeVector.GetX())).Normalize());
        }
    }

    Vector2 edgeVector = vertices[vertices.length() - 1] - vertices[0];
    normals.append((Vector2(-edgeVector.GetY(), edgeVector.GetX())).Normalize());

    pWriter->StartElement("CollidableObject");

    pWriter->StartElement("Vertices");

    for (Vector2 vertex : this->vertices)
    {
        pWriter->StartElement("VertexEntry");
        vertex.SaveToXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();

    pWriter->StartElement("Normals");

    for (Vector2 normal : normals)
    {
        pWriter->StartElement("NormalEntry");
        normal.SaveToXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();

    pWriter->StartElement("Position");
    this->position.SaveToXml(pWriter);
    pWriter->EndElement();

    pWriter->EndElement();
}

ObjectManipulatorSlots *HitBox::Polygon::GetManipulationWidget()
{
    return NULL;
}

HitBox::Polygon::ManipulationView::ManipulationView(HitBox::Polygon *pPolygon, IDrawable *pParent, UndoCommandSink *pUndoCommandSink)
    : IDrawable(pParent)
    , IManipulationView<HitBox::Polygon>(pPolygon, pParent, pUndoCommandSink)
{
    positionBeforeDrag = Vector2(0, 0);
    isMouseOver = false;
    isSelected = false;
    pPolygonItem = NULL;
}

HitBox::Polygon::ManipulationView::~ManipulationView()
{
    pPolygonItem = NULL;
}

void HitBox::Polygon::ManipulationView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    QPolygon polygon;

    for (Vector2 point : pObject->vertices)
    {
        polygon.append(point.ToQPoint());
    }

    pPolygonItem = pScene->addPolygon(polygon, QPen(QColor(0, 0, 0, 255)), QBrush(QColor(0, 0, 0, 128)));
    addedItems.append(pPolygonItem);
}

void HitBox::Polygon::ManipulationView::UpdateCore()
{
    //do stuff with pPolygonItem
}

RectangleWH HitBox::Polygon::ManipulationView::GetBoundingRect()
{
    return RectangleWH(pPolygonItem->boundingRect());
}

void HitBox::Polygon::ManipulationView::OnGotFocus()
{
    SetIsSelected(true);
}

void HitBox::Polygon::ManipulationView::OnLostFocus()
{
    SetIsSelected(false);
}

void HitBox::Polygon::ManipulationView::OnMouseEnter()
{
    SetIsMouseOver(true);
}

void HitBox::Polygon::ManipulationView::OnMouseLeave()
{
    SetIsMouseOver(false);
}

void HitBox::Polygon::ManipulationView::OnMousePress()
{
    positionBeforeDrag = this->pObject->position;
}

void HitBox::Polygon::ManipulationView::OnMouseDrag(Vector2 delta)
{
    SetPosition(this->pObject->position + delta);
}

void HitBox::Polygon::ManipulationView::OnMouseRelease()
{
    if (this->pObject->position != positionBeforeDrag)
    {
        AddUndoCommand(new MoveUndoCommand(this, positionBeforeDrag, this->pObject->position));
    }
}

void HitBox::Polygon::ManipulationView::SetPosition(Vector2 position)
{
    this->pObject->SetPosition(position.ToQPointF());
    Update();
}

void HitBox::Polygon::ManipulationView::SetIsMouseOver(bool isMouseOver)
{
    this->isMouseOver = isMouseOver;
    Update();
}

void HitBox::Polygon::ManipulationView::SetIsSelected(bool isSelected)
{
    this->isSelected = isSelected;
    Update();
}

QList<HitBox::Polygon::ManipulationView *> HitBox::GetPolygonManipulationViews(IDrawable *pParent, UndoCommandSink *pUndoCommandSink)
{
    QList<HitBox::Polygon::ManipulationView *> manipulationViewList;

    for (HitBox::Polygon *pPolygon : polygonList)
    {
        manipulationViewList.append(pPolygon->GetManipulationView(pParent, pUndoCommandSink));
    }

    return manipulationViewList;
}

GET_MANIPULATABLE_OBJECT_FROM_DEFINITION(HitBox::Polygon)
