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
        pDocument = new Document();
        pDocument->LoadFile(pFilePath);
#ifdef GAME_EXECUTABLE
    }
#endif
}

void XmlReader::ParseXmlContent(string xmlContent)
{
    delete pDocument;
    pDocument = new Document();
    pDocument->Parse(xmlContent);
}

void XmlReader::StartElement(const char *pElementName)
{
    Element *pElement = GetFirstDirectChildElementByName(pElementName);

    if (pElement != NULL)
    {
        elementStack.push(pElement);
    }
    else
    {
        // We didn't find the element - we should throw.
        throw Exception("Element not found.");
    }
}

bool XmlReader::ElementExists(const char *pElementName)
{
    Element *pElement = GetFirstDirectChildElementByName(pElementName);

    return pElement != NULL;
}

void XmlReader::EndElement()
{
    elementStack.pop();
}

void XmlReader::StartList(const char *pListElementName)
{
    ListIterator *pIterator = new ListIterator(this, pListElementName);
    listIteratorStack.push(pIterator);
}

bool XmlReader::MoveToNextListItem()
{
    ListIterator *pIterator = listIteratorStack.top();
    bool wasNextItem = pIterator->MoveToNextItem();

    // If there's no next item, then we're done,
    // and we want to delete the list iterator,
    // since we won't be using it anymore.
    if (!wasNextItem)
    {
        listIteratorStack.pop();
        delete pIterator;
    }

    return wasNextItem;
}

int XmlReader::ReadIntElement(const char *pElementName)
{
    int i;
    StartElement(pElementName);
    elementStack.top()->GetText(&i);
    EndElement();
    return i;
}

double XmlReader::ReadDoubleElement(const char *pElementName)
{
    double d;
    StartElement(pElementName);
    elementStack.top()->GetText(&d);
    EndElement();
    return d;
}

bool XmlReader::ReadBooleanElement(const char *pElementName)
{
    string s;
    StartElement(pElementName);
    elementStack.top()->GetText(&s);
    EndElement();
    return s == "true";
}

string XmlReader::ReadTextElement(const char *pElementName)
{
    string s;
    StartElement(pElementName);
    elementStack.top()->GetText(&s, false /* throwIfNotFound */);
    EndElement();
    return s;
}

#ifdef GAME_EXECUTABLE
Image * XmlReader::ReadPngElement(const char *pElementName)
{
    string s;
    StartElement(pElementName);
    elementStack.top()->GetText(&s);
    EndElement();

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

Element * XmlReader::GetFirstDirectChildElementByName(const char *pElementName)
{
    Element *pElement = NULL;
    Iterator<Element> child(pElementName);

    for (child = child.begin(GetTopNodeInStack()); child != child.end(); ++child)
    {
        pElement = child.Get();
        break;
    }

    return pElement;
}

Node * XmlReader::GetTopNodeInStack()
{
    return elementStack.empty() ? dynamic_cast<Node *>(pDocument) : dynamic_cast<Node *>(elementStack.top());
}

XmlReader::ListIterator::ListIterator(XmlReader *pParent, const char *pListElementName)
    : listElementName(string(pListElementName))
{
    pParentReader = pParent;
    pIterator = new Iterator<Element>(pListElementName);
    hasBegun = false;
}

XmlReader::ListIterator::ListIterator(const ListIterator &other)
{
    pParentReader = other.pParentReader;
    pIterator = new Iterator<Element>(listElementName.c_str());
    hasBegun = false;
}

XmlReader::ListIterator::~ListIterator()
{
    pParentReader = NULL;

    delete pIterator;
    pIterator = NULL;
}

bool XmlReader::ListIterator::MoveToNextItem()
{
    if (!hasBegun || *pIterator != pIterator->end())
    {
        if (!hasBegun)
        {
            *pIterator = pIterator->begin(pParentReader->GetTopNodeInStack());
            hasBegun = true;
        }
        else
        {
            pParentReader->elementStack.pop();
        }

        if (*pIterator != pIterator->end())
        {
            pParentReader->elementStack.push(pIterator->Get());

            (*pIterator)++;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        pParentReader->elementStack.pop();
        return false;
    }
}
