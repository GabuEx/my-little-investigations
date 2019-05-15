#ifndef NEWENCOUNTERDIALOG_H
#define NEWENCOUNTERDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

class Encounter;

class NewEncounterDialog : public NewObjectDialog<Encounter>
{
    Q_OBJECT
public:
    explicit NewEncounterDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Encounter * GetNewObject();

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

#endif // NEWENCOUNTERDIALOG_H
