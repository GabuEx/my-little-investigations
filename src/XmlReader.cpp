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

#include "Image.h"

#ifdef GAME_EXECUTABLE
#include "ResourceLoader.h"
#endif

#include <cryptopp/base64.h>

using namespace tinyxml2;

XmlReader::XmlReader()
{
    pDocument = NULL;
}

XmlReader::XmlReader(const char *pFilePath)
{
    ParseXmlFile(pFilePath);
}

XmlReader::XmlReader(const XmlReader &other)
{
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

#ifdef GAME_EXECUTABLE
    pDocument = ResourceLoader::GetInstance()->LoadDocument(pFilePath);

    if (pDocument == NULL)
    {
#endif
        pDocument = new XMLDocument();
        pDocument->LoadFile(pFilePath);
#ifdef GAME_EXECUTABLE
    }
#endif

    pCurrentNode = dynamic_cast<XMLNode *>(pDocument);
}

void XmlReader::ParseXmlContent(const string &xmlContent)
{
    delete pDocument;
    pDocument = new XMLDocument();
    XMLError error = pDocument->Parse(xmlContent.c_str());
    if (error != XML_NO_ERROR)
        throw MLIException("XML: Error while parsing file.");

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
    listStack.push(XMLList{string(pListElementName), false});
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
    XMLError error = pCurrentNode->ToElement()->QueryIntText(&value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML: error, expect int.");
    EndElement();
    return value;
}

double XmlReader::ReadDoubleElement(const char *pElementName)
{
    double value;
    StartElement(pElementName);
    XMLError error = pCurrentNode->ToElement()->QueryDoubleText(&value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML: error, expect double.");
    EndElement();
    return value;
}

bool XmlReader::ReadBooleanElement(const char *pElementName)
{
    bool value;
    StartElement(pElementName);
    XMLError error = pCurrentNode->ToElement()->QueryBoolText(&value);
    if (error != XML_NO_ERROR)
        throw MLIException("XML: error, expect double.");
    EndElement();
    return value;
}

string XmlReader::ReadTextElement(const char *pElementName)
{
    StartElement(pElementName);
    const char *value = pCurrentNode->ToElement()->GetText();
    EndElement();
    // looks like tinyxml2 collapse "<tag> </tag>" and return NULL
    // with GetText() despite of PRESERVE_WHITESPACE flag
    if (value == NULL)
        return string();
    else
        return string(value);

}

#ifdef GAME_EXECUTABLE
Image * XmlReader::ReadPngElement(const char *pElementName)
{
    string s = ReadTextElement(pElementName);

    string decodedString;
    CryptoPP::StringSource(s, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decodedString)));

    char *pDecodedString = new char[decodedString.length()];
    memcpy(pDecodedString, decodedString.c_str(), decodedString.length());

    SDL_RWops *pRW = SDL_RWFromMem(reinterpret_cast<void *>(pDecodedString), decodedString.length());
    Image *pSprite = Image::Load(pRW, true /* loadImmediately */);

    delete [] pDecodedString;

    return pSprite;
}
#endif
