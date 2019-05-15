#ifndef FIELDCHARACTER_STAGING_H
#define FIELDCHARACTER_STAGING_H

#include "Enums.Staging.h"

#include "Rectangle.h"
#include "Vector2.h"

#include <QMap>
#include <QString>

namespace Staging
{

class HitBox;
class Condition;

class FieldCharacter
{
public:
    FieldCharacter(XmlReader *pReader);
    virtual ~FieldCharacter();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap);

    QString CharacterStandingAnimationIds[FieldCharacterDirection_Count];
    QString CharacterWalkingAnimationIds[FieldCharacterDirection_Count];
    QString CharacterRunningAnimationIds[FieldCharacterDirection_Count];

    QString Id;
    QString Name;
    Vector2 Position;
    HitBox *pHitBox;
    CharacterDirection Direction;
    FieldCharacterDirection SpriteDirection;
    int AnchorPosition;
    RectangleWH ClickRect;
    QString ClickEncounterId;
    QString ClickCutsceneId;
    Vector2 InteractionLocation;
    bool InteractFromAnywhere;
    Condition *pCondition;
};

}

#endif // FIELDCHARACTER_STAGING_H
