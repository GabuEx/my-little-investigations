#include "Enums.Staging.h"

#include "MLIException.h"

Staging::CharacterDirection Staging::StringToCharacterDirection(const QString &s)
{
    if (s == "Left")
    {
        return Staging::CharacterDirection_Left;
    }
    else if (s == "Right")
    {
        return Staging::CharacterDirection_Right;
    }
    else
    {
        return Staging::CharacterDirection_None;
    }
}

QString Staging::CharacterDirectionToString(Staging::CharacterDirection d)
{
    if (d == Staging::CharacterDirection_Left)
    {
        return "Left";
    }
    else if (d == Staging::CharacterDirection_Right)
    {
        return "Right";
    }
    else
    {
        return "None";
    }
}

Staging::CharacterPosition Staging::StringToCharacterPosition(const QString &s)
{
    if (s == "Left")
    {
        return Staging::CharacterPosition_Left;
    }
    else if (s == "Right")
    {
        return Staging::CharacterPosition_Right;
    }
    else if (s == "Unknown")
    {
        return Staging::CharacterPosition_Unknown;
    }
    else if (s == "Offscreen")
    {
        return Staging::CharacterPosition_Offscreen;
    }
    else
    {
        return Staging::CharacterPosition_None;
    }
}

QString Staging::ConversationActionTypeToString(Staging::ConversationActionType t)
{
    switch (t)
    {
    case Staging::ConversationActionType_None:
        return "None";

    case Staging::ConversationActionType_MultipleCharacterChange:
        return "MultipleCharacterChange";

    case Staging::ConversationActionType_CharacterChange:
        return "CharacterChange";

    case Staging::ConversationActionType_SetFlag:
        return "SetFlag";

    case Staging::ConversationActionType_BranchOnCondition:
        return "BranchOnCondition";

    case Staging::ConversationActionType_BranchIfTrue:
        return "BranchIfTrue";

    case Staging::ConversationActionType_BranchIfFalse:
        return "BranchIfFalse";

    case Staging::ConversationActionType_EndBranchOnCondition:
        return "EndBranchOnCondition";

    case Staging::ConversationActionType_ShowDialog:
        return "ShowDialog";

    case Staging::ConversationActionType_ShowDialogAutomatic:
        return "ShowDialogAutomatic";

    case Staging::ConversationActionType_MustPresentEvidence:
        return "MustPresentEvidence";

    case Staging::ConversationActionType_SkipWrongEvidencePresented:
        return "SkipWrongEvidencePresented";

    case Staging::ConversationActionType_BeginWrongEvidencePresented:
        return "BeginWrongEvidencePresented";

    case Staging::ConversationActionType_EndWrongEvidencePresented:
        return "EndWrongEvidencePresented";

    case Staging::ConversationActionType_SkipEndRequested:
        return "SkipEndRequested";

    case Staging::ConversationActionType_BeginEndRequested:
        return "BeginEndRequested";

    case Staging::ConversationActionType_EndEndRequested:
        return "EndEndRequested";

    case Staging::ConversationActionType_EndMustPresentEvidence:
        return "EndMustPresentEvidence";

    case Staging::ConversationActionType_EnableConversation:
        return "EnableConversation";

    case Staging::ConversationActionType_EnableEvidence:
        return "EnableEvidence";

    case Staging::ConversationActionType_UpdateEvidence:
        return "UpdateEvidence";

    case Staging::ConversationActionType_DisableEvidence:
        return "DisableEvidence";

    case Staging::ConversationActionType_EnableCutscene:
        return "EnableCutscene";

    case Staging::ConversationActionType_Notification:
        return "Notification";

    case Staging::ConversationActionType_PlayBgm:
        return "PlayBgm";

    case Staging::ConversationActionType_PauseBgm:
        return "PauseBgm";

    case Staging::ConversationActionType_ResumeBgm:
        return "ResumeBgm";

    case Staging::ConversationActionType_StopBgm:
        return "StopBgm";

    case Staging::ConversationActionType_PlayAmbiance:
        return "PlayAmbiance";

    case Staging::ConversationActionType_PauseAmbiance:
        return "PauseAmbiance";

    case Staging::ConversationActionType_ResumeAmbiance:
        return "ResumeAmbiance";

    case Staging::ConversationActionType_StopAmbiance:
        return "StopAmbiance";

    case Staging::ConversationActionType_StartAnimation:
        return "StartAnimation";

    case Staging::ConversationActionType_StopAnimation:
        return "StopAnimation";

    case Staging::ConversationActionType_SetPartner:
        return "SetPartner";

    case Staging::ConversationActionType_GoToPresentWrongEvidence:
        return "GoToPresentWrongEvidence";

    case Staging::ConversationActionType_LockConversation:
        return "LockConversation";

    case Staging::ConversationActionType_ExitEncounter:
        return "ExitEncounter";

    case Staging::ConversationActionType_MoveToLocation:
        return "MoveToLocation";

    case Staging::ConversationActionType_MoveToZoomedView:
        return "MoveToZoomedView";

    case Staging::ConversationActionType_EndCase:
        return "EndCase";

    case Staging::ConversationActionType_BeginMultipleChoice:
        return "BeginMultipleChoice";

    case Staging::ConversationActionType_BeginMultipleChoiceOption:
        return "BeginMultipleChoiceOption";

    case Staging::ConversationActionType_ExitMultipleChoice:
        return "ExitMultipleChoice";

    case Staging::ConversationActionType_EndMultipleChoiceOption:
        return "EndMultipleChoiceOption";

    case Staging::ConversationActionType_EndMultipleChoice:
        return "EndMultipleChoice";

    case Staging::ConversationActionType_EnableFastForward:
        return "EnableFastForward";

    case Staging::ConversationActionType_DisableFastForward:
        return "DisableFastForward";

    case Staging::ConversationActionType_BeginBreakdown:
        return "BeginBreakdown";

    case Staging::ConversationActionType_EndBreakdown:
        return "EndBreakdown";

    case Staging::ConversationActionType_BeginInterrogationRepeat:
        return "BeginInterrogationRepeat";

    case Staging::ConversationActionType_ShowInterrogation:
        return "ShowInterrogation";

    case Staging::ConversationActionType_BeginPressForInfo:
        return "BeginPressForInfo";

    case Staging::ConversationActionType_EndPressForInfo:
        return "EndPressForInfo";

    case Staging::ConversationActionType_BeginPresentEvidence:
        return "BeginPresentEvidence";

    case Staging::ConversationActionType_EndPresentEvidence:
        return "EndPresentEvidence";

    case Staging::ConversationActionType_BeginUsePartner:
        return "BeginUsePartner";

    case Staging::ConversationActionType_EndUsePartner:
        return "EndUsePartner";

    case Staging::ConversationActionType_EndShowInterrogationBranches:
        return "EndShowInterrogationBranches";

    case Staging::ConversationActionType_ExitInterrogationRepeat:
        return "ExitInterrogationRepeat";

    case Staging::ConversationActionType_SkipWrongPartnerUsed:
        return "SkipWrongPartnerUsed";

    case Staging::ConversationActionType_BeginWrongPartnerUsed:
        return "BeginWrongPartnerUsed";

    case Staging::ConversationActionType_EndWrongPartnerUsed:
        return "EndWrongPartnerUsed";

    case Staging::ConversationActionType_EndInterrogationRepeat:
        return "EndInterrogationRepeat";

    case Staging::ConversationActionType_SetParticipants:
        return "SetParticipants";

    case Staging::ConversationActionType_SetIconOffset:
        return "SetIconOffset";

    case Staging::ConversationActionType_SetHealth:
        return "SetHealth";

    case Staging::ConversationActionType_InitializeBeginConfrontationTopicSelection:
        return "InitializeBeginConfrontationTopicSelection";

    case Staging::ConversationActionType_BeginConfrontationTopicSelection:
        return "BeginConfrontationTopicSelection";

    case Staging::ConversationActionType_BeginConfrontationTopic:
        return "BeginConfrontationTopic";

    case Staging::ConversationActionType_EndConfrontationTopic:
        return "EndConfrontationTopic";

    case Staging::ConversationActionType_SkipPlayerDefeated:
        return "SkipPlayerDefeated";

    case Staging::ConversationActionType_BeginPlayerDefeated:
        return "BeginPlayerDefeated";

    case Staging::ConversationActionType_EndPlayerDefeated:
        return "EndPlayerDefeated";

    case Staging::ConversationActionType_EndConfrontationTopicSelection:
        return "EndConfrontationTopicSelection";

    case Staging::ConversationActionType_EnableTopic:
        return "EnableTopic";

    case Staging::ConversationActionType_RestartConfrontation:
        return "RestartConfrontation";

    default:
        throw new MLIException("Unknown conversation action type.");
    }
}

