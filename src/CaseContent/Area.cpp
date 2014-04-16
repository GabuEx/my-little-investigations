/**
 * Handles a single area in the game.
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

#include "Area.h"
#include "../MouseHelper.h"
#include "../CaseInformation/Case.h"
#include "../XmlReader.h"
#include "../XmlWriter.h"

Area::Area(XmlReader *pReader)
{
    pStartLocation = NULL;
    pCurrentLocation = NULL;
    pTargetLocation = NULL;
    shouldSwapLocations = false;
    swappedLocations = false;

    pReader->StartElement("Area");
    id = pReader->ReadTextElement("Id");
    name = pReader->ReadTextElement("Name");
    startLocationId = pReader->ReadTextElement("StartLocationId");
    pReader->EndElement();
}

Area::~Area()
{
    EventProviders::GetLocationEventProvider()->ClearListener(this);
}

Location * Area::GetStartLocation()
{
    if (pStartLocation == NULL)
    {
        pStartLocation = Case::GetInstance()->GetContentManager()->GetLocationFromId(GetStartLocationId());
    }

    return pStartLocation;
}

void Area::Begin()
{
    Begin(GetStartLocation(), false /* isLoadingFromSaveFile */);
}

void Area::Begin(string startLocationId, bool isLoadingFromSaveFile)
{
    Begin(Case::GetInstance()->GetContentManager()->GetLocationFromId(startLocationId), isLoadingFromSaveFile);
}

void Area::Begin(Location *pNewLocation, bool isLoadingFromSaveFile)
{
    SwapLocations(pNewLocation, "", isLoadingFromSaveFile);

    if (pCurrentLocation != NULL)
    {
        pCurrentLocation->Begin(transitionId);
    }
}

void Area::Update(int delta)
{
    if (pCurrentLocation != NULL)
    {
        if (swappedLocations)
        {
            Case::GetInstance()->GetAudioManager()->PlayBgmWithId(pCurrentLocation->GetBgm());
            Case::GetInstance()->GetAudioManager()->PlayAmbianceWithId(pCurrentLocation->GetAmbianceSfxId());

            pCurrentLocation->Begin(transitionId);
            swappedLocations = false;
        }

        pCurrentLocation->Update(delta);
        pCurrentLocation->UpdateTabPositions(delta);

        // If the current location isn't accepting user input,
        // then we shouldn't be changing the cursor or mouse-over text.
        if (!pCurrentLocation->GetAcceptsUserInput())
        {
            MouseHelper::SetCursorType(CursorTypeNormal);
            MouseHelper::SetMouseOverText("");
        }

        if (shouldSwapLocations)
        {
            SwapLocations(pTargetLocation, transitionId, false /* isLoadingFromSaveFile */);
            pTargetLocation = NULL;
            shouldSwapLocations = false;
        }
    }
}

void Area::Draw()
{
    if (pCurrentLocation != NULL)
    {
        pCurrentLocation->Draw();
    }
}

void Area::DrawForScreenshot()
{
    if (pCurrentLocation != NULL)
    {
        pCurrentLocation->DrawForScreenshot();
    }
}

void Area::Reset()
{
    pCurrentLocation = NULL;
}

void Area::SaveToSaveFile(XmlWriter *pWriter)
{
    pWriter->WriteTextElement("CurrentAreaId", id);
    pCurrentLocation->SaveToSaveFile(pWriter);
}

void Area::LoadFromSaveFile(XmlReader *pReader, Area **ppCurrentArea)
{
    string currentAreaId = pReader->ReadTextElement("CurrentAreaId");
    string currentLocationId = pReader->ReadTextElement("CurrentLocationId");

    *ppCurrentArea = Case::GetInstance()->GetContentManager()->GetAreaFromId(currentAreaId);
    (*ppCurrentArea)->Begin(currentLocationId, true /* isLoadingFromSaveFile */);
    (*ppCurrentArea)->pCurrentLocation->LoadFromSaveFile(pReader);
}

void Area::OnLocationExited(Location *pSender, Location *pTargetLocation, string transitionId)
{
    if (pSender == pCurrentLocation)
    {
        this->pTargetLocation = pTargetLocation;
        this->transitionId = transitionId;
        shouldSwapLocations = true;
    }
}

void Area::SwapLocations(Location *pNewLocation, string transitionId, bool isLoadingFromSaveFile)
{
    if (pCurrentLocation != NULL)
    {
        EventProviders::GetLocationEventProvider()->ClearListener(this);
    }

    pCurrentLocation = pNewLocation;

    if (pCurrentLocation != NULL)
    {
        EventProviders::GetLocationEventProvider()->RegisterListener(this);
        swappedLocations = !isLoadingFromSaveFile;
    }

    if (pCurrentLocation != NULL)
    {
        pCurrentLocation->UpdateLoadedTextures();
    }
}
