#ifndef CONDITION_H
#define CONDITION_H

#include "CaseCreator/CaseContent/Staging/Condition.Staging.h"
#include "XmlStorableObject.h"

#include <QString>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>
#include <QVBoxLayout>

class ConditionEditor;
class CriterionEditor;
class EncounterSelector;

class Flag;
class FlagEditor;
class Evidence;
class EvidenceSelector;
class Character;
class CharacterSelector;
class Encounter;
class Conversation;
class ConversationSelector;

class Condition : public XmlStorableObject
{
    friend class ConditionEditor;

    BEGIN_XML_STORABLE_OBJECT(Condition)
        XML_STORABLE_CUSTOM_OBJECT(pCriterion, ConditionCriterion::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    Condition()
    {
        pCriterion = NULL;
    }

    Condition(Staging::Condition *pStagingCondition);
    virtual ~Condition();

    static Condition * CreateFromXml(XmlReader *pReader)
    {
        return new Condition(pReader);
    }

    QString ToString();
    ConditionEditor * GetEditor();

    Condition * Clone();

    class ConditionCriterion : public XmlStorableObject
    {
        BEGIN_XML_STORABLE_OBJECT(ConditionCriterion)
        END_XML_STORABLE_OBJECT()

    public:
        ConditionCriterion() { }
        virtual ~ConditionCriterion() {}

        virtual QString ToString() = 0;
        virtual CriterionEditor * GetEditor() = 0;

        virtual ConditionCriterion * Clone() = 0;

        static ConditionCriterion * CreateFromXml(XmlReader *pReader);
        static ConditionCriterion * LoadFromStagingCriterion(Staging::Condition::ConditionCriterion *pStagingConditionCriterion);
    };

    class FlagSetCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(FlagSetCondition, ConditionCriterion)
            XML_STORABLE_TEXT(flagId)
        END_XML_STORABLE_OBJECT()

    public:
        FlagSetCondition() { }
        FlagSetCondition(const QString &flagId);
        FlagSetCondition(Staging::Condition::FlagSetCondition *pStagingFlagSetCondition);
        ~FlagSetCondition();

        QString ToString() override { return QString("Flag is set - \"") + flagId + "\""; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;

    private:
        QString flagId;
    };

    class EvidencePresentCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(EvidencePresentCondition, ConditionCriterion)
            XML_STORABLE_TEXT(evidenceId)
        END_XML_STORABLE_OBJECT()

    public:
        EvidencePresentCondition() { }
        EvidencePresentCondition(const QString &partnerId);
        EvidencePresentCondition(Staging::Condition::EvidencePresentCondition *pStagingEvidencePresentCondition);
        ~EvidencePresentCondition();

        QString ToString() override { return QString("Evidence is present - \"") + evidenceId + "\""; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;

    private:
        QString evidenceId;
    };

    class PartnerPresentCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(PartnerPresentCondition, ConditionCriterion)
            XML_STORABLE_TEXT(partnerId)
        END_XML_STORABLE_OBJECT()

    public:
        PartnerPresentCondition() { }
        PartnerPresentCondition(const QString &partnerId);
        PartnerPresentCondition(Staging::Condition::PartnerPresentCondition *pStagingPartnerPresentCondition);
        ~PartnerPresentCondition();

        QString ToString() override { return QString("Partner is present - \"") + partnerId + "\""; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;

    private:
        QString partnerId;
    };

    class ConversationLockedCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(ConversationLockedCondition, ConditionCriterion)
            XML_STORABLE_TEXT(conversationId)
        END_XML_STORABLE_OBJECT()

    public:
        ConversationLockedCondition() { }
        ConversationLockedCondition(const QString &encounterId, const QString &conversationId);
        ConversationLockedCondition(Staging::Condition::ConversationLockedCondition *pStagingConversationLockedCondition);
        ~ConversationLockedCondition();

        QString ToString() override { return QString("Conversation is locked - \"") + conversationId + "\""; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;

    private:
        QString encounterId;
        QString conversationId;
    };

    class TutorialsEnabledCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(TutorialsEnabledCondition, ConditionCriterion)
        END_XML_STORABLE_OBJECT()

    public:
        TutorialsEnabledCondition() { }
        TutorialsEnabledCondition(Staging::Condition::TutorialsEnabledCondition *pStagingTutorialsEnabledCondition);
        ~TutorialsEnabledCondition();

        QString ToString() override { return "Tutorials are enabled"; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;
    };

    class AndCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(AndCondition, ConditionCriterion)
            XML_STORABLE_CUSTOM_OBJECT(pFirstCriterion, ConditionCriterion::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT(pSecondCriterion, ConditionCriterion::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        AndCondition()
        {
            pFirstCriterion = NULL;
            pSecondCriterion = NULL;
        }

        AndCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion);
        AndCondition(Staging::Condition::AndCondition *pStagingAndCondition);
        ~AndCondition();

        QString ToString() override { return QString("(") + pFirstCriterion->ToString() + " AND " + pSecondCriterion->ToString() + ")"; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;

    private:
        ConditionCriterion *pFirstCriterion;
        ConditionCriterion *pSecondCriterion;
    };

    class OrCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(OrCondition, ConditionCriterion)
            XML_STORABLE_CUSTOM_OBJECT(pFirstCriterion, ConditionCriterion::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT(pSecondCriterion, ConditionCriterion::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        OrCondition()
        {
            pFirstCriterion = NULL;
            pSecondCriterion = NULL;
        }

        OrCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion);
        OrCondition(Staging::Condition::OrCondition *pStagingOrCondition);
        ~OrCondition();

        QString ToString() override { return QString("(") + pFirstCriterion->ToString() + " OR " + pSecondCriterion->ToString() + ")"; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;

    private:
        ConditionCriterion *pFirstCriterion;
        ConditionCriterion *pSecondCriterion;
    };

    class NotCondition : public ConditionCriterion
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(NotCondition, ConditionCriterion)
            XML_STORABLE_CUSTOM_OBJECT(pCriterion, ConditionCriterion::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        NotCondition()
        {
            pCriterion = NULL;
        }

        NotCondition(ConditionCriterion *pCriterion);
        NotCondition(Staging::Condition::NotCondition *pStagingNotCondition);
        ~NotCondition();

        QString ToString() override { return QString("NOT (") + pCriterion->ToString() + ")"; }
        CriterionEditor * GetEditor() override;

        ConditionCriterion * Clone() override;

    private:
        ConditionCriterion *pCriterion;
    };

private:
    ConditionCriterion *pCriterion;
};

class ConditionEditor : public QWidget
{
    friend class Condition;

    Q_OBJECT

public:
    explicit ConditionEditor();

    Condition * GetCondition();

private:
    void SetCriterionEditor(CriterionEditor *pCriterionEditor);

    QVBoxLayout *pMainLayout;
    CriterionEditor *pCriterionEditor;
};

class CriterionEditor : public QWidget
{
public:
    explicit CriterionEditor();

    virtual Condition::ConditionCriterion * GetCriterion() = 0;
};

enum class SingleCriterionType
{
    FlagSet = 0,
    EvidencePresent = 1,
    PartnerPresent = 2,
    ConversationLocked = 3,
    TutorialsEnabled = 4,
};

class SingleCriterionEditor : public CriterionEditor
{
    Q_OBJECT

public:
    explicit SingleCriterionEditor();

    void SetCriterion(SingleCriterionType criterionType, const QString &criterionId = QString());
    void SetIsNegated(bool isNegated);

    Condition::ConditionCriterion * GetCriterion() override;

public slots:
    void CriterionTypeComboBoxCurrentIndexChanged(int newIndex);
    void FlagEditorFlagSelected(const QString &flagIdSelected);
    void EvidenceSelectorEvidenceSelected(const QString &evidenceIdSelected);
    void PartnerSelectorCharacterSelected(const QString &characterIdSelected);
    void ConversationLockedEncounterSelectorEncounterSelected(const QString &encounterIdSelected);
    void ConversationLockedConversationSelectorConversationSelected(const QString &conversationIdSelected);

private:
    QCheckBox *pNotCheckBox;
    QComboBox *pCriterionTypeComboBox;

    FlagEditor *pFlagEditor;
    EvidenceSelector *pEvidenceSelector;
    CharacterSelector *pPartnerSelector;

    EncounterSelector *pConversationLockedEncounterSelector;
    ConversationSelector *pConversationLockedSelector;

    SingleCriterionType criterionTypeSelected;
    Flag *pFlagSelected;
    Evidence *pEvidenceSelected;
    Character *pCharacterSelected;
    Encounter *pEncounterSelected;
    Conversation *pConversationSelected;
};

enum class MultipleCriterionType
{
    And = 0,
    Or = 1,
};

class MultipleCriterionEditor : public CriterionEditor
{
    Q_OBJECT

public:
    explicit MultipleCriterionEditor();

    void SetCriterions(MultipleCriterionType criterionType, Condition::ConditionCriterion *pFirstCriterion, Condition::ConditionCriterion *pSecondCriterion);

    Condition::ConditionCriterion * GetCriterion() override;

public slots:
    void CriterionTypeComboBoxCurrentIndexChanged(int newIndex);

private:
    QComboBox *pCriterionTypeComboBox;

    QHBoxLayout *pFirstCriterionHolder;
    QHBoxLayout *pSecondCriterionHolder;

    MultipleCriterionType criterionTypeSelected;
    CriterionEditor *pFirstCriterionEditor;
    CriterionEditor *pSecondCriterionEditor;
};

#endif
