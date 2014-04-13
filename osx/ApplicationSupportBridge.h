#ifndef APPLICATIONSUPPORTBRIDGE_H
#define APPLICATIONSUPPORTBRIDGE_H

#include <string>
#include <vector>

extern const char *pLocalApplicationSupportPath;
extern const char *pCasesPath;
extern const char *pUserApplicationSupportPath;
extern const char *pDialogSeenListsPath;
extern const char *pSavesPath;

std::vector<std::string> GetCaseFilePathsOSX();
std::vector<std::string> (*pfnGetSaveFilePathsForCaseOSX)(std::string pCaseUuid);
std::string GetVersionStringOSX(std::string pPropertyListFilePath);
char * GetPropertyListXMLForVersionStringOSX(std::string pPropertyListFilePath, std::string pVersionString, unsigned long *pVersionStringLength);

#endif
