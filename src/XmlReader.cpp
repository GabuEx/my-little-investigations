/**
 * Facilitates the reading of XML files.
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

#include "XmlReader.h"

#ifndef CASE_CREATOR
#include "Image.h"

#include <cryptopp/base64.h>
#else
#include "CaseCreator/CaseContent/CaseContent.h"
#endif

#include "ResourceLoader.h"

using namespace tinyxml2;

XmlReader::XmlReader()
{
    pDocument = NULL;
    formattingVersion = 1;
}

XmlReader::XmlReader(const XmlString &filePath)
    : XmlReader()
{
    ParseXmlFile(filePath);
}

XmlReader::XmlReader(const XmlReader &other)
    : XmlReader()
{
    ParseXmlFile(other.filePath);
}

XmlReader::~XmlReader()
{
    delete pDocument;
    pDocument = NULL;
}

void XmlReader::ParseXmlFile(const XmlString &filePath)
{
    this->filePath = filePath;

    delete pDocument;
    pDocument = ResourceLoader::GetInstance()->LoadDocument(filePath);

    if (pDocument == NULL)
    {
        pDocument = new XMLDocument();

        if (pDocument->LoadFile(XmlStringToCharArray(filePath)) != XML_NO_ERROR)
        {
            delete pDocument;
            ThrowException(string("File not found: ") + string(XmlStringToCharArray(filePath)));
        }
    }

    Init(pDocument);
}

void XmlReader::ParseXmlContent(const XmlString &xmlContent)
{
    delete pDocument;
    pDocument = new XMLDocument();
    XMLError error = pDocument->Parse(XmlStringToCharArray(xmlContent));
    if (error != XML_NO_ERROR)
    {
        delete pDocument;
        ThrowException("XML: Error while parsing file.");
    }

    Init(pDocument);
}

void XmlReader::Init(XMLDocument *pDocument)
{
    pCurrentNode = dynamic_cast<XMLNode *>(pDocument);

    if (ElementExists("FormattingVersion"))
    {
        formattingVersion = ReadIntElement("FormattingVersion");
    }
}

void XmlReader::StartElement(const XmlString &elementName)
{
    XMLElement *pElement = pCurrentNode->FirstChildElement(XmlStringToCharArray(elementName));

    if (pElement == NULL)
    {
        // We didn't find the element - we should throw.
        ThrowException("XML: Element not found.");
    }

    pCurrentNode = dynamic_cast<XMLNode *>(pElement);
}

bool XmlReader::ElementExists(const XmlString &elementName)
{
    XMLElement *pElement = pCurrentNode->FirstChildElement(XmlStringToCharArray(elementName));

    return pElement != NULL;
}

void XmlReader::VerifyCurrentElement(const XmlString &expectedElementName)
{
#ifdef QT_DEBUG
    XMLElement *pCurrentElement = dynamic_cast<XMLElement *>(pCurrentNode);

    if (pCurrentElement == NULL || expectedElementName != XmlString(pCurrentElement->Name()))
    {
        char buffer[256];

        snprintf(buffer, 256, "XML: Expected element named '%s', instead found '%s'.",
                XmlStringToCharArray(expectedElementName),
                (pCurrentElement == NULL ? "NULL" : pCurrentElement->Name()));

        ThrowException(buffer);
    }
#endif
}

void XmlReader::EndElement()
{
    pCurrentNode = pCurrentNode->Parent();
}

void XmlReader::StartList(const XmlString &elementName)
{
    listStack.push(XMLList(elementName));
}

bool XmlReader::MoveToNextListItem()
{
    XMLList &topList = listStack.top();
    XMLElement *pElement = NULL;
    if (topList.started)
        pElement = pCurrentNode->NextSiblingElement(XmlStringToCharArray(topList.elementsName));
    else
        pElement = pCurrentNode->FirstChildElement(XmlStringToCharArray(topList.elementsName));

    if (pElement != NULL)
    {
        topList.started = true;
        pCurrentNode = dynamic_cast<XMLNode *>(pElement);
    }
    else
    {
        listStack.pop();
        if (topList.started)
            pCurrentNode = pCurrentNode->Parent();
    }

    return (pElement != NULL);

}

int XmlReader::ReadIntElement(const XmlString &elementName)
{
    int value;
    StartElement(elementName);
    value = ReadInt();
    EndElement();
    return value;
}

double XmlReader::ReadDoubleElement(const XmlString &elementName)
{
    double value;
    StartElement(elementName);
    value = ReadDouble();
    EndElement();
    return value;
}

bool XmlReader::ReadBooleanElement(const XmlString &elementName)
{
    bool value;
    StartElement(elementName);
    value = ReadBoolean();
    EndElement();
    return value;
}

XmlString XmlReader::ReadTextElement(const XmlString &elementName)
{
    XmlString value;
    StartElement(elementName);
    value = ReadText();
    EndElement();
    return value;
}

#if defined(GAME_EXECUTABLE) || defined(CASE_CREATOR)
XmlImage XmlReader::ReadPngElement(const XmlString &elementName)
{
    XmlImage value;
    StartElement(elementName);
    value = ReadPng();
    EndElement();
    return value;
}
#endif

#ifdef CASE_CREATOR
XmlString XmlReader::ReadFilePathElement(const XmlString &elementName)
{
    XmlString value;
    StartElement(elementName);
    value = ReadFilePath();
    EndElement();
    return value;
}
#endif

int XmlReader::ReadInt()
{
    int value;
    XMLError error = pCurrentNode->ToElement()->QueryIntText(&value);
    if (error != XML_NO_ERROR)
        ThrowException("XML error: expected int.");
    return value;
}

double XmlReader::ReadDouble()
{
    double value;
    XMLError error = pCurrentNode->ToElement()->QueryDoubleText(&value);
    if (error != XML_NO_ERROR)
        ThrowException("XML error: expected double.");
    return value;
}

bool XmlReader::ReadBoolean()
{
    bool value;
    XMLError error = pCurrentNode->ToElement()->QueryBoolText(&value);
    if (error != XML_NO_ERROR)
        ThrowException("XML error: expected Boolean.");
    return value;
}

XmlString XmlReader::ReadText()
{
    const char *value = pCurrentNode->ToElement()->GetText();
    // looks like tinyxml2 collapse "<tag> </tag>" and return NULL
    // with GetText() despite of PRESERVE_WHITESPACE flag
    if (value == NULL)
        return XmlString();
    else
        return XmlString(value);
}

#ifndef CASE_CREATOR
#ifdef GAME_EXECUTABLE
Image * XmlReader::ReadPng()
{
    string s = ReadText();

    string decodedString;
    CryptoPP::StringSource(s, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decodedString)));

    char *pDecodedString = new char[decodedString.length()];
    memcpy(pDecodedString, decodedString.c_str(), decodedString.length());

    SDL_RWops *pRW = SDL_RWFromMem(reinterpret_cast<void *>(pDecodedString), static_cast<int>(decodedString.length()));
    Image *pSprite = Image::Load(pRW, true /* loadImmediately */);

    delete [] pDecodedString;

    return pSprite;
}
#endif
#else
QImage XmlReader::ReadPng()
{
    QString s = ReadText();

    QByteArray imageBytes = QByteArray::fromBase64(QByteArray(s.toUtf8().constData(), s.length()));
    return QImage(imageBytes);
}
#endif

