#include "Conversation.Staging.h"

#include "Condition.Staging.h"
#include "Flag.Staging.h"
#include "CaseContentLoadingStager.h"

Staging::Conversation::UnlockCondition * Staging::Conversation::UnlockCondition::LoadFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("FlagSetUnlockCondition"))
    {
        return new Staging::Conversation::FlagSetUnlockCondition(pReader);
    }
    else if (pReader->ElementExists("PartnerPresentUnlockCondition"))
    {
        return new Staging::Conversation::PartnerPresentUnlockCondition(pReader);
    }
    else
    {
        throw MLIException("Unknown unlock condition type.");
    }
}

Staging::Conversation::FlagSetUnlockCondition::FlagSetUnlockCondition(XmlReader *pReader)
{
    pReader->StartElement("FlagSetUnlockCondition");
    FlagId = pReader->ReadTextElement("FlagId");

    if (!CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.contains(FlagId))
    {
        CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.insert(FlagId, new Flag(FlagId));
    }

    pReader->EndElement();
}

Staging::Conversation::PartnerPresentUnlockCondition::PartnerPresentUnlockCondition(XmlReader *pReader)
{
    pReader->StartElement("PartnerPresentUnlockCondition");
    PartnerId = pReader->ReadTextElement("PartnerId");
    pReader->EndElement();
}

Staging::Conversation::Conversation(XmlReader *pReader)
{
    pReader->StartElement("Conversation");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Staging::Conversation::~Conversation()
{
    for (Action *pAction : ActionList)
    {
        delete pAction;
    }

    ActionList.clear();
}

Staging::Conversation * Staging::Conversation::LoadFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("Conversation"))
    {
        return new Staging::Conversation(pReader);
    }
    else if (pReader->ElementExists("Interrogation"))
    {
        return new Staging::Interrogation(pReader);
    }
    else if (pReader->ElementExists("Confrontation"))
    {
        return new Staging::Confrontation(pReader);
    }
    else
    {
        throw MLIException("Invalid conversation type.");
    }
}

void Staging::Conversation::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    for (Action *pAction : ActionList)
    {
        pAction->AddDialogPaths(dialogIdToSavePathMap);
    }
}

void Staging::Conversation::LoadFromXmlCore(XmlReader *pReader)
{
    if (pReader->ElementExists("Id"))
    {
        Id = pReader->ReadTextElement("Id");
    }

    if (pReader->ElementExists("Name"))
    {
        Name = pReader->ReadTextElement("Name");
    }

    IsEnabled = pReader->ReadBooleanElement("IsEnabled");
    HasBeenCompleted = pReader->ReadBooleanElement("HasBeenCompleted");

    pReader->StartElement("ActionList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ActionList.push_back(GetActionForNextElement(pReader));
    }

    pReader->EndElement();
}

