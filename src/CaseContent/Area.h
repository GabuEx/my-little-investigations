/**
 * Basic header/include file for Area.cpp.
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

#ifndef AREA_H
#define AREA_H

#include "Location.h"
#include "../Events/LocationEventProvider.h"

class XmlReader;
class XmlWriter;

class Area : public LocationEventListener
{
public:
    Area()
    {
        pStartLocation = NULL;
        pCurrentLocation = NULL;
        pTargetLocation = NULL;
        shouldSwapLocations = NULL;
    }

    Area(XmlReader *pReader);
    virtual ~Area();

    Location * GetStartLocation();

    string GetId() const { return this->id; }
    void SetId(string id) { this->id = id; }

    string GetName() const { return this->name; }
    void SetName(string name) { this->name = name; }

    string GetStartLocationId() const { return this->startLocationId; }
    void SetStartLocationId(string startLocationId) { this->startLocationId = startLocationId; }

    bool GetIsFinished() const { return pCurrentLocation == NULL; }

    void Begin();
    void Begin(string startLocationId, bool isLoadingFromSaveFile);
    void Update(int delta);
    void Draw();
    void DrawForScreenshot();
    void Reset();

    void SaveToSaveFile(XmlWriter *pWriter);
    static void LoadFromSaveFile(XmlReader *pReader, Area **ppCurrentArea);

    void OnLocationExited(Location *pSender, Location *pTargetLocation, string transitionId);

private:
    void Begin(Location *pNewLocation, bool isLoadingFromSaveFile);
    void SwapLocations(Location *pNewLocation, string transitionId, bool isLoadingFromSaveFile);
    void LoadFromSaveFile(XmlReader *pReader);

    Location *pStartLocation;
    Location *pCurrentLocation;
    string transitionId;

    string id;
    string name;
    string startLocationId;

    Location *pTargetLocation;
    bool shouldSwapLocations;
    bool swappedLocations;
};

#endif
