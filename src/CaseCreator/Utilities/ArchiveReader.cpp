#include "ArchiveReader.h"

#include "XmlReader.h"

ArchiveReader::ArchiveReader()
    : zipArchive(mz_zip_archive())
{
}

ArchiveReader::~ArchiveReader()
{
    mz_zip_reader_end(&zipArchive);
}

bool ArchiveReader::Init(const QString &archiveFilePath)
{
    if (mz_zip_reader_init_file(&zipArchive, archiveFilePath.toStdString().c_str(), 0) > 0)
    {
        XmlReader languageFileReader;
        languageFileReader.ParseXmlContent(LoadText(QString("languages.xml")));

        languageFileReader.StartElement("Languages");
        languageFileReader.StartList("Language");

        while (languageFileReader.MoveToNextListItem())
        {
            QString language = languageFileReader.ReadText();
            languages.push_back(language);

            if (languageFileReader.AttributeExists("IsBase") && languageFileReader.ReadBooleanAttribute("IsBase") == true)
            {
                baseLanguage = language;
                currentLanguage = language;
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}

void ArchiveReader::SetLanguage(const QString &language)
{
    if (languages.contains(language))
    {
        currentLanguage = language;
    }
}

void * ArchiveReader::LoadFile(const QString &relativeFilePath, unsigned int *pSize)
{
    size_t uncompressedSize = 0;
    void *p = NULL;

    *pSize = 0;

    if (currentLanguage.length() == 0)
    {
        p = mz_zip_reader_extract_file_to_heap(&zipArchive, relativeFilePath.toStdString().c_str(), &uncompressedSize, 0);
    }
    else
    {
        p = mz_zip_reader_extract_file_to_heap(&zipArchive, (currentLanguage + "/" + relativeFilePath).toStdString().c_str(), &uncompressedSize, 0);

        if (p == NULL && currentLanguage != baseLanguage)
        {
            p = mz_zip_reader_extract_file_to_heap(&zipArchive, (baseLanguage + "/" + relativeFilePath).toStdString().c_str(), &uncompressedSize, 0);
        }
    }

    if (p == NULL)
    {
        return NULL;
    }

    *pSize = (unsigned int)uncompressedSize;
    return p;
}

QString ArchiveReader::LoadText(const QString &relativeFilePath)
{
    unsigned int size = 0;
    void *p = LoadFile(relativeFilePath, &size);

    QString text = QString::fromUtf8(reinterpret_cast<char *>(p), size);

    free(p);

    return text;
}

QImage ArchiveReader::LoadImage(const QString &relativeFilePath, const char *pFormat)
{
    unsigned int size = 0;
    void *p = LoadFile(relativeFilePath, &size);

    QImage image;
    image.loadFromData(reinterpret_cast<uchar *>(p), size, pFormat);

    free(p);

    return image;
}
