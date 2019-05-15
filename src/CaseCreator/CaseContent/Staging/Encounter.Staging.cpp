#include "Encounter.Staging.h"

#include "Conversation.Staging.h"

Staging::Encounter::Encounter()
{
    pOneShotConversation = NULL;
    pPresentWrongEvidenceConversation = NULL;
    pPresentWrongProfileConversation = NULL;

    OwnsOneShotConversation = false;
}

Staging::Encounter::Encounter(XmlReader *pReader)
{
    pOneShotConversation = NULL;
    pPresentWrongEvidenceConversation = NULL;
    pPresentWrongProfileConversation = NULL;

    OwnsOneShotConversation = false;

    pReader->StartElement("Encounter");

    if (pReader->ElementExists("Id"))
    {
        Id = pReader->ReadTextElement("Id");
    }

    if (pReader->ElementExists("InitialLeftCharacterId"))
    {
        InitialLeftCharacterId = pReader->ReadTextElement("InitialLeftCharacterId");
    }

    if (pReader->ElementExists("InitialLeftCharacterEmotionId"))
    {
        InitialLeftCharacterEmotionId = pReader->ReadTextElement("InitialLeftCharacterEmotionId");
    }

    if (pReader->ElementExists("InitialRightCharacterId"))
    {
        InitialRightCharacterId = pReader->ReadTextElement("InitialRightCharacterId");
    }

    if (pReader->ElementExists("InitialRightCharacterEmotionId"))
    {
        InitialRightCharacterEmotionId = pReader->ReadTextElement("InitialRightCharacterEmotionId");
    }

    if (pReader->ElementExists("OneShotConversation"))
    {
        pReader->StartElement("OneShotConversation");
        pOneShotConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();

        OwnsOneShotConversation = true;
    }

    if (pReader->ElementExists("PresentWrongEvidenceConversation"))
    {
        pReader->StartElement("PresentWrongEvidenceConversation");
        pPresentWrongEvidenceConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();
    }

    if (pReader->ElementExists("PresentWrongProfileConversation"))
    {
        pReader->StartElement("PresentWrongProfileConversation");
        pPresentWrongProfileConversation = Conversation::LoadFromXml(pReader);
        pReader->EndElement();
    }

    if (pReader->ElementExists("InitialConversationId"))
    {
        InitialConversationId = pReader->ReadTextElement("InitialConversationId");
    }

    if (pReader->ElementExists("OneShotConversationId") && pOneShotConversation == NULL)
    {
        OneShotConversationId = pReader->ReadTextElement("OneShotConversationId");
    }

    pReader->StartElement("ConversationList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ConversationIdList.push_back(pReader->ReadTextElement("ConversationId"));
    }

    pReader->EndElement();

    pReader->StartElement("InterrogationList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        InterrogationIdList.push_back(pReader->ReadTextElement("InterrogationId"));
    }

    pReader->EndElement();

    pReader->StartElement("ConfrontationList");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ConfrontationIdList.push_back(pReader->ReadTextElement("ConfrontationId"));
    }

    pReader->EndElement();

    pReader->StartElement("PresentEvidenceConversationDictionary");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        QString evidenceId = pReader->ReadTextElement("EvidenceId");
        Conversation *pConversation = Conversation::LoadFromXml(pReader);

        PresentEvidenceConversationDictionary[evidenceId] = pConversation;
    }

    pReader->EndElement();

    pReader->EndElement();
}

Staging::Encounter::~Encounter()
{
    for (QMap<QString, Conversation *>::iterator iter = PresentEvidenceConversationDictionary.begin(); iter != PresentEvidenceConversationDictionary.end(); ++iter)
    {
        delete iter.value();
    }

    PresentEvidenceConversationDictionary.clear();

    if (OwnsOneShotConversation)
    {
        delete pOneShotConversation;
        pOneShotConversation = NULL;
    }

    delete pPresentWrongEvidenceConversation;
    pPresentWrongEvidenceConversation = NULL;
    delete pPresentWrongProfileConversation;
    pPresentWrongProfileConversation = NULL;
}

void Staging::Encounter::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    for (QMap<QString, Conversation *>::iterator iter = PresentEvidenceConversationDictionary.begin(); iter != PresentEvidenceConversationDictionary.end(); ++iter)
    {
        iter.value()->AddDialogPaths(dialogIdToSavePathMap);
    }

    if (pOneShotConversation != NULL && OwnsOneShotConversation)
    {
        pOneShotConversation->AddDialogPaths(dialogIdToSavePathMap);
    }

    if (pPresentWrongEvidenceConversation != NULL)
    {
        pPresentWrongEvidenceConversation->AddDialogPaths(dialogIdToSavePathMap);
    }

    if (pPresentWrongProfileConversation != NULL)
    {
        pPresentWrongProfileConversation->AddDialogPaths(dialogIdToSavePathMap);
    }
}
