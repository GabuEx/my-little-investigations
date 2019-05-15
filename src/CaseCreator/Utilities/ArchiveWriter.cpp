#include "ArchiveWriter.h"

#include <QFile>
#include <QDir>

ArchiveWriter::ArchiveWriter()
    : zipArchive(mz_zip_archive())
{
}

ArchiveWriter::~ArchiveWriter()
{
    mz_zip_writer_end(&zipArchive);
}

bool ArchiveWriter::Init(const QString &archiveFilePath)
{
    QDir archiveDir(archiveFilePath.left(archiveFilePath.lastIndexOf("/")));

    if (!archiveDir.mkpath("."))
    {
        return false;
    }

    return mz_zip_writer_init_file(&zipArchive, archiveFilePath.toStdString().c_str(), 0) > 0;
}

bool ArchiveWriter::SaveFile(const QString &diskFilePath, const QString &relativeFilePath)
{
    return mz_zip_writer_add_file(&zipArchive, relativeFilePath.toStdString().c_str(), diskFilePath.toStdString().c_str(), NULL, 0, 9);
}

bool ArchiveWriter::SaveToFile(const QString &fileContents, const QString &relativeFilePath)
{
    return SaveToFile(fileContents.toStdString().c_str(), fileContents.length(), relativeFilePath);
}

bool ArchiveWriter::SaveToFile(const char *pFileContents, unsigned int fileSize, const QString &relativeFilePath)
{
    return mz_zip_writer_add_mem(&zipArchive, relativeFilePath.toStdString().c_str(), pFileContents, fileSize, 9);
}

bool ArchiveWriter::Close()
{
    return mz_zip_writer_finalize_archive(&zipArchive);
}
