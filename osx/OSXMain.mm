/*   SDLMain.m - main entry point for our Cocoa-ized SDL app
       Initial Version: Darrell Walisser <dwaliss1@purdue.edu>
       Non-NIB-Code & other changes: Max Horn <max@quendi.de>

    Feel free to customize this file to suit your needs
*/

#include "SDL2/SDL.h"
#import "NSFileManagerDirectoryLocations.h"
#include "ApplicationSupportBridge.h"
#include <sys/param.h> /* for MAXPATHLEN */
#include <unistd.h>

string pLocalApplicationSupportPath;
string pCasesPath;
string pUserApplicationSupportPath;
string pDialogSeenListsPath;
string pSavesPath;

static NSString *NSCasesPath;
static NSString *NSUserCasesPath;
static NSString *NSSavesPath;

/* Main entry point to executable - should *not* be SDL_main! */
void BeginOSX()
{
    NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc] init];
    NSFileManager *defaultManager = [NSFileManager defaultManager];

    /* Create our Application Support folders if they don't exist yet and store the paths */
    NSString *pStrLocalApplicationSupportPath = [defaultManager localApplicationSupportDirectory];
    NSString *pStrUserApplicationSupportPath = [defaultManager userApplicationSupportDirectory];

    /* Next, create the folders that the executable will need during execution if they don't already exist. */
	NSString *pStrLocalGameApplicationSupportPath = pStrLocalApplicationSupportPath;
	NSString *pStrUserGameApplicationSupportPath = [pStrUserApplicationSupportPath stringByAppendingPathComponent:@"My Little Investigations"];
	NSString *pStrDialogSeenListsPath = [pStrUserGameApplicationSupportPath stringByAppendingPathComponent:@"DialogSeenLists"];

	NSCasesPath = [[pStrLocalGameApplicationSupportPath stringByAppendingPathComponent:@"Cases"] retain];
	NSUserCasesPath = [[pStrUserGameApplicationSupportPath stringByAppendingPathComponent:@"Cases"] retain];
	NSSavesPath = [[pStrUserGameApplicationSupportPath stringByAppendingPathComponent:@"Saves"] retain];


	NSError *error = nil;

	[defaultManager
		createDirectoryAtPath:pStrDialogSeenListsPath
		withIntermediateDirectories:YES
		attributes:nil
		error:&error];

	[defaultManager
		createDirectoryAtPath:NSSavesPath
		withIntermediateDirectories:YES
		attributes:nil
		error:&error];

	[defaultManager
	 createDirectoryAtPath:NSUserCasesPath
	 withIntermediateDirectories:YES
	 attributes:nil
	 error:&error];

    pLocalApplicationSupportPath = [pStrLocalGameApplicationSupportPath fileSystemRepresentation];
    pCasesPath = [NSCasesPath fileSystemRepresentation];
    pUserApplicationSupportPath = [pStrUserGameApplicationSupportPath fileSystemRepresentation];
    pDialogSeenListsPath = [pStrDialogSeenListsPath fileSystemRepresentation];
    pSavesPath = [NSSavesPath fileSystemRepresentation];

    [pPool drain];
}

vector<string> GetCaseFilePathsOSX()
{
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    NSError *error = nil;
    NSFileManager *defaultManager = [NSFileManager defaultManager];

    NSArray *pCaseFileList =
        [defaultManager
            contentsOfDirectoryAtPath: NSCasesPath
            error:&error];

    vector<string> ppCaseFileList;

    for (NSString *pStrCaseFileName in pCaseFileList)
    {
       //Ignore UNIX hidden files, like OS X's .DS_Store
        if ([pStrCaseFileName hasPrefix:@"."])
        {
            continue;
        }

        NSString *pStrCaseFilePath = [NSCasesPath stringByAppendingPathComponent:pStrCaseFileName];
		ppCaseFileList.push_back(string([pStrCaseFilePath fileSystemRepresentation]));
    }

	// Cases in the user's folder
	pCaseFileList = [defaultManager
					 contentsOfDirectoryAtPath: NSUserCasesPath
					 error:&error];

	for (NSString *object in pCaseFileList)
	{
		//Ignore UNIX hidden files, like OS X's .DS_Store
		if ([object hasPrefix:@"."])
		{
			continue;
		}
		NSString *fullCasePath = [NSUserCasesPath stringByAppendingPathComponent:object];
		ppCaseFileList.push_back(string([fullCasePath fileSystemRepresentation]));
	}

    [pool drain];
    return ppCaseFileList;
}

