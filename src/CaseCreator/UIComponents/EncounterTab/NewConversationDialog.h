#ifndef NEWCONVERSATIONDIALOG_H
#define NEWCONVERSATIONDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

class Conversation;

class NewConversationDialog : public NewObjectDialog<Conversation>
{
    Q_OBJECT
public:
    explicit NewConversationDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Conversation * GetNewObject();

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

#endif // NEWCONVERSATIONDIALOG_H
