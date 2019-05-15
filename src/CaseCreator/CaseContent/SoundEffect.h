#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

#include "XmlStorableObject.h"

#include <QString>
#include <QComboBox>

class XmlReader;
class XmlWriter;

class SoundEffect : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(SoundEffect)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_TEXT(filePath)
    END_XML_STORABLE_OBJECT()

public:
    SoundEffect() { }

    SoundEffect(const QString &id, const QString &filePath)
    {
        this->id = id;
        this->filePath = filePath;
    }

    virtual ~SoundEffect() { }

    QString GetId() { return id; }
    QString GetDisplayName() { return id; }

    void Preload();
    void Play();

private:
    QString id;
    QString filePath;
};

class SoundEffectSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit SoundEffectSelector(QWidget *parent = 0);

    QString GetId();
    void SetToId(const QString &id);

signals:
    void SoundEffectSelected(const QString &bgmId);

public slots:
    void CurrentIndexChanged(int currentIndex);

private:
    QStringList sfxIds;
    int previousIndex;
};

#endif // SOUNDEFFECT_H