Staging::Conversation::Action * Staging::Conversation::GetActionForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("MultipleCharacterChangeAction"))
    {
        return new MultipleCharacterChangeAction(pReader);
    }
    else if (pReader->ElementExists("CharacterChangeAction"))
    {
        return new CharacterChangeAction(pReader);
    }
    else if (pReader->ElementExists("SetFlagAction"))
    {
        return new SetFlagAction(pReader);
    }
    else if (pReader->ElementExists("BranchOnConditionAction"))
    {
        return new BranchOnConditionAction(pReader);
    }
    else if (pReader->ElementExists("BranchIfTrueAction"))
    {
        return new BranchIfTrueAction(pReader);
    }
    else if (pReader->ElementExists("BranchIfFalseAction"))
    {
        return new BranchIfFalseAction(pReader);
    }
    else if (pReader->ElementExists("EndBranchOnConditionAction"))
    {
        return new EndBranchOnConditionAction(pReader);
    }
    else if (pReader->ElementExists("ShowDialogAction"))
    {
        return new ShowDialogAction(pReader);
    }
    else if (pReader->ElementExists("ShowDialogAutomaticAction"))
    {
        return new ShowDialogAutomaticAction(pReader);
    }
    else if (pReader->ElementExists("MustPresentEvidenceAction"))
    {
        return new MustPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("SkipWrongEvidencePresentedAction"))
    {
        return new SkipWrongEvidencePresentedAction(pReader);
    }
    else if (pReader->ElementExists("BeginWrongEvidencePresentedAction"))
    {
        return new BeginWrongEvidencePresentedAction(pReader);
    }
    else if (pReader->ElementExists("EndWrongEvidencePresentedAction"))
    {
        return new EndWrongEvidencePresentedAction(pReader);
    }
    else if (pReader->ElementExists("SkipEndRequestedAction"))
    {
        return new SkipEndRequestedAction(pReader);
    }
    else if (pReader->ElementExists("BeginEndRequestedAction"))
    {
        return new BeginEndRequestedAction(pReader);
    }
    else if (pReader->ElementExists("EndEndRequestedAction"))
    {
        return new EndEndRequestedAction(pReader);
    }
    else if (pReader->ElementExists("EndMustPresentEvidenceAction"))
    {
        return new EndMustPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("EnableConversationAction"))
    {
        return new EnableConversationAction(pReader);
    }
    else if (pReader->ElementExists("EnableEvidenceAction"))
    {
        return new EnableEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("UpdateEvidenceAction"))
    {
        return new UpdateEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("DisableEvidenceAction"))
    {
        return new DisableEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("EnableCutsceneAction"))
    {
        return new EnableCutsceneAction(pReader);
    }
    else if (pReader->ElementExists("NotificationAction"))
    {
        return new NotificationAction(pReader);
    }
    else if (pReader->ElementExists("PlayBgmAction"))
    {
        return new PlayBgmAction(pReader);
    }
    else if (pReader->ElementExists("PauseBgmAction"))
    {
        return new PauseBgmAction(pReader);
    }
    else if (pReader->ElementExists("ResumeBgmAction"))
    {
        return new ResumeBgmAction(pReader);
    }
    else if (pReader->ElementExists("StopBgmAction"))
    {
        return new StopBgmAction(pReader);
    }
    else if (pReader->ElementExists("PlayAmbianceAction"))
    {
        return new PlayAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("PauseAmbianceAction"))
    {
        return new PauseAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("ResumeAmbianceAction"))
    {
        return new ResumeAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("StopAmbianceAction"))
    {
        return new StopAmbianceAction(pReader);
    }
    else if (pReader->ElementExists("StartAnimationAction"))
    {
        return new StartAnimationAction(pReader);
    }
    else if (pReader->ElementExists("StopAnimationAction"))
    {
        return new StopAnimationAction(pReader);
    }
    else if (pReader->ElementExists("SetPartnerAction"))
    {
        return new SetPartnerAction(pReader);
    }
    else if (pReader->ElementExists("GoToPresentWrongEvidenceAction"))
    {
        return new GoToPresentWrongEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("LockConversationAction"))
    {
        return new LockConversationAction(pReader);
    }
    else if (pReader->ElementExists("ExitEncounterAction"))
    {
        return new ExitEncounterAction(pReader);
    }
    else if (pReader->ElementExists("MoveToLocationAction"))
    {
        return new MoveToLocationAction(pReader);
    }
    else if (pReader->ElementExists("MoveToZoomedViewAction"))
    {
        return new MoveToZoomedViewAction(pReader);
    }
    else if (pReader->ElementExists("EndCaseAction"))
    {
        return new EndCaseAction(pReader);
    }
    else if (pReader->ElementExists("BeginMultipleChoiceAction"))
    {
        return new BeginMultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists("BeginMultipleChoiceOptionAction"))
    {
        return new BeginMultipleChoiceOptionAction(pReader);
    }
    else if (pReader->ElementExists("ExitMultipleChoiceAction"))
    {
        return new ExitMultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists("EndMultipleChoiceOptionAction"))
    {
        return new EndMultipleChoiceOptionAction(pReader);
    }
    else if (pReader->ElementExists("EndMultipleChoiceAction"))
    {
        return new EndMultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists("EnableFastForwardAction"))
    {
        return new EnableFastForwardAction(pReader);
    }
    else if (pReader->ElementExists("DisableFastForwardAction"))
    {
        return new DisableFastForwardAction(pReader);
    }
    else if (pReader->ElementExists("BeginBreakdownAction"))
    {
        return new BeginBreakdownAction(pReader);
    }
    else if (pReader->ElementExists("EndBreakdownAction"))
    {
        return new EndBreakdownAction(pReader);
    }
    else
    {
        throw MLIException("Unknown action type.");
    }
}

Staging::Conversation::MultipleCharacterChangeAction::MultipleCharacterChangeAction(XmlReader *pReader)
{
    pReader->StartElement("MultipleCharacterChangeAction");

    pReader->StartElement("CharacterChangeList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        CharacterChangeList.push_back(new CharacterChangeAction(pReader));
    }

    pReader->EndElement();

    pReader->EndElement();
}

Staging::Conversation::MultipleCharacterChangeAction::~MultipleCharacterChangeAction()
{
    for (CharacterChangeAction *pCharacterChangeAction : CharacterChangeList)
    {
        delete pCharacterChangeAction;
    }

    CharacterChangeList.clear();
}

Staging::Conversation::CharacterChangeAction::CharacterChangeAction(XmlReader *pReader)
{
    pReader->StartElement("CharacterChangeAction");
    Position = StringToCharacterPosition(pReader->ReadTextElement("Position"));
    CharacterIdToChangeTo = pReader->ReadTextElement("CharacterIdToChangeTo");

    if (CharacterIdToChangeTo == "None")
    {
        CharacterIdToChangeTo = "";
    }

    InitialEmotionId = pReader->ReadTextElement("InitialEmotionId");

    if (pReader->ElementExists("SfxId"))
    {
        SfxId = pReader->ReadTextElement("SfxId");
    }

    pReader->EndElement();
}

