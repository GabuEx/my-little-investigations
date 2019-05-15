#include "Utilities.h"

#include <QStringList>

#include "MLIException.h"

void SplitConversationIdFromCaseFile(const QString &mergedConversationId, QString *pEncounterId, QString *pConversationId)
{
    QString encounterId;
    QString conversationId;

    if (mergedConversationId.length() > 0)
    {
        QStringList conversationIdSplit = mergedConversationId.split('_');

        // If the conversation is in an encounter, then it will have the encounter ID prefacing it.
        // In that case, we want to remove it.  Additionally, if it's an interrogation or confrontation
        // in this circumstance, then it will have that fact appended onto it.
        // Otherwise, it's a conversation external to an encounter (e.g., in a cutscene),
        // in which case we'll just use the ID as-is.
        if (conversationIdSplit.size() == 2 || conversationIdSplit.size() == 3)
        {
            if (conversationIdSplit.size() == 3 && conversationIdSplit[2] != "Interrogation" && conversationIdSplit[2] != "Confrontation")
            {
                throw new MLIException("We expect every conversation ID from the case file with two differentiators to have the second read either \"Interrogation\" or \"Confrontation\".");
            }

            encounterId = conversationIdSplit[0];
            conversationId = conversationIdSplit[1];
        }
        else
        {
            if (conversationIdSplit.size() != 1)
            {
                throw new MLIException("We expect every conversation ID from the case file to have between one and three sections.");
            }

            encounterId = "";
            conversationId = mergedConversationId;
        }
    }
    else
    {
        encounterId = "";
        conversationId = "";
    }

    if (pEncounterId != NULL)
    {
        *pEncounterId = encounterId;
    }

    if (pConversationId != NULL)
    {
        *pConversationId = conversationId;
    }
}
