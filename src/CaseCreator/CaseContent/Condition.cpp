#include "Condition.h"

#include "CaseCreator/CaseContent/Staging/Condition.Staging.h"

#include "Character.h"
#include "Encounter.h"
#include "CaseCreator/CaseContent/CaseContent.h"
#include "CaseCreator/Utilities/Utilities.h"

#include <QHBoxLayout>
#include <QLabel>

Condition::Condition(Staging::Condition *pStagingCondition)
{
    if (pStagingCondition != NULL && pStagingCondition->pCriterion != NULL)
    {
        pCriterion = ConditionCriterion::LoadFromStagingCriterion(pStagingCondition->pCriterion);
    }
    else
    {
        pCriterion = NULL;
    }
}

Condition::~Condition()
{
    delete pCriterion;
    pCriterion = NULL;
}

QString Condition::ToString()
{
    return pCriterion->ToString();
}

ConditionEditor * Condition::GetEditor()
{
    ConditionEditor *pConditionEditor = new ConditionEditor();
    pConditionEditor->SetCriterionEditor(pCriterion->GetEditor());
    return pConditionEditor;
}

Condition * Condition::Clone()
{
    Condition *pCondition = new Condition();

    if (pCriterion != NULL)
    {
        pCondition->pCriterion = this->pCriterion->Clone();
    }

    return pCondition;
}

Condition::ConditionCriterion * Condition::ConditionCriterion::CreateFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("FlagSetCondition"))
    {
        return new Condition::FlagSetCondition(pReader);
    }
    else if (pReader->ElementExists("EvidencePresentCondition"))
    {
        return new Condition::EvidencePresentCondition(pReader);
    }
    else if (pReader->ElementExists("PartnerPresentCondition"))
    {
        return new Condition::PartnerPresentCondition(pReader);
    }
    else if (pReader->ElementExists("ConversationLockedCondition"))
    {
        return new Condition::ConversationLockedCondition(pReader);
    }
    else if (pReader->ElementExists("TutorialsEnabledCondition"))
    {
        return new Condition::TutorialsEnabledCondition(pReader);
    }
    else if (pReader->ElementExists("AndCondition"))
    {
        return new Condition::AndCondition(pReader);
    }
    else if (pReader->ElementExists("OrCondition"))
    {
        return new Condition::OrCondition(pReader);
    }
    else if (pReader->ElementExists("NotCondition"))
    {
        return new Condition::NotCondition(pReader);
    }
    else
    {
        throw MLIException("Invalid criterion type.");
    }
}

