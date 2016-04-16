/**
 * Functions dealing with file handling for use in the game.
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

#include "FileFunctions.h"

#include <SDL2/SDL.h>

#include "KeyboardHelper.h"

#include "SharedUtils.h"
#include "Utils.h"
#include "XmlReader.h"
#include "XmlWriter.h"
#include "globals.h"

#ifdef GAME_EXECUTABLE
#include "ResourceLoader.h"
#endif

#include <fstream>

#ifdef __WINDOWS
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR> > tstring;

string TStringToString(tstring tstr)
{
    string str = "";

    for (tstring::iterator iter = tstr.begin(); iter != tstr.end(); ++iter)
    {
        str.push_back((char)(*iter));
    }

    return str;
}

tstring StringToTString(string str)
{
    tstring tstr = "";

    for (string::iterator iter = str.begin(); iter != str.end(); ++iter)
    {
        tstr.push_back((TCHAR)(*iter));
    }

    return tstr;
}
#elif __OSX
#include "../osx/ApplicationSupportBridge.h"

#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif __unix
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

bool Exists(string path)
{
    struct stat st;
    return stat(path.c_str(), &st) != -1;
}

void MakeDirIfNotExists(string path)
{
    if(!Exists(path))
    {
        mkdir(path.c_str(), 0755);
    }
}

struct Dir_Autoclean
{
    DIR* dir;

    Dir_Autoclean(std::string path) : dir ( opendir(path.c_str()) ) {}
    Dir_Autoclean(const char* path) : dir ( opendir(path) ) {}

    ~Dir_Autoclean() { if ( dir ) closedir(dir); }

    operator void*() const { return dir; }
};

struct String_Hax : std::string
{
    String_Hax ( const char* s ) : std::string(s) {}
    String_Hax ( const std::string& s ) : std::string(s) {}
    operator bool() const { return true; }
};

#define FOR_EACH_FILE(name,dirpath) \
     if (Dir_Autoclean dir = dirpath ) \
         while(dirent *ep = readdir (dir.dir)) \
             if ( String_Hax name = std::string(dirpath) + ep->d_name )

string graphicalSudo = "xdg-su ";
#endif

string pathSeparator;
string otherPathSeparator;

string commonAppDataPath;
string localizedCommonResourcesPath;
string casesPath;
string userAppDataPath;
string dialogSeenListsPath;
string savesPath;

string executionPath;

#ifndef GAME_EXECUTABLE
string tempDirectoryPath;
#endif

void LoadFilePathsAndCaseUuids(string executableFilePath)
{
#ifdef __WINDOWS
        pathSeparator = "\\";
        otherPathSeparator = "/";

#ifndef GAME_EXECUTABLE
        TCHAR szTempPath[MAX_PATH] = { 0 };
        DWORD tempPathLength = GetTempPath(MAX_PATH, szTempPath);
        tempDirectoryPath = TStringToString(tstring(szTempPath, tempPathLength));
#endif

        TCHAR szPath[MAX_PATH] = { 0 };
        if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
        {
            PathAppend(szPath, TEXT("\\My Little Investigations\\"));
            commonAppDataPath = TStringToString(tstring(szPath));

            PathAppend(szPath, TEXT("Cases\\"));
            casesPath = TStringToString(tstring(szPath));
        }

        if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
        {
            PathAppend(szPath, TEXT("\\My Little Investigations\\"));
            DWORD ftyp = GetFileAttributes(szPath);
            if (ftyp == INVALID_FILE_ATTRIBUTES) CreateDirectory(szPath, NULL);
            userAppDataPath = TStringToString(tstring(szPath));

            PathAppend(szPath, TEXT("DialogSeenLists\\"));
            ftyp = GetFileAttributes(szPath);
            if (ftyp == INVALID_FILE_ATTRIBUTES) CreateDirectory(szPath, NULL);
            dialogSeenListsPath = TStringToString(tstring(szPath));
        }

        if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
        {
            PathAppend(szPath, TEXT("\\My Little Investigations\\"));
            PathAppend(szPath, TEXT("Saves\\"));
            DWORD ftyp = GetFileAttributes(szPath);
            if (ftyp == INVALID_FILE_ATTRIBUTES) CreateDirectory(szPath, NULL);
            savesPath = TStringToString(tstring(szPath));
        }
#elif __OSX
        pathSeparator = "/";
        otherPathSeparator = "\\";

#ifndef GAME_EXECUTABLE
        tempDirectoryPath = getenv("TMPDIR");
#endif

        // The file system representation Objective C function seems to omit the
        // trailing slash unless it's explicit.
        // stringByAppendingPathComponent has no way of doing this
        // explicitly, though.
        commonAppDataPath = string(pLocalApplicationSupportPath) + "/";
        localizedCommonResourcesPath = string(pLocalizedCommonResourcesPath) + "/";
        casesPath = string(pCasesPath) + "/";
        userAppDataPath = string(pUserApplicationSupportPath) + "/";
        dialogSeenListsPath = string(pDialogSeenListsPath) + "/";
        savesPath = string(pSavesPath) + "/";
#elif __unix
        pathSeparator = "/";
        otherPathSeparator = "\\";

#ifndef GAME_EXECUTABLE
        const char* tmp = getenv("TMPDIR");
        // If TMPDIR isn't set, assume the temporary directory is "/tmp".
        if(!tmp)
        {
            tmp = "/tmp";
        }

        tempDirectoryPath = string(tmp) + "/";
#endif
        string homedir = getenv("HOME");
        commonAppDataPath = "/usr/share/MyLittleInvestigations/";
        casesPath = commonAppDataPath + "Cases/";
        userAppDataPath = homedir + "/.MyLittleInvestigations/";
        MakeDirIfNotExists(userAppDataPath);
        dialogSeenListsPath = userAppDataPath + "DialogSeenLists/";
        MakeDirIfNotExists(dialogSeenListsPath);
        savesPath = userAppDataPath + "Saves/";
        MakeDirIfNotExists(savesPath);
        graphicalSudo = "xdg-su";
        const char* desktopEnvironment = getenv("XDG_CURRENT_DESKTOP");
        if (desktopEnvironment != NULL)
        {
            string de(desktopEnvironment);
            if (de == "GNOME")
            {
                graphicalSudo = "gksudo ";
            }
            else if (de == "KDE")
            {
                graphicalSudo = "kdesu ";
            }
        }
#else
#error NOT IMPLEMENTED
#endif

// If we're in debug mode for the updater, then we want to have hard-coded executable file paths,
// since otherwise we'll fail to update.  We want to test updating the actual executable,
// not the one in our compiler's bin directory.
#if defined(MLI_DEBUG) && defined (UPDATER)

#ifdef __WINDOWS
    executableFilePath = "C:\\Program Files (x86)\\My Little Investigations\\MyLittleInvestigations.exe";
#endif
#ifdef __OSX
    executableFilePath = "/Applications/My Little Investigations.app/Contents/MacOS/MyLittleInvestigations";
#endif

#else
    executableFilePath = ConvertSeparatorsInPath(executableFilePath);
#endif

    executionPath = executableFilePath.substr(0, executableFilePath.find_last_of(pathSeparator)) + pathSeparator;
}

#ifdef GAME_EXECUTABLE
string GetUuidFromFilePath(const string &filePath)
{
    string uuid = filePath;
    if (uuid.rfind(pathSeparator) != string::npos)
    {
        uuid = uuid.substr(uuid.rfind(pathSeparator) + 1);
    }

    if (uuid.rfind(".") != string::npos)
    {
        uuid = uuid.substr(0, uuid.rfind("."));
    }

    // What's left is the UUID.
    return uuid;
}

vector<string> GetCaseFilePaths()
{
    vector<string> filePaths;

#ifdef __WINDOWS
        tstring tstrPath = StringToTString(casesPath) + tstring(TEXT("*.mlicase"));

        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFile(tstrPath.c_str(), &ffd);

        if (INVALID_HANDLE_VALUE != hFind)
        {
            do
            {
                filePaths.push_back(casesPath + TStringToString(tstring(ffd.cFileName)));
            }
            while (FindNextFile(hFind, &ffd) != 0);

            FindClose(hFind);
        }
#elif __OSX
        vector<string> caseFilePaths = GetCaseFilePathsOSX();

        for (unsigned int i = 0; i < caseFilePaths.size(); i++)
        {
            string caseFilePath = caseFilePaths[i];

            if (caseFilePath.find(".mlicase") != string::npos)
            {
                filePaths.push_back(caseFilePath);
            }
        }
#elif __unix
        FOR_EACH_FILE(caseFilePath,casesPath)
        {
            if (caseFilePath.find(".mlicase") != string::npos)
            {
                filePaths.push_back(caseFilePath);
            }
        }
#else
#error NOT IMPLEMENTED
#endif

    return filePaths;
}

bool IsCaseCorrectlySigned(const string &caseFilePath, bool useLookupTable)
{
    if (useLookupTable && gCaseIsSignedByFilePathMap.count(caseFilePath))
    {
        return gCaseIsSignedByFilePathMap[caseFilePath];
    }

    bool isCaseCorrectlySigned = ResourceLoader::GetInstance()->IsCaseCorrectlySigned(caseFilePath);
    gCaseIsSignedByFilePathMap[caseFilePath] = isCaseCorrectlySigned;
    return isCaseCorrectlySigned;
}

void PopulateCaseSignatureMap()
{
    gCaseIsSignedByFilePathMap.clear();
    vector<string> caseFilePaths = GetCaseFilePaths();

    for (unsigned int i = 0; i < caseFilePaths.size(); i++)
    {
        string caseFilePath = caseFilePaths[i];

        // IsCaseCorrectlySigned will add the result to the lookup table if it's not already there,
        // so we'll just call it on every case file path to populate that table.
        IsCaseCorrectlySigned(caseFilePath, false /* useLookupTable */);
    }
}

