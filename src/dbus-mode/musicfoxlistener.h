#ifndef MUSICFOXLISTENER_H
#define MUSICFOXLISTENER_H

#include <dbus/dbus.h>
#include <cstring>
#include <string>

/**
 * @todo write docs
 */
class MusicfoxListener
{
public:
    /**
     * Default constructor
     */
    MusicfoxListener();

    /**
     * Destructor
     */
    ~MusicfoxListener();

    void connect_dbus();
    void find_musicfox();
    void close();

private:


private:
    DBusConnection* dbus_conn = nullptr;

    bool found_musicfox = false;
    std::string name;
    std::string owner;

};

#endif // MUSICFOXLISTENER_H
