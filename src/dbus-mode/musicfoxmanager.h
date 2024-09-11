#ifndef MUSICFOXMANAGER_H
#define MUSICFOXMANAGER_H

#include <dbus/dbus.h>
#include <cstring>
#include <string>
#include <vector>
#include "mprismetadata.h"

#define LISTENER_TYPE_FOUND_PLAYER "FoundPlayer"

/**
 * @todo write docs
 */
class MusicfoxManager
{
public:
    class Listener {
    public:
        virtual ~Listener() = 0;
        virtual const char* property() = 0;
        virtual void apply(DBusMessageIter* iter) = 0;
    };

public:
    /**
     * Default constructor
     */
    MusicfoxManager();

    /**
     * Destructor
     */
    ~MusicfoxManager();

    bool connect_dbus();
    bool find_musicfox();
    void add_listener(Listener* listener);
    void listen(int timeout);
    void listen_loop();
    void close();

    /**
     * get player current postion.
     */
    int64_t position();
    MprisMetadata metadata();
    std::string playback_status();

private:
    bool get_property(const char* destination, const char* interface, const char* properties, DBusMessage*& dbus_msg, DBusMessage*& dbus_rpl);

private:
    DBusConnection* dbus_conn = nullptr;

    bool found_musicfox = false;
    bool ready_lisen = false;
    std::string name;
    std::string owner;
    std::vector<Listener*> listeners;
};



#endif // MUSICFOXMANAGER_H
