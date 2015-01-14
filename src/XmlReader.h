/**
 * Basic header/include file for XmlReader.cpp.
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

#ifndef XMLREADER_H
#define XMLREADER_H

#include "XmlIncludes.h"

#include <stack>
#include "tinyxml2/tinyxml2.h"
#include "MLIException.h"

class XmlReader
{
public:
    XmlReader();
    XmlReader(const char *pFilePath);
    XmlReader(const XmlReader &other);
    ~XmlReader();

    void ParseXmlFile(const char *pFilePath);
    void ParseXmlContent(const XmlReaderString &xmlContent);

    void StartElement(const char *pElementName);
    bool ElementExists(const char *pElementName);
    void EndElement();
    void StartList(const char *pListElementName);
    bool MoveToNextListItem();

    int ReadIntElement(const char *pElementName);
    double ReadDoubleElement(const char *pElementName);
    bool ReadBooleanElement(const char *pElementName);
    XmlReaderString ReadTextElement(const char *pElementName);

#if defined(GAME_EXECUTABLE) || defined(CASE_CREATOR)
    XmlReaderImage ReadPngElement(const char *pElementName);
#endif

#ifdef CASE_CREATOR
    XmlReaderString ReadFilePathElement(const char *pElementName);
#endif

    int ReadInt();
    double ReadDouble();
    bool ReadBoolean();
    XmlReaderString ReadText();

#if defined(GAME_EXECUTABLE) || defined(CASE_CREATOR)
    XmlReaderImage ReadPng();
#endif

#ifdef CASE_CREATOR
    XmlReaderString ReadFilePath();
#endif

    bool AttributeExists(const char *pAttributeName);
    int ReadIntAttribute(const char *pAttributeName);
    double ReadDoubleAttribute(const char *pAttributeName);
    bool ReadBooleanAttribute(const char *pAttributeName);
    XmlReaderString ReadTextAttribute(const char *pAttributeName);

#ifdef CASE_CREATOR
    XmlReaderString ReadFilePathAttribute(const char *pAttributeName);
#endif

private:
    string filePath;
    tinyxml2::XMLDocument *pDocument;
    tinyxml2::XMLNode *pCurrentNode;

    struct XMLList
    {
        XMLList(string elementsName)
            : elementsName(elementsName)
            , started(false)
        {
        }

        string elementsName;
        bool started; // indicate, if we actually start iterate over list
    };

    stack<XMLList> listStack;
};

#endif
