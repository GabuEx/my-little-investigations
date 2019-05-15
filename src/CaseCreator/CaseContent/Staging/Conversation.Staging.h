#ifndef CONVERSATION_STAGING_H
#define CONVERSATION_STAGING_H

#include "Enums.Staging.h"

#include "Vector2.h"
#include "XmlReader.h"

#include <QList>
#include <QMap>
#include <QString>

namespace Staging
{

class Condition;

class Conversation
{
public:
    class Action;
    class ContinuousAction;
    class ShowDialogAction;

    class UnlockCondition
    {
    public:
        static UnlockCondition * LoadFromXml(XmlReader *pReader);

        UnlockCondition() { }
        virtual ~UnlockCondition() { }

        virtual UnlockConditionType GetType() = 0;
    };

    class FlagSetUnlockCondition : public UnlockCondition
    {
        friend class UnlockCondition;

    public:
        UnlockConditionType GetType()
        {
            return UnlockConditionType_FlagSet;
        }

        QString FlagId;

    private:
        FlagSetUnlockCondition(XmlReader *pReader);
    };

    class PartnerPresentUnlockCondition : public UnlockCondition
    {
        friend class UnlockCondition;

    public:
        UnlockConditionType GetType()
        {
            return UnlockConditionType_PartnerPresent;
        }

        QString PartnerId;

    private:
        PartnerPresentUnlockCondition(XmlReader *pReader);
    };

    Conversation() { }
    Conversation(XmlReader *pReader);
    virtual ~Conversation();

    static Conversation * LoadFromXml(XmlReader *pReader);

    void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

    QList<Action *> ActionList;

    QString Id;
    QString Name;
    QString RequiredPartnerId;
    bool IsEnabled;
    bool HasBeenCompleted;

protected:
    void LoadFromXmlCore(XmlReader *pReader);
    virtual Action * GetActionForNextElement(XmlReader *pReader);

public:
    class Action
    {
    public:
        Action() {}
        virtual ~Action() {}

        virtual ConversationActionType GetType() = 0;
        virtual void AddDialogPaths(QMap<QString, QString> &) { }
    };

    class SingleAction : public Action
    {
    public:
        SingleAction() {}
    };

    class ContinuousAction : public Action
    {
    public:
        ContinuousAction() { }
    };

    class CharacterChangeAction;

    class MultipleCharacterChangeAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        ~MultipleCharacterChangeAction();

        ConversationActionType GetType()
        {
            return ConversationActionType_MultipleCharacterChange;
        }

        QList<CharacterChangeAction *> CharacterChangeList;

    private:
        MultipleCharacterChangeAction(XmlReader *pReader);
    };

