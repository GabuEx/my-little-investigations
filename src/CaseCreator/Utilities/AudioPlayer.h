#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QString>
#include <QMap>

class AudioPlayer
{
public:
    static void Initialize();
    static void Cleanup();

    static void PreloadSong(const QString &id, const QString &filePath);
    static void PreloadSound(const QString &id, const QString &filePath);
    static void PreloadDialog(const QString &id, const QString &filePath);

    static void PlaySong(const QString &id);
    static void PlaySound(const QString &id);
    static void PlayDialog(const QString &id);

    static const QString & GetCurrentPlayingSong() { return currentPlayingSong; }
    static const QString & GetCurrentPlayingDialog() { return currentPlayingDialog; }

    static void StopSong();
    static void StopDialog();

    static void UnloadSongs();
    static void UnloadSounds();
    static void UnloadDialog();

private:
    AudioPlayer() {}

    static void HandleChannelDone(int channel);
    static void FlipMusicToPartB();

    static bool audioEnabled;
    static QString currentPlayingSong;
    static QString currentPlayingDialog;
};

#endif // AUDIOPLAYER_H*/