vector<string> GetCaseUuids()
{
    vector<string> caseUuids;
    vector<string> filePaths = GetCaseFilePaths();

    for (unsigned int i = 0; i < filePaths.size(); i++)
    {
        caseUuids.push_back(GetUuidFromFilePath(filePaths[i]));
    }

    return caseUuids;
}

bool IsCaseCompleted(const string &caseUuid)
{
    bool isCaseCompleted = false;

    for (unsigned int i = 0; i < gCompletedCaseGuidList.size(); i++)
    {
        if (caseUuid == gCompletedCaseGuidList[i])
        {
            isCaseCompleted = true;
            break;
        }
    }

    return isCaseCompleted;
}

bool CopyCaseFileToCaseFolder(const string &caseFilePath, const string &caseUuid)
{
    bool success = false;

    #ifdef __WINDOWS
    TCHAR szPath[MAX_PATH] = { 0 };
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
    {
        PathAppend(szPath, TEXT("\\My Little Investigations"));
        DWORD ftyp = GetFileAttributes(szPath);
        if (ftyp == INVALID_FILE_ATTRIBUTES) CreateDirectory(szPath, NULL);
        PathAppend(szPath, TEXT("\\Cases"));
        ftyp = GetFileAttributes(szPath);
        if (ftyp == INVALID_FILE_ATTRIBUTES) CreateDirectory(szPath, NULL);

        tstring tstrSzPath = tstring(szPath);
        string filePath = "";

        for (tstring::iterator iter = tstrSzPath.begin(); iter != tstrSzPath.end(); ++iter)
        {
            filePath.push_back((char)(*iter));
        }

        success = CopyFileA(caseFilePath.c_str(), (filePath + string("\\") + caseUuid + string(".mlicase")).c_str(), false /* bFailIfExists */) == TRUE;
    }
    #endif

    return success;
}

