#ifndef FOREGROUNDELEMENT_STAGING_H
#define FOREGROUNDELEMENT_STAGING_H

#include "Polygon.h"
#include "Vector2.h"
#include "XmlReader.h"

#include <QList>
#include <QString>

namespace Staging
{

class Animation;
class Encounter;
class HitBox;
class Condition;

class ForegroundElementAnimation
{
public:
    ForegroundElementAnimation(XmlReader *pReader);

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString basePath);

    QString AnimationId;
    Vector2 Position;
};

class ForegroundElement
{
public:
    ForegroundElement()
    {
        pEncounter = NULL;
        pHitBox = NULL;
        pCondition = NULL;
    }

    ForegroundElement(XmlReader *pReader);
    virtual ~ForegroundElement();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString basePath);
    void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

    QString Name;
    GeometricPolygon ClickPolygon;

    Encounter *pEncounter;
    QString ZoomedViewId;

    Vector2 Position;
    int AnchorPosition;

    QString SpriteId;

    Vector2 InteractionLocation;
    bool InteractFromAnywhere;
    HitBox *pHitBox;

    QList<ForegroundElementAnimation *> ForegroundElementAnimationList;

    Condition *pCondition;

protected:
    void LoadFromXmlCore(XmlReader *pReader);
};

class HiddenForegroundElement : public ForegroundElement
{
public:
    HiddenForegroundElement(XmlReader *pReader);

    QString RequiredPartnerId;
    bool IsRequired;
};

}

#endif // FOREGROUNDELEMENT_STAGING_H
