#ifndef CHARACTERDIALOGSPRITESPAGE_H
#define CHARACTERDIALOGSPRITESPAGE_H

#include "../BaseTypes/Page.h"
#include "../TemplateHelpers/ListManipulator.h"

#include "CaseCreator/CaseContent/Character.h"

class CharacterDialogSpritesPage : public Page<Character>
{
    Q_OBJECT

public:
    CharacterDialogSpritesPage(QWidget *parent = 0);

    void Init(Character *pObject);
    void Reset();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    ListManipulator<Character::DialogEmotion> *pEmotionManipulator;
    bool isActive;
};

#endif // CHARACTERDIALOGSPRITESPAGE_H
