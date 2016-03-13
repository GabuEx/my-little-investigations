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
#ifdef MLI_DEBUG
            DebugSignatureIsValid((const byte *)pDeltaContent, deltaContentSize, signature)
#else
            SignatureIsValid((const byte *)pDeltaContent, deltaContentSize, signature)
#endif
            )
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
#ifdef MLI_DEBUG
            DebugSignatureIsValid((const byte *)pFileContent, fileContentSize, signature)
#else
            SignatureIsValid((const byte *)pFileContent, fileContentSize, signature)
#endif
            )
        {
            string stagingFilePath = GetTempDirectoryPath() + GetFileNameFromFilePath(GetFilePath()) + ".toadd";
            ofstream fileStream(stagingFilePath.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);

            if (fileStream)
            {
                fileStream.write((const char *)pFileContent, fileContentSize);
                fileStream.close();

                delete [] pFileContent;
            }
            else
            {
                delete [] pFileContent;

                return NULL;
            }

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

string CheckForUpdatesScreen::ScheduledUpdate::GetApplyScriptFileInstrunctions(unsigned int versionUpdateIndex, unsigned int versionUpdateSubIndex)
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/PatchingText").c_str(), newFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRenameFileScriptInstructions(newFilePath, oldFilePath);
    scriptFileInstructions += GetCheckReturnValueScriptInstructions(versionUpdateIndex, versionUpdateSubIndex);
    scriptFileInstructions += GetApplyDeltaFileScriptInstructions(oldFilePath, deltaFilePath, newFilePath);
    scriptFileInstructions += GetCheckReturnValueScriptInstructions(versionUpdateIndex, versionUpdateSubIndex);
    scriptFileInstructions += GetRemoveFileScriptInstructions(deltaFilePath);
    return scriptFileInstructions;

}

string CheckForUpdatesScreen::ScheduledUpdate::GetRollBackScriptFileInstrunctions()
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/UndoingPatchText").c_str(), newFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRemoveFileScriptInstructions(newFilePath);
    scriptFileInstructions += GetRenameFileScriptInstructions(oldFilePath, newFilePath);
    return scriptFileInstructions;
}

string CheckForUpdatesScreen::ScheduledUpdate::GetCompletedScriptFileInstrunctions()
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/RemovingTemporaryFileText").c_str(), oldFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRemoveFileScriptInstructions(oldFilePath);
    return scriptFileInstructions;
}

string CheckForUpdatesScreen::ScheduledAdd::GetApplyScriptFileInstrunctions(unsigned int versionUpdateIndex, unsigned int versionUpdateSubIndex)
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/AddingText").c_str(), newFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRenameFileScriptInstructions(stagingFilePath, newFilePath);
    scriptFileInstructions += GetCheckReturnValueScriptInstructions(versionUpdateIndex, versionUpdateSubIndex);
    return scriptFileInstructions;
}

string CheckForUpdatesScreen::ScheduledAdd::GetRollBackScriptFileInstrunctions()
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/UndoingAddText").c_str(), newFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRemoveFileScriptInstructions(newFilePath);
    return scriptFileInstructions;
}

string CheckForUpdatesScreen::ScheduledAdd::GetCompletedScriptFileInstrunctions()
{
    // Nothing to do - the file already exists.
    return "";
}

string CheckForUpdatesScreen::ScheduledRemove::GetApplyScriptFileInstrunctions(unsigned int versionUpdateIndex, unsigned int versionUpdateSubIndex)
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/RemovingText").c_str(), oldFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRenameFileScriptInstructions(oldFilePath, stagingFilePath);
    scriptFileInstructions += GetCheckReturnValueScriptInstructions(versionUpdateIndex, versionUpdateSubIndex);
    return scriptFileInstructions;
}

string CheckForUpdatesScreen::ScheduledRemove::GetRollBackScriptFileInstrunctions()
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/UndoingRemoveText").c_str(), oldFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRenameFileScriptInstructions(stagingFilePath, oldFilePath);
    return scriptFileInstructions;
}

string CheckForUpdatesScreen::ScheduledRemove::GetCompletedScriptFileInstrunctions()
{
    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/RemovingTemporaryFileText").c_str(), stagingFilePath.c_str());

    string scriptFileInstructions = GetPrintStringScriptInstructions(buf);
    scriptFileInstructions += GetRemoveFileScriptInstructions(stagingFilePath);
    return scriptFileInstructions;
}

CheckForUpdatesScreen::VersionIncrementScriptContents::VersionIncrementScriptContents()
    : VersionIncrementScriptContents(0, "", "")
{
}

CheckForUpdatesScreen::VersionIncrementScriptContents::VersionIncrementScriptContents(unsigned int index, const string &oldVersionString, const string &newVersionString)
{
    this->index = index;
    this->oldVersionString = oldVersionString;
    this->newVersionString = newVersionString;

    applicationInstructionsList.clear();
    rollBackInstructionsList.clear();
    completionInstructionsList.clear();
}