vector<string> GetLanguageResourcesFilePaths()
{
    vector<string> filePaths;

#ifdef __WINDOWS
        tstring tstrPath = StringToTString(GetLocalizedCommonResourcesDirectoryPath()) + tstring(TEXT("*.dat"));

        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFile(tstrPath.c_str(), &ffd);

        if (INVALID_HANDLE_VALUE != hFind)
        {
            do
            {
                filePaths.push_back(GetLocalizedCommonResourcesDirectoryPath() + TStringToString(tstring(ffd.cFileName)));
            }
            while (FindNextFile(hFind, &ffd) != 0);

            FindClose(hFind);
        }
#elif __OSX
        vector<string> localizedCommonResourcesFilePaths = GetLocalizedCommonResourcesFilePathsOSX();

        for (unsigned int i = 0; i < localizedCommonResourcesFilePaths.size(); i++)
        {
            string localizedCommonResourcesFilePath = localizedCommonResourcesFilePaths[i];

            if (localizedCommonResourcesFilePath.find(".dat") != string::npos)
            {
                filePaths.push_back(localizedCommonResourcesFilePath);
            }
        }
#elif __unix
        FOR_EACH_FILE(localizedCommonResourcesFilePath,localizedCommonResourcesFilePaths)
        {
            if (localizedCommonResourcesFilePath.find(".dat") != string::npos)
            {
                filePaths.push_back(localizedCommonResourcesFilePath);
            }
        }
#else
#error NOT IMPLEMENTED
#endif

    return filePaths;
}
#endif

const string & GetPathSeparator() { return pathSeparator; }

string ConvertSeparatorsInPath(string &path)
{
    while (path.find_first_of(otherPathSeparator) != string::npos)
    {
        path = path.replace(path.find_first_of(otherPathSeparator), 1, pathSeparator);
    }

    return path;
}

string GetFileNameFromFilePath(const string &path)
{
    return path.substr(path.find_last_of(pathSeparator) + 1);
}

string GetCommonResourcesFilePath()
{
    return commonAppDataPath + "common.dat";
}

string GetLocalizedCommonResourcesDirectoryPath()
{
    return commonAppDataPath + pathSeparator + "Languages" + pathSeparator;
}

#ifdef __OSX
string GetPropertyListPath()
{
#if defined(MLI_DEBUG)
    return "/Applications/My Little Investigations.app/Contents/Info.plist";
#endif
    return executionPath + ".." + pathSeparator + "Info.plist";
}
#endif

string GetVersionFilePath()
{
    return commonAppDataPath + "version.xml";
}

Version GetCurrentVersion()
{
    string versionString;

#ifdef __WINDOWS
    TCHAR stringValue[255];
    HKEY hKey = NULL;
    DWORD dwType = REG_SZ;
    DWORD dwSize = 255;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MyLittleInvestigations", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS &&
        RegQueryValueEx(hKey, TEXT("DisplayVersion"), NULL, &dwType, (LPBYTE)&stringValue, &dwSize) == ERROR_SUCCESS)
    {
        versionString = TStringToString(stringValue);
    }

    RegCloseKey(hKey);
#elif __OSX
    versionString = GetVersionStringOSX(GetPropertyListPath());
#elif __unix
    string versionFilePath = commonAppDataPath + string(".version");
    if(Exists(versionFilePath))
    {
        ifstream versionFile (versionFilePath.c_str());
        if (versionFile.is_open() && !versionFile.eof())
        {
            versionFile >> versionString;
        }

        versionFile.close();
    }
#else
#error NOT IMPLEMENTED
#endif

    if (versionString.length() > 0)
    {
        return Version(versionString);
    }
    else
    {
        XmlReader versionReader(GetVersionFilePath().c_str());
        return Version(&versionReader);
    }
}

string GetConfigFilePath()
{
    return userAppDataPath + "Config.xml";
}

bool ConfigFileExists()
{
    ifstream configFileStream(GetConfigFilePath().c_str());

    return configFileStream.is_open();
}

#ifdef GAME_EXECUTABLE
void SaveConfigurations()
{
    XmlWriter configWriter(GetConfigFilePath().c_str());

    configWriter.StartElement("Configurations");
    configWriter.WriteBooleanElement("EnableTutorials", gEnableTutorials);
    configWriter.WriteBooleanElement("EnableHints", gEnableHints);
    configWriter.WriteBooleanElement("EnableFullscreen", gEnableFullscreen);
    configWriter.WriteBooleanElement("EnableSkippingUnseenDialog", gEnableSkippingUnseenDialog);
#ifdef ENABLE_DEBUG_MODE
    configWriter.WriteBooleanElement("EnableDebugMode", gEnableDebugMode);
#endif
    configWriter.WriteDoubleElement("BackgroundMusicVolume", gBackgroundMusicVolume);
    configWriter.WriteDoubleElement("SoundEffectsVolume", gSoundEffectsVolume);
    configWriter.WriteDoubleElement("VoiceVolume", gVoiceVolume);
    configWriter.WriteTextElement("LocalizedResourcesFileName", gLocalizedResourcesFileName);

    KeyboardHelper::WriteConfig(configWriter);

    configWriter.EndElement();
}

#endif

