#ifndef NEWFOREGROUNDELEMENTDIALOG_H
#define NEWFOREGROUNDELEMENTDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

#include <QString>

class ForegroundElement;

class NewForegroundElementDialog : public NewObjectDialog<ForegroundElement>
{
    Q_OBJECT

public:
    explicit NewForegroundElementDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    ForegroundElement * GetNewObject();

public slots:
    void RetrieveSpriteFilePath();

protected:
    void InitFields();
    bool ValidateFields();

private:
    bool ValidateName();
    bool ValidateSpriteFilePath(QString spriteFilePath);

    QLabel *pNamePromptLabel;
    QLineEdit *pNameLineEdit;
    QLabel *pNameErrorLabel;

    QLabel *pSpriteFilePathPromptLabel;
    QLabel *pSpriteFilePathLabel;
    QLabel *pSpriteFilePathErrorLabel;

    QString name;
    QString spriteFilePath;
};

#endif // NEWFOREGROUNDELEMENTDIALOG_H
