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
vector<string> GetSaveFilePathsForCaseOSX(const string &caseUuid);
string GetVersionStringOSX();
char * GetPropertyListXMLForVersionStringOSX(const string &pPropertyListFilePath, const string &pVersionString, unsigned long *pVersionStringLength);
string GetGameExecutable();
bool CopyCaseUserOSX(const string &caseFilePath, const string &caseUuid);

void BeginOSX();

#endif