Staging::Conversation::SetFlagAction::SetFlagAction(XmlReader *pReader)
{
    pReader->StartElement("SetFlagAction");
    FlagId = pReader->ReadTextElement("FlagId");

    if (!CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.contains(FlagId))
    {
        CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.insert(FlagId, new Flag(FlagId));
    }

    pReader->EndElement();
}

Staging::Conversation::BranchOnConditionAction::BranchOnConditionAction(XmlReader *pReader)
{
    pReader->StartElement("BranchOnConditionAction");

    pReader->StartElement("Condition");
    pCondition = new Condition(pReader);
    pReader->EndElement();

    TrueIndex = pReader->ReadIntElement("TrueIndex");
    FalseIndex = pReader->ReadIntElement("FalseIndex");
    pReader->EndElement();
}

Staging::Conversation::BranchOnConditionAction::~BranchOnConditionAction()
{
    delete pCondition;
    pCondition = NULL;
}

Staging::Conversation::BranchIfTrueAction::BranchIfTrueAction(XmlReader *pReader)
{
    pReader->StartElement("BranchIfTrueAction");
    EndIndex = pReader->ReadIntElement("EndIndex");
    pReader->EndElement();
}

Staging::Conversation::BranchIfFalseAction::BranchIfFalseAction(XmlReader *pReader)
{
    pReader->StartElement("BranchIfFalseAction");
    EndIndex = pReader->ReadIntElement("EndIndex");
    pReader->EndElement();
}

Staging::Conversation::EndBranchOnConditionAction::EndBranchOnConditionAction(XmlReader *pReader)
{
    pReader->StartElement("EndBranchOnConditionAction");
    pReader->EndElement();
}

void Staging::Conversation::ShowDialogAction::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    if (FilePath.length() > 0)
    {
        dialogIdToSavePathMap[Id] = FilePath;
    }
}

void Staging::Conversation::ShowDialogAction::LoadFromXmlCore(XmlReader *pReader)
{
    Id = pReader->ReadTextElement("Id");

    SpeakerPosition = StringToCharacterPosition(pReader->ReadTextElement("SpeakerPosition"));
    RawDialog = pReader->ReadTextElement("RawDialog");

    if (pReader->ElementExists("FilePath"))
    {
        FilePath = pReader->ReadTextElement("FilePath");
    }

    LeadInTime = pReader->ReadIntElement("LeadInTime");

    if (pReader->ElementExists("CharacterId"))
    {
        CharacterId = pReader->ReadTextElement("CharacterId");
    }

    IsInConfrontation = pReader->ReadBooleanElement("IsInConfrontation");
}

Staging::Conversation::ShowDialogAction::ShowDialogAction(XmlReader *pReader)
{
    pReader->StartElement("ShowDialogAction");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Staging::Conversation::ShowDialogAutomaticAction::ShowDialogAutomaticAction(XmlReader *pReader)
{
    pReader->StartElement("ShowDialogAutomaticAction");
    LoadFromXmlCore(pReader);
    DelayBeforeContinuing = pReader->ReadIntElement("DelayBeforeContinuing");
    pReader->EndElement();
}

void Staging::Conversation::MustPresentEvidenceAction::LoadFromXmlCore(XmlReader *pReader)
{
    ShowDialogAction::LoadFromXmlCore(pReader);

    pReader->StartElement("CorrectEvidenceIdList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        CorrectEvidenceIdList.push_back(pReader->ReadTextElement("CorrectEvidenceId"));
    }

    pReader->EndElement();

    CorrectEvidencePresentedIndex = pReader->ReadIntElement("CorrectEvidencePresentedIndex");
    WrongEvidencePresentedIndex = pReader->ReadIntElement("WrongEvidencePresentedIndex");
    EndRequestedIndex = pReader->ReadIntElement("EndRequestedIndex");
}

Staging::Conversation::MustPresentEvidenceAction::MustPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("MustPresentEvidenceAction");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Staging::Conversation::SkipWrongEvidencePresentedAction::SkipWrongEvidencePresentedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipWrongEvidencePresentedAction");
    AfterWrongEvidencePresentedIndex = pReader->ReadIntElement("AfterWrongEvidencePresentedIndex");
    pReader->EndElement();
}

Staging::Conversation::BeginWrongEvidencePresentedAction::BeginWrongEvidencePresentedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginWrongEvidencePresentedAction");
    pReader->EndElement();
}

Staging::Conversation::EndWrongEvidencePresentedAction::EndWrongEvidencePresentedAction(XmlReader *pReader)
{
    pReader->StartElement("EndWrongEvidencePresentedAction");
    pReader->EndElement();
}

