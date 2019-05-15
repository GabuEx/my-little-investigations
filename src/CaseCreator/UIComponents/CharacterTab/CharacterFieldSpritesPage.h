#ifndef CHARACTERFIELDSPRITESPAGE_H
#define CHARACTERFIELDSPRITESPAGE_H

#include "../BaseTypes/Page.h"
#include "../TemplateHelpers/ListManipulator.h"

#include "CaseCreator/CaseContent/Character.h"

class CharacterFieldSpritesPage : public Page<Character>
{
    Q_OBJECT

public:
    CharacterFieldSpritesPage(QWidget *parent = 0);

    void Init(Character *pObject);
    void Reset();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    ListManipulator<Character::FieldSprite> *pFieldSpriteManipulator;
    bool isActive;
};

#endif // CHARACTERFIELDSPRITESPAGE_H