vector<string> GetSaveFilePathsForCaseOSX(string pCaseUuid)
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	NSError *error = nil;
	NSFileManager *defaultManager = [NSFileManager defaultManager];

    //TODO: Save the NSString as, say, a static pointer.
    NSString *currentCaseSavePath = [NSSavesPath stringByAppendingPathComponent:[NSString stringWithUTF8String:pCaseUuid.c_str()]];

    [defaultManager
        createDirectoryAtPath: currentCaseSavePath
        withIntermediateDirectories:YES
        attributes:nil
        error:&error];

    NSArray *pSaveFileList =
        [defaultManager
            contentsOfDirectoryAtPath: currentCaseSavePath
            error:&error];

    vector<string> ppSaveFilePathList;

    for (NSString *pStrSaveFileName in pSaveFileList)
    {
       //Ignore UNIX hidden files, like OS X's .DS_Store
        if ([pStrSaveFileName hasPrefix:@"."])
        {
            continue;
        }

        NSString *pStrSaveFilePath = [currentCaseSavePath stringByAppendingPathComponent:pStrSaveFileName];
		ppSaveFilePathList.push_back(string([pStrSaveFilePath fileSystemRepresentation]));
    }

    [pool drain];
    return ppSaveFilePathList;
}

string GetVersionStringOSX(string PropertyListFilePath)
{
    NSFileManager *defaultManager = [NSFileManager defaultManager];
    NSString *pProperyListPath = [defaultManager stringWithFileSystemRepresentation:PropertyListFilePath.c_str() length: PropertyListFilePath.size()];

    if (![defaultManager fileExistsAtPath:pProperyListPath])
    {
        return string();
    }

    NSDictionary *pPropertyListDictionary =
        [NSDictionary dictionaryWithContentsOfFile:pProperyListPath];

    if (pPropertyListDictionary == NULL)
    {
        return string();
    }

    NSString *pVersionString = [pPropertyListDictionary objectForKey:@"VersionString"];
    return [pVersionString UTF8String];
}

char * GetPropertyListXMLForVersionStringOSX(string pPropertyListFilePath, string pVersionString, unsigned long *pVersionStringLength)
{
    *pVersionStringLength = 0;

    NSFileManager *defaultManager = [NSFileManager defaultManager];
    NSString *pErrorDesc = nil;
    //TODO: Save the NSString as, say, a static pointer.
    NSString *pProperyListPath = [defaultManager stringWithFileSystemRepresentation:pPropertyListFilePath.c_str() length: pPropertyListFilePath.size()];

    if (![defaultManager fileExistsAtPath:pProperyListPath])
    {
        return NULL;
    }

    NSMutableDictionary *pPropertyListDictionaryMutable =
        [NSMutableDictionary dictionaryWithContentsOfFile:pProperyListPath];

    if (pPropertyListDictionaryMutable == NULL)
    {
        return NULL;
    }

    [pPropertyListDictionaryMutable setObject:[NSString stringWithUTF8String:pVersionString.c_str()] forKey:@"VersionString"];

    NSData *pData = [NSPropertyListSerialization
        dataFromPropertyList:pPropertyListDictionaryMutable
        format:NSPropertyListXMLFormat_v1_0
        errorDescription:&pErrorDesc];

    if (pData == NULL)
    {
        return NULL;
    }

    NSUInteger dataLength = [pData length];

    char *pCharData = (char *)malloc(dataLength * sizeof(char));
    [pData getBytes:(void *)pCharData length:dataLength];

    *pVersionStringLength = dataLength;
    return pCharData;
}
