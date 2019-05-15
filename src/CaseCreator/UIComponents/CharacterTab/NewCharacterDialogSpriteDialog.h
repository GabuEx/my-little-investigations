#ifndef NEWCHARACTERDIALOGSPRITEDIALOG_H
#define NEWCHARACTERDIALOGSPRITEDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"
#include "CaseCreator/CaseContent/Character.h"

#include <QLabel>
#include <QLineEdit>

class NewCharacterDialogSpriteDialog : public NewObjectDialog<Character::DialogEmotion>
{
    Q_OBJECT

public:
    NewCharacterDialogSpriteDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Character::DialogEmotion * GetNewObject();

protected:
    void InitFields();
    bool ValidateFields();

private:
    bool ValidateId();

    QLabel *pIdPromptLabel;
    QLineEdit *pIdLineEdit;
    QLabel *pIdErrorLabel;

    QString id;
};

#endif // NEWCHARACTERDIALOGSPRITEDIALOG_H
