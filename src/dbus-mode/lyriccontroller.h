#ifndef LYRIC_CONTROLLER_H
#define LYRIC_CONTROLLER_H

#include <QObject>
#include <QTimer>
#include "mprismetadata.h"
#include "musicfoxmanager.h"
#include "lyric.h"
#include "qtlistener.h"

class LyricController : public QObject
{
    Q_OBJECT;
public:
    LyricController(int interval = 10, int positionUpdateInterval = 5000);
    ~LyricController();

signals:
    void pause();
    void play();
    void lyricChanged(const Lyric::LyricInfo lyricInfo);

private slots:
    void tick();
    void foundPlayer();
    void changeMetadata(MprisMetadata metadata);
    void playbackStatusChange(PlaybackStatusListener::PlaybackStatus status);

private:
    QTimer* timer = nullptr;
    MetadataListener* metadataListener = nullptr;
    PlaybackStatusListener* playbackStatusListener = nullptr;
    FoundPlayerListener* foundPlayerListener = nullptr;

    MusicfoxManager musicfoxManager;
    MprisMetadata metadata;
    Lyric lyric;
    int64_t position = 0;
    int64_t positionCount = 0;
    bool playing;
    int interval;
    int positionUpdateInterval;
    bool init = true;
};

#endif // LYRIC_CONTROLLER_H