Condition::ConditionCriterion * Condition::ConditionCriterion::LoadFromStagingCriterion(Staging::Condition::ConditionCriterion *pStagingConditionCriterion)
{
    switch (pStagingConditionCriterion->GetType())
    {
    case Staging::ConditionCriterionType_FlagSet:
        {
            return new Condition::FlagSetCondition(static_cast<Staging::Condition::FlagSetCondition *>(pStagingConditionCriterion));
        }

    case Staging::ConditionCriterionType_EvidencePresent:
        {
            return new Condition::EvidencePresentCondition(static_cast<Staging::Condition::EvidencePresentCondition *>(pStagingConditionCriterion));
        }

    case Staging::ConditionCriterionType_PartnerPresent:
        {
            return new Condition::PartnerPresentCondition(static_cast<Staging::Condition::PartnerPresentCondition *>(pStagingConditionCriterion));
        }

    case Staging::ConditionCriterionType_ConversationLocked:
        {
            return new Condition::ConversationLockedCondition(static_cast<Staging::Condition::ConversationLockedCondition *>(pStagingConditionCriterion));
        }

    case Staging::ConditionCriterionType_TutorialsEnabled:
        {
            return new Condition::TutorialsEnabledCondition(static_cast<Staging::Condition::TutorialsEnabledCondition *>(pStagingConditionCriterion));
        }

    case Staging::ConditionCriterionType_And:
        {
            return new Condition::AndCondition(static_cast<Staging::Condition::AndCondition *>(pStagingConditionCriterion));
        }

    case Staging::ConditionCriterionType_Or:
        {
            return new Condition::OrCondition(static_cast<Staging::Condition::OrCondition *>(pStagingConditionCriterion));
        }

    case Staging::ConditionCriterionType_Not:
        {
            // We only want negation to be occurring at the lowest level, so if we're negating an AND or an OR,
            // we'll get rid of that by propagating the negation down to the lower levels and flipping the Boolean operator.
            Condition::ConditionCriterion *pReturnCondition = NULL;
            Staging::Condition::NotCondition *pNotCondition = static_cast<Staging::Condition::NotCondition *>(pStagingConditionCriterion);

            if (pNotCondition->pCriterion->GetType() == Staging::ConditionCriterionType_And)
            {
                Staging::Condition::AndCondition *pChildAndCondition = static_cast<Staging::Condition::AndCondition *>(pNotCondition->pCriterion);

                Staging::Condition::NotCondition *pNegatedFirstCondition = new Staging::Condition::NotCondition(pChildAndCondition->pFirstCriterion);
                Staging::Condition::NotCondition *pNegatedSecondCondition = new Staging::Condition::NotCondition(pChildAndCondition->pSecondCriterion);

                Staging::Condition::OrCondition *pEquivalentOrCondition = new Staging::Condition::OrCondition(pNegatedFirstCondition, pNegatedSecondCondition);

                pChildAndCondition->pFirstCriterion = NULL;
                pChildAndCondition->pSecondCriterion = NULL;

                pReturnCondition = new Condition::OrCondition(pEquivalentOrCondition);

                delete pEquivalentOrCondition;
            }
            else if (pNotCondition->pCriterion->GetType() == Staging::ConditionCriterionType_Or)
            {
                Staging::Condition::OrCondition *pChildOrCondition = static_cast<Staging::Condition::OrCondition *>(pNotCondition->pCriterion);

                Staging::Condition::NotCondition *pNegatedFirstCondition = new Staging::Condition::NotCondition(pChildOrCondition->pFirstCriterion);
                Staging::Condition::NotCondition *pNegatedSecondCondition = new Staging::Condition::NotCondition(pChildOrCondition->pSecondCriterion);

                Staging::Condition::AndCondition *pEquivalentAndCondition = new Staging::Condition::AndCondition(pNegatedFirstCondition, pNegatedSecondCondition);

                pChildOrCondition->pFirstCriterion = NULL;
                pChildOrCondition->pSecondCriterion = NULL;

                pReturnCondition = new Condition::AndCondition(pEquivalentAndCondition);

                delete pEquivalentAndCondition;
            }
            else
            {
                pReturnCondition = new Condition::NotCondition(static_cast<Staging::Condition::NotCondition *>(pStagingConditionCriterion));
            }

            return pReturnCondition;
        }

    default:
        throw MLIException("Invalid criterion type.");
    }
}

Condition::FlagSetCondition::FlagSetCondition(const QString &flagId)
    : flagId(flagId)
{
}

Condition::FlagSetCondition::FlagSetCondition(Staging::Condition::FlagSetCondition *pStagingFlagSetCondition)
{
    flagId = pStagingFlagSetCondition->FlagId;
}

Condition::FlagSetCondition::~FlagSetCondition()
{
}

CriterionEditor * Condition::FlagSetCondition::GetEditor()
{
    SingleCriterionEditor *pSingleCriterionEditor = new SingleCriterionEditor();
    pSingleCriterionEditor->SetCriterion(SingleCriterionType::FlagSet, flagId);
    return pSingleCriterionEditor;
}

Condition::ConditionCriterion * Condition::FlagSetCondition::Clone()
{
    return new FlagSetCondition(flagId);
}

Condition::EvidencePresentCondition::EvidencePresentCondition(const QString &evidenceId)
    : evidenceId(evidenceId)
{
}

Condition::EvidencePresentCondition::EvidencePresentCondition(Staging::Condition::EvidencePresentCondition *pStagingEvidencePresentCondition)
{
    evidenceId = pStagingEvidencePresentCondition->EvidenceId;
}

Condition::EvidencePresentCondition::~EvidencePresentCondition()
{

}

CriterionEditor * Condition::EvidencePresentCondition::GetEditor()
{
    SingleCriterionEditor *pSingleCriterionEditor = new SingleCriterionEditor();
    pSingleCriterionEditor->SetCriterion(SingleCriterionType::EvidencePresent, evidenceId);
    return pSingleCriterionEditor;
}

Condition::ConditionCriterion * Condition::EvidencePresentCondition::Clone()
{
    return new EvidencePresentCondition(evidenceId);
}

