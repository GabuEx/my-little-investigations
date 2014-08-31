/**
 * The check-for-updates screen of the game.
 *
 * @author GabuEx, dawnmew
 * @since 1.0
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2012 Equestrian Dreamers
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

#ifdef UPDATER

#include "CheckForUpdatesScreen.h"
#include "../XmlReader.h"
#include "../FileFunctions.h"
#include "../globals.h"
#include "../CaseInformation/CommonCaseResources.h"

#include <cstdio>
#include <fstream>

const int DotsUpdateDelayMs = 500;

string CheckForUpdatesScreen::PendingUpdate::PendingFileUpdate::GetFilePath()
{
    string filePath = "";

    switch (fileLocation)
    {
    case FileLocationExecutionPath:
        filePath = GetExecutionPath();
        break;

    case FileLocationCommonAppDataPath:
        filePath = GetCommonAppDataPath();
        break;

    case FileLocationUserAppDataPath:
        filePath = GetUserAppDataPath();
        break;

    default:
        break;
    }

    filePath += ConvertSeparatorsInPath(fileName);
    return filePath;
}

CheckForUpdatesScreen::ScheduledAction * CheckForUpdatesScreen::PendingUpdate::PendingFileUpdate::ScheduleAction()
{
    CheckForUpdatesScreen::ScheduledAction *pScheduledAction = NULL;

    if (action == ActionUpdate)
    {
        byte *pDeltaContent = NULL;
        size_t deltaContentSize = 0;

        bool retrievedUpdateContent = RetrieveDataFromUriHttp(deltaLocation, &pDeltaContent, &deltaContentSize, CheckForUpdatesScreen::DownloadUpdatesProgressCallback, static_cast<CheckForUpdatesScreen *>(pOwningScreen));

        if (retrievedUpdateContent &&
            SignatureIsValid((const byte *)pDeltaContent, deltaContentSize, signature))
        {
            string localDeltaFilePath = GetTempDirectoryPath() + GetFileNameFromUri(deltaLocation);

            ofstream deltaFileStream(localDeltaFilePath.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);

            if (deltaFileStream)
            {
                deltaFileStream.write((const char *)pDeltaContent, deltaContentSize);
                deltaFileStream.close();

                pScheduledAction = new CheckForUpdatesScreen::ScheduledUpdate(GetFilePath() + ".pre" + newVersionString, localDeltaFilePath, GetFilePath());
            }

            delete [] pDeltaContent;
        }
    }
    else if (action == ActionAdd)
    {
        byte *pFileContent = NULL;
        size_t fileContentSize = 0;

        bool retrievedFileContent = RetrieveDataFromUriHttp(deltaLocation, &pFileContent, &fileContentSize, CheckForUpdatesScreen::DownloadUpdatesProgressCallback, static_cast<CheckForUpdatesScreen *>(pOwningScreen));

        if (retrievedFileContent &&
            SignatureIsValid((const byte *)pFileContent, fileContentSize, signature))
        {
            string stagingFilePath = GetTempDirectoryPath() + GetFileNameFromFilePath(GetFilePath()) + ".toadd";
            pScheduledAction = new CheckForUpdatesScreen::ScheduledAdd(stagingFilePath, GetFilePath(), pFileContent, fileContentSize);
        }
    }
    else if (action == ActionRemove)
    {
        string stagingFilePath = GetFilePath() + ".toremove";
        pScheduledAction = new CheckForUpdatesScreen::ScheduledRemove(stagingFilePath, GetFilePath());
    }

    return pScheduledAction;
}

bool CheckForUpdatesScreen::ScheduledUpdate::PerformUpdate()
{
    if (!RenameFile(newFilePath, oldFilePath))
    {
        return false;
    }

    bool success = ApplyDeltaFile(oldFilePath, deltaFilePath, newFilePath);
    RemoveFile(deltaFilePath);
    return success;
}

void CheckForUpdatesScreen::ScheduledUpdate::RollBack()
{
    RemoveFile(newFilePath);
    RenameFile(oldFilePath, newFilePath);
}

void CheckForUpdatesScreen::ScheduledUpdate::Complete()
{
    RemoveFile(oldFilePath);
}

bool CheckForUpdatesScreen::ScheduledAdd::PerformUpdate()
{
    ofstream fileStream(stagingFilePath.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);

    if (fileStream)
    {
        fileStream.write((const char *)pFileContent, fileContentSize);
        fileStream.close();

        delete [] pFileContent;

        RemoveFile(newFilePath);
        return RenameFile(stagingFilePath, newFilePath);
    }
    else
    {
        delete [] pFileContent;

        return false;
    }
}

void CheckForUpdatesScreen::ScheduledAdd::RollBack()
{
    RemoveFile(newFilePath);
}

void CheckForUpdatesScreen::ScheduledAdd::Complete()
{
    // Nothing to do - the file already exists.
}

bool CheckForUpdatesScreen::ScheduledRemove::PerformUpdate()
{
    return RenameFile(oldFilePath, stagingFilePath);
}

void CheckForUpdatesScreen::ScheduledRemove::RollBack()
{
    RenameFile(stagingFilePath, oldFilePath);
}

void CheckForUpdatesScreen::ScheduledRemove::Complete()
{
    RemoveFile(stagingFilePath);
}

CheckForUpdatesScreen::CheckForUpdatesScreen(MLIFont *pTextDisplayFont)
{
    this->pTextDisplayFont = pTextDisplayFont;

    pInteropSemaphore = SDL_CreateSemaphore(1);
}

CheckForUpdatesScreen::~CheckForUpdatesScreen()
{
    UnloadResources();

    delete pTextDisplayFont;
    pTextDisplayFont = NULL;

    SDL_DestroySemaphore(pInteropSemaphore);
    pInteropSemaphore = NULL;
}

void CheckForUpdatesScreen::LoadResources()
{
}

void CheckForUpdatesScreen::UnloadResources()
{
}

void CheckForUpdatesScreen::Init()
{
    MLIScreen::Init();

    drawnOnce = false;
    currentState = StateCheckingForUpdates;
    updatesCanceled = false;

    numDots = 1;
    timeSinceLastDotsUpdate = 0;

    finishedCheckingForUpdates = false;
    newestVersion = Version();
    totalFileSize = 0;

    totalFileSizeString = "";
    totalBytesDownloaded = 0;
    downloadComplete = false;
    wasErrorInDownload = false;
    problemUpdateFileForDownload = "";
    totalUpdateCount = 0;

    applicationComplete = false;
    wasErrorInApplication = false;
    problemUpdateFileForApplication = "";
    currentUpdateIndex = 0;

    SDL_CreateThread(CheckForUpdatesScreen::CheckForUpdatesStatic, "CheckForUpdatesThread", this);
}

int CheckForUpdatesScreen::CheckForUpdatesStatic(void *pData)
{
    CheckForUpdatesScreen *pScreen = reinterpret_cast<CheckForUpdatesScreen *>(pData);
    pScreen->CheckForUpdates();
    return 0;
}

void CheckForUpdatesScreen::CheckForUpdates()
{
    stack<PendingUpdate> pendingUpdateStackLocal;
    Version newestVersionLocal;
    int totalFileSizeLocal = 0;
    bool updatesExist = gVersionsXmlFilePath.length() > 0;
    string versionsXmlContent = "";

    if (!updatesExist)
    {
        updatesExist = CheckIfUpdatesExist(&versionsXmlContent);
    }

    if (updatesExist)
    {
        try
        {
            PendingUpdate pendingUpdate;
            XmlReader versionReader;

            if (gVersionsXmlFilePath.length() > 0)
            {
                versionReader.ParseXmlFile(gVersionsXmlFilePath.c_str());
            }
            else
            {
                versionReader.ParseXmlContent(versionsXmlContent);
            }

            versionReader.StartElement("VersionInformation");
            versionReader.StartElement("Versions");
            versionReader.StartList("Version");

            while (versionReader.MoveToNextListItem())
            {
                versionReader.StartElement("VersionNumber");
                Version newVersion(&versionReader);

                if (gVersion < newVersion)
                {
                    pendingUpdate = PendingUpdate();

                    if (newVersion > newestVersionLocal)
                    {
                        newestVersionLocal = newVersion;
                    }
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
                    string fileName;

                    if (versionReader.ElementExists("FileName"))
                    {
                        fileName = versionReader.ReadTextElement("FileName");
                    }
                    else
                    {
                    #ifdef __WINDOWS
                        fileName = versionReader.ReadTextElement("FileNameWindows");
                    #elif __OSX
                        fileName = versionReader.ReadTextElement("FileNameOSX");
                    #elif __unix
                        fileName = versionReader.ReadTextElement("FileNameUnix");
                    #else
                    #error NOT IMPLEMENTED
                    #endif
                    }

                    string fileLocation = versionReader.ReadTextElement("Location");
                    string action = versionReader.ReadTextElement("Action");

                    int deltaSize = 0;

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
                            fileName = versionReader.ReadTextElement("DeltaSizeUnix");
                        #else
                        #error NOT IMPLEMENTED
                        #endif
                        }
                    }

                    string deltaLocation = "";

                    if (action != "Remove")
                    {
                        if (versionReader.ElementExists("DeltaLocation"))
                        {
                            deltaLocation = versionReader.ReadTextElement("DeltaLocation");
                        }
                        else
                        {
                        #ifdef __WINDOWS
                            deltaLocation = versionReader.ReadTextElement("DeltaLocationWindows");
                        #elif __OSX
                            deltaLocation = versionReader.ReadTextElement("DeltaLocationOSX");
                        #elif __unix
                            deltaLocation = versionReader.ReadTextElement("DeltaLocationUnix");
                        #else
                        #error NOT IMPLEMENTED
                        #endif
                        }
                    }

                    string signature = "";

                    if (action != "Remove")
                    {
                        if (versionReader.ElementExists("Signature"))
                        {
                            signature = versionReader.ReadTextElement("Signature");
                        }
                        else
                        {
                        #ifdef __WINDOWS
                            signature = versionReader.ReadTextElement("SignatureWindows");
                        #elif __OSX
                            signature = versionReader.ReadTextElement("SignatureOSX");
                        #elif __unix
                            signature = versionReader.ReadTextElement("SignatureUnix");
                        #else
                        #error NOT IMPLEMENTED
                        #endif
                        }
                    }

                    pendingUpdate.AddFileUpdate(
                        fileName,
                        (string)newVersion,
                        fileLocation,
                        action,
                        deltaSize,
                        deltaLocation,
                        signature,
                        this);

                    totalFileSizeLocal += deltaSize;
                }

                versionReader.EndElement();

                pendingUpdateStackLocal.push(pendingUpdate);
            }

            versionReader.EndElement();
            versionReader.EndElement();
        }
        catch (MLIException e)
        {
            // If something bad happened while getting update information,
            // then we should clear the pending update stack to ensure we do nothing
            // while we're in a bad state.
            while (!pendingUpdateStackLocal.empty())
            {
                pendingUpdateStackLocal.pop();
            }
        }
    }

    if (gVersionsXmlFilePath.length() > 0)
    {
        RemoveFile(gVersionsXmlFilePath);
    }

    SDL_SemWait(pInteropSemaphore);
    finishedCheckingForUpdates = true;
    pendingUpdateStack = pendingUpdateStackLocal;
    newestVersion = newestVersionLocal;
    totalFileSize = totalFileSizeLocal;
    SDL_SemPost(pInteropSemaphore);
}

int CheckForUpdatesScreen::DownloadUpdatesStatic(void *pData)
{
    CheckForUpdatesScreen *pScreen = reinterpret_cast<CheckForUpdatesScreen *>(pData);
    pScreen->DownloadUpdates();
    return 0;
}

void CheckForUpdatesScreen::DownloadUpdates()
{
    while (!pendingUpdateStack.empty())
    {
        PendingUpdate pendingUpdate = pendingUpdateStack.top();
        pendingUpdateStack.pop();

        string deltaLocationUri;
        string deltaContentSignature;
        string filePathToApplyTo;
        string newVersionString;

        pendingUpdate.BeginFileUpdateIteration();

        while (pendingUpdate.MoveToNextFileUpdate())
        {
            CheckForUpdatesScreen::ScheduledAction *pScheduledAction = pendingUpdate.ScheduleFileUpdate();

            if (pScheduledAction != NULL)
            {
                scheduledActionQueue.push(pScheduledAction);

                SDL_SemWait(pInteropSemaphore);
                totalUpdateCount++;
                SDL_SemPost(pInteropSemaphore);
            }
            else
            {
                SDL_SemWait(pInteropSemaphore);
                downloadComplete = true;
                wasErrorInDownload = true;
                problemUpdateFileForDownload = pendingUpdate.GetFileUpdateFilePath();
                SDL_SemPost(pInteropSemaphore);

                return;
            }
        }
    }

    SDL_SemWait(pInteropSemaphore);
    downloadComplete = true;
    wasErrorInDownload = false;
    currentUpdateIndex = 1;
    SDL_SemPost(pInteropSemaphore);
}

int CheckForUpdatesScreen::DownloadUpdatesProgressCallback(void *pData, double downloadTotal, double downloadNow, double uploadTotal, double uploadNow)
{
    CheckForUpdatesScreen *pScreen = reinterpret_cast<CheckForUpdatesScreen *>(pData);
    pScreen->UpdateDownloadProgress(downloadNow);

    SDL_SemWait(pScreen->pInteropSemaphore);
    bool updatesCanceledLocal = pScreen->updatesCanceled;
    SDL_SemPost(pScreen->pInteropSemaphore);

    return updatesCanceledLocal ? 1 : 0;
}

void CheckForUpdatesScreen::UpdateDownloadProgress(double newBytesDownloaded)
{
    SDL_SemWait(pInteropSemaphore);
    totalBytesDownloaded += newBytesDownloaded;
    SDL_SemPost(pInteropSemaphore);
}

int CheckForUpdatesScreen::ApplyUpdatesStatic(void *pData)
{
    CheckForUpdatesScreen *pScreen = reinterpret_cast<CheckForUpdatesScreen *>(pData);
    pScreen->ApplyUpdates();
    return 0;
}

void CheckForUpdatesScreen::ApplyUpdates()
{
    while (!scheduledActionQueue.empty())
    {
        CheckForUpdatesScreen::ScheduledAction *pScheduledAction = scheduledActionQueue.front();
        scheduledActionQueue.pop();

        bool updateWasPerformed = pScheduledAction->PerformUpdate();

        SDL_SemWait(pInteropSemaphore);
        bool updatesCanceledLocal = updatesCanceled;
        SDL_SemPost(pInteropSemaphore);

        if (!updateWasPerformed || updatesCanceledLocal)
        {
            // If we failed to perform an update or if the update was canceled,
            // then we're in a bad state - we should roll back all applied updates
            // to ensure we end up back in a good state.
            pScheduledAction->RollBack();

            while (!completedActionStack.empty())
            {
                CheckForUpdatesScreen::ScheduledAction *pCompletedAction = completedActionStack.top();
                completedActionStack.pop();

                pCompletedAction->RollBack();
                delete pCompletedAction;
            }

            SDL_SemWait(pInteropSemaphore);
            applicationComplete = true;
            wasErrorInApplication = true;
            problemUpdateFileForApplication = pScheduledAction->GetUpdatedFileFilePath();
            SDL_SemPost(pInteropSemaphore);

            delete pScheduledAction;
            return;
        }
        else
        {
            SDL_SemWait(pInteropSemaphore);
            if (!scheduledActionQueue.empty())
            {
                currentUpdateIndex++;
            }
            SDL_SemPost(pInteropSemaphore);

            completedActionStack.push(pScheduledAction);
        }
    }

    // If all scheduled updates were completed successfully,
    // then we don't need the old files anymore, so we can delete them.
    while (!completedActionStack.empty())
    {
        CheckForUpdatesScreen::ScheduledAction *pCompletedAction = completedActionStack.top();
        completedActionStack.pop();

        pCompletedAction->Complete();
        delete pCompletedAction;
    }

    SDL_SemWait(pInteropSemaphore);
    applicationComplete = true;
    wasErrorInApplication = false;
    SDL_SemPost(pInteropSemaphore);
}

void CheckForUpdatesScreen::Update(int delta)
{
    if (isFinished)
    {
        return;
    }

    if (!drawnOnce)
    {
        return;
    }

    switch (currentState)
    {
    case StateCheckingForUpdates:
        UpdateCheckingForUpdates(delta);
        break;

    case StateDownloadingUpdates:
        UpdateDownloadingUpdates(delta);
        break;

    case StateApplyingUpdates:
        UpdateApplyingUpdates(delta);
        break;
    }
}

void CheckForUpdatesScreen::UpdateCheckingForUpdates(int delta)
{
    bool updateStatusKnown = false;
    stack<PendingUpdate> pendingUpdateStackLocal;
    Version newestVersionLocal;
    int totalFileSizeLocal = 0;

    SDL_SemWait(pInteropSemaphore);
    if (gIsQuitting)
    {
        updatesCanceled = true;
        gIsQuitting = false;
    }

    updateStatusKnown = finishedCheckingForUpdates;

    if (updateStatusKnown)
    {
        pendingUpdateStackLocal = pendingUpdateStack;
        newestVersionLocal = newestVersion;
        totalFileSizeLocal = totalFileSize;
    }
    SDL_SemPost(pInteropSemaphore);

    if (updateStatusKnown)
    {
        if (!updatesCanceled && !pendingUpdateStackLocal.empty())
        {
            // If we've received a versions XML file path from the launcher,
            // then we've already prompted the user whether or not they want to download updates,
            // so we don't need to do so again.
            if (gVersionsXmlFilePath.length() > 0 || PromptUserToDownloadUpdates(gVersion, newestVersionLocal, totalFileSizeLocal))
            {
                totalFileSizeString = FileSizeToString(totalFileSizeLocal);

                numDots = 1;
                timeSinceLastDotsUpdate = 0;

                currentState = StateDownloadingUpdates;
                SDL_CreateThread(CheckForUpdatesScreen::DownloadUpdatesStatic, "DownloadUpdatesThread", this);
            }
            else
            {
                Finish();
            }
        }
        else
        {
            Finish();
        }
    }
    else
    {
        timeSinceLastDotsUpdate += delta;

        while (timeSinceLastDotsUpdate > DotsUpdateDelayMs)
        {
            timeSinceLastDotsUpdate -= DotsUpdateDelayMs;
            numDots++;

            if (numDots > 3)
            {
                numDots = 1;
            }
        }
    }
}

void CheckForUpdatesScreen::UpdateDownloadingUpdates(int delta)
{
    SDL_SemWait(pInteropSemaphore);
    if (gIsQuitting)
    {
        updatesCanceled = true;
        gIsQuitting = false;
    }

    bool downloadCompleteLocal = downloadComplete;
    bool wasErrorInDownloadLocal = wasErrorInDownload;
    string problemUpdateFileForDownloadLocal = problemUpdateFileForDownload;
    SDL_SemPost(pInteropSemaphore);

    if (downloadCompleteLocal)
    {
        if (!wasErrorInDownloadLocal)
        {
            numDots = 1;
            timeSinceLastDotsUpdate = 0;

            currentState = StateApplyingUpdates;
            SDL_CreateThread(CheckForUpdatesScreen::ApplyUpdatesStatic, "ApplyUpdatesThread", this);
        }
        else
        {
            if (!updatesCanceled)
            {
                char error[1024];
                sprintf(error, "An error was encountered while downloading updates. The update for %s was corrupt.\n\nSorry about that. Try again later.", GetFileNameFromFilePath(ConvertSeparatorsInPath(problemUpdateFileForDownloadLocal)).c_str());
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error applying updates", error, gpWindow);
            }

            Finish();
        }
    }
    else if (updatesCanceled)
    {
        timeSinceLastDotsUpdate += delta;

        while (timeSinceLastDotsUpdate > DotsUpdateDelayMs)
        {
            timeSinceLastDotsUpdate -= DotsUpdateDelayMs;
            numDots++;

            if (numDots > 3)
            {
                numDots = 1;
            }
        }
    }
}

void CheckForUpdatesScreen::UpdateApplyingUpdates(int delta)
{
    SDL_SemWait(pInteropSemaphore);
    if (gIsQuitting)
    {
        updatesCanceled = true;
        gIsQuitting = false;
    }

    bool applicationCompleteLocal = applicationComplete;
    bool wasErrorInApplicationLocal = wasErrorInApplication;
    string problemUpdateFileForApplicationLocal = problemUpdateFileForApplication;
    SDL_SemPost(pInteropSemaphore);

    if (applicationCompleteLocal)
    {
        if (wasErrorInApplicationLocal)
        {
            if (!updatesCanceled)
            {
                char error[1024];
                sprintf(error, "An error was encountered while applying updates. Failed updating %s.\n\nSorry about that. Try again later.", GetFileNameFromFilePath(ConvertSeparatorsInPath(problemUpdateFileForApplicationLocal)).c_str());
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error applying updates", error, gpWindow);
            }
        }
        else
        {
            WriteNewVersion(newestVersion);
        }

        Finish();
    }
    else
    {
        timeSinceLastDotsUpdate += delta;

        while (timeSinceLastDotsUpdate > DotsUpdateDelayMs)
        {
            timeSinceLastDotsUpdate -= DotsUpdateDelayMs;
            numDots++;

            if (numDots > 3)
            {
                numDots = 1;
            }
        }
    }
}

void CheckForUpdatesScreen::Draw()
{
    if (isFinished)
    {
        return;
    }

    string stringForWidth;
    string statusString = GetStatusString(&stringForWidth);
    pTextDisplayFont->Draw(statusString, (Vector2(gScreenWidth, gScreenHeight) * 0.5) - (Vector2(pTextDisplayFont->GetWidth(stringForWidth), pTextDisplayFont->GetLineHeight()) * 0.5));
    drawnOnce = true;
}

string CheckForUpdatesScreen::GetStatusString(string *pStringForWidth)
{
    string statusString = "";
    *pStringForWidth = "";

    SDL_SemWait(pInteropSemaphore);
    bool updatesCanceledLocal = updatesCanceled;
    SDL_SemPost(pInteropSemaphore);

    if (updatesCanceledLocal)
    {
        statusString = "Canceling updates";
        *pStringForWidth = statusString;

        for (int i = 0; i < numDots; i++)
        {
            statusString += ".";
        }
    }
    else
    {
        switch (currentState)
        {
        case StateCheckingForUpdates:
            statusString = GetCheckingForUpdatesString(pStringForWidth);
            break;

        case StateDownloadingUpdates:
            statusString = GetDownloadingUpdatesString(pStringForWidth);
            break;

        case StateApplyingUpdates:
            statusString = GetApplyingUpdatesString(pStringForWidth);
            break;
        }
    }

    return statusString;
}

string CheckForUpdatesScreen::GetCheckingForUpdatesString(string *pStringForWidth)
{
    string statusString = "Checking for updates";
    *pStringForWidth = statusString;

    for (int i = 0; i < numDots; i++)
    {
        statusString += ".";
    }

    return statusString;
}

string CheckForUpdatesScreen::GetDownloadingUpdatesString(string *pStringForWidth)
{
    SDL_SemWait(pInteropSemaphore);
    double totalBytesDownloadedLocal = totalBytesDownloaded;
    SDL_SemPost(pInteropSemaphore);

    char statusString[256];
    sprintf(statusString, "Downloading updates (%d%% of %s)", (int)(100 * totalBytesDownloadedLocal / totalFileSize + 0.5), totalFileSizeString.c_str());
    char statusStringForWidth[256];
    sprintf(statusStringForWidth, "Downloading updates (100%% of %s)", totalFileSizeString.c_str());

    *pStringForWidth = string(statusStringForWidth);
    return string(statusString);
}

string CheckForUpdatesScreen::GetApplyingUpdatesString(string *pStringForWidth)
{
    SDL_SemWait(pInteropSemaphore);
    int totalUpdateCountLocal = totalUpdateCount;
    int currentUpdateIndexLocal = currentUpdateIndex;
    SDL_SemPost(pInteropSemaphore);

    char updateString[256];
    sprintf(updateString, "Applying updates (%d of %d)", currentUpdateIndexLocal, totalUpdateCountLocal);

    char updateStringForWidth[256];
    sprintf(updateStringForWidth, "Applying updates (%d of %d)", totalUpdateCountLocal, totalUpdateCountLocal);

    string statusString = string(updateString);
    *pStringForWidth = string(updateStringForWidth);

    for (int i = 0; i < numDots; i++)
    {
        statusString += ".";
    }

    return statusString;
}

void CheckForUpdatesScreen::Finish()
{
    nextScreenId = "";
    isFinished = true;
    UnloadResources();
}

#endif