void LoadConfigurations()
{
    if (ConfigFileExists())
    {
        try
        {
#ifdef GAME_EXECUTABLE
            bool enableTutorials = gEnableTutorials;
            bool enableHints = gEnableHints;
            bool enableFullscreen = gEnableFullscreen;
            bool enableSkippingUnseenDialog = gEnableSkippingUnseenDialog;
#ifdef ENABLE_DEBUG_MODE
            bool enableDebugMode = gEnableDebugMode;
#endif
            double backgroundMusicVolume = gBackgroundMusicVolume;
            double soundEffectsVolume = gSoundEffectsVolume;
            double voiceVolume = gVoiceVolume;
#endif
            string localizedResourcesFileName = gLocalizedResourcesFileName;

            XmlReader configReader(GetConfigFilePath().c_str());

            if (configReader.ElementExists("Configurations"))
            {
                configReader.StartElement("Configurations");

#ifdef GAME_EXECUTABLE
                if (configReader.ElementExists("EnableTutorials"))
                {
                    enableTutorials = configReader.ReadBooleanElement("EnableTutorials");
                }

                if (configReader.ElementExists("EnableHints"))
                {
                    enableHints = configReader.ReadBooleanElement("EnableHints");
                }

                if (configReader.ElementExists("EnableFullscreen"))
                {
                    enableFullscreen = configReader.ReadBooleanElement("EnableFullscreen");
                }

                if (configReader.ElementExists("EnableSkippingUnseenDialog"))
                {
                    enableSkippingUnseenDialog = configReader.ReadBooleanElement("EnableSkippingUnseenDialog");
                }

#ifdef ENABLE_DEBUG_MODE
                if (configReader.ElementExists("EnableDebugMode"))
                {
                    enableDebugMode = configReader.ReadBooleanElement("EnableDebugMode");
                }
#endif

                if (configReader.ElementExists("BackgroundMusicVolume"))
                {
                    backgroundMusicVolume = configReader.ReadDoubleElement("BackgroundMusicVolume");
                }

                if (configReader.ElementExists("SoundEffectsVolume"))
                {
                    soundEffectsVolume = configReader.ReadDoubleElement("SoundEffectsVolume");
                }

                if (configReader.ElementExists("VoiceVolume"))
                {
                    voiceVolume = configReader.ReadDoubleElement("VoiceVolume");
                }
#endif

                if (configReader.ElementExists("LocalizedResourcesFileName"))
                {
                    localizedResourcesFileName = configReader.ReadTextElement("LocalizedResourcesFileName");
                }

#ifdef GAME_EXECUTABLE
                KeyboardHelper::ReadConfig(configReader);
#endif

                configReader.EndElement();
            }

#ifdef GAME_EXECUTABLE
            gEnableTutorials = enableTutorials;
            gEnableHints = enableHints;
            gEnableFullscreen = enableFullscreen;
            gEnableSkippingUnseenDialog = enableSkippingUnseenDialog;
#ifdef ENABLE_DEBUG_MODE
            gEnableDebugMode = enableDebugMode;
#endif
            gBackgroundMusicVolume = backgroundMusicVolume;
            gSoundEffectsVolume = soundEffectsVolume;
            gVoiceVolume = voiceVolume;
#endif
            gLocalizedResourcesFileName = localizedResourcesFileName;
        }
        catch (MLIException e)
        {
            // Nothing to do - we just won't load any configurations if we ran into trouble.
        }
    }
}

#ifdef GAME_EXECUTABLE
string GetCompletedCasesFilePath()
{
    return userAppDataPath + "CompletedCases.xml";
}

bool CompletedCasesFileExists()
{
    ifstream completedCasesFileStream(GetCompletedCasesFilePath().c_str());

    return completedCasesFileStream.is_open();
}

void SaveCompletedCase(const string &caseUuid)
{
    bool caseAlreadyCompleted = false;

    for (unsigned int i = 0; i < gCompletedCaseGuidList.size(); i++)
    {
        if (gCompletedCaseGuidList[i] == caseUuid)
        {
            caseAlreadyCompleted = true;
            break;
        }
    }

    if (!caseAlreadyCompleted)
    {
        gCompletedCaseGuidList.push_back(caseUuid);
    }
    else
    {
        return;
    }

    XmlWriter configWriter(GetCompletedCasesFilePath().c_str());

    configWriter.StartElement("CompletedCaseList");

    for (unsigned int i = 0; i < gCompletedCaseGuidList.size(); i++)
    {
        configWriter.StartElement("Case");
        configWriter.WriteTextElement("UUID", gCompletedCaseGuidList[i]);
        configWriter.EndElement();
    }

    configWriter.EndElement();
}

void LoadCompletedCases()
{
    if (CompletedCasesFileExists())
    {
        try
        {
            vector<string> completedCaseGuidList;

            XmlReader completedCasesReader(GetCompletedCasesFilePath().c_str());

            if (completedCasesReader.ElementExists("CompletedCaseList"))
            {
                completedCasesReader.StartElement("CompletedCaseList");

                if (completedCasesReader.ElementExists("Case"))
                {
                    completedCasesReader.StartList("Case");

                    while (completedCasesReader.MoveToNextListItem())
                    {
                        if (completedCasesReader.ElementExists("UUID"))
                        {
                            completedCaseGuidList.push_back(completedCasesReader.ReadTextElement("UUID"));
                        }
                    }
                }

                completedCasesReader.EndElement();
            }

            gCompletedCaseGuidList.clear();

            for (unsigned int i = 0; i < completedCaseGuidList.size(); i++)
            {
                gCompletedCaseGuidList.push_back(completedCaseGuidList[i]);
            }
        }
        catch (MLIException e)
        {
            // Nothing to do - we just won't load completed cases if we ran into trouble.
        }
    }
}

bool SaveFileExists()
{
    bool saveFileExists = false;
    vector<string> caseUuids = GetCaseUuids();

    for (unsigned int i = 0; i < caseUuids.size(); i++)
    {
        string caseUuid = caseUuids[i];

        if (SaveFileExistsForCase(caseUuid))
        {
            saveFileExists = true;
            break;
        }
    }

    return saveFileExists;
}

bool SaveFileExistsForCase(const string &caseUuid)
{
    return GetSaveFilePathsForCase(caseUuid).size() > 0;
}

