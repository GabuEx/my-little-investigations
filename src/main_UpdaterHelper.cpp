/**
 * (OS X only) Utility executable for performing update operations as root.
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

#include <iostream>
#include <string>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>

using namespace std;

#define PRINT_AND_RETURN() \
    do { \
        int returnValue = success ? 0 : 1; \
        cout << "Return value: " << returnValue << endl; \
        return returnValue; \
    } while (0);

int main(int argc, char *argv[])
{
    bool success = false;

    // If we have no arguments at all, we'll just bail.
    if (argc < 2)
    {
        cerr << "UpdaterHelper: This is a utility executable to be run by MyLittleInvestigationsUpdater. It is not intended to be run in isolation, and will do nothing if it is." << endl;
        PRINT_AND_RETURN();
    }

    string operation = argv[1];

    if (operation == "update")
    {
        // If we're updating, we expect to receive three additional arguments:
        // the old file path, the delta file path, and the new file path.
        if (argc < 5)
        {
            cerr << "UpdaterHelper: Expected 5 arguments for update operation, but only received " << argc << "." << endl;
            PRINT_AND_RETURN();
        }

        string launcherFilePath = argv[0];
        string updaterFilePath = launcherFilePath.substr(0, launcherFilePath.find_last_of("/")) + "/deltatool/xdelta3";

        string oldFilePath = argv[2];
        string deltaFilePath = argv[3];
        string newFilePath = argv[4];

        vector<string> commandLineArguments;

        commandLineArguments.push_back(updaterFilePath);
        commandLineArguments.push_back("-f");
        commandLineArguments.push_back("-d");
        commandLineArguments.push_back("-s");
        commandLineArguments.push_back(oldFilePath);
        commandLineArguments.push_back(deltaFilePath);
        commandLineArguments.push_back(newFilePath);

        char **ppArgv = new char*[commandLineArguments.size() + 1];

        for (unsigned int i = 0; i < commandLineArguments.size(); i++)
        {
            ppArgv[i] = new char[commandLineArguments[i].length() + 1];
            strcpy(ppArgv[i], commandLineArguments[i].c_str());
        }

        ppArgv[commandLineArguments.size()] = NULL;

        pid_t processId = fork();

        if (processId == 0)
        {
            execv(ppArgv[0], ppArgv);
        }
        else if (processId > 0)
        {
            int childStatus;
            waitpid(processId, &childStatus, 0);

            success =
                WIFEXITED(childStatus) &&
                WEXITSTATUS(childStatus) == 0;
        }

        if (success)
        {
            success = chmod(newFilePath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0;
        }
    }
    else if (operation == "remove")
    {
        // If we're removing, we expect to receive one additional argument:
        // the file path to remove.
        if (argc < 3)
        {
            cerr << "UpdaterHelper: Expected 3 arguments for remove operation, but only received " << argc << "." << endl;
            PRINT_AND_RETURN();
        }

        string filePath = argv[2];

        success = remove(filePath.c_str()) == 0;
    }
    else if (operation == "rename")
    {
        // If we're renaming, we expect to receive two additional arguments:
        // the old file path and the new file path.
        if (argc < 4)
        {
            cerr << "UpdaterHelper: Expected 4 arguments for rename operation, but only received " << argc << "." << endl;
            PRINT_AND_RETURN();
        }

        string oldFilePath = argv[2];
        string newFilePath = argv[3];

        success = rename(oldFilePath.c_str(), newFilePath.c_str()) == 0;
    }
    else
    {
        cerr << "UpdaterHelper: Unknown operation '" << operation.c_str() << "'." << endl;
        PRINT_AND_RETURN();
    }

    PRINT_AND_RETURN();
}
