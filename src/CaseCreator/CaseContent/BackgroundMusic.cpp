#include "BackgroundMusic.h"

#include "CaseContent.h"

BackgroundMusicSelector::BackgroundMusicSelector(QWidget *parent)
    : QComboBox(parent)
{
    bgmIds = CaseContent::GetInstance()->GetIds<BackgroundMusic>();
    previousIndex = -1;

    addItems(bgmIds);

    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString BackgroundMusicSelector::GetId()
{
    return currentText();
}

void BackgroundMusicSelector::SetToId(const QString &id)
{
    int indexOfCurrentEvidence = bgmIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCurrentEvidence >= 0)
    {
        setCurrentIndex(indexOfCurrentEvidence);
    }
}

void BackgroundMusicSelector::CurrentIndexChanged(int currentIndex)
{
    previousIndex = currentIndex;
    emit BackgroundMusicSelected(bgmIds[currentIndex]);
}