string GetSaveFolderPathForCase(const string &caseUuid)
{
    string path = savesPath + caseUuid + pathSeparator;
#ifdef __unix
    MakeDirIfNotExists(path);
#endif
    return path;
}

vector<string> GetSaveFilePathsForCase(const string &caseUuid)
{
    vector<string> filePaths;

#ifdef __WINDOWS
        string caseSavesPath = savesPath + caseUuid + pathSeparator;
        tstring tstrPath = StringToTString(caseSavesPath);
        DWORD ftyp = GetFileAttributes(tstrPath.c_str());
        if (ftyp == INVALID_FILE_ATTRIBUTES) CreateDirectory(tstrPath.c_str(), NULL);
        tstrPath += tstring(TEXT("*.sav"));

        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFile(tstrPath.c_str(), &ffd);

        if (INVALID_HANDLE_VALUE != hFind)
        {
            do
            {
                filePaths.push_back(caseSavesPath + TStringToString(tstring(ffd.cFileName)));
            }
            while (FindNextFile(hFind, &ffd) != 0);

            FindClose(hFind);
        }
#elif __OSX
        vector<string> saveFilePaths = GetSaveFilePathsForCaseOSX(caseUuid);

        for (unsigned int j = 0; j < saveFilePaths.size(); j++)
        {
            string saveFilePath = saveFilePaths[j];

            if (saveFilePath.find(".sav") != string::npos)
            {
                filePaths.push_back(saveFilePath);
            }
        }
#elif __unix
        FOR_EACH_FILE(saveFilePath, GetSaveFolderPathForCase(caseUuid))
        {
            if (saveFilePath.find(".sav") != string::npos)
            {
                filePaths.push_back(saveFilePath);
            }
        }
#else
#error NOT IMPLEMENTED
#endif

    return filePaths;
}

bool IsAutosave(const string &saveFilePath)
{
    return GetUuidFromFilePath(saveFilePath) == "00000000-0000-0000-0000-000000000000";
}

string GetDialogsSeenListFilePathForCase(const string &caseUuid)
{
    return dialogSeenListsPath + caseUuid + string(".xml");
}

bool DialogsSeenListFileExistsForCase(const string &caseUuid)
{
    ifstream dialogsSeenListFileStream(GetDialogsSeenListFilePathForCase(caseUuid).c_str());

    return dialogsSeenListFileStream.is_open();
}

void SaveDialogsSeenListForCase(const string &caseUuid)
{
    XmlWriter dialogsSeenListWriter(GetDialogsSeenListFilePathForCase(caseUuid).c_str());

    dialogsSeenListWriter.StartElement("DialogsSeenList");

    for (unsigned int i = 0; i < gDialogsSeenList.size(); i++)
    {
        dialogsSeenListWriter.StartElement("Dialog");
        dialogsSeenListWriter.WriteTextElement("Id", gDialogsSeenList[i]);
        dialogsSeenListWriter.EndElement();
    }

    dialogsSeenListWriter.EndElement();
}

void LoadDialogsSeenListForCase(const string &caseUuid)
{
    if (DialogsSeenListFileExistsForCase(caseUuid))
    {
        try
        {
            vector<string> dialogsSeenList;

            XmlReader dialogsSeenListReader(GetDialogsSeenListFilePathForCase(caseUuid).c_str());

            if (dialogsSeenListReader.ElementExists("DialogsSeenList"))
            {
                dialogsSeenListReader.StartElement("DialogsSeenList");

                if (dialogsSeenListReader.ElementExists("Dialog"))
                {
                    dialogsSeenListReader.StartList("Dialog");

                    while (dialogsSeenListReader.MoveToNextListItem())
                    {
                        if (dialogsSeenListReader.ElementExists("Id"))
                        {
                            dialogsSeenList.push_back(dialogsSeenListReader.ReadTextElement("Id"));
                        }
                    }
                }

                dialogsSeenListReader.EndElement();
            }

            gDialogsSeenList.clear();

            for (unsigned int i = 0; i < dialogsSeenList.size(); i++)
            {
                gDialogsSeenList.push_back(dialogsSeenList[i]);
            }
        }
        catch (MLIException e)
        {
            // Nothing to do - we just won't load completed cases if we ran into trouble.
        }
    }
}

bool CheckForExistingInstance()
{
    bool existingInstanceExists = false;

    #ifdef __WINDOWS
        HWND hwnd = FindWindow("SDL_app", gTitle.c_str());

        if (hwnd)
        {
            SetForegroundWindow(hwnd);

            if (IsIconic(hwnd))
            {
                ShowWindow(hwnd, SW_RESTORE);
            }

            existingInstanceExists = true;
        }
    #endif

    return existingInstanceExists;
}
#endif

#ifndef GAME_EXECUTABLE
string GetFileNameFromUri(const string &uri)
{
    string fileName = uri;

    if (fileName.find_last_of("/") != string::npos)
    {
        fileName = fileName.substr(fileName.find_last_of("/") + 1);
    }

    return fileName;
}

string GetExecutionPath()
{
    return executionPath;
}

string GetCommonAppDataPath()
{
    return commonAppDataPath;
}

string GetUserAppDataPath()
{
    return userAppDataPath;
}

string GetTempDirectoryPath()
{
    return tempDirectoryPath;
}

#ifdef __WINDOWS
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwPID;
    GetWindowThreadProcessId(hwnd, &dwPID);

    if (dwPID == (DWORD)lParam)
    {
        SetFocus(hwnd);
        return FALSE;
    }

    return TRUE;
}
#endif

string GetGameExecutablePath()
{
#ifdef __WINDOWS
    return executionPath + "MyLittleInvestigations.exe";
#elif defined(__OSX) || defined(__unix)
    return executionPath + "MyLittleInvestigations";
#else
#error NOT IMPLEMENTED
#endif
}

