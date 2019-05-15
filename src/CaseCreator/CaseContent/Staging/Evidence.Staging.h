#ifndef EVIDENCE_STAGING_H
#define EVIDENCE_STAGING_H

#include "XmlReader.h"

#include <QMap>
#include <QString>

namespace Staging
{

class Evidence
{
public:
    Evidence(QString id, XmlReader *pReader);

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap);

    QString SmallSpriteId;
    QString LargeSpriteId;

    QString Id;
    QString Name;
    QString Description;
    bool IsProfile;
    bool IsEnabled;
    bool IsHidden;
};

class EvidenceIdPair
{
public:
    EvidenceIdPair(XmlReader *pReader);

    bool operator< (const EvidenceIdPair &other) const;

    QString EvidenceId1;
    QString EvidenceId2;
};

}

#endif // EVIDENCE_STAGING_H
