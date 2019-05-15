#ifndef ZOOMEDVIEW_H
#define ZOOMEDVIEW_H

#include "ForegroundElement.h"
#include "XmlStorableObject.h"

namespace Staging
{
    class ZoomedView;
}

class ZoomedView : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(ZoomedView)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_FILE_PATH(backgroundFilePath)
        XML_STORABLE_CUSTOM_OBJECT_LIST(foregroundElementList, ForegroundElement::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    ZoomedView() { }

    ZoomedView(QString id, QString backgroundFilePath)
        : ZoomedView()
    {
        this->id = id;
        this->backgroundFilePath = backgroundFilePath;
    }

    ZoomedView(Staging::ZoomedView *pStagingZoomedView);
    virtual ~ZoomedView();

    static ZoomedView * CreateFromXml(XmlReader *pReader)
    {
        return new ZoomedView(pReader);
    }

    void WriteToCaseXml(XmlWriter *pWriter);

    QString GetId() { return id; }
    QString GetDisplayName() { return id; }

    static QString GetObjectAdditionString() { return QString("zoomed view"); }
    static QString GetListTitle() { return QString("Zoomed Views"); }
    static bool GetIsMainList() { return false; }
    static bool GetAllowsNewObjects() { return true; }

    class OverviewView : public IManipulationSurfaceView<ZoomedView>
    {
        friend class ZoomedView;

    public:
        OverviewView(ZoomedView *pLocation);
        ~OverviewView();

        void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        void DrawChildren(QGraphicsScene *pScene);
        void UpdateChildren();
        void ResetChildren();

        QRectF GetSceneRect()
        {
            QSize pixmapSize = backgroundPixmap.size();
            return QRectF(0, 0, pixmapSize.width(), pixmapSize.height());
        }

    protected:
        QList<IManipulatable *> & GetManipulatableList()
        {
            return manipulatableList;
        }

    private:
        QPixmap backgroundPixmap;

        QList<IManipulatable *> manipulatableList;
        QList<IManipulatable *> foregroundElementManipulatableList;
    };

    ZoomedView::OverviewView * GetOverviewView()
    {
        return new ZoomedView::OverviewView(this);
    }

private:
    QString id;
    QString backgroundFilePath;

    QList<ForegroundElement *> foregroundElementList;
};

#endif // ZOOMEDVIEW_H
