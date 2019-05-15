#ifndef FLAG_H
#define FLAG_H

#include "XmlStorableObject.h"
#include "CaseCreator/UIComponents/TemplateHelpers/NewObjectDialog.h"
#include "Staging/Flag.Staging.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QStringList>

class XmlReader;
class XmlWriter;

class Flag : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(Flag)
        XML_STORABLE_TEXT(id)
    END_XML_STORABLE_OBJECT()

public:
    Flag() { }

    Flag(const QString &id);
    Flag(Staging::Flag *pStagingFlag);

    virtual ~Flag() { }

    QString GetId() { return id; }
    QString GetDisplayName() { return id; }

private:
    QString id;
};

class NewFlagDialog : public NewObjectDialog<Flag>
{
    Q_OBJECT

public:
    NewFlagDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Flag * GetNewObject();

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

class FlagEditor : public QComboBox
{
    Q_OBJECT

public:
    explicit FlagEditor(QWidget *parent = 0);

    QString GetId();
    void SetToId(const QString &id);

signals:
    void FlagSelected(const QString &flagId);

public slots:
    void CurrentIndexChanged(int currentIndex);

private:
    QStringList flagIds;
    int previousIndex;
    bool creatingNewFlag;
};

#endif // FLAG_H
