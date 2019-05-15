#include "Evidence.Staging.h"

Staging::Evidence::Evidence(QString id, XmlReader *pReader)
{
    Id = id;

    pReader->StartElement("Evidence");
    SmallSpriteId = pReader->ReadTextElement("SmallSpriteId");
    LargeSpriteId = pReader->ReadTextElement("LargeSpriteId");
    Name = pReader->ReadTextElement("Name");
    Description = pReader->ReadTextElement("Description");
    IsProfile = pReader->ReadBooleanElement("IsProfile");
    IsEnabled = pReader->ReadBooleanElement("IsEnabled");
    IsHidden = pReader->ReadBooleanElement("IsHidden");
    pReader->EndElement();
}

void Staging::Evidence::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap)
{
    QString baseDir = "Evidence/";

    spriteIdToSavePathMap[SmallSpriteId] = baseDir + Id + "Small.png";
    spriteIdToSavePathMap[LargeSpriteId] = baseDir + Id + "Large.png";
}

Staging::EvidenceIdPair::EvidenceIdPair(XmlReader *pReader)
{
    pReader->StartElement("EvidenceIdPair");
    EvidenceId1 = pReader->ReadTextElement("EvidenceId1");
    EvidenceId2 = pReader->ReadTextElement("EvidenceId2");
    pReader->EndElement();
}

bool Staging::EvidenceIdPair::operator< (const EvidenceIdPair &other) const
{
    return (EvidenceId1 + EvidenceId2) < (other.EvidenceId1 + other.EvidenceId2);
}
