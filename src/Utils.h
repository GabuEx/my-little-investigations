/**
 * Basic header/include file for Utils.cpp.
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

#ifndef UTILS_H
#define UTILS_H

#include "Version.h"

#include <string>
#include <deque>

typedef unsigned char byte;

using namespace std;

#ifndef GAME_EXECUTABLE
typedef int (*PFNPROGRESSCALLBACK)(void *, double, double, double, double);
#endif

deque<string> &split(const string &s, char delim, deque<string> &tokens);
deque<string> split(const string &s, char delim);

bool SignatureIsValid(const byte *pFileData, unsigned int fileSize, const string &hexEncodedSignature);

string UuidFromSHA256Hash(byte hash[]);

#ifndef GAME_EXECUTABLE
bool RetrieveDataFromUriHttp(const string &uri, byte **ppByteDataFromUriHttp, size_t *pByteDataSize, PFNPROGRESSCALLBACK pfnProgressCallback = NULL, void *pProgressCallbackData = NULL);
bool RetrieveStringFromUriHttp(const string &uri, string *pReturnString, PFNPROGRESSCALLBACK pfnProgressCallback = NULL, void *pProgressCallbackData = NULL);
bool CheckIfUpdatesExist(string *pVersionsXmlContent);
string FileSizeToString(int fileSize);
bool PromptUserToDownloadUpdates(Version currentVersion, Version newVersion, int updateFileSize);
#endif

#endif