CheckForUpdatesScreen::VersionIncrementScriptContents::VersionIncrementScriptContents(const VersionIncrementScriptContents &rhs)
{
    index = rhs.index;
    oldVersionString = rhs.oldVersionString;
    newVersionString = rhs.newVersionString;

    applicationInstructionsList = rhs.applicationInstructionsList;
    rollBackInstructionsList = rhs.rollBackInstructionsList;
    completionInstructionsList = rhs.completionInstructionsList;
}

void CheckForUpdatesScreen::VersionIncrementScriptContents::AddInstructionSet(
    const string &applicationInstructions,
    const string &rollBackInstructions,
    const string &completionInstructions)
{
    applicationInstructionsList.push_back(applicationInstructions);
    rollBackInstructionsList.push_back(rollBackInstructions);
    completionInstructionsList.push_back(completionInstructions);
}

string CheckForUpdatesScreen::VersionIncrementScriptContents::GenerateScriptContents()
{
    string scriptContents;

    char buf[256];
    snprintf(buf, 256, gpLocalizableContent->GetText("Updater/UpdatingToVersionText").c_str(), oldVersionString.c_str(), newVersionString.c_str());

    scriptContents += GetPrintStringScriptInstructions(buf);
    scriptContents += GetPrintEmptyLineScriptInstructions();

#ifdef __WINDOWS
    string errorLabelNamePrefix = ":HandleError" + IntegerToString(index);
    string completionLabelName = ":PerformCompletion" + IntegerToString(index);

    scriptContents += "goto :ExecuteVersionUpdate" + GetNewlineString() + GetNewlineString();
    unsigned int subIndex = 1;

    for (const string &rollBackInstructions : rollBackInstructionsList)
    {
        string errorLabelName = errorLabelNamePrefix + IntegerToString(subIndex);

        scriptContents += errorLabelName + GetNewlineString();
        scriptContents += rollBackInstructions;
        scriptContents += "goto :eof" + GetNewlineString() + GetNewlineString();

        subIndex++;
    }

    scriptContents += completionLabelName + GetNewlineString();

    for (const string &completionInstructions : completionInstructionsList)
    {
        scriptContents += completionInstructions;
    }

    scriptContents += GetNewlineString();
    scriptContents += GetWriteNewVersionScriptInstructions(newVersionString);
    scriptContents += GetNewlineString();

    scriptContents += "goto :eof" + GetNewlineString() + GetNewlineString();
    scriptContents += ":ExecuteVersionUpdate" + GetNewlineString() + GetNewlineString();

    for (const string &applicationInstructions : applicationInstructionsList)
    {
        scriptContents += applicationInstructions;
    }

    scriptContents += GetNewlineString();
    scriptContents += "call " + completionLabelName + GetNewlineString();
    scriptContents += GetPrintEmptyLineScriptInstructions();
#elif defined(__OSX) || defined(__unix)
    string errorFunctionNamePrefix = "HandleError" + IntegerToString(index);
    string completionFunctionName = "PerformCompletion" + IntegerToString(index);
    unsigned int subIndex = 1;

    for (const string &rollBackInstructions : rollBackInstructionsList)
    {
        string errorFunctionName = errorFunctionNamePrefix + IntegerToString(subIndex);

        scriptContents += errorFunctionName + " ()" + GetNewlineString();
        scriptContents += "{" + GetNewlineString();
        scriptContents += rollBackInstructions;
        scriptContents += "}" + GetNewlineString() + GetNewlineString();

        subIndex++;
    }

    scriptContents += completionFunctionName + " ()" + GetNewlineString();
    scriptContents += "{" + GetNewlineString();

    for (const string &completionInstructions : completionInstructionsList)
    {
        scriptContents += completionInstructions;
    }

    scriptContents += "}" + GetNewlineString() + GetNewlineString();

    for (const string &applicationInstructions : applicationInstructionsList)
    {
        scriptContents += applicationInstructions;
    }
#else
#error NOT IMPLEMENTED
#endif

    return scriptContents;
}

CheckForUpdatesScreen::VersionIncrementScriptContents & CheckForUpdatesScreen::VersionIncrementScriptContents::operator=(const VersionIncrementScriptContents &rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    index = rhs.index;
    oldVersionString = rhs.oldVersionString;
    newVersionString = rhs.newVersionString;

    applicationInstructionsList = rhs.applicationInstructionsList;
    rollBackInstructionsList = rhs.rollBackInstructionsList;
    completionInstructionsList = rhs.completionInstructionsList;

    return *this;
}

CheckForUpdatesScreen::CheckForUpdatesScreen(MLIFont *pTextDisplayFont)
{
    this->pTextDisplayFont = pTextDisplayFont;

    pInteropSemaphore = SDL_CreateSemaphore(1);
}

