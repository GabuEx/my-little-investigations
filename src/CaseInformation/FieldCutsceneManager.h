/**
 * Basic header/include file for FieldCutsceneManager.cpp.
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

#ifndef FIELDCUTSCENEMANAGER_H
#define FIELDCUTSCENEMANAGER_H

#include "../CaseContent/FieldCutscene.h"
#include <map>

using namespace std;

class XmlReader;
class XmlWriter;

class FieldCutsceneManager
{
public:
    ~FieldCutsceneManager();

    FieldCutscene * GetCutsceneFromId(const string &id);

    void Reset();

    void SaveToSaveFile(XmlWriter *pWriter);
    void LoadFromSaveFile(XmlReader *pReader);

    void LoadFromXml(XmlReader *pReader);

private:
    map<string, FieldCutscene *> idToCutsceneMap;
    map<FieldCutscene *, bool> cutsceneToOriginalEnabledStateMap;
};

#endif
