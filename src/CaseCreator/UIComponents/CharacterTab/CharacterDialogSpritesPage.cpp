#include "CharacterDialogSpritesPage.h"

#include <QVBoxLayout>

CharacterDialogSpritesPage::CharacterDialogSpritesPage(QWidget *parent) :
    Page<Character>(parent)
{
    isActive = false;

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pEmotionManipulator = new ListManipulator<Character::DialogEmotion>();
    pMainLayout->addWidget(pEmotionManipulator);

    setLayout(pMainLayout);
}

void CharacterDialogSpritesPage::Init(Character *pObject)
{
    if (pObject == NULL)
    {
        return;
    }

    Page<Character>::Init(pObject);
    pEmotionManipulator->SetParentObject(pObject);
}

void CharacterDialogSpritesPage::Reset()
{
    pEmotionManipulator->Reset();
}

bool CharacterDialogSpritesPage::GetIsActive()
{
    return isActive;
}

void CharacterDialogSpritesPage::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pEmotionManipulator->SetIsActive(isActive);
    }
}
