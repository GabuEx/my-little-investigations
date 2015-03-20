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

#define kCaseExt @"mlicase"

// Only use the autorelease directive on OS X 10.7 or later SDKs
// as they should a recent enough version of Clang to use it.
#if __clang__ && defined(MAC_OS_X_VERSION_10_7)
#define AUTORELEASE_POOL @autoreleasepool
#define AUTORELEASE_POOL_START @autoreleasepool {
#else
class Autorelease_Pool_Wrapper {
public:
    Autorelease_Pool_Wrapper() { m_pool = [[NSAutoreleasePool alloc] init]; }
    ~Autorelease_Pool_Wrapper() { [m_pool drain]; }
    operator bool() const { return true; }
private:
    NSAutoreleasePool *m_pool;
};

#define POOL_NAME(x, y) x##_##y
#define POOL_NAME2(x, y) POOL_NAME(x, y)
#define AUTORELEASE_POOL if(Autorelease_Pool_Wrapper POOL_NAME2(pool, __LINE__) = Autorelease_Pool_Wrapper())
#define AUTORELEASE_POOL_START {\
Autorelease_Pool_Wrapper POOL_NAME2(pool, __LINE__) = Autorelease_Pool_Wrapper();
#endif // !__clang__

#define AUTORELEASE_POOL_STOP }

/* Main entry point to executable - should *not* be SDL_main! */
void BeginOSX()
{
    AUTORELEASE_POOL_START
    NSFileManager *defaultManager = [NSFileManager defaultManager];

    /* Create our Application Support folders if they don't exist yet and store the paths */
    NSString *localAppSupport = [defaultManager localApplicationSupportDirectory];
    NSString *userAppSupport = [defaultManager userApplicationSupportDirectory];

    /* Next, create the folders that the executable will need during execution if they don't already exist. */
    NSString *localGameAppSupportPath = localAppSupport;
    NSString *userGameAppSupportPath = [userAppSupport stringByAppendingPathComponent:@"My Little Investigations"];
    NSString *dialogSeenPath = [userGameAppSupportPath stringByAppendingPathComponent:@"DialogSeenLists"];

    NSCasesPath = [[localGameAppSupportPath stringByAppendingPathComponent:@"Cases"] retain];
    NSUserCasesPath = [[userGameAppSupportPath stringByAppendingPathComponent:@"Cases"] retain];
    NSSavesPath = [[userGameAppSupportPath stringByAppendingPathComponent:@"Saves"] retain];

	NSError *error = nil;

	[defaultManager
		createDirectoryAtPath:dialogSeenPath
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

    pLocalApplicationSupportPath = [localGameAppSupportPath fileSystemRepresentation];
    pCasesPath = [NSCasesPath fileSystemRepresentation];
    pUserApplicationSupportPath = [userGameAppSupportPath fileSystemRepresentation];
    pDialogSeenListsPath = [dialogSeenPath fileSystemRepresentation];
    pSavesPath = [NSSavesPath fileSystemRepresentation];

    AUTORELEASE_POOL_STOP
}

vector<string> GetCaseFilePathsOSX()
{
    AUTORELEASE_POOL_START
    NSError *error = nil;
    NSFileManager *defaultManager = [NSFileManager defaultManager];

    NSArray *caseFiles = [defaultManager
                          contentsOfDirectoryAtPath: NSCasesPath
                          error:&error];

    NSMutableArray *uniqueCaseList;
    vector<string> caseFileList;
    NSMutableArray *localCaseList = [[NSMutableArray alloc] initWithCapacity:caseFiles.count];

    for (NSString *object in caseFiles) {
        //Ignore UNIX hidden files, like OS X's .DS_Store
        if ([object hasPrefix:@"."]) {
            continue;
        }

        NSString *fullCasePath = [NSCasesPath stringByAppendingPathComponent:object];
        [localCaseList addObject:fullCasePath];
    }

    // Cases in the user's folder
    caseFiles = [defaultManager
                 contentsOfDirectoryAtPath: NSUserCasesPath
                 error:&error];
    
    NSMutableArray *userCaseList = [[NSMutableArray alloc] initWithCapacity:caseFiles.count];
    
    for (NSString *object in caseFiles) {
        //Ignore UNIX hidden files, like OS X's .DS_Store
        if ([object hasPrefix:@"."]) {
            continue;
        }
        NSString *fullCasePath = [NSUserCasesPath stringByAppendingPathComponent:object];
        [userCaseList addObject:fullCasePath];
    }

    uniqueCaseList = [[NSMutableArray alloc] initWithCapacity:localCaseList.count + userCaseList.count];

    AUTORELEASE_POOL_START
    NSIndexSet *uniqueLocalCases = [localCaseList indexesOfObjectsPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
        NSString *lastPathComponent = [obj lastPathComponent];
        
        return ![caseFiles containsObject:lastPathComponent];
    }];

    //prefer a user's case over the local cases
    [uniqueCaseList addObjectsFromArray:[localCaseList objectsAtIndexes:uniqueLocalCases]];
    [uniqueCaseList addObjectsFromArray:userCaseList];
    AUTORELEASE_POOL_STOP

    [localCaseList release]; localCaseList = nil;
    [userCaseList release]; userCaseList = nil;

    for (NSString *path in uniqueCaseList) {
        caseFileList.push_back(string([path fileSystemRepresentation]));
    }
    [uniqueCaseList release]; uniqueCaseList = nil;
    
    return caseFileList;
    
    AUTORELEASE_POOL_STOP
}

