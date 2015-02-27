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

#ifdef GAME_EXECUTABLE
#include "ResourceLoader.h"
#endif

using namespace tinyxml2;

XmlReader::XmlReader()
{
    pDocument = NULL;
}

XmlReader::XmlReader(const char *pFilePath)
{
    pDocument = NULL;
    ParseXmlFile(pFilePath);
}

XmlReader::XmlReader(const XmlReader &other)
{
    pDocument = NULL;
    ParseXmlFile(other.filePath.c_str());
}

XmlReader::~XmlReader()
{
    delete pDocument;
    pDocument = NULL;
}

void XmlReader::ParseXmlFile(const char *pFilePath)
{
    filePath = string(pFilePath);

    delete pDocument;
#ifdef GAME_EXECUTABLE
    pDocument = ResourceLoader::GetInstance()->LoadDocument(pFilePath);

    if (pDocument == NULL)
    {
#endif
        pDocument = new XMLDocument();

        if (pDocument->LoadFile(pFilePath) != XML_NO_ERROR)
        {
            delete pDocument;
            throw MLIException(string("File not found: ") + filePath);
        }
#ifdef GAME_EXECUTABLE
    }
#endif

    pCurrentNode = dynamic_cast<XMLNode *>(pDocument);
}

void XmlReader::ParseXmlContent(const XmlReaderString &xmlContent)
{
    delete pDocument;
    pDocument = new XMLDocument();
    XMLError error = pDocument->Parse(XmlReaderStringToCharArray(xmlContent));
    if (error != XML_NO_ERROR)
    {
        delete pDocument;
        throw MLIException("XML: Error while parsing file.");
    }

    pCurrentNode = dynamic_cast<XMLNode *>(pDocument);
}

void XmlReader::StartElement(const char *pElementName)
{
    XMLElement *pElement = pCurrentNode->FirstChildElement(pElementName);

    if (pElement == NULL)
    {
        // We didn't find the element - we should throw.
        throw MLIException("XML: Element not found.");
    }

    pCurrentNode = dynamic_cast<XMLNode *>(pElement);
}

bool XmlReader::ElementExists(const char *pElementName)
{
    XMLElement *pElement = pCurrentNode->FirstChildElement(pElementName);

    return pElement != NULL;
}

void XmlReader::EndElement()
{
    pCurrentNode = pCurrentNode->Parent();
}

void XmlReader::StartList(const char *pListElementName)
{
    listStack.push(XMLList(string(pListElementName)));
}

bool XmlReader::MoveToNextListItem()
{
    XMLList &topList = listStack.top();
    XMLElement *pElement = NULL;
    if (topList.started)
        pElement = pCurrentNode->NextSiblingElement(topList.elementsName.c_str());
    else
        pElement = pCurrentNode->FirstChildElement(topList.elementsName.c_str());

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

int XmlReader::ReadIntElement(const char *pElementName)
{
    int value;
    StartElement(pElementName);
    value = ReadInt();
    EndElement();
    return value;
}

double XmlReader::ReadDoubleElement(const char *pElementName)
{
    double value;
    StartElement(pElementName);
    value = ReadDouble();
    EndElement();
    return value;
}

bool XmlReader::ReadBooleanElement(const char *pElementName)
{
    bool value;
    StartElement(pElementName);
    value = ReadBoolean();
    EndElement();
    return value;
}

XmlReaderString XmlReader::ReadTextElement(const char *pElementName)
{
    XmlReaderString value;
    StartElement(pElementName);
    value = ReadText();
    EndElement();
    return value;
}

#if defined(GAME_EXECUTABLE) || defined(CASE_CREATOR)
XmlReaderImage XmlReader::ReadPngElement(const char *pElementName)
{
    XmlReaderImage value;
    StartElement(pElementName);
    value = ReadPng();
    EndElement();
    return value;
}
#endif

#ifdef CASE_CREATOR
XmlReaderString XmlReader::ReadFilePathElement(const char *pElementName)
{
    XmlReaderString value;
    StartElement(pElementName);
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
        throw MLIException("XML error: expected int.");
    return value;
}

double XmlReader::ReadDouble()
{
    double value;
    XMLError error = pCurrentNode->ToElement()->QueryDoubleText(&value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML error: expected double.");
    return value;
}

bool XmlReader::ReadBoolean()
{
    bool value;
    XMLError error = pCurrentNode->ToElement()->QueryBoolText(&value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML error: expected Boolean.");
    return value;
}

XmlReaderString XmlReader::ReadText()
{
    const char *value = pCurrentNode->ToElement()->GetText();
    // looks like tinyxml2 collapse "<tag> </tag>" and return NULL
    // with GetText() despite of PRESERVE_WHITESPACE flag
    if (value == NULL)
        return XmlReaderString();
    else
        return XmlReaderString(value);
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

    SDL_RWops *pRW = SDL_RWFromMem(reinterpret_cast<void *>(pDecodedString), (int)decodedString.length());
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
XmlReaderString XmlReader::ReadFilePath()
{
    // We always store file paths as relative paths but use them as absolute paths,
    // so convert this to an absolute path before returning it.
    return CaseContent::GetInstance()->RelativePathToAbsolutePath(ReadText());
}
#endif

bool XmlReader::AttributeExists(const char *pAttributeName)
{
    return pCurrentNode->ToElement()->Attribute(pAttributeName) != NULL;
}

int XmlReader::ReadIntAttribute(const char *pAttributeName)
{
    int value;
    XMLError error = pCurrentNode->ToElement()->QueryIntAttribute(pAttributeName, &value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML error: expected int.");
    return value;
}

double XmlReader::ReadDoubleAttribute(const char *pAttributeName)
{
    double value;
    XMLError error = pCurrentNode->ToElement()->QueryDoubleAttribute(pAttributeName, &value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML error: expected double.");
    return value;
}

bool XmlReader::ReadBooleanAttribute(const char *pAttributeName)
{
    bool value;
    XMLError error = pCurrentNode->ToElement()->QueryBoolAttribute(pAttributeName, &value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML error: expected Boolean.");
    return value;
}

XmlReaderString XmlReader::ReadTextAttribute(const char *pAttributeName)
{
    const char *value = pCurrentNode->ToElement()->Attribute(pAttributeName);
    if (value == NULL)
        return XmlReaderString();
    else
        return XmlReaderString(value);
}

#ifdef CASE_CREATOR
XmlReaderString XmlReader::ReadFilePathAttribute(const char *pAttributeName)
{
    // We always store file paths as relative paths but use them as absolute paths,
    // so convert this to an absolute path before returning it.
    return CaseContent::GetInstance()->RelativePathToAbsolutePath(ReadTextAttribute(pAttributeName));
}
#endif
