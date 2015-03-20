/**
 * Basic header/include file for FileFunctions.cpp.
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

#ifndef FILEFUNCTIONS_H
#define FILEFUNCTIONS_H

#include "Version.h"

#include <string>
#include <vector>

using namespace std;

void LoadFilePathsAndCaseUuids(string executableFilePath);

#ifdef GAME_EXECUTABLE
string GetUuidFromFilePath(const string &caseFilePath);

vector<string> GetCaseFilePaths();
bool IsCaseCorrectlySigned(const string &caseFilePath, bool useLookupTable = true);
void PopulateCaseSignatureMap();
vector<string> GetCaseUuids();
bool IsCaseCompleted(const string &caseUuid);
bool CopyCaseFileToCaseFolder(const string &caseFilePath, const string &caseUuid);
#endif

string ConvertSeparatorsInPath(string &path);
inline string ConvertSeparatorsInPath(const string &path)
{
    string path1 = path;
    return ConvertSeparatorsInPath(path1);
}
string GetFileNameFromFilePath(const string &path);

string GetCommonResourcesFilePath();

string GetVersionFilePath();
Version GetCurrentVersion();

#ifdef UPDATER
void WriteNewVersion(Version newVersion);
#endif

#ifdef GAME_EXECUTABLE
string GetConfigFilePath();
bool ConfigFileExists();
void SaveConfigurations();
void LoadConfigurations();

string GetCompletedCasesFilePath();
bool CompletedCasesFileExists();
void SaveCompletedCase(const string &caseUuid);
void LoadCompletedCases();

bool SaveFileExists();
bool SaveFileExistsForCase(const string &caseUuid);
string GetSaveFolderPathForCase(const string &caseUuid);
vector<string> GetSaveFilePathsForCase(const string &caseUuid);
bool IsAutosave(const string &saveFilePath);

string GetDialogsSeenListFilePathForCase(const string &caseUuid);
bool DialogsSeenListFileExistsForCase(const string &caseUuid);
void SaveDialogsSeenListForCase(const string &caseUuid);
void LoadDialogsSeenListForCase(const string &caseUuid);

bool CheckForExistingInstance();
#endif

#ifndef GAME_EXECUTABLE
string GetFileNameFromUri(const string &uri);

string GetExecutionPath();
string GetCommonAppDataPath();
string GetUserAppDataPath();
string GetTempDirectoryPath();

string GetLauncherFontFilePath();

#ifdef __OSX
string GetUpdaterHelperFilePath();

bool TryAcquireUpdateAdministratorRightsOSX();
void FreeAdministratorRights();
#endif

bool LaunchExecutable(const char *pExecutablePath, vector<string> commandLineArguments, bool waitForCompletion, bool asAdmin);
void LaunchGameExecutable();
#endif

#ifdef UPDATER
bool ApplyDeltaFile(const string &oldFilePath, const string &deltaFilePath, const string &newFilePath);
bool RemoveFile(const string &filePath);
bool RenameFile(const string &oldFilePath, const string &newFilePath);
#endif
#ifdef LAUNCHER
bool LaunchUpdater(const string &versionsXmlFilePath);
string SaveVersionsXmlFile(const string &versionsXmlContent);
#endif

#endif