Staging::FieldCharacterDirection Staging::StringToFieldCharacterDirection(const QString &s)
{
    if (s == "Up")
    {
        return Staging::FieldCharacterDirection_Up;
    }
    else if (s == "DiagonalUp")
    {
        return Staging::FieldCharacterDirection_DiagonalUp;
    }
    else if (s == "Side")
    {
        return Staging::FieldCharacterDirection_Side;
    }
    else if (s == "DiagonalDown")
    {
        return Staging::FieldCharacterDirection_DiagonalDown;
    }
    else if (s == "Down")
    {
        return Staging::FieldCharacterDirection_Down;
    }
    else
    {
        return Staging::FieldCharacterDirection_None;
    }
}

QString Staging::FieldCharacterDirectionToString(Staging::FieldCharacterDirection d)
{
    if (d == Staging::FieldCharacterDirection_Up)
    {
        return "Up";
    }
    else if (d == Staging::FieldCharacterDirection_DiagonalUp)
    {
        return "DiagonalUp";
    }
    else if (d == Staging::FieldCharacterDirection_Side)
    {
        return "Side";
    }
    else if (d == Staging::FieldCharacterDirection_DiagonalDown)
    {
        return "DiagonalDown";
    }
    else if (d == Staging::FieldCharacterDirection_Down)
    {
        return "Down";
    }
    else
    {
        return "None";
    }
}

