/**
 * Utility methods to handle various tasks.
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

#include "Utils.h"

#include "globals.h"
#include "XmlReader.h"
#include <sstream>

#include <cryptopp/hex.h>
#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>

using namespace CryptoPP;

#ifndef GAME_EXECUTABLE
extern "C"
{
    #include <curl/curl.h>
}

string versionCheckUri = "http://www.equestriandreamers.com/updates/my-little-investigations/mli-versions.xml";
#endif

deque<string> &split(const string &s, char delim, deque<string> &tokens)
{
    stringstream ss(s);
    string item;

    while(getline(ss, item, delim))
    {
        tokens.push_back(item);
    }

    return tokens;
}

deque<string> split(const string &s, char delim)
{
    deque<string> tokens;
    return split(s, delim, tokens);
}

string UuidFromSHA256Hash(byte hash[CryptoPP::SHA256::DIGESTSIZE])
{
    string uuid = "";

    for (int i = 0; i < CryptoPP::SHA256::DIGESTSIZE && i < 16; i++)
    {
        byte highNibble = (hash[i] >> 4) & 0x0F;
        byte lowNibble = hash[i] & 0x0F;

        if (i == 6)
        {
            uuid += "5";
        }
        else if (i == 16)
        {
            uuid += "A";
        }
        else
        {
            if (highNibble < 10)
            {
                uuid += (char)('0' + highNibble);
            }
            else
            {
                uuid += (char)('A' + (highNibble - 10));
            }
        }

        if (lowNibble < 10)
        {
            uuid += (char)('0' + lowNibble);
        }
        else
        {
            uuid += (char)('A' + (lowNibble - 10));
        }

        if (i == 3 ||
            i == 5 ||
            i == 7 ||
            i == 9)
        {
            uuid += "-";
        }
    }

    return uuid;
}

bool SignatureIsValid(const byte *pFileData, unsigned int fileSize, const string &hexEncodedSignature)
{
    Integer modulus("23568332026097843589330224341232423824489227725618860714509096199525106220740317569413957190650367349886057276376951603027658474222300018538090882724127806422184385919973062121722991179344505183372523752666554302712122813070863946812173550830454356506133615226034873458142962497061909667034748542366872519001572246306747534032691513595335005177558602926751208654397458873075388398851331968483672593974371591236988537248649056651919444044050631175982858540930336744213602080363401671845205556669413373412866688398634151430976573005566658278720024181234244924513150893829727777179327452060753074929344789195138168478643");
    Integer publicExponent("17");

    string signatureString;
    CryptoPP::StringSource(hexEncodedSignature, true, new CryptoPP::HexDecoder(new CryptoPP::StringSink(signatureString)));

    RSA::PublicKey publicKey;
    publicKey.Initialize(modulus, publicExponent);

    SecByteBlock fileDataBlock(fileSize);
    fileDataBlock.Assign(pFileData, fileSize);

    RSASS<PKCS1v15, SHA256>::Verifier verifier(publicKey);

    SecByteBlock signature(signatureString.length());
    signature.Assign((const byte *)signatureString.c_str(), signatureString.length());

    return verifier.VerifyMessage(fileDataBlock, fileDataBlock.size(), signature, signature.size());
}

#ifndef GAME_EXECUTABLE
PFNPROGRESSCALLBACK pfnProgressCallbackCurrent = NULL;
void *pProgressCallbackDataCurrent = NULL;

byte *pByteDataFromUriHttp = NULL;
size_t byteDataSize = 0;

size_t WriteByteData(void *buffer, size_t size, size_t nmemb, void *userp)
{
    size_t newByteDataSize = size * nmemb;
    byte *pNewByteData = new byte[byteDataSize + newByteDataSize];
    bool transferCanceled = false;

    if (pByteDataFromUriHttp != NULL)
    {
        memcpy(pNewByteData, pByteDataFromUriHttp, byteDataSize);
        delete pByteDataFromUriHttp;
    }

    memcpy(pNewByteData + byteDataSize, buffer, newByteDataSize);

    pByteDataFromUriHttp = pNewByteData;
    byteDataSize += newByteDataSize;

    transferCanceled = pfnProgressCallbackCurrent(pProgressCallbackDataCurrent, byteDataSize, newByteDataSize, 0, 0) != 0;

    return transferCanceled ? 0 : newByteDataSize;
}

bool RetrieveDataFromUriHttp(const string &uri, byte **ppByteDataFromUriHttp, size_t *pByteDataSize, PFNPROGRESSCALLBACK pfnProgressCallback, void *pProgressCallbackData)
{
    pByteDataFromUriHttp = NULL;
    byteDataSize = 0;

    curl_easy_setopt(gpCurlHandle, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(gpCurlHandle, CURLOPT_WRITEFUNCTION, WriteByteData);

    // We'll only wait for two seconds - if we aren't connected to the server by then,
    // then we probably aren't going to get anything at all.
    curl_easy_setopt(gpCurlHandle, CURLOPT_CONNECTTIMEOUT, 2);

    pfnProgressCallbackCurrent = pfnProgressCallback;
    pProgressCallbackDataCurrent = pProgressCallbackData;

    if (pfnProgressCallback != NULL)
    {
        curl_easy_setopt(gpCurlHandle, CURLOPT_PROGRESSFUNCTION, pfnProgressCallback);

        if (pProgressCallbackData != NULL)
        {
            curl_easy_setopt(gpCurlHandle, CURLOPT_PROGRESSDATA, pProgressCallbackData);
        }
    }

    CURLcode returnCode = curl_easy_perform(gpCurlHandle);

    if (returnCode == CURLE_OK)
    {
        *ppByteDataFromUriHttp = pByteDataFromUriHttp;
        *pByteDataSize = byteDataSize;
        return true;
    }
    else
    {
        *ppByteDataFromUriHttp = NULL;
        *pByteDataSize = 0;
        return false;
    }
}

string stringFromUriHttp;

size_t WriteStringData(void *buffer, size_t size, size_t nmemb, void *userp)
{
    string newContent((char *)buffer, size * nmemb);
    stringFromUriHttp += newContent;
    return newContent.length();
}

bool RetrieveStringFromUriHttp(const string &uri, string *pReturnString, PFNPROGRESSCALLBACK pfnProgressCallback, void *pProgressCallbackData)
{
    stringFromUriHttp = "";

    curl_easy_setopt(gpCurlHandle, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(gpCurlHandle, CURLOPT_WRITEFUNCTION, WriteStringData);

    // We'll only wait for two seconds - if we haven't connected to the server by then,
    // then we probably aren't going to get anything at all.
    curl_easy_setopt(gpCurlHandle, CURLOPT_CONNECTTIMEOUT, 2);

    pfnProgressCallbackCurrent = pfnProgressCallback;
    pProgressCallbackDataCurrent = pProgressCallbackData;

    if (pfnProgressCallback != NULL)
    {
        curl_easy_setopt(gpCurlHandle, CURLOPT_PROGRESSFUNCTION, pfnProgressCallback);

        if (pProgressCallbackData != NULL)
        {
            curl_easy_setopt(gpCurlHandle, CURLOPT_PROGRESSDATA, pProgressCallbackData);
        }
    }

    CURLcode returnCode = curl_easy_perform(gpCurlHandle);

    if (returnCode == CURLE_OK)
    {
        *pReturnString = stringFromUriHttp;
        return true;
    }
    else
    {
        *pReturnString = "";
        return false;
    }
}

bool CheckIfUpdatesExist(string *pVersionsXmlContent)
{
    bool updatesExist = false;
    Version newestVersion = Version(0, 0, 0);
    int totalFileSize = 0;

    *pVersionsXmlContent = "";

    string versionsXmlContent;
    string versionsXmlSignatureHexEncoded;

    bool retrievedVersionsXmlContent = false;
    bool retrievedVersionsXmlSignature = false;

    retrievedVersionsXmlContent = RetrieveStringFromUriHttp(versionCheckUri, &versionsXmlContent);

    if (retrievedVersionsXmlContent)
    {
        retrievedVersionsXmlSignature = RetrieveStringFromUriHttp(versionCheckUri + ".signature", &versionsXmlSignatureHexEncoded);
    }

    if (retrievedVersionsXmlContent &&
        retrievedVersionsXmlSignature &&
        SignatureIsValid((const byte *)versionsXmlContent.c_str(), versionsXmlContent.length(), versionsXmlSignatureHexEncoded))
    {
        try
        {
            XmlReader versionReader;
            versionReader.ParseXmlContent(versionsXmlContent);

            versionReader.StartElement("VersionInformation");
            versionReader.StartElement("Versions");
            versionReader.StartList("Version");

            while (versionReader.MoveToNextListItem())
            {
                versionReader.StartElement("VersionNumber");
                Version newVersion(&versionReader);
                if (gVersion < newVersion)
                {
                    if (newVersion > newestVersion)
                    {
                        newestVersion = newVersion;
                    }

                    updatesExist = true;
                }
                else
                {
                    // We've found all of the updates that we need to get - we can stop now.
                    break;
                }

                versionReader.EndElement();

                versionReader.StartElement("FilesChanged");
                versionReader.StartList("File");

                while (versionReader.MoveToNextListItem())
                {
                    int deltaSize = 0;
                    string action = versionReader.ReadTextElement("Action");

                    if (action != "Remove")
                    {
                        if (versionReader.ElementExists("DeltaSize"))
                        {
                            deltaSize = versionReader.ReadIntElement("DeltaSize");
                        }
                        else
                        {
                        #ifdef __WINDOWS
                            deltaSize = versionReader.ReadIntElement("DeltaSizeWindows");
                        #elif __OSX
                            deltaSize = versionReader.ReadIntElement("DeltaSizeOSX");
                        #elif __unix
                            deltaSize = versionReader.ReadIntElement("DeltaSizeUnix");
                        #else
                        #error NOT IMPLEMENTED
                        #endif
                        }
                    }

                    totalFileSize += deltaSize;
                }

                versionReader.EndElement();
            }

            versionReader.EndElement();
            versionReader.EndElement();
        }
        catch (MLIException e)
        {
            // If something bad happened while getting update information,
            // then we'll just report that there are no updates.
            updatesExist = false;
        }

#ifdef LAUNCHER
        if (updatesExist)
        {
            updatesExist = PromptUserToDownloadUpdates(gVersion, newestVersion, totalFileSize);
        }
#endif

        if (updatesExist)
        {
            *pVersionsXmlContent = versionsXmlContent;
        }
    }

    return updatesExist;
}

string FileSizeToString(int fileSize)
{
    string suffix = "B";
    double units = (double)fileSize;

    if (units > 1000000000)
    {
        units /= 1000000000;
        suffix = "GB";
    }
    else if (units > 1000000)
    {
        units /= 1000000;
        suffix = "MB";
    }
    else if (units > 1000)
    {
        units /= 1000;
        suffix = "KB";
    }

    char buf[256];

    if (suffix == "B")
    {
        sprintf(buf, "%.0f %s", units, suffix.c_str());
    }
    else
    {
        sprintf(buf, "%.2f %s", units, suffix.c_str());
    }

    return string(buf);
}

bool PromptUserToDownloadUpdates(Version currentVersion, Version newVersion, int updateFileSize)
{
    string totalFileSizeString = FileSizeToString(updateFileSize);

    SDL_MessageBoxData messageBoxData;
    SDL_MessageBoxButtonData buttonData[2];

    buttonData[0].buttonid = 1;
    buttonData[0].text = "No";
    buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;

    buttonData[1].buttonid = 0;
    buttonData[1].text = "Yes";
    buttonData[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;

    char updateMessage[1024];
    sprintf(
        updateMessage, "A newer version of My Little Investigations is available for download.\n\nCurrent version: %s\nNewest version: %s\n\nDownload update? (Download size: %s. Requires administrator privileges.)",
        ((string)currentVersion).c_str(),
        ((string)newVersion).c_str(),
        totalFileSizeString.c_str());

    messageBoxData.flags = SDL_MESSAGEBOX_INFORMATION;
    messageBoxData.title = "Update available";
    messageBoxData.message = updateMessage;
    messageBoxData.buttons = buttonData;
    messageBoxData.numbuttons = 2;
    messageBoxData.window = gpWindow;

    int buttonClickedId;

    return SDL_ShowMessageBox(&messageBoxData, &buttonClickedId) == 0 && buttonClickedId == buttonData[1].buttonid;
}
#endif
