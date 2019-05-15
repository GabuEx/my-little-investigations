#ifndef HITBOX_H
#define HITBOX_H

#include <QList>
#include <QGraphicsPolygonItem>

#include "Vector2.h"
#include "Rectangle.h"
#include "XmlStorableObject.h"

#include "CaseCreator/Utilities/Interfaces.h"

namespace Staging
{
    class HitBox;
    class HitBoxPolygon;
}

class ObjectManipulatorSlots;

class HitBox : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(HitBox)
        XML_STORABLE_CUSTOM_OBJECT_LIST(polygonList, HitBox::Polygon::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    class Polygon;
    class Rectangle;

    HitBox() { }
    HitBox(Staging::HitBox *pStagingHitBox);
    virtual ~HitBox();

    static HitBox * CreateFromXml(XmlReader *pReader)
    {
        return new HitBox(pReader);
    }

    void WriteToCaseXml(XmlWriter *pWriter);

    RectangleWH GetBoundingBox() const;

    class Polygon : public IManipulatableObject, public XmlStorableObject
    {
        BEGIN_XML_STORABLE_OBJECT(Polygon)
            XML_STORABLE_CUSTOM_OBJECT_LIST(vertices, Vector2::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT(position, Vector2::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        Polygon() { }
        Polygon(Staging::HitBoxPolygon *pStagingHitBoxPolygon);
        virtual ~Polygon();

        static HitBox::Polygon * CreateFromXml(XmlReader *pReader)
        {
            return new HitBox::Polygon(pReader);
        }

        void WriteToCaseXml(XmlWriter *pWriter);

        QList<Vector2> * GetVertices() { return &this->vertices; }

        Vector2 GetPosition() const { return this->position; }
        void SetPosition(QPointF position) { this->position = position; }

        virtual ObjectManipulatorSlots * GetManipulationWidget();

        class ManipulationView : public IManipulationView<HitBox::Polygon>
        {
        public:
            ManipulationView(HitBox::Polygon *pPolygon, IDrawable *pParent, UndoCommandSink *pUndoCommandSink);
            virtual ~ManipulationView();

            virtual void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
            virtual void UpdateCore();

            Vector2 GetPosition()
            {
                return pObject->GetPosition();
            }

            virtual RectangleWH GetBoundingRect();

            virtual qreal GetZOrder()
            {
                // We'll set the z-value of hitboxes to a very large value
                // to ensure that it always appears on top of everything else,
                // except view boxes and mouse-over text.
                // For non-selected hitboxes, we'll set it to one less than that
                // so these appear underneath selected bounding boxes, but
                // above everything else.
                return 100 * 100000 - (isSelected ? 1 : 2);
            }

            qreal GetOpacity()
            {
                return
                    pParent->GetOpacity() *
                        (!IsEnabled() ? 0.0 :
                            (isSelected ? 1.0 :
                                (isMouseOver ? 2.0 / 3.0 : 1.0 / 3.0)));
            }

        protected:
            void OnGotFocus();
            void OnLostFocus();

            void OnMouseEnter();
            void OnMouseLeave();

            void OnMousePress();
            void OnMouseDrag(Vector2 delta);
            void OnMouseRelease();

            void SetPosition(Vector2 position);

        private:
            void SetIsMouseOver(bool isMouseOver);
            void SetIsSelected(bool isSelected);

            Vector2 positionBeforeDrag;
            bool isMouseOver;
            bool isSelected;

            QGraphicsPolygonItem *pPolygonItem;
        };

        GET_MANIPULATION_VIEW_DEFINITION(HitBox::Polygon)

    private:
        QList<Vector2> vertices;
        Vector2 position;
    };

    QList<Polygon::ManipulationView *> GetPolygonManipulationViews(IDrawable *pParent, UndoCommandSink *pUndoCommandSink = NULL);

private:
    QList<Polygon *> polygonList;
};

#endif // HITBOX_H
