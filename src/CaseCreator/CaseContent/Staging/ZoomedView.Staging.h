#ifndef ZOOMEDVIEW_STAGING_H
#define ZOOMEDVIEW_STAGING_H

#include "XmlReader.h"

#include <QList>
#include <QString>

namespace Staging
{

class ForegroundElement;

class ZoomedView
{
public:
    ZoomedView(XmlReader *pReader);
    ~ZoomedView();

    void AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString baseDir);

    QString Id;

    QList<ForegroundElement *> ForegroundElementList;

    QString BackgroundSpriteId;
};

}

#endif // ZOOMEDVIEW_STAGING_H
