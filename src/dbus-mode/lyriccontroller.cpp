#include "lyriccontroller.h"
#include "../debug.h"
#include <iostream>

LyricController::LyricController(int _interval, int _positionUpdateInterval) :
    QObject(),
    timer(new QTimer(this)),
    metadataListener(new MetadataListener()),
    playbackStatusListener(new PlaybackStatusListener()),
    foundPlayerListener(new FoundPlayerListener()),
    interval(_interval),
    positionUpdateInterval(_positionUpdateInterval / _interval)
{
    if (!musicfoxManager.connect_dbus())
    {
        DEBUG("connect to dbus failed");
    }
    else
    {
        DEBUG("launching listener");
        musicfoxManager.add_listener(metadataListener);
        musicfoxManager.add_listener(playbackStatusListener);
        musicfoxManager.add_listener(foundPlayerListener);

        connect(this->timer, &QTimer::timeout, this, &LyricController::tick);
        connect(this->metadataListener, &MetadataListener::metadataChanged, this, &LyricController::changeMetadata);
        connect(this->playbackStatusListener, &PlaybackStatusListener::playbackStatusChanged, this, &LyricController::playbackStatusChange);
        connect(this->foundPlayerListener, &FoundPlayerListener::foundPlayer, this, &LyricController::foundPlayer);

        this->timer->setInterval(interval);
        this->timer->start();
    }
}

LyricController::~LyricController()
{
    if (nullptr != timer)
    {
        disconnect(timer);
        timer->stop();
        timer->deleteLater();
        timer = nullptr;
    }

    if (nullptr != metadataListener)
    {
        disconnect(metadataListener);
        metadataListener->deleteLater();
        metadataListener = nullptr;
    }
    if (nullptr != playbackStatusListener)
    {
        disconnect(playbackStatusListener);
        playbackStatusListener->deleteLater();
        playbackStatusListener = nullptr;
    }
    if (nullptr != foundPlayerListener)
    {
        disconnect(foundPlayerListener);
        foundPlayerListener->deleteLater();
        foundPlayerListener = nullptr;
    }
    DEBUG("close controller");
    this->musicfoxManager.close();
}

void LyricController::foundPlayer()
{
    DCODE(std::cout << "found musicfox" << std::endl);
    changeMetadata(musicfoxManager.metadata());
    std::string status = musicfoxManager.playback_status();
    if (status == "Playing")
    {
        playbackStatusChange(PlaybackStatusListener::Playing);
    }
    else if (status == "Paused")
    {
        playbackStatusChange(PlaybackStatusListener::Paused);
    }
}


void LyricController::tick()
{
    this->musicfoxManager.listen(1);

    if (!init && !playing) return;

    init = false;
    int64_t prevPosition = this->position;
    this->position += interval * 1000;

    if (this->positionCount++ % positionUpdateInterval == 0)
    {
        this->position = musicfoxManager.position();
        // failed get current play position
        if (this->position == -1)
        {
            playbackStatusChange(PlaybackStatusListener::Paused);
            return;
        }
    }

    if (1 == this->positionCount || lyric.get_lyric(this->position) != lyric.get_lyric(prevPosition))
    {
        // update lyric

        emit lyricChanged(this->lyric.get_lyric_info(this->position));
    }
}

void LyricController::changeMetadata(MprisMetadata metadata)
{
    metadata.to_string();
    if (this->metadata.xesam_as_text != metadata.xesam_as_text)
    {
        this->lyric.rebuild_from(metadata.xesam_as_text);
    }
    this->metadata = metadata;
    this->position = this->positionCount = 0;
}

void LyricController::playbackStatusChange(PlaybackStatusListener::PlaybackStatus status)
{
    switch (status)
    {
        case PlaybackStatusListener::Playing:
            this->playing = true;
            emit play();
            break;
        case PlaybackStatusListener::Paused:
            this->playing = false;
            emit pause();
            break;
        default:
            break;
    }
}


