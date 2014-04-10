
#include "ApplicationSupportBridge.h"

const char *pLocalApplicationSupportPath;
const char *pCasesPath;
const char *pUserApplicationSupportPath;
const char *pDialogSeenListsPath;
const char *pSavesPath;

const char ** (*pfnGetCaseFilePathsOSX)(unsigned int *pCaseFileCount);
const char ** (*pfnGetSaveFilePathsForCaseOSX)(const char *pCaseUuid, unsigned int *pSaveFileCount);
const char * (*pfnGetVersionStringOSX)(const char *pPropertyListFilePath);
char * (*pfnGetPropertyListXMLForVersionStringOSX)(const char *pPropertyListFilePath, const char *pVersionString, unsigned long *pVersionStringLength);
