/**
 * Represents a condition in the game.
 *
 * @author GabuEx, dawnmew
 * @since 1.0
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2014 Equestrian Dreamers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Condition.h"
#include "globals.h"
#include "CaseInformation/Case.h"
#include "XmlReader.h"

Condition::Condition(XmlReader *pReader)
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

Condition::~Condition()
{
    delete pCriterion;
    pCriterion = NULL;
}

bool Condition::IsTrue()
{
    return pCriterion != NULL ? pCriterion->IsTrue() : false;
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

Condition::ConditionCriterion * Condition::ConditionCriterion::LoadFromXml(XmlReader *pReader)
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
        throw Exception("Invalid criterion type.");
    }
}

Condition::FlagSetCondition::FlagSetCondition(string flagId)
    : flagId(flagId)
{
}

Condition::FlagSetCondition::FlagSetCondition(XmlReader *pReader)
{
    pReader->StartElement("FlagSetCondition");
    flagId = pReader->ReadTextElement("FlagId");
    pReader->EndElement();
}

Condition::FlagSetCondition::~FlagSetCondition()
{
}

bool Condition::FlagSetCondition::IsTrue()
{
    return Case::GetInstance()->GetFlagManager()->IsFlagSet(flagId);
}

Condition::ConditionCriterion * Condition::FlagSetCondition::Clone()
{
    return new FlagSetCondition(flagId);
}

Condition::EvidencePresentCondition::EvidencePresentCondition(string evidenceId)
    : evidenceId(evidenceId)
{
}

Condition::EvidencePresentCondition::EvidencePresentCondition(XmlReader *pReader)
{
    pReader->StartElement("EvidencePresentCondition");
    evidenceId = pReader->ReadTextElement("EvidenceId");
    pReader->EndElement();
}

Condition::EvidencePresentCondition::~EvidencePresentCondition()
{

}

bool Condition::EvidencePresentCondition::IsTrue()
{
    return Case::GetInstance()->GetEvidenceManager()->IsEvidenceWithIdVisible(evidenceId);
}

Condition::ConditionCriterion * Condition::EvidencePresentCondition::Clone()
{
    return new EvidencePresentCondition(evidenceId);
}

Condition::PartnerPresentCondition::PartnerPresentCondition(string partnerId)
    : partnerId(partnerId)
{
}

Condition::PartnerPresentCondition::PartnerPresentCondition(XmlReader *pReader)
{
    pReader->StartElement("PartnerPresentCondition");
    partnerId = pReader->ReadTextElement("PartnerId");
    pReader->EndElement();
}

Condition::PartnerPresentCondition::~PartnerPresentCondition()
{
}

bool Condition::PartnerPresentCondition::IsTrue()
{
    return partnerId == Case::GetInstance()->GetPartnerManager()->GetCurrentPartnerId();
}

Condition::ConditionCriterion * Condition::PartnerPresentCondition::Clone()
{
    return new PartnerPresentCondition(partnerId);
}

Condition::ConversationLockedCondition::ConversationLockedCondition(string conversationId)
    : conversationId(conversationId)
{
}

Condition::ConversationLockedCondition::ConversationLockedCondition(XmlReader *pReader)
{
    pReader->StartElement("ConversationLockedCondition");
    conversationId = pReader->ReadTextElement("ConversationId");
    pReader->EndElement();
}

Condition::ConversationLockedCondition::~ConversationLockedCondition()
{
}

bool Condition::ConversationLockedCondition::IsTrue()
{
    Conversation *pConversation = Case::GetInstance()->GetContentManager()->GetConversationFromId(conversationId);

    return pConversation->GetIsLocked() && !pConversation->GetHasBeenUnlocked();
}

Condition::ConditionCriterion * Condition::ConversationLockedCondition::Clone()
{
    return new ConversationLockedCondition(conversationId);
}

Condition::TutorialsEnabledCondition::TutorialsEnabledCondition()
{
}

Condition::TutorialsEnabledCondition::TutorialsEnabledCondition(XmlReader *pReader)
{
    pReader->StartElement("TutorialsEnabledCondition");
    pReader->EndElement();
}

Condition::TutorialsEnabledCondition::~TutorialsEnabledCondition()
{
}

bool Condition::TutorialsEnabledCondition::IsTrue()
{
    return gEnableTutorials;
}

Condition::ConditionCriterion * Condition::TutorialsEnabledCondition::Clone()
{
    return new TutorialsEnabledCondition();
}

Condition::AndCondition::AndCondition(Condition::ConditionCriterion *pFirstCriterion, Condition::ConditionCriterion *pSecondCriterion)
{
    this->pFirstCriterion = pFirstCriterion;
    this->pSecondCriterion = pSecondCriterion;
}

Condition::AndCondition::AndCondition(XmlReader *pReader)
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

Condition::AndCondition::~AndCondition()
{
    delete pFirstCriterion;
    pFirstCriterion = NULL;
    delete pSecondCriterion;
    pSecondCriterion = NULL;
}

bool Condition::AndCondition::IsTrue()
{
    return pFirstCriterion->IsTrue() && pSecondCriterion->IsTrue();
}

Condition::ConditionCriterion * Condition::AndCondition::Clone()
{
    return new AndCondition(pFirstCriterion->Clone(), pSecondCriterion->Clone());
}

Condition::OrCondition::OrCondition(Condition::ConditionCriterion *pFirstCriterion, Condition::ConditionCriterion *pSecondCriterion)
{
    this->pFirstCriterion = pFirstCriterion;
    this->pSecondCriterion = pSecondCriterion;
}

Condition::OrCondition::OrCondition(XmlReader *pReader)
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

Condition::OrCondition::~OrCondition()
{
    delete pFirstCriterion;
    pFirstCriterion = NULL;
    delete pSecondCriterion;
    pSecondCriterion = NULL;
}

bool Condition::OrCondition::IsTrue()
{
    return pFirstCriterion->IsTrue() || pSecondCriterion->IsTrue();
}

Condition::ConditionCriterion * Condition::OrCondition::Clone()
{
    return new OrCondition(pFirstCriterion->Clone(), pSecondCriterion->Clone());
}

Condition::NotCondition::NotCondition(Condition::ConditionCriterion *pCriterion)
{
    this->pCriterion = pCriterion;
}

Condition::NotCondition::NotCondition(XmlReader *pReader)
{
    pReader->StartElement("NotCondition");

    pReader->StartElement("Criterion");
    pCriterion = ConditionCriterion::LoadFromXml(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

Condition::NotCondition::~NotCondition()
{
    delete pCriterion;
    pCriterion = NULL;
}

bool Condition::NotCondition::IsTrue()
{
    return !pCriterion->IsTrue();
}

Condition::ConditionCriterion * Condition::NotCondition::Clone()
{
    return new NotCondition(pCriterion->Clone());
}
