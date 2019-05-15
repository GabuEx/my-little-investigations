#include "AudioPlayer.h"

#include <CaseCreator/CaseContent/CaseContent.h>

#include <SDL2/SDL.h>

#ifdef __OSX
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif

#define DIALOG_CHANNEL 0
#define AMBIENCE_CHANNEL 1
#define PARTNER_ABILITY_LOOP_CHANNEL 2
#define SOUND_LOOP_CHANNEL_START 3
#define NUM_SOUND_LOOP_CHANNELS 4
#define NUM_RESERVED_CHANNELS (SOUND_LOOP_CHANNEL_START + NUM_SOUND_LOOP_CHANNELS)

QMap<QString, Mix_Music *> idToSongMap;
QMap<QString, Mix_Chunk *> idToSoundMap;
QMap<QString, Mix_Chunk *> idToDialogMap;

bool AudioPlayer::audioEnabled = false;
QString AudioPlayer::currentPlayingSong;
QString AudioPlayer::currentPlayingDialog;

void AudioPlayer::Initialize()
{
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096) != 0)
    {
        audioEnabled = false;
    }
    else
    {
        // We'll allocate the number of reserved channels needed plus 8 more - that should be more than enough.
        Mix_AllocateChannels(NUM_RESERVED_CHANNELS + 8);

        // If the audio was successfully initialized, reserve channel 0 (for dialog), 1 (for ambiance), and 2-5 (for looping sounds),
        // and set up the callback to handle a channel being done playing.
        Mix_ReserveChannels(SOUND_LOOP_CHANNEL_START + NUM_SOUND_LOOP_CHANNELS);
        Mix_ChannelFinished(AudioPlayer::HandleChannelDone);
        audioEnabled = true;
    }
}

void AudioPlayer::Cleanup()
{
    if (audioEnabled)
    {
        if (currentPlayingSong.length() > 0)
        {
            Mix_HookMusicFinished(NULL);
            Mix_HaltMusic();
        }

        Mix_HaltChannel(-1);

        UnloadSongs();
        UnloadSounds();
        UnloadDialog();

        Mix_CloseAudio();
    }
}

void AudioPlayer::PreloadSong(const QString &id, const QString &filePath)
{
    if (!audioEnabled)
    {
        return;
    }

    QString fullFilePathA = CaseContent::GetInstance()->RelativePathToAbsolutePath(filePath + "A.ogg");
    QString fullFilePathB = CaseContent::GetInstance()->RelativePathToAbsolutePath(filePath + "B.ogg");

    Mix_Music *pMusicA = Mix_LoadMUS(fullFilePathA.toStdString().c_str());

    if (pMusicA == NULL)
    {
        return;
    }

    Mix_Music *pMusicB = Mix_LoadMUS(fullFilePathB.toStdString().c_str());

    if (pMusicB == NULL)
    {
        Mix_FreeMusic(pMusicA);
        return;
    }

    idToSongMap[id + "_A"] = pMusicA;
    idToSongMap[id + "_B"] = pMusicB;
}

void AudioPlayer::PreloadSound(const QString &id, const QString &filePath)
{
    if (!audioEnabled)
    {
        return;
    }

    QString fullFilePath = CaseContent::GetInstance()->RelativePathToAbsolutePath(filePath + ".ogg");
    Mix_Chunk *pSound = Mix_LoadWAV(fullFilePath.toStdString().c_str());

    if (pSound == NULL)
    {
        return;
    }

    idToSoundMap[id] = pSound;
}

void AudioPlayer::PreloadDialog(const QString &id, const QString &filePath)
{
    if (!audioEnabled)
    {
        return;
    }

    Mix_Chunk *pOldDialog = idToDialogMap[id];

    if (pOldDialog != NULL)
    {
        Mix_FreeChunk(pOldDialog);
        idToDialogMap.remove(id);
    }

    QString fullFilePath = CaseContent::GetInstance()->RelativePathToAbsolutePath(filePath + ".ogg");
    Mix_Chunk *pDialog = Mix_LoadWAV(fullFilePath.toStdString().c_str());

    if (pDialog == NULL)
    {
        return;
    }

    idToDialogMap[id] = pDialog;
}

void AudioPlayer::PlaySong(const QString &id)
{
    if (!audioEnabled)
    {
        return;
    }

    Mix_Music *pMusicA = idToSongMap[id + "_A"];

    if (!pMusicA)
    {
        return;
    }

    Mix_Music *pMusicB = idToSongMap[id + "_B"];

    if (!pMusicB)
    {
        return;
    }

    if (currentPlayingSong.length() > 0)
    {
        Mix_HookMusicFinished(NULL);
        Mix_HaltMusic();
    }

    currentPlayingSong = id;

    Mix_HookMusicFinished(AudioPlayer::FlipMusicToPartB);

    if (Mix_PlayMusic(pMusicA, 0) == -1)
    {
        currentPlayingSong = "";
    }
}

void AudioPlayer::PlaySound(const QString &id)
{
    if (!audioEnabled)
    {
        return;
    }

    Mix_Chunk *pSound = idToSoundMap[id];

    if (!pSound)
    {
        return;
    }

    Mix_PlayChannel(-1, pSound, 0);
}

void AudioPlayer::PlayDialog(const QString &id)
{
    if (!audioEnabled)
    {
        return;
    }

    if (currentPlayingDialog.length() > 0)
    {
        Mix_HaltChannel(DIALOG_CHANNEL);
    }

    Mix_Chunk *pDialog = idToDialogMap[id];

    if (!pDialog)
    {
        return;
    }

    currentPlayingDialog = id;

    if (Mix_PlayChannel(DIALOG_CHANNEL, pDialog, 0) != DIALOG_CHANNEL)
    {
        currentPlayingDialog = "";
    }
}

void AudioPlayer::StopSong()
{
    if (!audioEnabled || currentPlayingSong.length() == 0)
    {
        return;
    }

    Mix_HookMusicFinished(NULL);
    Mix_HaltMusic();

    currentPlayingSong = "";
}

void AudioPlayer::StopDialog()
{
    if (!audioEnabled || currentPlayingDialog.length() == 0)
    {
        return;
    }

    Mix_HaltChannel(DIALOG_CHANNEL);
}

void AudioPlayer::UnloadSongs()
{
    StopSong();

    for(QMap<QString, Mix_Music *>::const_iterator iter = idToSongMap.begin(); iter != idToSongMap.end(); ++iter)
    {
        Mix_FreeMusic(iter.value());
    }

    idToSongMap.clear();
}

void AudioPlayer::UnloadSounds()
{
    for(QMap<QString, Mix_Chunk *>::const_iterator iter = idToSoundMap.begin(); iter != idToSoundMap.end(); ++iter)
    {
        Mix_FreeChunk(iter.value());
    }

    idToSoundMap.clear();
}

void AudioPlayer::UnloadDialog()
{
    StopDialog();

    for(QMap<QString, Mix_Chunk *>::const_iterator iter = idToDialogMap.begin(); iter != idToDialogMap.end(); ++iter)
    {
        Mix_FreeChunk(iter.value());
    }

    idToDialogMap.clear();
}

void AudioPlayer::HandleChannelDone(int channel)
{
    // Called whenever a channel is stopped.
    // We only really care if it was the dialog channel.
    // If it was, there isn't any dialog playing anymore.
    if (channel == DIALOG_CHANNEL)
    {
        currentPlayingDialog = "";
    }
}

void AudioPlayer::FlipMusicToPartB()
{
    Mix_HookMusicFinished(NULL);
    Mix_PlayMusic(idToSongMap[currentPlayingSong + "_B"], -1);
}