CheckForUpdatesScreen::~CheckForUpdatesScreen()
{
    UnloadResources();

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
                    pendingUpdate = PendingUpdate(newVersion);

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
    vector<VersionIncrementScriptContents> versionIncrementScriptContentsList;
    string oldVersionString = ((string)gVersion);

    unsigned int versionUpdateIndex = 1;

    while (!pendingUpdateStack.empty())
    {
        PendingUpdate pendingUpdate = pendingUpdateStack.top();
        pendingUpdateStack.pop();

        string newVersionString = pendingUpdate.GetNewVersionString();

        VersionIncrementScriptContents versionIncrementScriptContents(versionUpdateIndex, oldVersionString, newVersionString);

        pendingUpdate.BeginFileUpdateIteration();
        string undoScriptInstructions;

        unsigned int versionUpdateSubIndex = 1;

        while (pendingUpdate.MoveToNextFileUpdate())
        {
            CheckForUpdatesScreen::ScheduledAction *pScheduledAction = pendingUpdate.ScheduleFileUpdate();

            if (pScheduledAction != NULL)
            {
                undoScriptInstructions = pScheduledAction->GetRollBackScriptFileInstrunctions() + undoScriptInstructions;

                versionIncrementScriptContents.AddInstructionSet(
                    pScheduledAction->GetApplyScriptFileInstrunctions(versionUpdateIndex, versionUpdateSubIndex),
                    undoScriptInstructions,
                    pScheduledAction->GetCompletedScriptFileInstrunctions());

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

            versionUpdateSubIndex++;
        }

        versionIncrementScriptContentsList.push_back(versionIncrementScriptContents);

        oldVersionString = newVersionString;
        versionUpdateIndex++;
    }

    string scriptFileContents = GetScriptInstructionsHeader();
    scriptFileContents += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/UpdateWarningText"));
    scriptFileContents += GetPrintEmptyLineScriptInstructions();
    scriptFileContents += GetWaitForExitScriptInstructions();
    scriptFileContents += GetPrintEmptyLineScriptInstructions();

    for (VersionIncrementScriptContents versionIncrementScriptContents : versionIncrementScriptContentsList)
    {
        scriptFileContents += versionIncrementScriptContents.GenerateScriptContents();
    }

    scriptFileContents += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/UpdateCompleteText"));
    scriptFileContents += GetPrintEmptyLineScriptInstructions();
    scriptFileContents += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/StartingExecutableText"));
    scriptFileContents += GetStartGameScriptInstructions();

    gUpdateScriptFilePath = CreateUpdateScript(scriptFileContents);

    SDL_SemWait(pInteropSemaphore);
    downloadComplete = true;
    wasErrorInDownload = false;
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
            Finish();
        }
        else
        {
            if (!updatesCanceled)
            {
                char error[1024];
                sprintf(error, gpLocalizableContent->GetText("Updater/ErrorEncounteredMessageBoxBodyText").c_str(), GetFileNameFromFilePath(ConvertSeparatorsInPath(problemUpdateFileForDownloadLocal)).c_str());
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, gpLocalizableContent->GetText("Updater/ErrorEncounteredMessageBoxTitleText").c_str(), error, gpWindow);
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
        *pStringForWidth = gpLocalizableContent->GetText("Updater/CancelingUpdatesTextForWidth");

        switch (numDots)
        {
        case 1:
            statusString = gpLocalizableContent->GetText("Updater/CancelingUpdatesText0");
            break;

        case 2:
            statusString = gpLocalizableContent->GetText("Updater/CancelingUpdatesText1");
            break;

        case 3:
            statusString = gpLocalizableContent->GetText("Updater/CancelingUpdatesText2");
            break;

        default:
            ThrowException("We should never have less than one or greater than three dots.");
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
        }
    }

    return statusString;
}

string CheckForUpdatesScreen::GetCheckingForUpdatesString(string *pStringForWidth)
{
    *pStringForWidth = gpLocalizableContent->GetText("Updater/CheckingForUpdatesTextForWidth");

    switch (numDots)
    {
    case 1:
        return gpLocalizableContent->GetText("Updater/CheckingForUpdatesText0");
        break;

    case 2:
        return gpLocalizableContent->GetText("Updater/CheckingForUpdatesText1");
        break;

    case 3:
        return gpLocalizableContent->GetText("Updater/CheckingForUpdatesText2");
        break;

    default:
        ThrowException("We should never have less than one or greater than three dots.");
    }
}

string CheckForUpdatesScreen::GetDownloadingUpdatesString(string *pStringForWidth)
{
    SDL_SemWait(pInteropSemaphore);
    double totalBytesDownloadedLocal = totalBytesDownloaded;
    SDL_SemPost(pInteropSemaphore);

    char statusString[256];
    sprintf(statusString, gpLocalizableContent->GetText("Updater/DownloadingUpdatesText").c_str(), PartialFileSizeToString(totalBytesDownloadedLocal, totalFileSize).c_str(), totalFileSizeString.c_str());
    char statusStringForWidth[256];
    sprintf(statusStringForWidth, gpLocalizableContent->GetText("Updater/DownloadingUpdatesText").c_str(), totalFileSizeString.c_str(), totalFileSizeString.c_str());

    *pStringForWidth = string(statusStringForWidth);
    return string(statusString);
}

void CheckForUpdatesScreen::Finish()
{
    nextScreenId = "";
    isFinished = true;
    UnloadResources();
}

#endif