#ifdef CASE_CREATOR
XmlString XmlReader::ReadFilePath()
{
    // We always store file paths as relative paths but use them as absolute paths,
    // so convert this to an absolute path before returning it.
    return CaseContent::GetInstance()->RelativePathToAbsolutePath(ReadText());
}
#endif

bool XmlReader::AttributeExists(const XmlString &attributeName)
{
    return pCurrentNode->ToElement()->Attribute(XmlStringToCharArray(attributeName)) != NULL;
}

int XmlReader::ReadIntAttribute(const XmlString &attributeName)
{
    int value;
    XMLError error = pCurrentNode->ToElement()->QueryIntAttribute(XmlStringToCharArray(attributeName), &value);
    if (error != XML_NO_ERROR)
        ThrowException("XML error: expected int.");
    return value;
}

double XmlReader::ReadDoubleAttribute(const XmlString &attributeName)
{
    double value;
    XMLError error = pCurrentNode->ToElement()->QueryDoubleAttribute(XmlStringToCharArray(attributeName), &value);
    if (error != XML_NO_ERROR)
        ThrowException("XML error: expected double.");
    return value;
}

bool XmlReader::ReadBooleanAttribute(const XmlString &attributeName)
{
    bool value;
    XMLError error = pCurrentNode->ToElement()->QueryBoolAttribute(XmlStringToCharArray(attributeName), &value);
    if (error != XML_NO_ERROR)
        ThrowException("XML error: expected Boolean.");
    return value;
}

XmlString XmlReader::ReadTextAttribute(const XmlString &attributeName)
{
    const char *value = pCurrentNode->ToElement()->Attribute(XmlStringToCharArray(attributeName));
    if (value == NULL)
        return XmlString();
    else
        return XmlString(value);
}

#ifdef CASE_CREATOR
XmlString XmlReader::ReadFilePathAttribute(const XmlString &attributeName)
{
    // We always store file paths as relative paths but use them as absolute paths,
    // so convert this to an absolute path before returning it.
    return CaseContent::GetInstance()->RelativePathToAbsolutePath(ReadTextAttribute(attributeName));
}
#endif
