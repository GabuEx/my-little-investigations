#include "NewCharacterFieldSpriteDialog.h"

#include "MLIException.h"

template<>
NewObjectDialog<Character::FieldSprite> * NewObjectDialog<Character::FieldSprite>::Create(QWidget *parent, Qt::WindowFlags flags)
{
    return new NewCharacterFieldSpriteDialog(parent, flags);
}

NewCharacterFieldSpriteDialog::NewCharacterFieldSpriteDialog(QWidget *parent, Qt::WindowFlags flags)
    : NewObjectDialog<Character::FieldSprite>(parent, flags)
{
    throw new MLIException("We should never create a new field sprite for a character.");
}

Character::FieldSprite * NewCharacterFieldSpriteDialog::GetNewObject()
{
    throw new MLIException("We should never create a new field sprite for a character.");
}

void NewCharacterFieldSpriteDialog::InitFields()
{
    throw new MLIException("We should never create a new field sprite for a character.");
}

bool NewCharacterFieldSpriteDialog::ValidateFields()
{
    throw new MLIException("We should never create a new field sprite for a character.");
}
