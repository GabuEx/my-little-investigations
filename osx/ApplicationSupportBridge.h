#ifndef APPLICATIONSUPPORTBRIDGE_H
#define APPLICATIONSUPPORTBRIDGE_H

extern const char *pLocalApplicationSupportPath;
extern const char *pCasesPath;
extern const char *pUserApplicationSupportPath;
extern const char *pDialogSeenListsPath;
extern const char *pSavesPath;

extern const char ** (*pfnGetCaseFilePathsOSX)(unsigned int *pCaseFileCount);
extern const char ** (*pfnGetSaveFilePathsForCaseOSX)(const char *pCaseUuid, unsigned int *pSaveFileCount);
extern const char * (*pfnGetVersionStringOSX)(const char *pPropertyListFilePath);
extern char * (*pfnGetPropertyListXMLForVersionStringOSX)(const char *pPropertyListFilePath, const char *pVersionString, unsigned long *pVersionStringLength);

#endif
