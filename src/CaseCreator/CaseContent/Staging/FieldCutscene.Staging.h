#ifndef FIELDCUTSCENE_STAGING_H
#define FIELDCUTSCENE_STAGING_H

#include "Vector2.h"

#include "Enums.Staging.h"

#include <QList>
#include <QMap>
#include <QString>

namespace Staging
{

class FieldCharacter;
class Encounter;

class FieldCutscene
{
public:
    class FieldCutscenePhase;
    class FieldCutsceneConcurrentMovements;

    FieldCutscene(XmlReader *pReader);
    ~FieldCutscene();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap);
    void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

    QList<FieldCutscenePhase *> PhaseList;
    QMap<QString, FieldCharacter *> IdToCharacterMap;

    QMap<FieldCharacter *, CharacterDirection> CharacterToOriginalCharacterDirectionMap;
    QMap<FieldCharacter *, FieldCharacterDirection> CharacterToOriginalFieldCharacterDirectionMap;
    QMap<FieldCharacter *, Vector2> CharacterToOriginalPositionMap;

    Vector2 InitialCameraPosition;

    QString Id;
    bool IsEnabled;
    bool PreventsInitialBgm;
    QString InitialBgmReplacement;
    bool PreventsInitialAmbiance;
    QString InitialAmbianceReplacement;

private:
    FieldCutscenePhase * GetPhaseForNextElement(XmlReader *pReader);

public:
    class FieldCutscenePhase
    {
    public:
        virtual ~FieldCutscenePhase() {}

        virtual FieldCutscenePhaseType GetType() = 0;

        virtual void AddSpritePaths(QMap<QString, QString> &, QString) {}
        virtual void AddDialogPaths(QMap<QString, QString> &) {}

        int MillisecondDelayBeforeBegin;
    };

    class FieldCutsceneConversation : public FieldCutscenePhase
    {
    public:
        ~FieldCutsceneConversation();

        FieldCutsceneConversation(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_Conversation; }

        void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

        Encounter *pEncounter;
    };

    class FieldCutsceneConcurrentMovements : public FieldCutscenePhase
    {
    public:
        ~FieldCutsceneConcurrentMovements();

        FieldCutsceneConcurrentMovements(XmlReader *pReader);
        FieldCutscenePhase * GetPhaseForNextElement(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_ConcurrentMovements; }

        void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString baseDir);

        QList<FieldCutscenePhase *> MovementList;
    };

    class FieldCutsceneMovement : public FieldCutscenePhase
    {
    public:
        FieldCutsceneMovement(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_Movement; }

        QString CharacterId;
        Vector2 TargetPosition;
        FieldCharacterState MovementState;
    };

    class FieldCutsceneOrient : public FieldCutscenePhase
    {
    public:
        FieldCutsceneOrient(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_Orient; }

        QString CharacterId;
        CharacterDirection Direction;
        FieldCharacterDirection SpriteDirection;
    };

    class FieldCutsceneSetBackground : public FieldCutscenePhase
    {
    public:
        FieldCutsceneSetBackground(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_SetBackground; }

        void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString baseDir);

        static int BackgroundCount;

        QString BackgroundSpriteId;
        int MsFadeDuration;
    };

    class FieldCutsceneCenterCamera : public FieldCutscenePhase
    {
    public:
        FieldCutsceneCenterCamera(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_CenterCamera; }

        QString CharacterId;
        Vector2 CameraCenterPosition;
    };

    class FieldCutscenePause : public FieldCutscenePhase
    {
    public:
        FieldCutscenePause(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_Pause; }

        int MsPauseDuration;
    };

    class FieldCutscenePlayBgm : public FieldCutscenePhase
    {
    public:
        FieldCutscenePlayBgm(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_PlayBgm; }

        QString BgmId;
    };

    class FieldCutsceneStopBgm : public FieldCutscenePhase
    {
    public:
        FieldCutsceneStopBgm(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_StopBgm; }

        bool IsInstant;
    };

    class FieldCutscenePlayAmbiance : public FieldCutscenePhase
    {
    public:
        FieldCutscenePlayAmbiance(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_PlayAmbiance; }

        QString AmbianceSfxId;
    };

    class FieldCutsceneStopAmbiance : public FieldCutscenePhase
    {
    public:
        FieldCutsceneStopAmbiance(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_StopAmbiance; }

        bool IsInstant;
    };

    class FieldCutsceneEndCase : public FieldCutscenePhase
    {
    public:
        FieldCutsceneEndCase(XmlReader *pReader);

        FieldCutscenePhaseType GetType() override { return FieldCutscenePhaseType_EndCase; }

        bool CompletesCase;
    };
};

}

#endif // FIELDCUTSCENE_STAGING_H