Condition::PartnerPresentCondition::PartnerPresentCondition(const QString &partnerId)
    : partnerId(partnerId)
{
}

Condition::PartnerPresentCondition::PartnerPresentCondition(Staging::Condition::PartnerPresentCondition *pStagingPartnerPresentCondition)
{
    partnerId = pStagingPartnerPresentCondition->PartnerId;
}

Condition::PartnerPresentCondition::~PartnerPresentCondition()
{
}

CriterionEditor * Condition::PartnerPresentCondition::GetEditor()
{
    SingleCriterionEditor *pSingleCriterionEditor = new SingleCriterionEditor();
    pSingleCriterionEditor->SetCriterion(SingleCriterionType::PartnerPresent, partnerId);
    return pSingleCriterionEditor;
}

Condition::ConditionCriterion * Condition::PartnerPresentCondition::Clone()
{
    return new PartnerPresentCondition(partnerId);
}

Condition::ConversationLockedCondition::ConversationLockedCondition(const QString &encounterId, const QString &conversationId)
    : encounterId(encounterId)
    , conversationId(conversationId)
{
}

Condition::ConversationLockedCondition::ConversationLockedCondition(Staging::Condition::ConversationLockedCondition *pStagingConversationLockedCondition)
{
    SplitConversationIdFromCaseFile(pStagingConversationLockedCondition->ConversationId, &encounterId, &conversationId);
}

Condition::ConversationLockedCondition::~ConversationLockedCondition()
{
}

CriterionEditor * Condition::ConversationLockedCondition::GetEditor()
{
    SingleCriterionEditor *pSingleCriterionEditor = new SingleCriterionEditor();
    pSingleCriterionEditor->SetCriterion(SingleCriterionType::ConversationLocked, conversationId);
    return pSingleCriterionEditor;
}

Condition::ConditionCriterion * Condition::ConversationLockedCondition::Clone()
{
    return new ConversationLockedCondition(encounterId, conversationId);
}

Condition::TutorialsEnabledCondition::TutorialsEnabledCondition(Staging::Condition::TutorialsEnabledCondition */*pStagingTutorialsEnabledCondition*/)
{
}

Condition::TutorialsEnabledCondition::~TutorialsEnabledCondition()
{
}

CriterionEditor * Condition::TutorialsEnabledCondition::GetEditor()
{
    SingleCriterionEditor *pSingleCriterionEditor = new SingleCriterionEditor();
    pSingleCriterionEditor->SetCriterion(SingleCriterionType::TutorialsEnabled);
    return pSingleCriterionEditor;
}

Condition::ConditionCriterion * Condition::TutorialsEnabledCondition::Clone()
{
    return new TutorialsEnabledCondition();
}

Condition::AndCondition::AndCondition(Condition::ConditionCriterion *pFirstCriterion, Condition::ConditionCriterion *pSecondCriterion)
    : AndCondition()
{
    this->pFirstCriterion = pFirstCriterion;
    this->pSecondCriterion = pSecondCriterion;
}

Condition::AndCondition::AndCondition(Staging::Condition::AndCondition *pStagingAndCondition)
    : AndCondition()
{
    pFirstCriterion = ConditionCriterion::LoadFromStagingCriterion(pStagingAndCondition->pFirstCriterion);
    pSecondCriterion = ConditionCriterion::LoadFromStagingCriterion(pStagingAndCondition->pSecondCriterion);
}

Condition::AndCondition::~AndCondition()
{
    delete pFirstCriterion;
    pFirstCriterion = NULL;
    delete pSecondCriterion;
    pSecondCriterion = NULL;
}

CriterionEditor * Condition::AndCondition::GetEditor()
{
    MultipleCriterionEditor *pMultipleCriterionEditor = new MultipleCriterionEditor();
    pMultipleCriterionEditor->SetCriterions(MultipleCriterionType::And, pFirstCriterion, pSecondCriterion);
    return pMultipleCriterionEditor;
}

Condition::ConditionCriterion * Condition::AndCondition::Clone()
{
    return new AndCondition(pFirstCriterion->Clone(), pSecondCriterion->Clone());
}

Condition::OrCondition::OrCondition(Condition::ConditionCriterion *pFirstCriterion, Condition::ConditionCriterion *pSecondCriterion)
    : OrCondition()
{
    this->pFirstCriterion = pFirstCriterion;
    this->pSecondCriterion = pSecondCriterion;
}

