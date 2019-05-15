#ifndef ARCHIVEWRITER_H
#define ARCHIVEWRITER_H

#include "miniz.h"

#include <QString>
#include <QImage>

class ArchiveWriter
{
public:
    ArchiveWriter();
    ~ArchiveWriter();

    bool Init(const QString &archiveFilePath);
    bool SaveFile(const QString &diskFilePath, const QString &relativeFilePath);
    bool SaveToFile(const QString &fileContents, const QString &relativeFilePath);
    bool SaveToFile(const char *pFileContents, unsigned int fileSize, const QString &relativeFilePath);
    bool Close();

private:
    mz_zip_archive zipArchive;
};

#endif // ARCHIVEWRITER_H
