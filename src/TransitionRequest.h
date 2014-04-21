/**
 * Basic header/include file for TransitionRequest.cpp.
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

#ifndef TRANSITIONREQUEST_H
#define TRANSITIONREQUEST_H

#include <string>

using namespace std;

enum TransitionDestination
{
    TransitionDestinationNone,
    TransitionDestinationNewLocation,
    TransitionDestinationZoomedView,
};

class TransitionRequest
{
public:
    TransitionRequest()
        : newAreaRequestedId("")
        , newLocationRequestedId("")
        , transitionId("")
        , zoomedViewId("")
        , transitionDestination(TransitionDestinationNone)
    {
    }

    TransitionRequest(const string &newAreaRequestedId, const string &newLocationRequestedId, const string &transitionId)
        : newAreaRequestedId(newAreaRequestedId)
        , newLocationRequestedId(newLocationRequestedId)
        , transitionId(transitionId)
        , zoomedViewId("")
        , transitionDestination(TransitionDestinationNewLocation)
    {
    }

    TransitionRequest(const string &zoomedViewId)
        : newAreaRequestedId("")
        , newLocationRequestedId("")
        , transitionId("")
        , zoomedViewId(zoomedViewId)
        , transitionDestination(TransitionDestinationZoomedView)
    {
    }

    string GetNewAreaRequestedId() const { return this->newAreaRequestedId; }
    string GetNewLocationRequestedId() const { return this->newLocationRequestedId; }
    string GetTransitionId() const { return this->transitionId; }
    string GetZoomedViewId() const { return this->zoomedViewId; }

    TransitionDestination GetTransitionDestination() const { return this->transitionDestination; }

private:
    string newAreaRequestedId;
    string newLocationRequestedId;
    string transitionId;
    string zoomedViewId;

    TransitionDestination transitionDestination;
};

#endif