Condition::OrCondition::OrCondition(Staging::Condition::OrCondition *pStagingOrCondition)
    : OrCondition()
{
    pFirstCriterion = ConditionCriterion::LoadFromStagingCriterion(pStagingOrCondition->pFirstCriterion);
    pSecondCriterion = ConditionCriterion::LoadFromStagingCriterion(pStagingOrCondition->pSecondCriterion);
}

Condition::OrCondition::~OrCondition()
{
    delete pFirstCriterion;
    pFirstCriterion = NULL;
    delete pSecondCriterion;
    pSecondCriterion = NULL;
}

CriterionEditor * Condition::OrCondition::GetEditor()
{
    MultipleCriterionEditor *pMultipleCriterionEditor = new MultipleCriterionEditor();
    pMultipleCriterionEditor->SetCriterions(MultipleCriterionType::Or, pFirstCriterion, pSecondCriterion);
    return pMultipleCriterionEditor;
}

Condition::ConditionCriterion * Condition::OrCondition::Clone()
{
    return new OrCondition(pFirstCriterion->Clone(), pSecondCriterion->Clone());
}

Condition::NotCondition::NotCondition(Condition::ConditionCriterion *pCriterion)
    : NotCondition()
{
    this->pCriterion = pCriterion;
}

Condition::NotCondition::NotCondition(Staging::Condition::NotCondition *pStagingNotCondition)
    : NotCondition()
{
    pCriterion = ConditionCriterion::LoadFromStagingCriterion(pStagingNotCondition->pCriterion);
}

Condition::NotCondition::~NotCondition()
{
    delete pCriterion;
    pCriterion = NULL;
}

CriterionEditor * Condition::NotCondition::GetEditor()
{
    CriterionEditor *pChildCriterionEditor = pCriterion->GetEditor();
    SingleCriterionEditor *pChildSingleCriterionEditor = dynamic_cast<SingleCriterionEditor *>(pChildCriterionEditor);

    if (pChildSingleCriterionEditor == NULL)
    {
        qFatal("NotConditions should always only modify leaf elements in a condition tree.");
    }

    pChildSingleCriterionEditor->SetIsNegated(true);
    return pChildSingleCriterionEditor;
}

Condition::ConditionCriterion * Condition::NotCondition::Clone()
{
    return new NotCondition(pCriterion->Clone());
}

ConditionEditor::ConditionEditor()
    : QWidget()
{
    pMainLayout = new QVBoxLayout();
    pMainLayout->setSpacing(0);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pMainLayout);

    pCriterionEditor = NULL;

    QStringList flagIds = CaseContent::GetInstance()->GetIds<Flag>();

    SingleCriterionEditor *pInitialCriterionEditor = new SingleCriterionEditor();
    pInitialCriterionEditor->SetCriterion(SingleCriterionType::FlagSet, flagIds.length() > 0 ? flagIds[0] : "");

    SetCriterionEditor(pInitialCriterionEditor);
}

void ConditionEditor::SetCriterionEditor(CriterionEditor *pCriterionEditor)
{
    if (this->pCriterionEditor != NULL)
    {
        pMainLayout->removeWidget(this->pCriterionEditor);
        delete this->pCriterionEditor;
    }

    this->pCriterionEditor = pCriterionEditor;

    if (this->pCriterionEditor != NULL)
    {
        pMainLayout->addWidget(this->pCriterionEditor);
    }
}

Condition * ConditionEditor::GetCondition()
{
    Condition *pCondition = new Condition();

    pCondition->pCriterion = pCriterionEditor->GetCriterion();

    return pCondition;
}

CriterionEditor::CriterionEditor()
    : QWidget()
{
}

