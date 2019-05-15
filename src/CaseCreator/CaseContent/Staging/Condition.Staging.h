#ifndef CONDITION_STAGING_H
#define CONDITION_STAGING_H

#include "Enums.Staging.h"

#include "XmlReader.h"

#include <QString>

namespace Staging
{

class Condition
{
public:
    Condition()
    {
        pCriterion = NULL;
    }

    Condition(XmlReader *pReader);
    ~Condition();

    class ConditionCriterion
    {
    public:
        static ConditionCriterion * LoadFromXml(XmlReader *pReader);

        virtual ~ConditionCriterion() {}

        virtual ConditionCriterionType GetType() = 0;
    };

    class FlagSetCondition : public ConditionCriterion
    {
    public:
        FlagSetCondition(XmlReader *pReader);

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_FlagSet;
        }

        QString FlagId;
    };

    class EvidencePresentCondition : public ConditionCriterion
    {
    public:
        EvidencePresentCondition(XmlReader *pReader);

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_EvidencePresent;
        }

        QString EvidenceId;
    };

    class PartnerPresentCondition : public ConditionCriterion
    {
    public:
        PartnerPresentCondition(XmlReader *pReader);

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_PartnerPresent;
        }

        QString PartnerId;
    };

    class ConversationLockedCondition : public ConditionCriterion
    {
    public:
        ConversationLockedCondition(XmlReader *pReader);

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_ConversationLocked;
        }

        QString ConversationId;
    };

    class TutorialsEnabledCondition : public ConditionCriterion
    {
    public:
        TutorialsEnabledCondition(XmlReader *pReader);

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_TutorialsEnabled;
        }
    };

    class AndCondition : public ConditionCriterion
    {
    public:
        AndCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion);
        AndCondition(XmlReader *pReader);
        ~AndCondition();

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_And;
        }

        ConditionCriterion *pFirstCriterion;
        ConditionCriterion *pSecondCriterion;
    };

    class OrCondition : public ConditionCriterion
    {
    public:
        OrCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion);
        OrCondition(XmlReader *pReader);
        ~OrCondition();

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_Or;
        }

        ConditionCriterion *pFirstCriterion;
        ConditionCriterion *pSecondCriterion;
    };

    class NotCondition : public ConditionCriterion
    {
    public:
        NotCondition(ConditionCriterion *pCriterion);
        NotCondition(XmlReader *pReader);
        ~NotCondition();

        ConditionCriterionType GetType()
        {
            return ConditionCriterionType_Not;
        }

        ConditionCriterion *pCriterion;
    };

    ConditionCriterion *pCriterion;
};

}

#endif // CONDITION_STAGING_H
