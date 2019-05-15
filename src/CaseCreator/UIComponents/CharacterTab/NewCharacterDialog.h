#ifndef NEWCHARACTERDIALOG_H
#define NEWCHARACTERDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

#include <QString>

class Character;

class NewCharacterDialog : public NewObjectDialog<Character>
{
    Q_OBJECT

public:
    NewCharacterDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Character * GetNewObject();

protected:
    void InitFields();
    bool ValidateFields();

private:
    bool ValidateId();
    bool ValidateName();

    QLabel *pIdPromptLabel;
    QLineEdit *pIdLineEdit;
    QLabel *pIdErrorLabel;

    QLabel *pNamePromptLabel;
    QLineEdit *pNameLineEdit;
    QLabel *pNameErrorLabel;

    QString id;
    QString name;
};

#endif // NEWCHARACTERDIALOG_H
