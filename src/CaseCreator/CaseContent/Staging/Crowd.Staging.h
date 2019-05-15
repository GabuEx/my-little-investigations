#ifndef CROWD_STAGING_H
#define CROWD_STAGING_H

#include "Polygon.h"

#include "XmlReader.h"

#include <QString>

namespace Staging
{

class Encounter;
class HitBox;

class Crowd
{
public:
    Crowd(XmlReader *pReader);
    virtual ~Crowd();

    void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

    QString Name;
    GeometricPolygon ClickPolygon;

    Encounter *pEncounter;
    QString VideoId;

    Vector2 Position;
    int AnchorPosition;

    HitBox *pHitBox;

    Vector2 InteractionLocation;
    bool InteractFromAnywhere;
};

}

#endif // CROWD_STAGING_H
