#ifndef NEWCHARACTERFIELDSPRITEDIALOG_H
#define NEWCHARACTERFIELDSPRITEDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"
#include "CaseCreator/CaseContent/Character.h"

class NewCharacterFieldSpriteDialog : public NewObjectDialog<Character::FieldSprite>
{
public:
    NewCharacterFieldSpriteDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Character::FieldSprite * GetNewObject() override;

protected:
    void InitFields();
    bool ValidateFields();
};

#endif // NEWCHARACTERFIELDSPRITEDIALOG_H