vector<string> GetSaveFilePathsForCaseOSX(const string &pCaseUuid)
{
    vector<string> ppSaveFilePathList;

    AUTORELEASE_POOL_START
	NSError *error = nil;
	NSFileManager *defaultManager = [NSFileManager defaultManager];

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

    AUTORELEASE_POOL_STOP
    return ppSaveFilePathList;
}

string GetVersionStringOSX()
{
    AUTORELEASE_POOL_START
    NSBundle *ourBundle = [NSBundle mainBundle];
    NSDictionary *infoPlist = ourBundle.infoDictionary;
    NSString *versStr = [infoPlist objectForKey:@"CFBundleVersion"];
    return versStr != nil ? versStr.UTF8String : "";
    AUTORELEASE_POOL_STOP
}

char *GetPropertyListXMLForVersionStringOSX(const string &pPropertyListFilePath, const string &pVersionString, unsigned long *pVersionStringLength)
{
    AUTORELEASE_POOL_START
    *pVersionStringLength = 0;

    NSFileManager *defaultManager = [NSFileManager defaultManager];
    NSError *pErrorDesc = nil;
    //TODO: Save the NSString as, say, a static pointer.
    NSString *propertyListPath = [defaultManager
								  stringWithFileSystemRepresentation:pPropertyListFilePath.c_str()
								  length: pPropertyListFilePath.size()];
	propertyListPath = [propertyListPath stringByStandardizingPath];

    if (![defaultManager fileExistsAtPath:propertyListPath])
    {
        return NULL;
    }

    NSMutableDictionary *plistDict =
        [[NSMutableDictionary alloc] initWithContentsOfFile:propertyListPath];

    if (plistDict == NULL)
    {
        return NULL;
    }

    NSString *newStringValue = [NSString stringWithUTF8String:pVersionString.c_str()];
    [plistDict setObject:newStringValue forKey:@"CFBundleShortVersionString"];
    [plistDict setObject:newStringValue forKey:@"CFBundleVersion"];

    NSData *pData = [NSPropertyListSerialization
        dataWithPropertyList:plistDict
        format:NSPropertyListXMLFormat_v1_0
        options:0
        error:&pErrorDesc];
    [plistDict release];

    if (pData == NULL)
    {
        return NULL;
    }

    NSUInteger dataLength = [pData length];

    char *pCharData = (char *)malloc(dataLength * sizeof(char));
    [pData getBytes:(void *)pCharData length:dataLength];

    *pVersionStringLength = dataLength;
    return pCharData;
    AUTORELEASE_POOL_STOP
}

bool CopyCaseUserOSX(const string &cppCasePath, const string &cppCaseUuid)
{
    bool success = false;
    AUTORELEASE_POOL_START
    NSFileManager *fm = [NSFileManager defaultManager];
    NSString *caseFilePath = [fm stringWithFileSystemRepresentation:cppCasePath.c_str() length:cppCasePath.length()];
    NSString *caseUUID = [NSString stringWithUTF8String:cppCaseUuid.c_str()];
    NSString *userCasePath = [NSUserCasesPath stringByAppendingPathComponent:caseUUID];
    userCasePath = [userCasePath stringByAppendingPathExtension:kCaseExt];

    if ([fm fileExistsAtPath:userCasePath]) {
        // something's already there...
        success = false;
    } else {
        success = [fm copyItemAtPath:caseFilePath toPath:userCasePath error:NULL];
    }

    AUTORELEASE_POOL_STOP
    return success;
}