Staging::Conversation::SkipEndRequestedAction::SkipEndRequestedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipEndRequestedAction");
    AfterEndRequestedIndex = pReader->ReadIntElement("AfterEndRequestedIndex");
    pReader->EndElement();
}

Staging::Conversation::BeginEndRequestedAction::BeginEndRequestedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginEndRequestedAction");
    pReader->EndElement();
}

Staging::Conversation::EndEndRequestedAction::EndEndRequestedAction(XmlReader *pReader)
{
    pReader->StartElement("EndEndRequestedAction");
    pReader->EndElement();
}

Staging::Conversation::EndMustPresentEvidenceAction::EndMustPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("EndMustPresentEvidenceAction");
    pReader->EndElement();
}

Staging::Conversation::EnableConversationAction::EnableConversationAction(XmlReader *pReader)
{
    pReader->StartElement("EnableConversationAction");
    ConversationId = pReader->ReadTextElement("ConversationId");
    pReader->EndElement();
}

Staging::Conversation::EnableEvidenceAction::EnableEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("EnableEvidenceAction");
    EvidenceId = pReader->ReadTextElement("EvidenceId");
    pReader->EndElement();
}

Staging::Conversation::UpdateEvidenceAction::UpdateEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("UpdateEvidenceAction");
    EvidenceId = pReader->ReadTextElement("EvidenceId");
    NewEvidenceId = pReader->ReadTextElement("NewEvidenceId");
    pReader->EndElement();
}

Staging::Conversation::DisableEvidenceAction::DisableEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("DisableEvidenceAction");
    EvidenceId = pReader->ReadTextElement("EvidenceId");
    pReader->EndElement();
}

Staging::Conversation::EnableCutsceneAction::EnableCutsceneAction(XmlReader *pReader)
{
    pReader->StartElement("EnableCutsceneAction");
    CutsceneId = pReader->ReadTextElement("CutsceneId");
    pReader->EndElement();
}

Staging::Conversation::NotificationAction::NotificationAction(XmlReader *pReader)
{
    pReader->StartElement("NotificationAction");
    RawNotificationText = pReader->ReadTextElement("RawNotificationText");

    if (pReader->ElementExists("PartnerId"))
    {
        PartnerId = pReader->ReadTextElement("PartnerId");
    }

    if (pReader->ElementExists("OldEvidenceId"))
    {
        OldEvidenceId = pReader->ReadTextElement("OldEvidenceId");
    }

    if (pReader->ElementExists("NewEvidenceId"))
    {
        NewEvidenceId = pReader->ReadTextElement("NewEvidenceId");
    }

    pReader->EndElement();
}

Staging::Conversation::PlayBgmAction::PlayBgmAction(XmlReader *pReader)
{
    pReader->StartElement("PlayBgmAction");
    BgmId = pReader->ReadTextElement("BgmId");
    PreserveBgm = pReader->ReadBooleanElement("PreserveBgm");
    pReader->EndElement();
}

Staging::Conversation::PauseBgmAction::PauseBgmAction(XmlReader *pReader)
{
    pReader->StartElement("PauseBgmAction");
    pReader->EndElement();
}

Staging::Conversation::ResumeBgmAction::ResumeBgmAction(XmlReader *pReader)
{
    pReader->StartElement("ResumeBgmAction");
    pReader->EndElement();
}

Staging::Conversation::StopBgmAction::StopBgmAction(XmlReader *pReader)
{
    pReader->StartElement("StopBgmAction");
    IsInstant = pReader->ReadBooleanElement("IsInstant");
    PreserveBgm = pReader->ReadBooleanElement("PreserveBgm");
    pReader->EndElement();
}

Staging::Conversation::PlayAmbianceAction::PlayAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("PlayAmbianceAction");
    AmbianceSfxId = pReader->ReadTextElement("AmbianceSfxId");
    PreserveAmbiance = pReader->ReadBooleanElement("PreserveAmbiance");
    pReader->EndElement();
}

Staging::Conversation::PauseAmbianceAction::PauseAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("PauseAmbianceAction");
    pReader->EndElement();
}

Staging::Conversation::ResumeAmbianceAction::ResumeAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("ResumeAmbianceAction");
    pReader->EndElement();
}

Staging::Conversation::StopAmbianceAction::StopAmbianceAction(XmlReader *pReader)
{
    pReader->StartElement("StopAmbianceAction");
    IsInstant = pReader->ReadBooleanElement("IsInstant");
    PreserveAmbiance = pReader->ReadBooleanElement("PreserveAmbiance");
    pReader->EndElement();
}

Staging::Conversation::StartAnimationAction::StartAnimationAction(XmlReader *pReader)
{
    pReader->StartElement("StartAnimationAction");
    AnimationId = pReader->ReadTextElement("AnimationId");
    pReader->EndElement();
}

Staging::Conversation::StopAnimationAction::StopAnimationAction(XmlReader *pReader)
{
    pReader->StartElement("StopAnimationAction");
    pReader->EndElement();
}

