#include "Encounter.h"

#include "CaseContent.h"
#include "Conversation.h"

#include "XmlReader.h"
#include "XmlWriter.h"

#include "Staging/Encounter.Staging.h"

#include "CaseCreator/Utilities/Utilities.h"

Encounter::Encounter(Staging::Encounter *pStagingEncounter)
    : Encounter()
{
    id = pStagingEncounter->Id;

    initialLeftCharacterId = pStagingEncounter->InitialLeftCharacterId;
    initialLeftCharacterEmotionId = pStagingEncounter->InitialLeftCharacterEmotionId;
    initialRightCharacterId = pStagingEncounter->InitialRightCharacterId;
    initialRightCharacterEmotionId = pStagingEncounter->InitialRightCharacterEmotionId;

    for (QString evidenceId : pStagingEncounter->PresentEvidenceConversationDictionary.keys())
    {
        Conversation *pConversation = Conversation::CreateFromStaging(pStagingEncounter->PresentEvidenceConversationDictionary[evidenceId]);
        pConversation->SetOwningEncounter(this);
        presentEvidenceConversationDictionary.insert(evidenceId, pConversation);
    }

    SplitConversationIdFromCaseFile(pStagingEncounter->InitialConversationId, NULL, &initialConversationId);
    SplitConversationIdFromCaseFile(pStagingEncounter->OneShotConversationId, NULL, &oneShotConversationId);

    if (pStagingEncounter->pOneShotConversation != NULL)
    {
        pOneShotConversation = Conversation::CreateFromStaging(pStagingEncounter->pOneShotConversation);
        pOneShotConversation->SetOwningEncounter(this);
    }

    ownsOneShotConversation = pStagingEncounter->OwnsOneShotConversation;

    if (pStagingEncounter->pPresentWrongEvidenceConversation != NULL)
    {
        pPresentWrongEvidenceConversation = Conversation::CreateFromStaging(pStagingEncounter->pPresentWrongEvidenceConversation);
        pPresentWrongEvidenceConversation->SetOwningEncounter(this);
    }

    if (pStagingEncounter->pPresentWrongProfileConversation != NULL)
    {
        pPresentWrongProfileConversation = Conversation::CreateFromStaging(pStagingEncounter->pPresentWrongProfileConversation);
        pPresentWrongEvidenceConversation->SetOwningEncounter(this);
    }
}

Encounter::~Encounter()
{
    if (ownsOneShotConversation)
    {
        delete pOneShotConversation;
    }

    pOneShotConversation = NULL;

    delete pPresentWrongEvidenceConversation;
    pPresentWrongEvidenceConversation = NULL;

    delete pPresentWrongProfileConversation;
    pPresentWrongProfileConversation = NULL;
}

void Encounter::LoadElementsFromXml(XmlReader *pReader)
{
    XmlStorableObject::LoadElementsFromXml(pReader);

    for (QString conversationId : conversationFromIdMap.keys())
    {
        conversationFromIdMap[conversationId]->SetOwningEncounter(this);
    }

    for (QString interrogationId : interrogationFromIdMap.keys())
    {
        interrogationFromIdMap[interrogationId]->SetOwningEncounter(this);
    }

    for (QString confrontationId : confrontationFromIdMap.keys())
    {
        confrontationFromIdMap[confrontationId]->SetOwningEncounter(this);
    }

    for (QString evidenceId : presentEvidenceConversationDictionary.keys())
    {
        presentEvidenceConversationDictionary[evidenceId]->SetOwningEncounter(this);
    }

    if (pOneShotConversation != NULL)
    {
        pOneShotConversation->SetOwningEncounter(this);
    }

    if (pPresentWrongEvidenceConversation != NULL)
    {
        pPresentWrongEvidenceConversation->SetOwningEncounter(this);
    }

    if (pPresentWrongProfileConversation != NULL)
    {
        pPresentWrongEvidenceConversation->SetOwningEncounter(this);
    }
}

void Encounter::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("Encounter");

    if (pOneShotConversation != NULL)
    {
        pWriter->StartElement("OneShotConversation");
        pOneShotConversation->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();
}

void Encounter::AddConversation(Conversation *pElement, QString id)
{
    pElement->SetOwningEncounter(this);
    conversationFromIdMap.insert(id, pElement);
}

Conversation * Encounter::GetConversationById(QString id)
{
    if (conversationFromIdMap.contains(id))
    {
        return conversationFromIdMap[id];
    }
    else
    {
        return NULL;
    }
}

QStringList Encounter::GetConversationIds()
{
    return QStringList(conversationFromIdMap.keys());
}

QStringList Encounter::GetConversationDisplayNames()
{
    QStringList displayNames;

    for (QString conversationId : conversationFromIdMap.keys())
    {
        displayNames.append(conversationFromIdMap[conversationId]->GetDisplayName());
    }

    return displayNames;
}

void Encounter::AddInterrogation(Interrogation *pElement, QString id)
{
    pElement->SetOwningEncounter(this);
    interrogationFromIdMap.insert(id, pElement);
}

Interrogation * Encounter::GetInterrogationById(QString id)
{
    if (interrogationFromIdMap.contains(id))
    {
        return interrogationFromIdMap[id];
    }
    else
    {
        return NULL;
    }
}

QStringList Encounter::GetInterrogationIds()
{
    return QStringList(interrogationFromIdMap.keys());
}

QStringList Encounter::GetInterrogationDisplayNames()
{
    QStringList displayNames;

    for (QString interrogationId : interrogationFromIdMap.keys())
    {
        displayNames.append(interrogationFromIdMap[interrogationId]->GetDisplayName());
    }

    return displayNames;
}

void Encounter::AddConfrontation(Confrontation *pElement, QString id)
{
    pElement->SetOwningEncounter(this);
    confrontationFromIdMap.insert(id, pElement);
}

Confrontation * Encounter::GetConfrontationById(QString id)
{
    if (confrontationFromIdMap.contains(id))
    {
        return confrontationFromIdMap[id];
    }
    else
    {
        return NULL;
    }
}

QStringList Encounter::GetConfrontationIds()
{
    return QStringList(confrontationFromIdMap.keys());
}

QStringList Encounter::GetConfrontationDisplayNames()
{
    QStringList displayNames;

    for (QString confrontationId : confrontationFromIdMap.keys())
    {
        displayNames.append(confrontationFromIdMap[confrontationId]->GetDisplayName());
    }

    return displayNames;
}

void Encounter::SetOneShotConversation(Conversation *pElement)
{
    pElement->SetOwningEncounter(this);
    pOneShotConversation = pElement;
}

Conversation * Encounter::GetOneShotConversation()
{
    return pOneShotConversation;
}

EncounterSelector::EncounterSelector(QWidget *parent)
    : QComboBox(parent)
{
    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString EncounterSelector::GetId()
{
    return currentText();
}

void EncounterSelector::SetToId(const QString &id)
{
    int indexOfCurrentEncounter = encounterIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCurrentEncounter >= 0)
    {
        setCurrentIndex(indexOfCurrentEncounter);
    }
    else
    {
        setCurrentIndex(0);
    }
}

void EncounterSelector::Reset()
{
    encounterIds = CaseContent::GetInstance()->GetIds<Encounter>();
    previousIndex = -1;

    clear();
    addItems(encounterIds);
}

void EncounterSelector::CurrentIndexChanged(int currentIndex)
{
    previousIndex = currentIndex;

    if (currentIndex >= 0)
    {
        emit EncounterSelected(encounterIds[currentIndex]);
    }
    else
    {
        emit EncounterSelected("");
    }
}
