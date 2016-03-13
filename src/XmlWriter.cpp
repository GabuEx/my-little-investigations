/**
 * Facilitates the writing of XML files.
 *
 * @author GabuEx, dawnmew
 * @since 1.0
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2014 Equestrian Dreamers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "XmlWriter.h"
#include "Utils.h"

#include <fstream>

#ifndef CASE_CREATOR
#include <cryptopp/base64.h>
#include <cryptopp/sha.h>
#else
#include "CaseCreator/CaseContent/CaseContent.h"
#endif

XmlWriter::XmlWriter(const char *pFilePath, const char *pFilePathExtension, bool makeHumanReadable, int formattingVersion)
{
    filePath = string(pFilePath);

    if (pFilePathExtension != NULL)
    {
        filePathExtension = string(pFilePathExtension);
    }

    this->makeHumanReadable = makeHumanReadable;
    indentLevel = 0;

    stringStream.str("");
    stringStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    if (makeHumanReadable)
    {
        stringStream << endl;
    }

    WriteIntElement("FormattingVersion", formattingVersion);
}

XmlWriter::~XmlWriter()
{
    string fileContents = stringStream.str();
    string fullFilePath = filePath;

#ifndef CASE_CREATOR
    // If we have an extension, that means that the file path is just a directory,
    // and that we want to hash the file contents to generate a file name.
    if (filePathExtension.length() > 0)
    {
        CryptoPP::SHA256 sha256;
        byte hash[CryptoPP::SHA256::DIGESTSIZE];
        sha256.CalculateDigest(hash, reinterpret_cast<const byte *>(fileContents.c_str()), fileContents.length());
        fullFilePath += UuidFromSHA256Hash(hash) + filePathExtension;
    }
#endif

    ofstream fileStream;
    fileStream.open(fullFilePath.c_str(), ios_base::out | ios_base::trunc);
    fileStream << fileContents;
    fileStream.close();
}

void XmlWriter::StartElement(const XmlString &elementName)
{
    StartElement(elementName, true /* addCarriageReturn */);
}

void XmlWriter::StartElement(const XmlString &elementName, bool addCarriageReturn)
{
    if (makeHumanReadable)
    {
        for (int i = 0; i < indentLevel; i++)
        {
            stringStream << "    ";
        }
    }

    stringStream << "<" << XmlStringToCharArray(elementName) << ">";

    if (makeHumanReadable && addCarriageReturn)
    {
        stringStream << endl;
        indentLevel++;
    }

    elementNameStack.push(elementName);
    shouldUnindentStack.push(addCarriageReturn);
}

void XmlWriter::VerifyCurrentElement(const XmlString &expectedElementName)
{
#ifdef QT_DEBUG
    XmlString actualCurrentElement;

    if (elementNameStack.empty() || expectedElementName != (actualCurrentElement = XmlStringToCharArray(elementNameStack.top())))
    {
        char buffer[256];

        snprintf(buffer, 256, "XML: Expected element named '%s', instead found '%s'.",
                XmlStringToCharArray(expectedElementName),
                (elementNameStack.empty() ? "NULL" : XmlStringToCharArray(actualCurrentElement)));

        ThrowException(buffer);
    }
#endif
}

void XmlWriter::EndElement()
{
    bool shouldUnindent = shouldUnindentStack.top();
    shouldUnindentStack.pop();

    if (shouldUnindent)
    {
        indentLevel--;
    }

    if (makeHumanReadable && shouldUnindent)
    {
        for (int i = 0; i < indentLevel; i++)
        {
            stringStream << "    ";
        }
    }

    stringStream << "</" << XmlStringToCharArray(elementNameStack.top()) << ">";
    elementNameStack.pop();

    if (makeHumanReadable)
    {
        stringStream << endl;
    }
}

void XmlWriter::WriteEmptyElement(const XmlString &elementName)
{
    if (makeHumanReadable)
    {
        for (int i = 0; i < indentLevel; i++)
        {
            stringStream << "    ";
        }
    }

    stringStream << "<" << XmlStringToCharArray(elementName) << " />";

    if (makeHumanReadable)
    {
        stringStream << endl;
    }
}

void XmlWriter::WriteIntElement(const XmlString &elementName, int elementValue)
{
    StartElement(elementName, false /* addCarriageReturn */);
    stringStream << elementValue;
    EndElement();
}

void XmlWriter::WriteDoubleElement(const XmlString &elementName, double elementValue)
{
    StartElement(elementName, false /* addCarriageReturn */);
    stringStream << elementValue;
    EndElement();
}

void XmlWriter::WriteBooleanElement(const XmlString &elementName, bool elementValue)
{
    StartElement(elementName, false /* addCarriageReturn */);
    stringStream << (elementValue ? "true" : "false");
    EndElement();
}

void XmlWriter::WriteTextElement(const XmlString &elementName, const XmlString &elementValue)
{
    StartElement(elementName, false /* addCarriageReturn */);
    stringStream << XmlStringToCharArray(elementValue);
    EndElement();
}

#ifdef CASE_CREATOR
void XmlWriter::WriteFilePathElement(const XmlString &elementName, const XmlString &elementValue)
{
    // We always store file paths as relative paths but use them as absolute paths,
    // so convert this to a relative path before returning it.
    WriteTextElement(elementName, CaseContent::GetInstance()->AbsolutePathToRelativePath(elementValue));
}

#endif

#ifndef CASE_CREATOR
void XmlWriter::WritePngElement(const XmlString &elementName, void *pElementValue, size_t elementSize)
#else
void XmlWriter::WritePngElement(const XmlString &, void *, size_t )
#endif
{
#ifndef CASE_CREATOR
    char *pElementString = reinterpret_cast<char *>(pElementValue);

    string encodedString;
    CryptoPP::StringSource(string(pElementString, elementSize), true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encodedString)));

    StartElement(elementName);
    stringStream << encodedString;
    EndElement();
#endif
}