Staging::Conversation::SetPartnerAction::SetPartnerAction(XmlReader *pReader)
{
    pReader->StartElement("SetPartnerAction");
    PartnerId = pReader->ReadTextElement("PartnerId");
    pReader->EndElement();
}

Staging::Conversation::GoToPresentWrongEvidenceAction::GoToPresentWrongEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("GoToPresentWrongEvidenceAction");
    pReader->EndElement();
}

Staging::Conversation::LockConversationAction::LockConversationAction(XmlReader *pReader)
{
    pReader->StartElement("LockConversationAction");

    pReader->StartElement("UnlockCondition");
    pUnlockCondition = Conversation::UnlockCondition::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Staging::Conversation::LockConversationAction::~LockConversationAction()
{
    delete pUnlockCondition;
    pUnlockCondition = NULL;
}

Staging::Conversation::ExitEncounterAction::ExitEncounterAction(XmlReader *pReader)
{
    pReader->StartElement("ExitEncounterAction");
    pReader->EndElement();
}

Staging::Conversation::MoveToLocationAction::MoveToLocationAction(XmlReader *pReader)
{
    pReader->StartElement("MoveToLocationAction");
    NewAreaId = pReader->ReadTextElement("NewAreaId");
    NewLocationId = pReader->ReadTextElement("NewLocationId");
    TransitionId = pReader->ReadTextElement("TransitionId");
    pReader->EndElement();
}

Staging::Conversation::MoveToZoomedViewAction::MoveToZoomedViewAction(XmlReader *pReader)
{
    pReader->StartElement("MoveToZoomedViewAction");
    ZoomedViewId = pReader->ReadTextElement("ZoomedViewId");
    pReader->EndElement();
}

Staging::Conversation::EndCaseAction::EndCaseAction(XmlReader *pReader)
{
    pReader->StartElement("EndCaseAction");
    CompletesCase = pReader->ReadBooleanElement("CompletesCase");
    pReader->EndElement();
}

Staging::Conversation::BeginMultipleChoiceAction::BeginMultipleChoiceAction(XmlReader *pReader)
{
    pReader->StartElement("BeginMultipleChoiceAction");

    pReader->StartElement("OptionTexts");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        OptionTexts.push_back(pReader->ReadTextElement("Text"));
    }

    pReader->EndElement();

    pReader->StartElement("OptionIndexes");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        OptionIndexes.push_back(pReader->ReadIntElement("Index"));
    }

    pReader->EndElement();

    pReader->EndElement();

    if (OptionTexts.size() != OptionIndexes.size())
    {
        throw MLIException("OptionTexts and OptionIndexes must be the same size.");
    }
}

Staging::Conversation::BeginMultipleChoiceOptionAction::BeginMultipleChoiceOptionAction(XmlReader *pReader)
{
    pReader->StartElement("BeginMultipleChoiceOptionAction");
    pReader->EndElement();
}

Staging::Conversation::ExitMultipleChoiceAction::ExitMultipleChoiceAction(XmlReader *pReader)
{
    pReader->StartElement("ExitMultipleChoiceAction");
    ExitIndex = pReader->ReadIntElement("ExitIndex");
    pReader->EndElement();
}

Staging::Conversation::EndMultipleChoiceOptionAction::EndMultipleChoiceOptionAction(XmlReader *pReader)
{
    pReader->StartElement("EndMultipleChoiceOptionAction");
    StartIndex = pReader->ReadIntElement("StartIndex");
    pReader->EndElement();
}

Staging::Conversation::EndMultipleChoiceAction::EndMultipleChoiceAction(XmlReader *pReader)
{
    pReader->StartElement("EndMultipleChoiceAction");
    pReader->EndElement();
}

Staging::Conversation::EnableFastForwardAction::EnableFastForwardAction(XmlReader *pReader)
{
    pReader->StartElement("EnableFastForwardAction");
    pReader->EndElement();
}

Staging::Conversation::DisableFastForwardAction::DisableFastForwardAction(XmlReader *pReader)
{
    pReader->StartElement("DisableFastForwardAction");
    pReader->EndElement();
}

Staging::Conversation::BeginBreakdownAction::BeginBreakdownAction(XmlReader *pReader)
{
    pReader->StartElement("BeginBreakdownAction");
    CharacterPositionValue = StringToCharacterPosition(pReader->ReadTextElement("CharacterPosition"));
    pReader->EndElement();
}

Staging::Conversation::EndBreakdownAction::EndBreakdownAction(XmlReader *pReader)
{
    pReader->StartElement("EndBreakdownAction");
    pReader->EndElement();
}

