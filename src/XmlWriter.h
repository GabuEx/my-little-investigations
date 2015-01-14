/**
 * Basic header/include file for XmlWriter.cpp.
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

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include "XmlIncludes.h"

#include <sstream>
#include <stack>

class XmlWriter
{
public:
    XmlWriter(const char *pFilePath, const char *pFilePathExtension = NULL, bool makeHumanReadable = false);
    ~XmlWriter();

    void StartElement(const XmlReaderString &elementName);
    void EndElement();
    void WriteIntElement(const XmlReaderString &elementName, int elementValue);
    void WriteDoubleElement(const XmlReaderString &elementName, double elementValue);
    void WriteBooleanElement(const XmlReaderString &elementName, bool elementValue);
    void WriteTextElement(const XmlReaderString &elementName, const XmlReaderString &elementValue);
    void WritePngElement(const XmlReaderString &elementName, void *pElementValue, size_t elementSize);

#ifdef CASE_CREATOR
    void WriteFilePathElement(const XmlReaderString &elementName, const XmlReaderString &elementValue);
#endif

private:
    void StartElement(const XmlReaderString &elementName, bool addCarriageReturn);

    bool makeHumanReadable;
    int indentLevel;

    stringstream stringStream;
    string filePath;
    string filePathExtension;
    stack<XmlReaderString> elementNameStack;
    stack<bool> shouldUnindentStack;
};

#endif