string GetUpdaterExecutableName()
{
#ifdef __WINDOWS
    return "MyLittleInvestigationsUpdater.exe";
#elif defined(__OSX) || defined(__unix)
    return "MyLittleInvestigationsUpdater";
#else
#error NOT IMPLEMENTED
#endif
}

string GetUpdaterExecutablePath()
{
    return executionPath + GetUpdaterExecutableName();
}

bool LaunchExecutable(const char *pExecutablePath, vector<string> commandLineArguments, bool waitForCompletion, bool asAdmin)
{
    bool success = false;

#ifdef __WINDOWS
    char *pCommandLineArgumentsNoConst = NULL;

    if (commandLineArguments.size() > 0)
    {
        string commandLineArgumentsString = "";

        for (unsigned int i = 0; i < commandLineArguments.size(); i++)
        {
            commandLineArgumentsString += string(" \"") + commandLineArguments[i] + string("\"");
        }

        pCommandLineArgumentsNoConst = new char[commandLineArgumentsString.length() + 1];
        strcpy(pCommandLineArgumentsNoConst, commandLineArgumentsString.c_str());
    }

    if (!asAdmin)
    {
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInformation;

        memset(&startupInfo, 0, sizeof(STARTUPINFO));
        memset(&processInformation, 0, sizeof(PROCESS_INFORMATION));

        startupInfo.cb = sizeof(STARTUPINFO);
        startupInfo.dwFlags = STARTF_USESHOWWINDOW;
        startupInfo.wShowWindow = SW_SHOW;

        success =
            CreateProcessA(
                pExecutablePath,
                pCommandLineArgumentsNoConst,
                NULL, NULL,
                FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
                NULL, NULL,
                &startupInfo,
                &processInformation);

        if (success)
        {
            if (waitForCompletion)
            {
                DWORD exitCode = 0;

                WaitForSingleObject(processInformation.hProcess, INFINITE);
                bool gotExitCode = GetExitCodeProcess(processInformation.hProcess, &exitCode);
                CloseHandle(processInformation.hProcess);

                success =
                    gotExitCode &&
                    exitCode == 0;
            }
            else
            {
                // We're not waiting for the new window, so let's ensure it's brought to the foreground instead.
                EnumWindows(EnumWindowsProc, (LPARAM)processInformation.dwProcessId);
            }
        }
    }
    else
    {
        SHELLEXECUTEINFO shExecInfo;

        shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

        shExecInfo.fMask = 0;
        shExecInfo.hwnd = NULL;
        shExecInfo.lpVerb = "runas";
        shExecInfo.lpFile = pExecutablePath;
        shExecInfo.lpParameters = pCommandLineArgumentsNoConst;
        shExecInfo.lpDirectory = NULL;
        shExecInfo.nShow = SW_SHOW;
        shExecInfo.hInstApp = NULL;

        success = ShellExecuteEx(&shExecInfo);
    }
#elif __OSX
    if (asAdmin)
    {
        ThrowException("We should never need to execute anything as admin on OS X.");
    }
    else
    {
        char **ppArgv = new char*[commandLineArguments.size() + 2];

        ppArgv[0] = new char[strlen(pExecutablePath) + 1];
        strcpy(ppArgv[0], pExecutablePath);

        for (unsigned int i = 0; i < commandLineArguments.size(); i++)
        {
            ppArgv[i + 1] = new char[commandLineArguments[i].length() + 1];
            strcpy(ppArgv[i + 1], commandLineArguments[i].c_str());
        }

        ppArgv[commandLineArguments.size() + 1] = NULL;

        pid_t processId = fork();

        if (processId == 0)
        {
            execv(pExecutablePath, ppArgv);
        }
        else
        {
            if (waitForCompletion)
            {
                int childStatus;
                waitpid(processId, &childStatus, 0);

                success =
                    WIFEXITED(childStatus) &&
                    WEXITSTATUS(childStatus) == 0;
            }
            else
            {
                success = true;
            }
        }
    }
#elif __unix
    pid_t pid = fork();
    ostringstream result;
    for (typename vector<string>::const_iterator i = commandLineArguments.begin(); i != commandLineArguments.end(); i++)
    {
        if (i != commandLineArguments.begin())
        {
            result << " ";
        }

        result << *i;
    }

    string args = result.str();
    switch (pid)
    {
        case 0:
            if (asAdmin)
            {
                execl(strcat(const_cast<char *>(graphicalSudo.c_str()), pExecutablePath), args.c_str(), (char*) NULL);
            }
            else
            {
                execl(pExecutablePath, args.c_str(), (char*) NULL);
            }
            exit(1);
        default:
            if(waitForCompletion)
            {
                int status;
                waitpid(pid, &status, 0);
                success = status == 0;
            }
            else
            {
                success = true;
            }
    }
#else
#error NOT IMPLEMENTED
#endif

    return success;
}

bool LaunchUpdaterScript(const string &scriptFilePath)
{
    string executablePath;
    vector<string> commandLineArguments;

#ifdef __WINDOWS
    TCHAR szPath[MAX_PATH] = { 0 };
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, szPath)))
    {
        PathAppend(szPath, TEXT("cmd.exe"));
        executablePath = TStringToString(tstring(szPath));
        commandLineArguments.push_back(string("/C ") + scriptFilePath);
    }
    else
    {
        return false;
    }

    if (!LaunchExecutable(executablePath.c_str(), commandLineArguments, false /* waitForCompletion */, true /* asAdmin */))
    {
        return false;
    }
#elif defined(__OSX)
    system((string("osascript -e 'tell app \"Terminal\"' -e 'activate' -e 'do script \"") + scriptFilePath.c_str() + " \"' -e 'end tell'").c_str());
#else
#error NOT IMPLEMENTED
#endif

    return true;
}