Staging::FieldCharacterState Staging::StringToFieldCharacterState(const QString &s)
{
    if (s == "Standing")
    {
        return Staging::FieldCharacterState_Standing;
    }
    else if (s == "Walking")
    {
        return Staging::FieldCharacterState_Walking;
    }
    else if (s == "Running")
    {
        return Staging::FieldCharacterState_Running;
    }
    else
    {
        return Staging::FieldCharacterState_None;
    }
}

Staging::TransitionDirection Staging::StringToTransitionDirection(const QString &s)
{
    if (s == "North")
    {
        return Staging::TransitionDirection_North;
    }
    else if (s == "NorthEast")
    {
        return Staging::TransitionDirection_NorthEast;
    }
    else if (s == "East")
    {
        return Staging::TransitionDirection_East;
    }
    else if (s == "SouthEast")
    {
        return Staging::TransitionDirection_SouthEast;
    }
    else if (s == "South")
    {
        return Staging::TransitionDirection_South;
    }
    else if (s == "SouthWest")
    {
        return Staging::TransitionDirection_SouthWest;
    }
    else if (s == "West")
    {
        return Staging::TransitionDirection_West;
    }
    else if (s == "NorthWest")
    {
        return Staging::TransitionDirection_NorthWest;
    }
    else if (s == "Door")
    {
        return Staging::TransitionDirection_Door;
    }

    return Staging::TransitionDirection_North;
}