SingleCriterionEditor::SingleCriterionEditor()
    : CriterionEditor()
{
    criterionTypeSelected = SingleCriterionType::FlagSet;
    pFlagSelected = NULL;
    pEvidenceSelected = NULL;
    pCharacterSelected = NULL;
    pEncounterSelected = NULL;
    pConversationSelected = NULL;

    QHBoxLayout *pMainLayout = new QHBoxLayout();

    pNotCheckBox = new QCheckBox();
    pNotCheckBox->setChecked(false);
    pNotCheckBox->setText("Not");
    pMainLayout->addWidget(pNotCheckBox);

    pCriterionTypeComboBox = new QComboBox();
    pCriterionTypeComboBox->addItem("Flag is set...");
    pCriterionTypeComboBox->addItem("Evidence is present...");
    pCriterionTypeComboBox->addItem("Partner is present...");
    pCriterionTypeComboBox->addItem("Conversation is locked...");
    pCriterionTypeComboBox->addItem("Tutorials are enabled");
    pCriterionTypeComboBox->setCurrentIndex(0);
    QObject::connect(pCriterionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(CriterionTypeComboBoxCurrentIndexChanged(int)));
    pMainLayout->addWidget(pCriterionTypeComboBox);

    pFlagEditor = new FlagEditor();
    QObject::connect(pFlagEditor, SIGNAL(FlagSelected(QString)), this, SLOT(FlagEditorFlagSelected(QString)));
    pMainLayout->addWidget(pFlagEditor);

    pEvidenceSelector = new EvidenceSelector();
    QObject::connect(pEvidenceSelector, SIGNAL(EvidenceSelected(QString)), this, SLOT(EvidenceSelectorEvidenceSelected(QString)));
    pMainLayout->addWidget(pEvidenceSelector);

    pPartnerSelector = new CharacterSelector(false /* allowNoCharacter */);
    QObject::connect(pPartnerSelector, SIGNAL(CharacterSelected(QString)), this, SLOT(PartnerSelectorCharacterSelected(QString)));
    pMainLayout->addWidget(pPartnerSelector);

    pConversationLockedEncounterSelector = new EncounterSelector();
    QObject::connect(pConversationLockedEncounterSelector, SIGNAL(EncounterSelected(QString)), this, SLOT(ConversationLockedEncounterSelectorEncounterSelected(QString)));
    pMainLayout->addWidget(pConversationLockedEncounterSelector);

    pConversationLockedSelector = new ConversationSelector();
    QObject::connect(pConversationLockedSelector, SIGNAL(ConversationSelected(QString)), this, SLOT(ConversationLockedConversationSelectorConversationSelected(QString)));
    pMainLayout->addWidget(pConversationLockedSelector);

    pMainLayout->addStretch(1);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pMainLayout);

    // Call these explicitly to make sure we're initialized properly.
    CriterionTypeComboBoxCurrentIndexChanged(0);
    FlagEditorFlagSelected(pFlagEditor->itemText(0));
}

void SingleCriterionEditor::SetCriterion(SingleCriterionType criterionType, const QString &criterionId)
{
    criterionTypeSelected = criterionType;
    pCriterionTypeComboBox->setCurrentIndex((int)criterionTypeSelected);

    switch (criterionType)
    {
    case SingleCriterionType::FlagSet:
        pFlagEditor->setCurrentText(criterionId);
        FlagEditorFlagSelected(criterionId);
        break;

    case SingleCriterionType::EvidencePresent:
        pEvidenceSelector->SetToId(criterionId);
        EvidenceSelectorEvidenceSelected(criterionId);
        break;

    case SingleCriterionType::PartnerPresent:
        pPartnerSelector->SetToCharacterById(criterionId);
        PartnerSelectorCharacterSelected(criterionId);
        break;

    case SingleCriterionType::ConversationLocked:
    {
        QString encounterIdSelected;

        for (const QString &encounterId : CaseContent::GetInstance()->GetIds<Encounter>())
        {
            Encounter *pEncounter = CaseContent::GetInstance()->GetById<Encounter>(encounterId);

            if (CaseContent::GetInstance()->GetIds<Conversation>(pEncounter).contains(criterionId))
            {
                encounterIdSelected = encounterId;
                break;
            }
        }

        if (encounterIdSelected.length() > 0)
        {
            pConversationLockedEncounterSelector->SetToId(encounterIdSelected);
            ConversationLockedEncounterSelectorEncounterSelected(encounterIdSelected);
            pConversationLockedSelector->SetToId(criterionId);
            ConversationLockedConversationSelectorConversationSelected(criterionId);
        }

        break;
    }

    case SingleCriterionType::TutorialsEnabled:
        break;

    default:
        qFatal("Invalid single criterion type selected.");
    }
}

void SingleCriterionEditor::SetIsNegated(bool isNegated)
{
    pNotCheckBox->setChecked(isNegated);
}