    class CharacterChangeAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_CharacterChange;
        }

        CharacterPosition Position;
        QString CharacterIdToChangeTo;
        QString InitialEmotionId;
        QString SfxId;

    private:
        CharacterChangeAction(XmlReader *pReader);
    };

    class SetFlagAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SetFlag;
        }

        QString FlagId;

    private:
        SetFlagAction(XmlReader *pReader);
    };

    class BranchOnConditionAction : public SingleAction
    {
        friend class Conversation;

    public:
        ~BranchOnConditionAction();

        ConversationActionType GetType()
        {
            return ConversationActionType_BranchOnCondition;
        }

        Condition *pCondition;
        int TrueIndex;
        int FalseIndex;

    private:
        BranchOnConditionAction(XmlReader *pReader);
    };

    class BranchIfTrueAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BranchIfTrue;
        }

        int EndIndex;

    private:
        BranchIfTrueAction(XmlReader *pReader);
    };

    class BranchIfFalseAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BranchIfFalse;
        }

        int EndIndex;

    private:
        BranchIfFalseAction(XmlReader *pReader);
    };

    class EndBranchOnConditionAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndBranchOnCondition;
        }

    private:
        EndBranchOnConditionAction(XmlReader *pReader);
    };

    class ShowDialogAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ShowDialog;
        }

        void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

        QString Id;

        CharacterPosition SpeakerPosition;
        QString CharacterId;
        QString RawDialog;
        QString FilePath;
        int LeadInTime;

        bool IsInConfrontation;

    protected:
        void LoadFromXmlCore(XmlReader *pReader);

        ShowDialogAction() { }
        ShowDialogAction(XmlReader *pReader);
    };

    class ShowDialogAutomaticAction : public ShowDialogAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ShowDialogAutomatic;
        }

        int DelayBeforeContinuing;

    private:
        ShowDialogAutomaticAction(XmlReader *pReader);
    };

    class MustPresentEvidenceAction : public ShowDialogAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_MustPresentEvidence;
        }

        QList<QString> CorrectEvidenceIdList;

        int CorrectEvidencePresentedIndex;
        int WrongEvidencePresentedIndex;
        int EndRequestedIndex;

    protected:
        virtual void LoadFromXmlCore(XmlReader *pReader);

    private:
        MustPresentEvidenceAction(XmlReader *pReader);
    };

    class SkipWrongEvidencePresentedAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SkipWrongEvidencePresented;
        }

        int AfterWrongEvidencePresentedIndex;

    private:
        SkipWrongEvidencePresentedAction(XmlReader *pReader);
    };

    class BeginWrongEvidencePresentedAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginWrongEvidencePresented;
        }

    private:
        BeginWrongEvidencePresentedAction(XmlReader *pReader);
    };

    class EndWrongEvidencePresentedAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndWrongEvidencePresented;
        }

    private:
        EndWrongEvidencePresentedAction(XmlReader *pReader);
    };

    class SkipEndRequestedAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SkipEndRequested;
        }

        int AfterEndRequestedIndex;

    private:
        SkipEndRequestedAction(XmlReader *pReader);
    };

    class BeginEndRequestedAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginEndRequested;
        }

    private:
        BeginEndRequestedAction(XmlReader *pReader);
    };

    class EndEndRequestedAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndEndRequested;
        }

    private:
        EndEndRequestedAction(XmlReader *pReader);
    };

    class EndMustPresentEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndMustPresentEvidence;
        }

    private:
        EndMustPresentEvidenceAction(XmlReader *pReader);
    };

    class EnableConversationAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EnableConversation;
        }

        QString ConversationId;

    private:
        EnableConversationAction(XmlReader *pReader);
    };

    class EnableEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EnableEvidence;
        }

        QString EvidenceId;

    private:
        EnableEvidenceAction(XmlReader *pReader);
    };

    class UpdateEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_UpdateEvidence;
        }

        QString EvidenceId;
        QString NewEvidenceId;

    private:
        UpdateEvidenceAction(XmlReader *pReader);
    };

    class DisableEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_DisableEvidence;
        }

        QString EvidenceId;

    private:
        DisableEvidenceAction(XmlReader *pReader);
    };

    class EnableCutsceneAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EnableCutscene;
        }

        QString CutsceneId;

    private:
        EnableCutsceneAction(XmlReader *pReader);
    };

    class NotificationAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_Notification;
        }

        QString RawNotificationText;
        QString PartnerId;
        QString OldEvidenceId;
        QString NewEvidenceId;
        QString LocationWithCutsceneId;
        QString CutsceneId;

    private:
        NotificationAction(XmlReader *pReader);
    };

    class PlayBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_PlayBgm;
        }

        QString BgmId;
        bool PreserveBgm;

    private:
        PlayBgmAction(XmlReader *pReader);
    };

    class PauseBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_PauseBgm;
        }

    private:
        PauseBgmAction(XmlReader *pReader);
    };

    class ResumeBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ResumeBgm;
        }

    private:
        ResumeBgmAction(XmlReader *pReader);
    };

    class StopBgmAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_StopBgm;
        }

        bool IsInstant;
        bool PreserveBgm;

    private:
        StopBgmAction(XmlReader *pReader);
    };

    class PlayAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_PlayAmbiance;
        }

        QString AmbianceSfxId;
        bool PreserveAmbiance;

    private:
        PlayAmbianceAction(XmlReader *pReader);
    };

    class PauseAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_PauseAmbiance;
        }

    private:
        PauseAmbianceAction(XmlReader *pReader);
    };

    class ResumeAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ResumeAmbiance;
        }

    private:
        ResumeAmbianceAction(XmlReader *pReader);
    };

    class StopAmbianceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_StopAmbiance;
        }

        bool IsInstant;
        bool PreserveAmbiance;

    private:
        StopAmbianceAction(XmlReader *pReader);
    };

    class StartAnimationAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_StartAnimation;
        }

        QString AnimationId;

    private:
        StartAnimationAction(XmlReader *pReader);
    };

    class StopAnimationAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_StopAnimation;
        }

    private:
        StopAnimationAction(XmlReader *pReader);
    };

    class SetPartnerAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SetPartner;
        }

        QString PartnerId;

    private:
        SetPartnerAction(XmlReader *pReader);
    };

    class GoToPresentWrongEvidenceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_GoToPresentWrongEvidence;
        }

    private:
        GoToPresentWrongEvidenceAction(XmlReader *pReader);
    };

    class LockConversationAction : public SingleAction
    {
        friend class Conversation;

    public:
        virtual ~LockConversationAction();

        ConversationActionType GetType()
        {
            return ConversationActionType_LockConversation;
        }

        UnlockCondition *pUnlockCondition;

    private:
        LockConversationAction(XmlReader *pReader);

    };

    class ExitEncounterAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ExitEncounter;
        }

    private:
        ExitEncounterAction(XmlReader *pReader);
    };

    class MoveToLocationAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_MoveToLocation;
        }

        QString NewAreaId;
        QString NewLocationId;
        QString TransitionId;

    private:
        MoveToLocationAction(XmlReader *pReader);
    };

    class MoveToZoomedViewAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_MoveToZoomedView;
        }

        QString ZoomedViewId;

    private:
        MoveToZoomedViewAction(XmlReader *pReader);
    };

    class EndCaseAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndCase;
        }

        bool CompletesCase;

    private:
        EndCaseAction(XmlReader *pReader);
    };

    class BeginMultipleChoiceAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginMultipleChoice;
        }

        QList<QString> OptionTexts;
        QList<int> OptionIndexes;

    private:
        BeginMultipleChoiceAction(XmlReader *pReader);
    };

    class BeginMultipleChoiceOptionAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginMultipleChoiceOption;
        }

    private:
        BeginMultipleChoiceOptionAction(XmlReader *pReader);
    };

    class ExitMultipleChoiceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ExitMultipleChoice;
        }

        int ExitIndex;

    private:
        ExitMultipleChoiceAction(XmlReader *pReader);
    };

    class EndMultipleChoiceOptionAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndMultipleChoiceOption;
        }

        int StartIndex;

    private:
        EndMultipleChoiceOptionAction(XmlReader *pReader);
    };

    class EndMultipleChoiceAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndMultipleChoice;
        }

    private:
        EndMultipleChoiceAction(XmlReader *pReader);
    };

    class EnableFastForwardAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EnableFastForward;
        }

    private:
        EnableFastForwardAction(XmlReader *pReader);
    };

    class DisableFastForwardAction : public SingleAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_DisableFastForward;
        }

    private:
        DisableFastForwardAction(XmlReader *pReader);
    };

    class BeginBreakdownAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginBreakdown;
        }

        CharacterPosition CharacterPositionValue;

    private:
        BeginBreakdownAction(XmlReader *pReader);
    };

    class EndBreakdownAction : public ContinuousAction
    {
        friend class Conversation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndBreakdown;
        }

    private:
        EndBreakdownAction(XmlReader *pReader);
    };
};