void LaunchGameExecutable()
{
    string executablePath = GetGameExecutablePath();
    if (!LaunchExecutable(executablePath.c_str(), vector<string>(), false /* waitForCompletion */, false /* asAdmin */))
    {
        char error[256];
        snprintf(error, 256, "Error: Couldn't find MLI executable at %s!", executablePath.c_str());

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error starting MLI", error, NULL);
    }
}
#endif

#ifdef UPDATER
string GetNewlineString()
{
    return "\n";
}

string GetNullRedirectionString()
{
#ifdef __WINDOWS
    return " > nul";
#elif defined(__OSX) || defined(__unix)
    return " > /dev/null";
#else
#error NOT IMPLEMENTED
#endif
}

string GetScriptInstructionsHeader()
{
#ifdef __WINDOWS
    return "@echo off" + GetNewlineString() + GetNewlineString();
#elif defined(__OSX) || defined(__unix)
    return "";
#else
#error NOT IMPLEMENTED
#endif
}

string GetWaitForExitScriptInstructions()
{
    string scriptInstructions = GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/WaitingForProcessToCompleteText"));

#ifdef __WINDOWS
    scriptInstructions += ":WaitingForProcessLoop" + GetNewlineString();
    scriptInstructions += "tasklist | find /i " + GetUpdaterExecutableName() + GetNullRedirectionString() + " 2>&1" + GetNewlineString();
    scriptInstructions += "if %ERRORLEVEL% equ 0 (" + GetNewlineString();
    scriptInstructions += "timeout /t 1 /nobreak" + GetNullRedirectionString() + GetNewlineString();
    scriptInstructions += "goto :WaitingForProcessLoop" + GetNewlineString();
    scriptInstructions += ")" + GetNewlineString();
#elif defined(__OSX) || defined(__unix)
    scriptInstructions = "while pgrep -u root process_name > /dev/null; do sleep 1; done;" + GetNewlineString();
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions;
}

string GetPrintStringScriptInstructions(const string &s)
{
    return string("echo ") + s + GetNewlineString();
}

string GetPrintEmptyLineScriptInstructions()
{
    string scriptInstructions;

#ifdef __WINDOWS
    scriptInstructions = "echo.";
#elif defined(__OSX) || defined(__unix)
    scriptInstructions = "echo";
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions + GetNewlineString() + GetNewlineString();
}

string GetMakeExecutableScriptInstructions(const string &filePath)
{
    string scriptInstructions;

#ifdef __WINDOWS
    // Nothing to do on Windows - there, executability is a matter of file extension, not permissions.
#elif __OSX
    scriptInstructions += "chmod 755 \"" + filePath + "\"" + GetNullRedirectionString() + GetNewlineString();
#elif __unix
    scriptInstructions += "chmod 755 \"" + filePath + "\"" + GetNullRedirectionString() + GetNewlineString();
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions;
}

string GetApplyDeltaFileScriptInstructions(const string &oldFilePath, const string &deltaFilePath, const string &newFilePath)
{
    string scriptInstructions;

#ifdef __WINDOWS
    scriptInstructions = "\"" + executionPath + "deltatool" + pathSeparator + "xdelta3.exe\" -f -d -s \"" + oldFilePath + "\" \"" + deltaFilePath + "\" \"" + newFilePath + "\"";
#elif __OSX
    scriptInstructions = "\"" + executionPath + "deltatool" + pathSeparator + "xdelta3\" -f -d -s \"" + oldFilePath + "\" \"" + deltaFilePath + "\" \"" + newFilePath + "\"";
#elif __unix
    scriptInstructions = "xdelta3 -f -d -s \"" + oldFilePath + "\" \"" + deltaFilePath + "\" \"" + newFilePath + "\"";
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions + GetNullRedirectionString() + GetNewlineString();
}

string GetRemoveFileScriptInstructions(const string &filePath)
{
    string scriptInstructions;

#ifdef __WINDOWS
    scriptInstructions = string("if exist \"") + filePath + "\" del \"" + filePath + "\"" + GetNullRedirectionString();
#elif defined(__OSX) || defined(__unix)
    scriptInstructions = string("if [ -f \"") + filePath + "\" ]; then rm -f \"" + filePath + "\"" + GetNullRedirectionString() + "; fi";
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions + GetNewlineString();
}

string GetRenameFileScriptInstructions(const string &oldFilePath, const string &newFilePath)
{
    string scriptInstructions;

#ifdef __WINDOWS
    scriptInstructions = string("if exist \"") + oldFilePath + "\" move \"" + oldFilePath + "\" \"" + newFilePath + "\"" + GetNullRedirectionString();
#elif defined(__OSX) || defined(__unix)
    scriptInstructions = string("if [ -f \"") + oldFilePath + "\" ]; then mv -f \"" + oldFilePath + "\" \"" + newFilePath + "\"" + GetNullRedirectionString() + "; fi";
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions + GetNewlineString();
}

string GetCheckReturnValueScriptInstructions(unsigned int versionUpdateIndex, unsigned int versionUpdateSubIndex)
{
    string scriptInstructions = GetNewlineString();

#ifdef __WINDOWS
    string errorLabelNameToCall = ":HandleError" + IntegerToString(versionUpdateIndex) + IntegerToString(versionUpdateSubIndex);

    scriptInstructions += "if %ERRORLEVEL% neq 0 (" + GetNewlineString();
    scriptInstructions += "call " + errorLabelNameToCall + GetNewlineString();
    scriptInstructions += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/UpdateFailedText"));
    scriptInstructions += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/PressEnterToContinueText"));
    scriptInstructions += "pause" + GetNullRedirectionString() + GetNewlineString();
    scriptInstructions += GetStartGameScriptInstructions();
    scriptInstructions += "goto :eof" + GetNewlineString();
    scriptInstructions += ")" + GetNewlineString();
#elif defined(__OSX) || defined(__unix)
    string errorFunctionNameToCall = "HandleError" + IntegerToString(versionUpdateIndex) + IntegerToString(versionUpdateSubIndex);

    scriptInstructions += "if [ $? -ne 0 ]" + GetNewlineString();
    scriptInstructions += "then" + GetNewlineString();
    scriptInstructions += errorFunctionNameToCall + GetNewlineString();
    scriptInstructions += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/UpdateFailedText"));
    scriptInstructions += "read -p \"" + gpLocalizableContent->GetText("Updater/PressEnterToContinueText") + "\"" + GetNewlineString();
    scriptInstructions += GetStartGameScriptInstructions();
    scriptInstructions += "exit" + GetNewlineString();
    scriptInstructions += "fi" + GetNewlineString();
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions + GetNewlineString();
}

