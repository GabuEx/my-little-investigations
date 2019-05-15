#ifndef NEWCONFRONTATIONDIALOG_H
#define NEWCONFRONTATIONDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

class Confrontation;

class NewConfrontationDialog : public NewObjectDialog<Confrontation>
{
    Q_OBJECT
public:
    explicit NewConfrontationDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Confrontation * GetNewObject();

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

#endif // NEWCONFRONTATIONDIALOG_H
