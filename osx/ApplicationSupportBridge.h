#ifndef APPLICATIONSUPPORTBRIDGE_H
#define APPLICATIONSUPPORTBRIDGE_H

#include <string>
#include <vector>

using namespace std;

extern string pLocalApplicationSupportPath;
extern string pCasesPath;
extern string pUserApplicationSupportPath;
extern string pDialogSeenListsPath;
extern string pSavesPath;

vector<string> GetCaseFilePathsOSX();
vector<string> GetSaveFilePathsForCaseOSX(string caseUuid);
string GetVersionStringOSX(string PropertyListFilePath);
char * GetPropertyListXMLForVersionStringOSX(string pPropertyListFilePath, string pVersionString, unsigned long *pVersionStringLength);
string GetGameExecutable();

#endif