Staging::Interrogation::Interrogation(XmlReader *pReader)
{
    pReader->StartElement("Interrogation");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Staging::Conversation::Action * Staging::Interrogation::GetActionForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("BeginInterrogationRepeatAction"))
    {
        return new BeginInterrogationRepeatAction(pReader);
    }
    else if (pReader->ElementExists("ShowInterrogationAction"))
    {
        return new ShowInterrogationAction(pReader);
    }
    else if (pReader->ElementExists("BeginPressForInfoAction"))
    {
        return new BeginPressForInfoAction(pReader);
    }
    else if (pReader->ElementExists("EndPressForInfoAction"))
    {
        return new EndPressForInfoAction(pReader);
    }
    else if (pReader->ElementExists("BeginPresentEvidenceAction"))
    {
        return new BeginPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("EndPresentEvidenceAction"))
    {
        return new EndPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists("BeginUsePartnerAction"))
    {
        return new BeginUsePartnerAction(pReader);
    }
    else if (pReader->ElementExists("EndUsePartnerAction"))
    {
        return new EndUsePartnerAction(pReader);
    }
    else if (pReader->ElementExists("EndShowInterrogationBranchesAction"))
    {
        return new EndShowInterrogationBranchesAction(pReader);
    }
    else if (pReader->ElementExists("ExitInterrogationRepeatAction"))
    {
        return new ExitInterrogationRepeatAction(pReader);
    }
    else if (pReader->ElementExists("SkipWrongPartnerUsedAction"))
    {
        return new SkipWrongPartnerUsedAction(pReader);
    }
    else if (pReader->ElementExists("BeginWrongPartnerUsedAction"))
    {
        return new BeginWrongPartnerUsedAction(pReader);
    }
    else if (pReader->ElementExists("EndWrongPartnerUsedAction"))
    {
        return new EndWrongPartnerUsedAction(pReader);
    }
    else if (pReader->ElementExists("EndInterrogationRepeatAction"))
    {
        return new EndInterrogationRepeatAction(pReader);
    }
    else
    {
        return Conversation::GetActionForNextElement(pReader);
    }
}

Staging::Interrogation::BeginInterrogationRepeatAction::BeginInterrogationRepeatAction(XmlReader *pReader)
{
    pReader->StartElement("BeginInterrogationRepeatAction");
    pReader->EndElement();
}

void Staging::Interrogation::ShowInterrogationAction::LoadFromXmlCore(XmlReader *pReader)
{
    Conversation::ShowDialogAction::LoadFromXmlCore(pReader);
    pReader->StartElement("EvidencePresentedIndexes");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString evidenceId = pReader->ReadTextElement("EvidenceId");
        int index = pReader->ReadIntElement("Index");
        EvidencePresentedIndexes[evidenceId] = index;
    }

    pReader->EndElement();

    pReader->StartElement("PartnerUsedIndexes");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString partnerId = pReader->ReadTextElement("PartnerId");
        int index = pReader->ReadIntElement("Index");
        PartnerUsedIndexes[partnerId] = index;
    }

    pReader->EndElement();

    if (pReader->ElementExists("ConditionFlag"))
    {
        ConditionFlag = pReader->ReadTextElement("ConditionFlag");
    }

    PreviousInterrogationIndex = pReader->ReadIntElement("PreviousInterrogationIndex");
    NextInterrogationIndex = pReader->ReadIntElement("NextInterrogationIndex");
    InterrogationFinishIndex = pReader->ReadIntElement("InterrogationFinishIndex");
    PressForInfoIndex = pReader->ReadIntElement("PressForInfoIndex");
    WrongEvidencePresentedIndex = pReader->ReadIntElement("WrongEvidencePresentedIndex");
    WrongPartnerUsedIndex = pReader->ReadIntElement("WrongPartnerUsedIndex");
    EndRequestedIndex = pReader->ReadIntElement("EndRequestedIndex");
}

Staging::Interrogation::ShowInterrogationAction::ShowInterrogationAction(XmlReader *pReader)
{
    pReader->StartElement("ShowInterrogationAction");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

Staging::Interrogation::BeginPressForInfoAction::BeginPressForInfoAction(XmlReader *pReader)
{
    pReader->StartElement("BeginPressForInfoAction");
    pReader->EndElement();
}

Staging::Interrogation::EndPressForInfoAction::EndPressForInfoAction(XmlReader *pReader)
{
    pReader->StartElement("EndPressForInfoAction");
    NextDialogIndex = pReader->ReadIntElement("NextDialogIndex");
    pReader->EndElement();
}

Staging::Interrogation::BeginPresentEvidenceAction::BeginPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("BeginPresentEvidenceAction");
    pReader->EndElement();
}

Staging::Interrogation::EndPresentEvidenceAction::EndPresentEvidenceAction(XmlReader *pReader)
{
    pReader->StartElement("EndPresentEvidenceAction");
    NextDialogIndex = pReader->ReadIntElement("NextDialogIndex");
    pReader->EndElement();
}

