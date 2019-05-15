#ifndef ENUMS_STAGING_H
#define ENUMS_STAGING_H

#include <QString>

namespace Staging
{

enum AnimationSoundType
{
    AnimationSoundType_None,
    AnimationSoundType_Hoofstep,
    AnimationSoundType_Specified,
};

enum CharacterDirection
{
    CharacterDirection_Left = 0,
    CharacterDirection_Right = 1,
    CharacterDirection_None = -1,
};

CharacterDirection StringToCharacterDirection(const QString &s);
QString CharacterDirectionToString(CharacterDirection d);

enum CharacterPosition
{
    CharacterPosition_None,
    CharacterPosition_Unknown,
    CharacterPosition_Offscreen,
    CharacterPosition_Left,
    CharacterPosition_Right,
};

CharacterPosition StringToCharacterPosition(const QString &s);

enum ConditionCriterionType
{
    ConditionCriterionType_None,
    ConditionCriterionType_FlagSet,
    ConditionCriterionType_EvidencePresent,
    ConditionCriterionType_PartnerPresent,
    ConditionCriterionType_ConversationLocked,
    ConditionCriterionType_TutorialsEnabled,
    ConditionCriterionType_And,
    ConditionCriterionType_Or,
    ConditionCriterionType_Not,
};

enum ConversationActionType
{
    ConversationActionType_None,

    // Conversation actions
    ConversationActionType_MultipleCharacterChange,
    ConversationActionType_CharacterChange,
    ConversationActionType_SetFlag,
    ConversationActionType_BranchOnCondition,
    ConversationActionType_BranchIfTrue,
    ConversationActionType_BranchIfFalse,
    ConversationActionType_EndBranchOnCondition,
    ConversationActionType_ShowDialog,
    ConversationActionType_ShowDialogAutomatic,
    ConversationActionType_MustPresentEvidence,
    ConversationActionType_SkipWrongEvidencePresented,
    ConversationActionType_BeginWrongEvidencePresented,
    ConversationActionType_EndWrongEvidencePresented,
    ConversationActionType_SkipEndRequested,
    ConversationActionType_BeginEndRequested,
    ConversationActionType_EndEndRequested,
    ConversationActionType_EndMustPresentEvidence,
    ConversationActionType_EnableConversation,
    ConversationActionType_EnableEvidence,
    ConversationActionType_UpdateEvidence,
    ConversationActionType_DisableEvidence,
    ConversationActionType_EnableCutscene,
    ConversationActionType_Notification,
    ConversationActionType_PlayBgm,
    ConversationActionType_PauseBgm,
    ConversationActionType_ResumeBgm,
    ConversationActionType_StopBgm,
    ConversationActionType_PlayAmbiance,
    ConversationActionType_PauseAmbiance,
    ConversationActionType_ResumeAmbiance,
    ConversationActionType_StopAmbiance,
    ConversationActionType_StartAnimation,
    ConversationActionType_StopAnimation,
    ConversationActionType_SetPartner,
    ConversationActionType_GoToPresentWrongEvidence,
    ConversationActionType_LockConversation,
    ConversationActionType_ExitEncounter,
    ConversationActionType_MoveToLocation,
    ConversationActionType_MoveToZoomedView,
    ConversationActionType_EndCase,
    ConversationActionType_BeginMultipleChoice,
    ConversationActionType_BeginMultipleChoiceOption,
    ConversationActionType_ExitMultipleChoice,
    ConversationActionType_EndMultipleChoiceOption,
    ConversationActionType_EndMultipleChoice,
    ConversationActionType_EnableFastForward,
    ConversationActionType_DisableFastForward,
    ConversationActionType_BeginBreakdown,
    ConversationActionType_EndBreakdown,

    // Interrogation actions
    ConversationActionType_BeginInterrogationRepeat,
    ConversationActionType_ShowInterrogation,
    ConversationActionType_BeginPressForInfo,
    ConversationActionType_EndPressForInfo,
    ConversationActionType_BeginPresentEvidence,
    ConversationActionType_EndPresentEvidence,
    ConversationActionType_BeginUsePartner,
    ConversationActionType_EndUsePartner,
    ConversationActionType_EndShowInterrogationBranches,
    ConversationActionType_ExitInterrogationRepeat,
    ConversationActionType_SkipWrongPartnerUsed,
    ConversationActionType_BeginWrongPartnerUsed,
    ConversationActionType_EndWrongPartnerUsed,
    ConversationActionType_EndInterrogationRepeat,

    // Confrontation actions
    ConversationActionType_SetParticipants,
    ConversationActionType_SetIconOffset,
    ConversationActionType_SetHealth,
    ConversationActionType_InitializeBeginConfrontationTopicSelection,
    ConversationActionType_BeginConfrontationTopicSelection,
    ConversationActionType_BeginConfrontationTopic,
    ConversationActionType_EndConfrontationTopic,
    ConversationActionType_SkipPlayerDefeated,
    ConversationActionType_BeginPlayerDefeated,
    ConversationActionType_EndPlayerDefeated,
    ConversationActionType_EndConfrontationTopicSelection,
    ConversationActionType_EnableTopic,
    ConversationActionType_RestartConfrontation,
};

QString ConversationActionTypeToString(ConversationActionType t);

enum FieldCutscenePhaseType
{
    FieldCutscenePhaseType_None,
    FieldCutscenePhaseType_Conversation,
    FieldCutscenePhaseType_ConcurrentMovements,
    FieldCutscenePhaseType_Movement,
    FieldCutscenePhaseType_Orient,
    FieldCutscenePhaseType_SetBackground,
    FieldCutscenePhaseType_CenterCamera,
    FieldCutscenePhaseType_Pause,
    FieldCutscenePhaseType_PlayBgm,
    FieldCutscenePhaseType_StopBgm,
    FieldCutscenePhaseType_PlayAmbiance,
    FieldCutscenePhaseType_StopAmbiance,
    FieldCutscenePhaseType_EndCase,
};

enum FieldCharacterDirection
{
    FieldCharacterDirection_Up = 0,
    FieldCharacterDirection_DiagonalUp = 1,
    FieldCharacterDirection_Side = 2,
    FieldCharacterDirection_DiagonalDown = 3,
    FieldCharacterDirection_Down = 4,
    FieldCharacterDirection_Count = 5,
    FieldCharacterDirection_None = -1,
};

FieldCharacterDirection StringToFieldCharacterDirection(const QString &s);
QString FieldCharacterDirectionToString(FieldCharacterDirection d);

enum FieldCharacterState
{
    FieldCharacterState_Standing = 0,
    FieldCharacterState_Walking = 1,
    FieldCharacterState_Running = 2,
    FieldCharacterState_None = -1,
};

FieldCharacterState StringToFieldCharacterState(const QString &s);

enum TransitionDirection
{
    TransitionDirection_North,
    TransitionDirection_NorthEast,
    TransitionDirection_East,
    TransitionDirection_SouthEast,
    TransitionDirection_South,
    TransitionDirection_SouthWest,
    TransitionDirection_West,
    TransitionDirection_NorthWest,
    TransitionDirection_Door,
};

TransitionDirection StringToTransitionDirection(const QString &s);

enum UnlockConditionType
{
    UnlockConditionType_None,
    UnlockConditionType_FlagSet,
    UnlockConditionType_PartnerPresent,
};

}

#endif // ENUMS_STAGING_H
