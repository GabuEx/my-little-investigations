#include "SoundEffect.h"

#include "CaseContent.h"
#include "CaseCreator/Utilities/AudioPlayer.h"

void SoundEffect::Preload()
{
    AudioPlayer::PreloadSound(id, filePath);
}

void SoundEffect::Play()
{
    AudioPlayer::PlaySound(id);
}

SoundEffectSelector::SoundEffectSelector(QWidget *parent)
    : QComboBox(parent)
{
    sfxIds = CaseContent::GetInstance()->GetIds<SoundEffect>();
    previousIndex = -1;

    addItems(sfxIds);

    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString SoundEffectSelector::GetId()
{
    return currentText();
}

void SoundEffectSelector::SetToId(const QString &id)
{
    int indexOfCurrentEvidence = sfxIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCurrentEvidence >= 0)
    {
        setCurrentIndex(indexOfCurrentEvidence);
    }
}

void SoundEffectSelector::CurrentIndexChanged(int currentIndex)
{
    previousIndex = currentIndex;
    emit SoundEffectSelected(sfxIds[currentIndex]);
}

