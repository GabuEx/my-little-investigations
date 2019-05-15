#ifndef EVIDENCE_H
#define EVIDENCE_H

#include "XmlStorableObject.h"
#include "CaseCreator/UIComponents/TemplateHelpers/NewObjectDialog.h"
#include "Staging/Evidence.Staging.h"

#include <QComboBox>

class Evidence : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(Evidence)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_TEXT(name)
        XML_STORABLE_TEXT(description)
        XML_STORABLE_BOOL(isProfile)
        XML_STORABLE_FILE_PATH(smallSpriteFilePath)
        XML_STORABLE_FILE_PATH(largeSpriteFilePath)
    END_XML_STORABLE_OBJECT()

public:
    Evidence() { }

    Evidence(const QString &id);
    Evidence(Staging::Evidence *pStagingEvidence);

    virtual ~Evidence() { }

    QString GetId() { return id; }
    QString GetDisplayName() { return name; }

private:
    QString smallSpriteFilePath;
    QString largeSpriteFilePath;

    QString id;
    QString name;
    QString description;
    bool isProfile;
};

class EvidenceSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit EvidenceSelector(QWidget *parent = 0);

    QString GetId();
    void SetToId(const QString &id);

signals:
    void EvidenceSelected(const QString &evidenceId);

public slots:
    void CurrentIndexChanged(int currentIndex);

private:
    QStringList evidenceIds;
    int previousIndex;
};

#endif // EVIDENCE_H
