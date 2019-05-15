#ifndef ENCOUNTER_STAGING_H
#define ENCOUNTER_STAGING_H

#include "XmlReader.h"

#include <QString>
#include <QMap>
#include <QList>

namespace Staging
{

class Conversation;

class Encounter
{
public:
    Encounter();
    Encounter(XmlReader *pReader);
    ~Encounter();

    void AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap);

    QList<QString> ConversationIdList;
    QList<QString> InterrogationIdList;
    QList<QString> ConfrontationIdList;
    QMap<QString, Conversation *> PresentEvidenceConversationDictionary;

    QString Id;
    QString InitialLeftCharacterId;
    QString InitialLeftCharacterEmotionId;
    QString InitialRightCharacterId;
    QString InitialRightCharacterEmotionId;

    QString InitialConversationId;
    QString OneShotConversationId;

    Conversation *pOneShotConversation;
    bool OwnsOneShotConversation;
    Conversation *pPresentWrongEvidenceConversation;
    Conversation *pPresentWrongProfileConversation;
};

}

#endif // ENCOUNTER_H
