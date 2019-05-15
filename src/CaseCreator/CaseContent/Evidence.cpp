#include "Evidence.h"

#include "CaseContent.h"
#include "Staging/CaseContentLoadingStager.h"

Evidence::Evidence(const QString &id)
{
    this->id = id;
}

Evidence::Evidence(Staging::Evidence *pStagingEvidence)
{
    smallSpriteFilePath = CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(pStagingEvidence->SmallSpriteId);
    largeSpriteFilePath = CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(pStagingEvidence->LargeSpriteId);

    id = pStagingEvidence->Id;
    name = pStagingEvidence->Name;
    description = pStagingEvidence->Description;
    isProfile = pStagingEvidence->IsProfile;
}

EvidenceSelector::EvidenceSelector(QWidget *parent)
    : QComboBox(parent)
{
    evidenceIds = CaseContent::GetInstance()->GetIds<Evidence>();
    previousIndex = -1;

    addItems(evidenceIds);

    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString EvidenceSelector::GetId()
{
    return currentText();
}

void EvidenceSelector::SetToId(const QString &id)
{
    int indexOfCurrentEvidence = evidenceIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCurrentEvidence >= 0)
    {
        setCurrentIndex(indexOfCurrentEvidence);
    }
}

void EvidenceSelector::CurrentIndexChanged(int currentIndex)
{
    previousIndex = currentIndex;
    emit EvidenceSelected(evidenceIds[currentIndex]);
}