Condition::ConditionCriterion * SingleCriterionEditor::GetCriterion()
{
    Condition::ConditionCriterion *pCriterion = NULL;

    switch (criterionTypeSelected)
    {
    case SingleCriterionType::FlagSet:
        pCriterion = new Condition::FlagSetCondition(pFlagSelected->GetId());
        break;

    case SingleCriterionType::EvidencePresent:
        pCriterion = new Condition::EvidencePresentCondition(pEvidenceSelected->GetId());
        break;

    case SingleCriterionType::PartnerPresent:
        pCriterion = new Condition::PartnerPresentCondition(pCharacterSelected->GetId());
        break;

    case SingleCriterionType::ConversationLocked:
        pCriterion = new Condition::ConversationLockedCondition(pEncounterSelected->GetId(), pConversationSelected->GetId());
        break;

    case SingleCriterionType::TutorialsEnabled:
        pCriterion = new Condition::TutorialsEnabledCondition();
        break;

    default:
        qFatal("Invalid single criterion type selected.");
    }

    if (pCriterion != NULL && pNotCheckBox->isChecked())
    {
        pCriterion = new Condition::NotCondition(pCriterion);
    }

    return pCriterion;
}

void SingleCriterionEditor::CriterionTypeComboBoxCurrentIndexChanged(int newIndex)
{
    criterionTypeSelected = (SingleCriterionType)newIndex;

    switch (criterionTypeSelected)
    {
    case SingleCriterionType::FlagSet:
        if (pFlagSelected != NULL)
        {
            pFlagEditor->setCurrentText(pFlagSelected->GetId());
        }
        else
        {
            pFlagEditor->setCurrentIndex(0);
        }

        pFlagEditor->show();

        pEvidenceSelector->hide();
        pPartnerSelector->hide();
        pConversationLockedEncounterSelector->hide();
        pConversationLockedSelector->hide();

        FlagEditorFlagSelected(pFlagEditor->GetId());
        break;

    case SingleCriterionType::EvidencePresent:
        if (pEvidenceSelected != NULL)
        {
            pEvidenceSelector->SetToId(pEvidenceSelected->GetId());
        }
        else
        {
            pEvidenceSelector->SetToId("");
        }

        pEvidenceSelector->show();

        pFlagEditor->hide();
        pPartnerSelector->hide();
        pConversationLockedEncounterSelector->hide();
        pConversationLockedSelector->hide();

        EvidenceSelectorEvidenceSelected(pEvidenceSelector->GetId());
        break;

    case SingleCriterionType::PartnerPresent:
        if (pCharacterSelected != NULL)
        {
            pPartnerSelector->SetToCharacterById(pCharacterSelected->GetId());
        }
        else
        {
            pPartnerSelector->SetToCharacterById("");
        }

        pPartnerSelector->show();

        pFlagEditor->hide();
        pEvidenceSelector->hide();
        pConversationLockedSelector->hide();
        pConversationLockedSelector->hide();

        PartnerSelectorCharacterSelected(pPartnerSelector->GetSelectedCharacterId());
        break;

    case SingleCriterionType::ConversationLocked:
        if (pEncounterSelected != NULL && pConversationSelected != NULL)
        {
            pConversationLockedEncounterSelector->SetToId(pEncounterSelected->GetId());
            pConversationLockedSelector->SetToId(pConversationSelected->GetId());
        }
        else
        {
            pConversationLockedEncounterSelector->SetToId("");
            pConversationLockedSelector->SetToId("");
        }

        pConversationLockedEncounterSelector->show();
        pConversationLockedSelector->show();

        pFlagEditor->hide();
        pEvidenceSelector->hide();
        pPartnerSelector->hide();

        ConversationLockedEncounterSelectorEncounterSelected(pConversationLockedEncounterSelector->GetId());
        ConversationLockedConversationSelectorConversationSelected(pConversationLockedSelector->GetId());
        break;

    case SingleCriterionType::TutorialsEnabled:
        pFlagEditor->hide();
        pEvidenceSelector->hide();
        pPartnerSelector->hide();
        pConversationLockedEncounterSelector->hide();
        pConversationLockedSelector->hide();
        break;

    default:
        qFatal("Invalid single criterion type selected.");
    }
}

