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
    XmlReader(const XmlString &filePath);
    XmlReader(const XmlReader &other);
    ~XmlReader();

    void ParseXmlFile(const XmlString &filePath);
    void ParseXmlContent(const XmlString &xmlContent);

private:
    void Init(tinyxml2::XMLDocument *pDocument);

public:
    int GetFormattingVersion() { return formattingVersion; }

    void StartElement(const XmlString &elementName);
    bool ElementExists(const XmlString &elementName);
    void VerifyCurrentElement(const XmlString &expectedElementName);
    void EndElement();
    void StartList(const XmlString &elementName);
    bool MoveToNextListItem();

    int ReadIntElement(const XmlString &elementName);
    double ReadDoubleElement(const XmlString &elementName);
    bool ReadBooleanElement(const XmlString &elementName);
    XmlString ReadTextElement(const XmlString &elementName);

#if defined(GAME_EXECUTABLE) || defined(CASE_CREATOR)
    XmlImage ReadPngElement(const XmlString &elementName);
#endif

#ifdef CASE_CREATOR
    XmlString ReadFilePathElement(const XmlString &elementName);
#endif

    int ReadInt();
    double ReadDouble();
    bool ReadBoolean();
    XmlString ReadText();

#if defined(GAME_EXECUTABLE) || defined(CASE_CREATOR)
    XmlImage ReadPng();
#endif

#ifdef CASE_CREATOR
    XmlString ReadFilePath();
#endif

    bool AttributeExists(const XmlString &attributeName);
    int ReadIntAttribute(const XmlString &attributeName);
    double ReadDoubleAttribute(const XmlString &attributeName);
    bool ReadBooleanAttribute(const XmlString &attributeName);
    XmlString ReadTextAttribute(const XmlString &attributeName);

#ifdef CASE_CREATOR
    XmlString ReadFilePathAttribute(const XmlString &attributeName);
#endif

private:
    XmlString filePath;
    tinyxml2::XMLDocument *pDocument;
    tinyxml2::XMLNode *pCurrentNode;

    struct XMLList
    {
        XMLList(XmlString elementsName)
            : elementsName(elementsName)
            , started(false)
        {
        }

        XmlString elementsName;
        bool started; // indicate, if we actually start iterate over list
    };

    stack<XMLList> listStack;

    int formattingVersion;
};

#endif
