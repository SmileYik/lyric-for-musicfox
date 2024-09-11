#include "qtlistener.h"
#include <iostream>
#include "../debug.h"
#include "lyric.h"

MetadataListener::MetadataListener() : QObject(), MusicfoxManager::Listener()
{

}

MetadataListener::~MetadataListener() {

}


void MetadataListener::apply(DBusMessageIter* iter)
{
    MprisMetadata metadata;
    if (MprisMetadata::read(iter, metadata))
    {
        DCODE(std::cout << "SUCCESS READED!" << std::endl);
        // DCODE(
        //     // metadata.to_string();
        //     Lyric lyric(metadata.xesam_as_text);
        //     std::cout << *lyric.get_lyric(100000000) << std::endl;
        //
        //     int64_t pos = 100000000;
        //     std::cout << "position: " << pos << std::endl;
        //     const Lyric::LyricInfo info = lyric.get_lyric_info(pos);
        //     if (info.prev_lyric != nullptr)
        //     {
        //         std::cout << info.time_to_prev << ": " << *info.prev_lyric << std::endl;
        //     }
        //     if (info.lyric != nullptr)
        //     {
        //         std::cout << "0: " << *info.lyric << std::endl;
        //     }
        //     if (info.next_lyric != nullptr)
        //     {
        //         std::cout << info.time_to_next << ": " << *info.next_lyric << std::endl;
        //     }
        // );
        emit metadataChanged(metadata);
    }
}

const char* MetadataListener::property()
{
    return "Metadata";
}

PlaybackStatusListener::PlaybackStatusListener() : QObject(), MusicfoxManager::Listener()
{

}

PlaybackStatusListener::~PlaybackStatusListener() {

}

void PlaybackStatusListener::apply(DBusMessageIter* iter)
{
    char* status = nullptr;
    dbus_message_iter_get_basic(iter, &status);
    PlaybackStatus s = None;
    if (0 == strcmp(status, "Paused"))
    {
        s = Paused;
    }
    else if (0 == strcmp(status, "Playing"))
    {
        s = Playing;
    }
    DEBUG("playback status changed: " << s << "\n");
    emit playbackStatusChanged(s);
}

const char * PlaybackStatusListener::property()
{
    return "PlaybackStatus";
}

FoundPlayerListener::FoundPlayerListener() : QObject(), MusicfoxManager::Listener()
{

}

FoundPlayerListener::~FoundPlayerListener()
{

}

void FoundPlayerListener::apply(DBusMessageIter* iter)
{
    emit foundPlayer();
}

const char * FoundPlayerListener::property()
{
    return "FoundPlayer";
}



