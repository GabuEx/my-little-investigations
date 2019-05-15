#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

#include <QComboBox>

#include "Conversation.h"
#include "XmlStorableObject.h"

class XmlReader;
class XmlWriter;

namespace Staging
{
    class Encounter;
}

class Encounter : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(Encounter)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_TEXT(initialLeftCharacterId)
        XML_STORABLE_TEXT(initialLeftCharacterEmotionId)
        XML_STORABLE_TEXT(initialRightCharacterId)
        XML_STORABLE_TEXT(initialRightCharacterEmotionId)
        XML_STORABLE_CUSTOM_OBJECT_MAP(conversationFromIdMap, Conversation::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(interrogationFromIdMap, Interrogation::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(confrontationFromIdMap, Confrontation::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(presentEvidenceConversationDictionary, Conversation::CreateFromXml)
        XML_STORABLE_TEXT(initialConversationId)
        XML_STORABLE_TEXT(oneShotConversationId)
        XML_STORABLE_CUSTOM_OBJECT(pOneShotConversation, Conversation::CreateFromXml)
        XML_STORABLE_BOOL(ownsOneShotConversation)
        XML_STORABLE_CUSTOM_OBJECT(pPresentWrongEvidenceConversation, Conversation::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT(pPresentWrongProfileConversation, Conversation::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    Encounter()
    {
        pOneShotConversation = NULL;
        ownsOneShotConversation = false;
        pPresentWrongEvidenceConversation = NULL;
        pPresentWrongProfileConversation = NULL;
    }

    Encounter(QString id)
        : Encounter()
    {
        this->id = id;
    }

    Encounter(Staging::Encounter *pStagingEncounter);
    virtual ~Encounter();

    static Encounter * CreateFromXml(XmlReader *pReader)
    {
        return new Encounter(pReader);
    }

    void LoadElementsFromXml(XmlReader *pReader);
    void WriteToCaseXml(XmlWriter *pWriter);

    QString GetId() { return this->id; }
    QString GetDisplayName() { return this->id; }

    QString GetInitialLeftCharacterId() { return this->initialLeftCharacterId; }
    QString GetInitialLeftCharacterEmotionId() { return this->initialLeftCharacterEmotionId; }
    QString GetInitialRightCharacterId() { return this->initialRightCharacterId; }
    QString GetInitialRightCharacterEmotionId() { return this->initialRightCharacterEmotionId; }

    void AddConversation(Conversation *pElement, QString id = QString());
    Conversation * GetConversationById(QString id);

    QStringList GetConversationIds();
    QStringList GetConversationDisplayNames();

    void AddInterrogation(Interrogation *pElement, QString id = QString());
    Interrogation * GetInterrogationById(QString id);

    QStringList GetInterrogationIds();
    QStringList GetInterrogationDisplayNames();

    void AddConfrontation(Confrontation *pElement, QString id = QString());
    Confrontation * GetConfrontationById(QString id);

    QStringList GetConfrontationIds();
    QStringList GetConfrontationDisplayNames();

    void SetOneShotConversation(Conversation *pElement);
    Conversation * GetOneShotConversation();

    static QString GetObjectAdditionString() { return QString("encounter"); }
    static QString GetListTitle() { return QString("Encounters"); }
    static bool GetIsMainList() { return true; }
    static bool GetAllowsNewObjects() { return true; }

private:
    QString id;

    QString initialLeftCharacterId;
    QString initialLeftCharacterEmotionId;
    QString initialRightCharacterId;
    QString initialRightCharacterEmotionId;

    QMap<QString, Conversation *> conversationFromIdMap;
    QMap<QString, Interrogation *> interrogationFromIdMap;
    QMap<QString, Confrontation *> confrontationFromIdMap;
    QMap<QString, Conversation *> presentEvidenceConversationDictionary;

    QString initialConversationId;
    QString oneShotConversationId;

    Conversation *pOneShotConversation;
    bool ownsOneShotConversation;
    Conversation *pPresentWrongEvidenceConversation;
    Conversation *pPresentWrongProfileConversation;
};

class EncounterSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit EncounterSelector(QWidget *parent = 0);

    QString GetId();
    void SetToId(const QString &id);

    void Reset();

signals:
    void EncounterSelected(const QString &encounterId);

public slots:
    void CurrentIndexChanged(int currentIndex);

private:
    QStringList encounterIds;
    int previousIndex;
};

#endif // ENCOUNTER_H
