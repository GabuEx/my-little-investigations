/**
 * My Little Investigations main method and helpers.
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
#include "globals.h"
#include "Utils.h"

#ifndef LAUNCHER
#include "Game.h"
#include "MouseHelper.h"
#include "KeyboardHelper.h"
#include "CaseInformation/Case.h"
#include "CaseInformation/CommonCaseResources.h"
#endif

#ifdef GAME_EXECUTABLE
#include "ResourceLoader.h"
#include "TextInputHelper.h"
#include <cryptopp/sha.h>
#endif

#ifdef LAUNCHER
#include "XmlReader.h"
#endif

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <math.h>

#ifdef __OSX
#include "../osx/ApplicationSupportBridge.h"
#endif

#ifdef __unix
#include <unistd.h>
#endif

using namespace std;

#ifdef GAME_EXECUTABLE
string title = "My Little Investigations";
#elif defined(UPDATER)
string title = "My Little Investigations Updater";
#elif defined(LAUNCHER)
string title = "My Little Investigations Launcher";
#endif

#ifdef GAME_EXECUTABLE
bool ValidateCaseFile(const string &caseFileName, string *pCaseUuid);
#endif

int main(int argc, char * argv[])
{
#ifdef __OSX
    BeginOSX();
#endif

    LoadFilePathsAndCaseUuids(argc > 0 ? string(argv[0]) : "");

#ifdef UPDATER
#ifdef __OSX
    // We'll need to acquire update administrator rights in OS X.
    // If we can't, then we'll just bail and carry on to the game executable.
    if (!TryAcquireUpdateAdministratorRightsOSX())
    {
        LaunchGameExecutable();
        return 0;
    }
#endif
#endif

#ifdef UPDATER
    // If we're currently in the updater, we should have received
    // as a command line argument the path to the versions XML file.
    // If we haven't, however, then we'll get it later.
    if (argc >= 2)
    {
        gVersionsXmlFilePath = string(argv[1]);
    }
#ifdef __unix
    // Grab the real user id passed by the launcher so we can exit root privileges after we're done.
    int uid = getuid();
    if(argc >= 3)
    {
        uid = atoi(argv[2]);
    }
#endif
#endif

#ifdef LAUNCHER
    bool retrievedVersion = false;
#endif

    try
    {
        // If the version XML file exists, it's got the most up-to-date version in it,
        // so we'll use that version in that case.
        gVersion = GetCurrentVersion();
#ifdef LAUNCHER
        retrievedVersion = true;
#endif
    }
    catch (MLIException e)
    {
        // We'll just use the built-in version if we couldn't retrieve it from the XML file.
    }

#ifdef LAUNCHER
    bool updatesExist = false;
    string versionsXmlFilePath = "";

    if (retrievedVersion)
    {
        curl_global_init(CURL_GLOBAL_ALL);
        gpCurlHandle = curl_easy_init();

        string versionsXmlContent;

        // First, we'll get whether or not there are any changes - if not, then no need to bother the user
        // by prompting him or her to go into administrative mode for the launcher.
        updatesExist = CheckIfUpdatesExist(&versionsXmlContent);

        if (updatesExist)
        {
            versionsXmlFilePath = SaveVersionsXmlFile(versionsXmlContent);
        }
    }
    else
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error finding version", "Couldn't detect the version of the MLI executable! This indicates that your install is probably corrupt.\n\nYou can still play the game, but you will not receive any update notifications.\n\nYou should reinstall My Little Investigations when you get the chance.", NULL);
    }

    // Try to launch the updater - if we fail to do so, then we'll just launch the game instead.
    if (!updatesExist || versionsXmlFilePath.length() == 0 || !LaunchUpdater(versionsXmlFilePath))
    {
        LaunchGameExecutable();
    }

    if (retrievedVersion)
    {
        curl_easy_cleanup(gpCurlHandle);
        gpCurlHandle = NULL;

        curl_global_cleanup();
    }

    return 0;
#else

#if defined(GAME_EXECUTABLE) || defined(UPDATER) || defined(LAUNCHER)
    gTitle = title + string(" v") + (string)gVersion;
#endif

#ifdef GAME_EXECUTABLE

    // Enable dropping a file onto the game's icon.
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    // Initialize the resource loader.  If this fails, the common resource data file is missing,
    // which is a very bad thing.  Quit if this happens to be the case.
    if (!ResourceLoader::GetInstance()->Init(GetCommonResourcesFilePath()))
    {
        char msg[256];
        sprintf(msg, "Couldn't find common resources data file at %s!", GetCommonResourcesFilePath().c_str());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't start", msg, NULL);
        return 1;
    }

    if (argc > 1)
    {
        string caseFileName = string(argv[1]);
        string caseUuid;

        if (ValidateCaseFile(caseFileName, &caseUuid))
        {
            if (CopyCaseFileToCaseFolder(caseFileName, caseUuid))
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Successfully installed case file!", NULL);
            }
            else
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failure", "Couldn't install case file - unable to copy it to the case directory.", NULL);
            }
        }
        else
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failure", "Couldn't install case file - it appears to be corrupt.", NULL);
        }

        return 0;
    }

    // If we already have an instance of the executable open, then
    // we should prevent another instance from being opened.
    if (CheckForExistingInstance())
    {
        return 0;
    }

    {
        XmlReader localizableContentReader("XML/LocalizableContent.xml");
        pgLocalizableContent = new LocalizableContent(&localizableContentReader);
    }
#endif

    // Create the game and initialize all of its components, or just quit if we can't.
    if (!Game::CreateAndInit())
    {
        char msg[256];
        sprintf(msg, "Error initializing game: %s", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't start", msg, NULL);
        return 1;
    }

    double now = -1.0f; // Used to temporarily store the current time, for timing-related calculations.
    double lastSecond = 0; // Updated once per second, used to keep track of how long a second actually is. (If now>=(lastSecond+1000), new second.) Used for FPS.
    Uint32 frame = 0; // Keeps track of the number of frames rendered during the current second. Used for FPS calculation later.
    double remainder = 0.0f; // Keeps track of amount of milliseconds lost to whole number rounding, to ensure a smooth and perfectly-accurate framerate.
    // Define event handler. Used later to poll the event queue.
    SDL_Event event;

#ifdef GAME_EXECUTABLE
    // We'll keep track of the state of the mouse; if any of these change,
    // then we need to inform the mouse helper of that fact.
    bool isLeftMouseButtonDown = false;
    int mouseX = -1;
    int mouseY = -1;
    bool drawCursor = false;
    bool isReloadingSprites = false;
#endif

    while (!gIsQuitting)
    {
        int delta = 0;
        double oldNow = now;

        now = (double)SDL_GetTicks();

        // First we calculate how much time has elapsed since the last time through this loop -
        // this is the value we'll pass into the update pass.
        if (oldNow >= 0)
        {
            delta = (int)(now - oldNow);
        }

    #ifdef GAME_EXECUTABLE
        // If we want to toggle fullscreen, we'll want to do that now -
        // this will cause an SDL_WINDOWEVENT_SIZE_CHANGED event to be raised
        // that will let us know that we should recreate textures.
        if (gToggleFullscreen)
        {
            gIsFullscreen = !gIsFullscreen;
            SDL_SetWindowFullscreen(gpWindow, gIsFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

            if (!gIsFullscreen)
            {
                SDL_SetWindowSize(gpWindow, gScreenWidth, gScreenHeight);
            }

            gToggleFullscreen = false;
        }
    #endif

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            #ifdef GAME_EXECUTABLE
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_ENTER)
                    {
                        drawCursor = true;
                        isLeftMouseButtonDown = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK);
                    }
                    else if (event.window.event == SDL_WINDOWEVENT_LEAVE)
                    {
                        drawCursor = false;
                        isLeftMouseButtonDown = false;
                    }

                    MouseHelper::UpdateState(isLeftMouseButtonDown, mouseX, mouseY, drawCursor);
                    break;

                case SDL_MOUSEMOTION:
                    // The mouse has moved; we need to notify the mouse helper.
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;

                    MouseHelper::UpdateState(isLeftMouseButtonDown, mouseX, mouseY, drawCursor);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    // A mouse button has changed; if it was the left mouse button,
                    // then we need to notify the mouse helper.
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        isLeftMouseButtonDown = event.button.state == SDL_PRESSED;
                    }

                    MouseHelper::UpdateState(isLeftMouseButtonDown, mouseX, mouseY, drawCursor);
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    if(TextInputHelper::GetInSession())
                    {
                        TextInputHelper::NotifyKeyState(event.key.keysym.sym, event.key.state);
                    }
                    else
                    {
                        if(KeyboardHelper::IsActionKey(KeyboardHelper::Click, event.key.keysym.sym))
                        {
                            KeyboardHelper::SetClickState(event.key.state);
                        }

                        if(KeyboardHelper::IsActionKey(KeyboardHelper::Up, event.key.keysym.sym))
                        {
                            KeyboardHelper::SetUpState(event.key.state);
                        }

                        if(KeyboardHelper::IsActionKey(KeyboardHelper::Left, event.key.keysym.sym))
                        {
                            KeyboardHelper::SetLeftState(event.key.state);
                        }

                        if(KeyboardHelper::IsActionKey(KeyboardHelper::Down, event.key.keysym.sym))
                        {
                            KeyboardHelper::SetDownState(event.key.state);
                        }

                        if(KeyboardHelper::IsActionKey(KeyboardHelper::Right, event.key.keysym.sym))
                        {
                            KeyboardHelper::SetRightState(event.key.state);
                        }

                        if(KeyboardHelper::IsActionKey(KeyboardHelper::Run, event.key.keysym.sym))
                        {
                            KeyboardHelper::SetRunState(event.key.state);
                        }
                    }
                    break;

                case SDL_TEXTINPUT:
                    TextInputHelper::NotifyTextInput(event.text.text);
                    break;

                case SDL_DROPFILE:
                {
                    string caseFileName = string(event.drop.file);
                    SDL_free(event.drop.file);
                    string caseUuid;
                    
                    if (ValidateCaseFile(caseFileName, &caseUuid))
                    {
                        if (CopyCaseFileToCaseFolder(caseFileName, caseUuid))
                        {
                            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Successfully installed case file!", gpWindow);
                        }
                        else
                        {
                            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failure", "Couldn't install case file - unable to copy it to the case directory.", gpWindow);
                        }
                    }
                    else
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failure", "Couldn't install case file - it appears to be corrupt.", gpWindow);
                    }
                }
                    break;
            #endif

                case SDL_QUIT:
                    // Someone tried to close the window, or otherwise requested the task to end.
                    // Flip the exit switch, which will be obeyed when this frame finishes rendering.
                    gIsQuitting = true;
                    break;
            }
        }

    #ifdef GAME_EXECUTABLE
        // If we have any textures that we need to load or delete, let's do so now.
        if (ResourceLoader::GetInstance()->HasImageTexturesToLoad())
        {
            ResourceLoader::GetInstance()->TryLoadOneImageTexture();
        }
        else if (ResourceLoader::GetInstance()->HasLoadStep())
        {
            ResourceLoader::GetInstance()->TryRunOneLoadStep();
        }

        // Check if we're still in the process of reloading sprites after a change either to or from fullscreen.
        // If we are, then we'll just keep going until we're done.
        if (isReloadingSprites)
        {
            if (!ResourceLoader::GetInstance()->HasImageTexturesToLoad())
            {
                isReloadingSprites = false;
            }
            else
            {
                continue;
            }
        }

        // Update anything having to do with common audio.
        CommonCaseResources::GetInstance()->GetAudioManager()->Update(delta);

        // Update the state of the game based on how much time has elapsed since this loop was last executed.
        MouseHelper::SetCursorType(CursorTypeNormal);
        MouseHelper::SetMouseOverText("");

        MouseHelper::UpdateTiming();
        TextInputHelper::Update(delta);
    #endif

        Game::GetInstance()->Update(delta);

    #ifdef GAME_EXECUTABLE
        // Call AppleCursorUpdate and UpdateCursor after updating the game, as otherwise this won't properly take into account
        // changes to the mouse-over text.
        MouseHelper::ApplyCursorUpdate();
        MouseHelper::UpdateCursor(delta);
    #endif

        // If the game is over now, then we should quit.
        if (Game::GetInstance()->GetIsFinished())
        {
            gIsQuitting = true;
        }

    #ifdef GAME_EXECUTABLE
        // We should handle any clicks at this point in case we have any.
        // Anyone who cares will have responded to it by now.
        MouseHelper::HandleClick();
        MouseHelper::HandleDoubleClick();
        KeyboardHelper::UpdateKeyState();
    #endif

        // Blank the screen before drawing the new frame.
        SDL_SetRenderDrawColor(gpRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gpRenderer);

        // Draw the current state of the game to the screen.
        Game::GetInstance()->Draw();

    #ifdef GAME_EXECUTABLE
        if (Game::GetInstance()->GetShowCursor())
        {
            MouseHelper::DrawCursor();
        }
    #endif

        // Handle FPS calculation every second. (Only in Debug build target, with MLI_DEBUG_NO_FPS not defined.)
        if (now - 1000 >= lastSecond)
        {
            if (lastSecond > 0)
            {
                #ifdef MLI_DEBUG
                    #ifndef MLI_DEBUG_NO_FPS
                        cout << "FPS: " << frame << endl;
                    #endif
                #endif

                frame = 0;
            }

            lastSecond = now;
        }

        // Swap the double buffer to display the new frame.
        SDL_RenderPresent(gpRenderer);

        // Increment the frame counter (for FPS).
        frame++;

        // Calculate how long it took to execute all of the above, to compensate for the additional delay for framerate regulation.
        double executionTime = (double)SDL_GetTicks() - now;

        // If we should wait to enforce the desired framerate, do so now.
        if (gFramerate > 0.0 && executionTime < 1000.0 / gFramerate)
        {
            double wait = 1000.0 / gFramerate - executionTime;
            remainder += fmod(wait, 1.0);

            if (remainder >= 1.0)
            {
                wait += remainder - fmod(remainder, 1.0);
                remainder = fmod(remainder, 1.0);
            }

            SDL_Delay((int)wait);
        }
        else
        {
            // Always wait at least 1 ms - this defers to other threads as needed.
            SDL_Delay(1);
        }
    }

#ifdef GAME_EXECUTABLE
    // Delete this before Game::Finish() is called, since otherwise we'll AV
    // if we try to delete this object's semaphore after SDL is cleaned up.
    delete pgLocalizableContent;
    pgLocalizableContent = NULL;

    // The game's done now, so finish it up.
    CommonCaseResources::Close();
#endif

    Game::Finish();

#ifdef GAME_EXECUTABLE
    ResourceLoader::Close();
#endif

#ifdef UPDATER
    // If we're currently in the updater and everything has gone smoothly,
    // then we want to launch the game executable now.
#ifdef __unix
    // If we're running under Unix, we need to drop our root privileges now.
    setuid(uid);
#endif
    LaunchGameExecutable();
#endif
#endif

#ifdef UPDATER
#ifdef __OSX
    // We should destroy our administrator rights now, since we're done updating.
    FreeAdministratorRights();
#endif
#endif

    // Exit normally.
    return 0;
}

#ifdef GAME_EXECUTABLE
bool ValidateCaseFile(const string &caseFileName, string *pCaseUuid)
{
    if (!ResourceLoader::GetInstance()->LoadCase(caseFileName))
    {
        return false;
    }

    bool valid = false;

    try
    {
        Case::ValidateXml();
        valid = true;
    }
    catch (MLIException e)
    {
    }

    if (!valid)
    {
        return false;
    }

    byte hash[CryptoPP::SHA256::DIGESTSIZE];
    ResourceLoader::GetInstance()->HashFile("case.xml", hash);
    *pCaseUuid = UuidFromSHA256Hash(hash);
    return true;
}
#endif
