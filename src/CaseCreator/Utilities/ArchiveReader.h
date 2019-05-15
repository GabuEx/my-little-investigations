#ifndef ARCHIVEREADER_H
#define ARCHIVEREADER_H

#include "miniz.h"

#include <QString>
#include <QStringList>
#include <QImage>

class ArchiveReader
{
public:
    ArchiveReader();
    ~ArchiveReader();

    bool Init(const QString &archiveFilePath);

    const QStringList & GetSupportedLanguages() { return languages; }
    void SetLanguage(const QString &language);

    void * LoadFile(const QString &relativeFilePath, unsigned int *pSize);
    QString LoadText(const QString &relativeFilePath);
    QImage LoadImage(const QString &relativeFilePath, const char *pFormat = "png");

private:
    mz_zip_archive zipArchive;

    QStringList languages;
    QString baseLanguage;
    QString currentLanguage;
};

#endif // ARCHIVEREADER_H
