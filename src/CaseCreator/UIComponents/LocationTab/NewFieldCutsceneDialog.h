#ifndef NEWFIELDCUTSCENEDIALOG_H
#define NEWFIELDCUTSCENEDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

#include <QString>

class FieldCutscene;

class NewFieldCutsceneDialog : public NewObjectDialog<FieldCutscene>
{
    Q_OBJECT

public:
    explicit NewFieldCutsceneDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    FieldCutscene * GetNewObject();

protected:
    void InitFields();
    bool ValidateFields();

private:
    bool ValidateId();
    bool ValidateSpriteFilePath(QString spriteFilePath);

    QLabel *pIdPromptLabel;
    QLineEdit *pIdLineEdit;
    QLabel *pIdErrorLabel;

    QString id;
};

#endif // NEWFIELDCUTSCENEDIALOG_H