class Interrogation : public Conversation
{
    friend class Conversation;

public:
    Interrogation() { }
    Interrogation(XmlReader *pReader);

protected:
    virtual Action * GetActionForNextElement(XmlReader *pReader);

public:
    class BeginInterrogationRepeatAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginInterrogationRepeat;
        }

    private:
        BeginInterrogationRepeatAction(XmlReader *pReader);
    };

    class ShowInterrogationAction : public ShowDialogAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ShowInterrogation;
        }

        QMap<QString, int> EvidencePresentedIndexes;
        QMap<QString, int> PartnerUsedIndexes;

        QString ConditionFlag;

        int PreviousInterrogationIndex;
        int NextInterrogationIndex;
        int InterrogationFinishIndex;
        int PressForInfoIndex;
        int WrongEvidencePresentedIndex;
        int WrongPartnerUsedIndex;
        int EndRequestedIndex;

    protected:
        virtual void LoadFromXmlCore(XmlReader *pReader);

    private:
        ShowInterrogationAction(XmlReader *pReader);
    };

    class BeginPressForInfoAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginPressForInfo;
        }

    private:
        BeginPressForInfoAction(XmlReader *pReader);
    };

    class EndPressForInfoAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndPressForInfo;
        }

        int NextDialogIndex;

    private:
        EndPressForInfoAction(XmlReader *pReader);
    };

    class BeginPresentEvidenceAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginPresentEvidence;
        }

    private:
        BeginPresentEvidenceAction(XmlReader *pReader);
    };

    class EndPresentEvidenceAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndPresentEvidence;
        }

        int NextDialogIndex;

    private:
        EndPresentEvidenceAction(XmlReader *pReader);
    };

    class BeginUsePartnerAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginUsePartner;
        }

    private:
        BeginUsePartnerAction(XmlReader *pReader);
    };

    class EndUsePartnerAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndUsePartner;
        }

        int NextDialogIndex;

    private:
        EndUsePartnerAction(XmlReader *pReader);
    };

    class EndShowInterrogationBranchesAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndShowInterrogationBranches;
        }

    private:
        EndShowInterrogationBranchesAction(XmlReader *pReader);
    };

    class ExitInterrogationRepeatAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_ExitInterrogationRepeat;
        }

        int AfterEndInterrogationRepeatIndex;

    private:
        ExitInterrogationRepeatAction(XmlReader *pReader);
    };

    class SkipWrongPartnerUsedAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SkipWrongPartnerUsed;
        }

        int AfterWrongPartnerUsedIndex;

    private:
        SkipWrongPartnerUsedAction(XmlReader *pReader);
    };

    class BeginWrongPartnerUsedAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginWrongPartnerUsed;
        }

    private:
        BeginWrongPartnerUsedAction(XmlReader *pReader);
    };

    class EndWrongPartnerUsedAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndWrongPartnerUsed;
        }

    private:
        EndWrongPartnerUsedAction(XmlReader *pReader);
    };

    class EndInterrogationRepeatAction : public SingleAction
    {
        friend class Interrogation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndInterrogationRepeat;
        }

        int BeginInterrogationRepeatIndex;

    private:
        EndInterrogationRepeatAction(XmlReader *pReader);
    };
};

