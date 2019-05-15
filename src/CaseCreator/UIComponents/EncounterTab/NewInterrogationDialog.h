#ifndef NEWINTERROGATIONDIALOG_H
#define NEWINTERROGATIONDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

class Interrogation;

class NewInterrogationDialog : public NewObjectDialog<Interrogation>
{
    Q_OBJECT
public:
    explicit NewInterrogationDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Interrogation * GetNewObject();

signals:

public slots:

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

#endif // NEWINTERROGATIONDIALOG_H