Staging::Interrogation::BeginUsePartnerAction::BeginUsePartnerAction(XmlReader *pReader)
{
    pReader->StartElement("BeginUsePartnerAction");
    pReader->EndElement();
}

Staging::Interrogation::EndUsePartnerAction::EndUsePartnerAction(XmlReader *pReader)
{
    pReader->StartElement("EndUsePartnerAction");
    NextDialogIndex = pReader->ReadIntElement("NextDialogIndex");
    pReader->EndElement();
}

Staging::Interrogation::EndShowInterrogationBranchesAction::EndShowInterrogationBranchesAction(XmlReader *pReader)
{
    pReader->StartElement("EndShowInterrogationBranchesAction");
    pReader->EndElement();
}

Staging::Interrogation::ExitInterrogationRepeatAction::ExitInterrogationRepeatAction(XmlReader *pReader)
{
    pReader->StartElement("ExitInterrogationRepeatAction");
    AfterEndInterrogationRepeatIndex = pReader->ReadIntElement("AfterEndInterrogationRepeatIndex");
    pReader->EndElement();
}

Staging::Interrogation::SkipWrongPartnerUsedAction::SkipWrongPartnerUsedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipWrongPartnerUsedAction");
    AfterWrongPartnerUsedIndex = pReader->ReadIntElement("AfterWrongPartnerUsedIndex");
    pReader->EndElement();
}

Staging::Interrogation::BeginWrongPartnerUsedAction::BeginWrongPartnerUsedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginWrongPartnerUsedAction");
    pReader->EndElement();
}

Staging::Interrogation::EndWrongPartnerUsedAction::EndWrongPartnerUsedAction(XmlReader *pReader)
{
    pReader->StartElement("EndWrongPartnerUsedAction");
    pReader->EndElement();
}

Staging::Interrogation::EndInterrogationRepeatAction::EndInterrogationRepeatAction(XmlReader *pReader)
{
    pReader->StartElement("EndInterrogationRepeatAction");
    BeginInterrogationRepeatIndex = pReader->ReadIntElement("BeginInterrogationRepeatIndex");
    pReader->EndElement();
}

Staging::Confrontation::Topic::Topic(XmlReader *pReader)
{
    pReader->StartElement("ConfrontationTopic");

    if (pReader->ElementExists("Id"))
    {
        Id = pReader->ReadTextElement("Id");
    }

    if (pReader->ElementExists("Name"))
    {
        Name = pReader->ReadTextElement("Name");
    }

    ActionIndex = pReader->ReadIntElement("ActionIndex");
    IsEnabled = pReader->ReadBooleanElement("IsEnabled");

    pReader->EndElement();
}

Staging::Confrontation::Confrontation(XmlReader *pReader)
{
    pReader->StartElement("Confrontation");
    LoadFromXmlCore(pReader);
    pReader->EndElement();
}

void Staging::Confrontation::LoadFromXmlCore(XmlReader *pReader)
{
    Interrogation::LoadFromXmlCore(pReader);

    pReader->StartElement("ConfrontationTopicNamesById");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString id = pReader->ReadTextElement("Id");
        QString confrontationTopicName = pReader->ReadTextElement("ConfrontationTopicName");
        ConfrontationTopicNamesById[id] = confrontationTopicName;
    }

    pReader->EndElement();
}

Staging::Conversation::Action * Staging::Confrontation::GetActionForNextElement(XmlReader *pReader)
{
    if (pReader->ElementExists("SetParticipantsAction"))
    {
        return new SetParticipantsAction(pReader);
    }
    else if (pReader->ElementExists("SetIconOffsetAction"))
    {
        return new SetIconOffsetAction(pReader);
    }
    else if (pReader->ElementExists("SetHealthAction"))
    {
        return new SetHealthAction(pReader);
    }
    else if (pReader->ElementExists("InitializeBeginConfrontationTopicSelectionAction"))
    {
        return new InitializeBeginConfrontationTopicSelectionAction(pReader);
    }
    else if (pReader->ElementExists("BeginConfrontationTopicSelectionAction"))
    {
        return new BeginConfrontationTopicSelectionAction(pReader);
    }
    else if (pReader->ElementExists("BeginConfrontationTopicAction"))
    {
        return new BeginConfrontationTopicAction(pReader);
    }
    else if (pReader->ElementExists("EndConfrontationTopicAction"))
    {
        return new EndConfrontationTopicAction(pReader);
    }
    else if (pReader->ElementExists("SkipPlayerDefeatedAction"))
    {
        return new SkipPlayerDefeatedAction(pReader);
    }
    else if (pReader->ElementExists("BeginPlayerDefeatedAction"))
    {
        return new BeginPlayerDefeatedAction(pReader);
    }
    else if (pReader->ElementExists("EndPlayerDefeatedAction"))
    {
        return new EndPlayerDefeatedAction(pReader);
    }
    else if (pReader->ElementExists("EndConfrontationTopicSelectionAction"))
    {
        return new EndConfrontationTopicSelectionAction(pReader);
    }
    else if (pReader->ElementExists("EnableTopicAction"))
    {
        return new EnableTopicAction(pReader);
    }
    else if (pReader->ElementExists("RestartConfrontationAction"))
    {
        return new RestartConfrontationAction(pReader);
    }
    else
    {
        return Interrogation::GetActionForNextElement(pReader);
    }
}