string GetWriteNewVersionScriptInstructions(const string &newVersionString)
{
#ifdef __WINDOWS
    return string("reg add \"HKEY_LOCAL_MACHINE\\Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MyLittleInvestigations\" /v DisplayVersion /t REG_SZ /d ") + newVersionString + " /f" + GetNullRedirectionString() + GetNewlineString();
#elif __OSX
    string scriptInstructions;

    unsigned long propertyListXmlDataLength = 0;
    char *pPropertyListXmlData = GetPropertyListXMLForVersionStringOSX(GetPropertyListPath(), newVersionString, &propertyListXmlDataLength);

    deque<string> propertyListXmlDataLines = split(string(pPropertyListXmlData, propertyListXmlDataLength), '\n');

    bool firstLine = true;
    string tempPropertyListPath = GetTempDirectoryPath() + "MyLittleInvestigations.plist";

    for (string propertyListXmlDataLine : propertyListXmlDataLines)
    {
        string redirectionString = firstLine ? ">" : ">>";
        scriptInstructions += "echo \"" + propertyListXmlDataLine + "\" " + redirectionString + " \"" + tempPropertyListPath + "\"" + GetNewlineString();
        firstLine = false;
    }

    scriptInstructions += GetNewlineString();
    scriptInstructions += GetRenameFileScriptInstructions(tempPropertyListPath, GetPropertyListPath());

    delete [] pPropertyListXmlData;

    return scriptInstructions;
#elif __unix
    return "echo \"" + newVersionString + "\" > \"" + commonAppDataPath + string(".version") + "\"" + GetNewlineString();
#endif
}

string GetStartGameScriptInstructions()
{
    string scriptInstructions;

#ifdef __WINDOWS
    scriptInstructions += GetPrintEmptyLineScriptInstructions();
    scriptInstructions += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/StartingExecutableText"));
    scriptInstructions += "start \"\" \"" + GetGameExecutablePath() + "\"" + GetNullRedirectionString() + GetNewlineString();
#elif defined(__OSX) || defined(__unix)
    // We won't do anything here - we start the game through the update applier.
    scriptInstructions += GetPrintEmptyLineScriptInstructions();
    scriptInstructions += GetPrintStringScriptInstructions(gpLocalizableContent->GetText("Updater/StartingExecutableText"));
    scriptInstructions += "sudo -u#" + IntegerToString(getuid()) + " \"" + GetGameExecutablePath() + "\" &>/dev/null &" + GetNewlineString();
#else
#error NOT IMPLEMENTED
#endif

    return scriptInstructions;
}

string CreateUpdateScript(const string &scriptContents)
{
    string updateScriptFilePath = GetTempDirectoryPath();
    string finalScriptContents = scriptContents;

#ifdef __WINDOWS
    updateScriptFilePath += "update.cmd";
#elif defined(__OSX) || defined(__unix)
    updateScriptFilePath += "update.sh";
#else
#error NOT IMPLEMENTED
#endif

#ifdef __WINDOWS
    finalScriptContents += GetRemoveFileScriptInstructions(updateScriptFilePath);
#endif

    ofstream scriptFileStream(updateScriptFilePath, ios_base::out | ios_base::trunc);

    if (scriptFileStream)
    {
        // We'll add on an instruction to make the script delete itself right at the end,
        // in order to ensure that that's the last thing it does, always.
        scriptFileStream.write(finalScriptContents.c_str(), finalScriptContents.length());
        scriptFileStream.close();
    }

#if defined(__OSX) || defined(__unix)
    chmod(updateScriptFilePath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif

    return updateScriptFilePath;
}
#endif

#ifdef LAUNCHER
bool LaunchUpdater(const string &versionsXmlFilePath)
{
    string executablePath = GetUpdaterExecutablePath();

    vector<string> commandLineArguments;
    commandLineArguments.push_back(versionsXmlFilePath);
#ifdef __unix
    // If we're on Unix, we need to pass the current user id to the updater so it can drop root privileges
    // before launching the game.
    std::stringstream out;
    out << getuid();
    commandLineArguments.push_back(out.str());
#endif

    // On Windows, we can launch the entire update application in admin mode.
    // On OS X, however, we need to instead launch it in standard mode and then acquire rights to run
    // the update applications as root.
    // On Unix, we can launch the entire update application in admin mode.
    return LaunchExecutable(
        executablePath.c_str(),
        commandLineArguments,
        false /* waitForCompletion */,
#if defined(__WINDOWS) || defined(__unix)
        true /* asAdmin */
#elif __OSX
        false /* asAdmin */
#else
        false /* asAdmin */
#error NOT IMPLEMENTED
#endif
        );
}

string SaveVersionsXmlFile(const string &versionsXmlContent)
{
    string versionsXmlFilePath = GetTempDirectoryPath() + "mli-versions.xml";

    ofstream versionsXmlFileStream(versionsXmlFilePath.c_str());

    if (versionsXmlFileStream)
    {
        versionsXmlFileStream.write(versionsXmlContent.c_str(), versionsXmlContent.length());
        versionsXmlFileStream.close();

        return versionsXmlFilePath;
    }
    else
    {
        return "";
    }
}
#endif
