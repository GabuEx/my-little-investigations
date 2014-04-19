/**
 * Basic header/include file for CheckForUpdatesScreen.cpp.
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

#ifndef CHECKFORUPDATESSCREEN_H
#define CHECKFORUPDATESSCREEN_H

#ifdef UPDATER
#include "MLIScreen.h"
#include "../Utils.h"
#include "../Version.h"

#include <SDL2/SDL.h>

#include <queue>
#include <stack>
#include <vector>

using namespace std;

class MLIFont;

class CheckForUpdatesScreen : public MLIScreen
{
private:
    enum State
    {
        StateCheckingForUpdates,
        StateDownloadingUpdates,
        StateApplyingUpdates,
    };

    class ScheduledAction;

    class PendingUpdate
    {
        class PendingFileUpdate
        {
            enum FileLocation
            {
                FileLocationNone,
                FileLocationExecutionPath,
                FileLocationCommonAppDataPath,
                FileLocationUserAppDataPath,
            };

            enum Action
            {
                ActionNone,
                ActionUpdate,
                ActionAdd,
                ActionRemove,
            };

        public:
            PendingFileUpdate(string fileName, string newVersionString, string fileLocation, string action, int deltaSize, string deltaLocation, string signature, CheckForUpdatesScreen *pOwningScreen)
            {
                this->fileName = fileName;
                this->newVersionString = newVersionString;
                this->fileLocation = FileLocationNone;

                if (fileLocation == "ExecutionPath")
                {
                    this->fileLocation = FileLocationExecutionPath;
                }
                else if (fileLocation == "CommonAppDataPath")
                {
                    this->fileLocation = FileLocationCommonAppDataPath;
                }
                else if (fileLocation == "UserAppDataPath")
                {
                    this->fileLocation = FileLocationUserAppDataPath;
                }

                this->action = ActionNone;

                if (action == "Update")
                {
                    this->action = ActionUpdate;
                }
                else if (action == "Add")
                {
                    this->action = ActionAdd;
                }
                else if (action == "Remove")
                {
                    this->action = ActionRemove;
                }

                this->deltaSize = deltaSize;
                this->deltaLocation = deltaLocation;
                this->signature = signature;
                this->pOwningScreen = pOwningScreen;
            }

            string GetFileName() { return this->fileName; }
            string GetFilePath();
            string GetVersionString() { return this->newVersionString; }
            int GetDeltaSize() { return this->deltaSize; }
            string GetDeltaLocation() { return this->deltaLocation; }
            string GetSignature() { return this->signature; }

            ScheduledAction * ScheduleAction();

        private:
            string fileName;
            string newVersionString;
            FileLocation fileLocation;
            Action action;
            int deltaSize;
            string deltaLocation;
            string signature;
            CheckForUpdatesScreen *pOwningScreen;
        };

    public:
        PendingUpdate()
        {
            pendingFileUpdates.clear();
        }

        void AddFileUpdate(string fileName, string newVersionString, string fileLocation, string action, int deltaSize, string deltaLocation, string signature, CheckForUpdatesScreen *pOwningScreen)
        {
            pendingFileUpdates.push_back(PendingFileUpdate(fileName, newVersionString, fileLocation, action, deltaSize, deltaLocation, signature, pOwningScreen));
        }

        void BeginFileUpdateIteration()
        {
            pendingFileUpdatesIterator = pendingFileUpdates.begin();
        }

        bool MoveToNextFileUpdate()
        {
            if (pendingFileUpdatesIterator == pendingFileUpdates.end())
            {
                return false;
            }

            pCurrentFileUpdate = &(*pendingFileUpdatesIterator);
            pendingFileUpdatesIterator++;
            return true;
        }

        ScheduledAction * ScheduleFileUpdate()
        {
            return pCurrentFileUpdate->ScheduleAction();
        }

        string GetFileUpdateFilePath()
        {
            return pCurrentFileUpdate->GetFilePath();
        }

    private:
        vector<PendingFileUpdate> pendingFileUpdates;
        vector<PendingFileUpdate>::iterator pendingFileUpdatesIterator;

        PendingFileUpdate *pCurrentFileUpdate;
    };

    class ScheduledAction
    {
    public:
        ScheduledAction(string updatedFileFilePath)
        {
            this->updatedFileFilePath = updatedFileFilePath;
        }

        virtual ~ScheduledAction() {}

        string GetUpdatedFileFilePath() { return updatedFileFilePath; }

        virtual bool PerformUpdate() = 0;
        virtual void RollBack() = 0;
        virtual void Complete() = 0;

    private:
        string updatedFileFilePath;
    };

    class ScheduledUpdate : public ScheduledAction
    {
    public:
        ScheduledUpdate(string oldFilePath, string deltaFilePath, string newFilePath)
            : ScheduledAction(newFilePath)
        {
            this->oldFilePath = oldFilePath;
            this->deltaFilePath = deltaFilePath;
            this->newFilePath = newFilePath;
        }

        virtual bool PerformUpdate();
        virtual void RollBack();
        virtual void Complete();

    private:
        string oldFilePath;
        string deltaFilePath;
        string newFilePath;
    };

    class ScheduledAdd : public ScheduledAction
    {
    public:
        ScheduledAdd(string stagingFilePath, string newFilePath, byte *pFileContent, size_t fileContentSize)
            : ScheduledAction(newFilePath)
        {
            this->stagingFilePath = stagingFilePath;
            this->newFilePath = newFilePath;
            this->pFileContent = pFileContent;
            this->fileContentSize = fileContentSize;
        }

        virtual bool PerformUpdate();
        virtual void RollBack();
        virtual void Complete();

    private:
        string stagingFilePath;
        string newFilePath;
        byte *pFileContent;
        size_t fileContentSize;
    };

    class ScheduledRemove : public ScheduledAction
    {
    public:
        ScheduledRemove(string stagingFilePath, string oldFilePath)
            : ScheduledAction(oldFilePath)
        {
            this->stagingFilePath = stagingFilePath;
            this->oldFilePath = oldFilePath;
        }

        virtual bool PerformUpdate();
        virtual void RollBack();
        virtual void Complete();

    private:
        string stagingFilePath;
        string oldFilePath;
    };

public:
    CheckForUpdatesScreen(MLIFont *pTextDisplayFont);
    ~CheckForUpdatesScreen();

    void LoadResources();
    void UnloadResources();
    void Init();

    static int CheckForUpdatesStatic(void *pData);
    void CheckForUpdates();

    static int DownloadUpdatesStatic(void *pData);
    void DownloadUpdates();

    static int DownloadUpdatesProgressCallback(void *pScreen, double downloadTotal, double downloadNow, double uploadTotal, double uploadNow);
    void UpdateDownloadProgress(double newBytesDownloaded);

    static int ApplyUpdatesStatic(void *pData);
    void ApplyUpdates();

    void Update(int delta);

    void UpdateCheckingForUpdates(int delta);
    void UpdateDownloadingUpdates(int delta);
    void UpdateApplyingUpdates(int delta);

    void Draw();

    string GetStatusString(string *pStringForWidth);
    string GetCheckingForUpdatesString(string *pStringForWidth);
    string GetDownloadingUpdatesString(string *pStringForWidth);
    string GetApplyingUpdatesString(string *pStringForWidth);

    void Finish();

    bool GetShowCursor() { return true; }

private:
    MLIFont *pTextDisplayFont;
    SDL_sem *pInteropSemaphore;

    bool drawnOnce;
    State currentState;
    bool updatesCanceled;

    int numDots;
    int timeSinceLastDotsUpdate;

    // Checking for updates fields
    bool finishedCheckingForUpdates;
    stack<PendingUpdate> pendingUpdateStack;
    Version newestVersion;
    int totalFileSize;

    // Downloading updates fields
    string totalFileSizeString;
    double totalBytesDownloaded;
    queue<ScheduledAction *> scheduledActionQueue;
    int totalUpdateCount;
    bool downloadComplete;
    bool wasErrorInDownload;
    string problemUpdateFileForDownload;

    // Applying updates fields
    bool applicationComplete;
    bool wasErrorInApplication;
    string problemUpdateFileForApplication;
    stack<ScheduledAction *> completedActionStack;
    int currentUpdateIndex;
};

#endif

#endif