void SingleCriterionEditor::FlagEditorFlagSelected(const QString &flagIdSelected)
{
    pFlagSelected = CaseContent::GetInstance()->GetById<Flag>(flagIdSelected);
}

void SingleCriterionEditor::EvidenceSelectorEvidenceSelected(const QString &evidenceIdSelected)
{
    pEvidenceSelected = CaseContent::GetInstance()->GetById<Evidence>(evidenceIdSelected);
}

void SingleCriterionEditor::PartnerSelectorCharacterSelected(const QString &characterIdSelected)
{
    pCharacterSelected = CaseContent::GetInstance()->GetById<Character>(characterIdSelected);
}

void SingleCriterionEditor::ConversationLockedEncounterSelectorEncounterSelected(const QString &encounterIdSelected)
{
    pEncounterSelected = CaseContent::GetInstance()->GetById<Encounter>(encounterIdSelected);

    if (pEncounterSelected != NULL)
    {
        pConversationLockedSelector->Reset(pEncounterSelected);
    }
}

void SingleCriterionEditor::ConversationLockedConversationSelectorConversationSelected(const QString &conversationIdSelected)
{
    if (pEncounterSelected != NULL)
    {
        pConversationSelected = CaseContent::GetInstance()->GetById<Conversation>(conversationIdSelected, pEncounterSelected);
    }
}

MultipleCriterionEditor::MultipleCriterionEditor()
{
    criterionTypeSelected = MultipleCriterionType::And;
    pFirstCriterionEditor = NULL;
    pSecondCriterionEditor = NULL;

    QGridLayout *pMainLayout = new QGridLayout();

    pCriterionTypeComboBox = new QComboBox();
    pCriterionTypeComboBox->addItem("And");
    pCriterionTypeComboBox->addItem("Or");
    pCriterionTypeComboBox->setCurrentIndex(0);
    QObject::connect(pCriterionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(CriterionTypeComboBoxCurrentIndexChanged(int)));
    pMainLayout->addWidget(pCriterionTypeComboBox, 0, 0, 2, 1);

    pFirstCriterionHolder = new QHBoxLayout();
    pMainLayout->addLayout(pFirstCriterionHolder, 0, 1);

    pSecondCriterionHolder = new QHBoxLayout();
    pMainLayout->addLayout(pSecondCriterionHolder, 1, 1);

    pMainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pMainLayout);

    CriterionTypeComboBoxCurrentIndexChanged(0);
}

void MultipleCriterionEditor::SetCriterions(MultipleCriterionType criterionType, Condition::ConditionCriterion *pFirstCriterion, Condition::ConditionCriterion *pSecondCriterion)
{
    criterionTypeSelected = criterionType;
    pCriterionTypeComboBox->setCurrentIndex((int)criterionTypeSelected);

    CriterionEditor *pOldFirstCriterionEditor = pFirstCriterionEditor;
    CriterionEditor *pOldSecondCriterionEditor = pSecondCriterionEditor;

    pFirstCriterionEditor = pFirstCriterion->GetEditor();
    pSecondCriterionEditor = pSecondCriterion->GetEditor();

    pFirstCriterionHolder->removeWidget(pOldFirstCriterionEditor);
    pFirstCriterionHolder->addWidget(pFirstCriterionEditor);
    pSecondCriterionHolder->removeWidget(pOldSecondCriterionEditor);
    pSecondCriterionHolder->addWidget(pSecondCriterionEditor);

    delete pOldFirstCriterionEditor;
    delete pOldSecondCriterionEditor;
}

Condition::ConditionCriterion * MultipleCriterionEditor::GetCriterion()
{
    Condition::ConditionCriterion *pCriterion = NULL;

    switch (criterionTypeSelected)
    {
    case MultipleCriterionType::And:
        pCriterion = new Condition::AndCondition(pFirstCriterionEditor->GetCriterion(), pSecondCriterionEditor->GetCriterion());
        break;

    case MultipleCriterionType::Or:
        pCriterion = new Condition::OrCondition(pFirstCriterionEditor->GetCriterion(), pSecondCriterionEditor->GetCriterion());
        break;

    default:
        qFatal("Invalid multiple criterion type selected.");
    }

    return pCriterion;
}

void MultipleCriterionEditor::CriterionTypeComboBoxCurrentIndexChanged(int newIndex)
{
    criterionTypeSelected = (MultipleCriterionType)newIndex;
}
