#include "CharacterFieldSpritesPage.h"

CharacterFieldSpritesPage::CharacterFieldSpritesPage(QWidget *parent) :
    Page<Character>(parent)
{
    isActive = false;

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pFieldSpriteManipulator = new ListManipulator<Character::FieldSprite>();
    pMainLayout->addWidget(pFieldSpriteManipulator);

    setLayout(pMainLayout);
}

void CharacterFieldSpritesPage::Init(Character *pObject)
{
    if (pObject == NULL)
    {
        return;
    }

    Page<Character>::Init(pObject);
    pFieldSpriteManipulator->SetParentObject(pObject);
}

void CharacterFieldSpritesPage::Reset()
{
    pFieldSpriteManipulator->Reset();
}

bool CharacterFieldSpritesPage::GetIsActive()
{
    return isActive;
}

void CharacterFieldSpritesPage::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pFieldSpriteManipulator->SetIsActive(isActive);
    }
}
