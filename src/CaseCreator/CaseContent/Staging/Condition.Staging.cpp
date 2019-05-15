#include "Condition.Staging.h"

#include "Flag.Staging.h"
#include "CaseContentLoadingStager.h"

Staging::Condition::Condition(XmlReader *pReader)
{
    pCriterion = NULL;

    pReader->StartElement("Condition");

    if (pReader->ElementExists("Criterion"))
    {
        pReader->StartElement("Criterion");
        pCriterion = ConditionCriterion::LoadFromXml(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();
}

Staging::Condition::~Condition()
{
    delete pCriterion;
    pCriterion = NULL;
}

Staging::Condition::ConditionCriterion * Staging::Condition::ConditionCriterion::LoadFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("FlagSetCondition"))
    {
        return new FlagSetCondition(pReader);
    }
    else if (pReader->ElementExists("EvidencePresentCondition"))
    {
        return new EvidencePresentCondition(pReader);
    }
    else if (pReader->ElementExists("PartnerPresentCondition"))
    {
        return new PartnerPresentCondition(pReader);
    }
    else if (pReader->ElementExists("ConversationLockedCondition"))
    {
        return new ConversationLockedCondition(pReader);
    }
    else if (pReader->ElementExists("TutorialsEnabledCondition"))
    {
        return new TutorialsEnabledCondition(pReader);
    }
    else if (pReader->ElementExists("AndCondition"))
    {
        return new AndCondition(pReader);
    }
    else if (pReader->ElementExists("OrCondition"))
    {
        return new OrCondition(pReader);
    }
    else if (pReader->ElementExists("NotCondition"))
    {
        return new NotCondition(pReader);
    }
    else
    {
        throw MLIException("Invalid criterion type.");
    }
}

Staging::Condition::FlagSetCondition::FlagSetCondition(XmlReader *pReader)
{
    pReader->StartElement("FlagSetCondition");
    FlagId = pReader->ReadTextElement("FlagId");

    if (!CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.contains(FlagId))
    {
        CaseContentLoadingStager::GetCurrent()->FlagIdToFlagMap.insert(FlagId, new Flag(FlagId));
    }

    pReader->EndElement();
}

Staging::Condition::EvidencePresentCondition::EvidencePresentCondition(XmlReader *pReader)
{
    pReader->StartElement("EvidencePresentCondition");
    EvidenceId = pReader->ReadTextElement("EvidenceId");
    pReader->EndElement();
}

Staging::Condition::PartnerPresentCondition::PartnerPresentCondition(XmlReader *pReader)
{
    pReader->StartElement("PartnerPresentCondition");
    PartnerId = pReader->ReadTextElement("PartnerId");
    pReader->EndElement();
}

Staging::Condition::ConversationLockedCondition::ConversationLockedCondition(XmlReader *pReader)
{
    pReader->StartElement("ConversationLockedCondition");
    ConversationId = pReader->ReadTextElement("ConversationId");
    pReader->EndElement();
}

Staging::Condition::TutorialsEnabledCondition::TutorialsEnabledCondition(XmlReader *pReader)
{
    pReader->StartElement("TutorialsEnabledCondition");
    pReader->EndElement();
}

Staging::Condition::AndCondition::AndCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion)
{
    this->pFirstCriterion = pFirstCriterion;
    this->pSecondCriterion = pSecondCriterion;
}

Staging::Condition::AndCondition::AndCondition(XmlReader *pReader)
{
    pReader->StartElement("AndCondition");

    pReader->StartElement("FirstCriterion");
    pFirstCriterion = ConditionCriterion::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->StartElement("SecondCriterion");
    pSecondCriterion = ConditionCriterion::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Staging::Condition::AndCondition::~AndCondition()
{
    delete pFirstCriterion;
    pFirstCriterion = NULL;

    delete pSecondCriterion;
    pSecondCriterion = NULL;
}

Staging::Condition::OrCondition::OrCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion)
{
    this->pFirstCriterion = pFirstCriterion;
    this->pSecondCriterion = pSecondCriterion;
}

Staging::Condition::OrCondition::OrCondition(XmlReader *pReader)
{
    pReader->StartElement("OrCondition");

    pReader->StartElement("FirstCriterion");
    pFirstCriterion = ConditionCriterion::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->StartElement("SecondCriterion");
    pSecondCriterion = ConditionCriterion::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Staging::Condition::OrCondition::~OrCondition()
{
    delete pFirstCriterion;
    pFirstCriterion = NULL;

    delete pSecondCriterion;
    pSecondCriterion = NULL;
}

Staging::Condition::NotCondition::NotCondition(ConditionCriterion *pCriterion)
{
    this->pCriterion = pCriterion;
}

Staging::Condition::NotCondition::NotCondition(XmlReader *pReader)
{
    pReader->StartElement("NotCondition");

    pReader->StartElement("Criterion");
    pCriterion = ConditionCriterion::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Staging::Condition::NotCondition::~NotCondition()
{
    delete pCriterion;
    pCriterion = NULL;
}
