#ifndef BACKGROUNDMUSIC_H
#define BACKGROUNDMUSIC_H

#include "XmlStorableObject.h"

#include <QString>
#include <QComboBox>

class XmlReader;
class XmlWriter;

class BackgroundMusic : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(BackgroundMusic)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_TEXT(filePath)
    END_XML_STORABLE_OBJECT()

public:
    BackgroundMusic() { }

    BackgroundMusic(const QString &id, const QString &filePath)
    {
        this->id = id;
        this->filePath = filePath;
    }

    virtual ~BackgroundMusic() { }

    QString GetId() { return id; }
    QString GetDisplayName() { return id; }

private:
    QString id;
    QString filePath;
};

class BackgroundMusicSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit BackgroundMusicSelector(QWidget *parent = 0);

    QString GetId();
    void SetToId(const QString &id);

signals:
    void BackgroundMusicSelected(const QString &bgmId);

public slots:
    void CurrentIndexChanged(int currentIndex);

private:
    QStringList bgmIds;
    int previousIndex;
};

#endif // BACKGROUNDMUSIC_H
