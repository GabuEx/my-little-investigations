#ifndef LOCATION_STAGING_H
#define LOCATION_STAGING_H

#include "Enums.Staging.h"

#include "Vector2.h"
#include "XmlReader.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

namespace Staging
{

class HitBox;
class FieldCharacter;
class Crowd;
class ForegroundElement;
class HiddenForegroundElement;
class ZoomedView;
class FieldCutscene;
class HeightMap;
class Encounter;
class Condition;

class Location
{
public:
    class Transition
    {
        friend class Location;

    public:
        ~Transition();

        void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

        QString TargetLocationId;
        QString TargetLocationName;
        HitBox *pHitBox;
        TransitionDirection TransitionDirectionValue;
        Vector2 InteractionLocation;
        bool InteractFromAnywhere;
        QString TransitionStartSfxId;
        QString TransitionEndSfxId;
        bool HideWhenLocked;
        Condition *pCondition;
        Encounter *pEncounter;

    private:
        Transition(XmlReader *pReader);
    };

    class StartPosition
    {
        friend class Location;

    public:
        StartPosition()
        {
            Direction = CharacterDirection_None;
            FieldDirection = FieldCharacterDirection_None;
        }

        Vector2 Position;
        CharacterDirection Direction;
        FieldCharacterDirection FieldDirection;

    private:
        StartPosition(Vector2 position, CharacterDirection direction, FieldCharacterDirection fieldDirection)
        {
            Position = position;
            Direction = direction;
            FieldDirection = fieldDirection;
        }

        StartPosition(XmlReader *pReader);
    };

    class LoopingSound
    {
    public:
        LoopingSound(XmlReader *pReader);

        QString SoundId;
        Vector2 Origin;
    };

    Location(XmlReader *pReader);
    ~Location();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap);
    void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

    QString Id;
    QString BackgroundSpriteId;
    QString Bgm;
    QString AmbianceSfxId;

    HitBox *pAreaHitBox;
    QStringList CutsceneIdList;

    QList<FieldCharacter *> CharacterList;
    QList<Crowd *> CrowdList;
    QList<ForegroundElement *> ForegroundElementList;
    QList<HiddenForegroundElement *> HiddenForegroundElementList;
    QMap<QString, ZoomedView *> ZoomedViewsByIdMap;
    QList<Transition *> TransitionList;
    QList<LoopingSound *> LoopingSoundList;
    QList<HeightMap *> HeightMapList;

    StartPosition StartPositionFromMap;
    QMap<QString, StartPosition> TransitionIdToStartPositionMap;
    QMap<QString, StartPosition> TransitionIdToPartnerStartPositionMap;
};

}

#endif // LOCATION_H