class Confrontation : public Interrogation
{
    friend class Conversation;

public:
    class BeginConfrontationTopicSelectionAction;
    class SkipPlayerDefeatedAction;

    class Topic
    {
    public:
        Topic(XmlReader *pReader);

        QString Id;
        QString Name;
        int ActionIndex;
        bool IsEnabled;
        bool IsCompleted;
    };

    Confrontation(XmlReader *pReader);

    QMap<QString, QString> ConfrontationTopicNamesById;

protected:
    virtual void LoadFromXmlCore(XmlReader *pReader);
    virtual Action * GetActionForNextElement(XmlReader *pReader);

public:
    class SetParticipantsAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SetParticipants;
        }

        QString PlayerCharacterId;
        QString OpponentCharacterId;

    private:
        SetParticipantsAction(XmlReader *pReader);
    };

    class SetIconOffsetAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SetIconOffset;
        }

        Vector2 Offset;
        bool IsPlayer;

    private:
        SetIconOffsetAction(XmlReader *pReader);
    };

    class SetHealthAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SetHealth;
        }

        int InitialHealth;
        bool IsPlayer;

    private:
        SetHealthAction(XmlReader *pReader);
    };

    class InitializeBeginConfrontationTopicSelectionAction : public ContinuousAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_InitializeBeginConfrontationTopicSelection;
        }

    private:
        InitializeBeginConfrontationTopicSelectionAction(XmlReader *pReader);
    };

    class BeginConfrontationTopicSelectionAction : public ContinuousAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginConfrontationTopicSelection;
        }

        QList<Topic *> TopicList;
        QMap<Topic *, bool> TopicToInitialEnabledStateMap;
        Topic *pInitialTopic;
        bool InitialTopicIsEnabled;

        int PlayerDefeatedIndex;
        int EndActionIndex;

    private:
        BeginConfrontationTopicSelectionAction(XmlReader *pReader);
    };

    class BeginConfrontationTopicAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginConfrontationTopic;
        }

    private:
        BeginConfrontationTopicAction(XmlReader *pReader);
    };

    class EndConfrontationTopicAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndConfrontationTopic;
        }

        int BeginConfrontationTopicSelectionActionIndex;

    private:
        EndConfrontationTopicAction(XmlReader *pReader);
    };

    class SkipPlayerDefeatedAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_SkipPlayerDefeated;
        }

        int SkipToIndex;

    private:
        SkipPlayerDefeatedAction(XmlReader *pReader);
    };

    class BeginPlayerDefeatedAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_BeginPlayerDefeated;
        }

    private:
        BeginPlayerDefeatedAction(XmlReader *pReader);
    };

    class EndPlayerDefeatedAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndPlayerDefeated;
        }

    private:
        EndPlayerDefeatedAction(XmlReader *pReader);
    };

    class EndConfrontationTopicSelectionAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EndConfrontationTopicSelection;
        }

    private:
        EndConfrontationTopicSelectionAction(XmlReader *pReader);
    };

    class EnableTopicAction : public SingleAction
    {
        friend class Confrontation;

    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_EnableTopic;
        }

        QString TopicId;

    private:
        EnableTopicAction(XmlReader *pReader);
    };

    class RestartConfrontationAction : public SingleAction
    {
    public:
        ConversationActionType GetType()
        {
            return ConversationActionType_RestartConfrontation;
        }

        RestartConfrontationAction(XmlReader *pReader);
    };
};

}

#endif // CONVERSATION_STAGING_H
