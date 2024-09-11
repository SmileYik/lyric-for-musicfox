#ifndef QT_MUSICFOX_MANAGER_LISTENER_H
#define QT_MUSICFOX_MANAGER_LISTENER_H

#include <QObject>
#include "musicfoxmanager.h"
#include "mprismetadata.h"

class FoundPlayerListener : public QObject, public MusicfoxManager::Listener
{
    Q_OBJECT;
public:
    FoundPlayerListener();
    ~FoundPlayerListener() override;

    const char* property() override;
    void apply(DBusMessageIter* iter) override;
signals:
    void foundPlayer();
};

class MetadataListener : public QObject, public MusicfoxManager::Listener
{
    Q_OBJECT;
public:
    MetadataListener();
    ~MetadataListener() override;

    const char* property() override;
    void apply(DBusMessageIter* iter) override;
signals:
    void metadataChanged(MprisMetadata metadata);
};

class PlaybackStatusListener : public QObject, public MusicfoxManager::Listener
{
    Q_OBJECT;
public:
    enum PlaybackStatus {
        None,
        Paused,
        Playing
    };

    PlaybackStatusListener();
    ~PlaybackStatusListener() override;

    const char* property() override;
    void apply(DBusMessageIter* iter) override;
signals:
    void playbackStatusChanged(PlaybackStatus playbackStatus);
};

#endif // QT_MUSICFOX_MANAGER_LISTENER_H