Staging::Confrontation::SetParticipantsAction::SetParticipantsAction(XmlReader *pReader)
{
    pReader->StartElement("SetParticipantsAction");
    PlayerCharacterId = pReader->ReadTextElement("PlayerCharacterId");
    OpponentCharacterId = pReader->ReadTextElement("OpponentCharacterId");
    pReader->EndElement();
}

Staging::Confrontation::SetIconOffsetAction::SetIconOffsetAction(XmlReader *pReader)
{
    pReader->StartElement("SetIconOffsetAction");

    pReader->StartElement("Offset");
    Offset = Vector2(pReader);
    pReader->EndElement();

    IsPlayer = pReader->ReadBooleanElement("IsPlayer");
    pReader->EndElement();
}

Staging::Confrontation::SetHealthAction::SetHealthAction(XmlReader *pReader)
{
    pReader->StartElement("SetHealthAction");
    InitialHealth = pReader->ReadIntElement("InitialHealth");
    IsPlayer = pReader->ReadBooleanElement("IsPlayer");
    pReader->EndElement();
}

Staging::Confrontation::InitializeBeginConfrontationTopicSelectionAction::InitializeBeginConfrontationTopicSelectionAction(XmlReader *pReader)
{
    pReader->StartElement("InitializeBeginConfrontationTopicSelectionAction");
    pReader->EndElement();
}

Staging::Confrontation::BeginConfrontationTopicSelectionAction::BeginConfrontationTopicSelectionAction(XmlReader *pReader)
{
    pInitialTopic = NULL;
    InitialTopicIsEnabled = false;

    pReader->StartElement("BeginConfrontationTopicSelectionAction");
    PlayerDefeatedIndex = pReader->ReadIntElement("PlayerDefeatedIndex");
    EndActionIndex = pReader->ReadIntElement("EndActionIndex");

    if (pReader->ElementExists("InitialTopic"))
    {
        pReader->StartElement("InitialTopic");
        pInitialTopic = new Confrontation::Topic(pReader);
        InitialTopicIsEnabled = pInitialTopic->IsEnabled;
        pReader->EndElement();
    }

    pReader->StartElement("TopicList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        Confrontation::Topic *pTopic = new Confrontation::Topic(pReader);
        TopicList.push_back(pTopic);
        TopicToInitialEnabledStateMap[pTopic] = pInitialTopic->IsEnabled;
    }

    pReader->EndElement();

    pReader->EndElement();
}

Staging::Confrontation::BeginConfrontationTopicAction::BeginConfrontationTopicAction(XmlReader *pReader)
{
    pReader->StartElement("BeginConfrontationTopicAction");
    pReader->EndElement();
}

Staging::Confrontation::EndConfrontationTopicAction::EndConfrontationTopicAction(XmlReader *pReader)
{
    pReader->StartElement("EndConfrontationTopicAction");
    BeginConfrontationTopicSelectionActionIndex = pReader->ReadIntElement("BeginConfrontationTopicSelectionActionIndex");
    pReader->EndElement();
}

Staging::Confrontation::SkipPlayerDefeatedAction::SkipPlayerDefeatedAction(XmlReader *pReader)
{
    pReader->StartElement("SkipPlayerDefeatedAction");
    SkipToIndex = pReader->ReadIntElement("SkipToIndex");
    pReader->EndElement();
}

Staging::Confrontation::BeginPlayerDefeatedAction::BeginPlayerDefeatedAction(XmlReader *pReader)
{
    pReader->StartElement("BeginPlayerDefeatedAction");
    pReader->EndElement();
}

Staging::Confrontation::EndPlayerDefeatedAction::EndPlayerDefeatedAction(XmlReader *pReader)
{
    pReader->StartElement("EndPlayerDefeatedAction");
    pReader->EndElement();
}

Staging::Confrontation::EndConfrontationTopicSelectionAction::EndConfrontationTopicSelectionAction(XmlReader *pReader)
{
    pReader->StartElement("EndConfrontationTopicSelectionAction");
    pReader->EndElement();
}

Staging::Confrontation::EnableTopicAction::EnableTopicAction(XmlReader *pReader)
{
    pReader->StartElement("EnableTopicAction");
    TopicId = pReader->ReadTextElement("TopicId");
    pReader->EndElement();
}

Staging::Confrontation::RestartConfrontationAction::RestartConfrontationAction(XmlReader *pReader)
{
    pReader->StartElement("RestartConfrontationAction");
    pReader->EndElement();
}
