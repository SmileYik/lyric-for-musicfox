// SPDX-FileCopyrightText: 2024 <copyright holder> <email>
// SPDX-License-Identifier: MIT

#include "musicfoxmanager.h"
#include <iostream>
#include "../debug.h"
#include "mprismetadata.h"
#include "lyric.h"

#define DBUS_METHOD_LISTNAMES "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames"
#define DBUS_METHOD_GETNAMEOWNER "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "GetNameOwner"
#define DBUS_METHOD_GET_MEDIA_PLAYER2 "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "Get"

void tostring(DBusMessageIter* iter, std::string& str);

void tostring_object(DBusMessageIter* iter, std::string& str)
{
    char* key = nullptr;
    dbus_message_iter_get_basic(iter, &key);
    str.append(key);
}

void tostring_array(DBusMessageIter* iter, std::string& str)
{
    // std::cout << "tostring_array: " << str << std::endl;
    DBusMessageIter elems;
    dbus_message_iter_recurse(iter, &elems);
    str.append("[");
    tostring(&elems, str);
    str.append("]");
}

void tostring_dict(DBusMessageIter* iter, std::string& str)
{
    // std::cout << "tostring_dict: " << str << std::endl;
    DBusMessageIter dics;
    dbus_message_iter_recurse(iter, &dics);
    char* key = nullptr;
    dbus_message_iter_get_basic(&dics, &key);
    dbus_message_iter_next(&dics);
    std::string val = "";
    tostring(&dics, val);
    str.append(" \"").append(key).append("\": ").append(val);
}

void tostring(DBusMessageIter* iter, std::string& str)
{
    // std::cout << "tostring: " << str << std::endl;
    int arg_type = dbus_message_iter_get_arg_type(iter);

    dbus_uint64_t val_val;
    void* val = &val_val;
    char* val_str = nullptr;

    switch (arg_type)
    {
        case DBUS_TYPE_UINT32:
        case DBUS_TYPE_BOOLEAN:
            dbus_message_iter_get_basic(iter, (dbus_uint32_t*) val);
            str.append(" ").append(std::to_string(*(dbus_uint32_t*)val)).append(",");
            break;
        case DBUS_TYPE_BYTE:
            dbus_message_iter_get_basic(iter, (char*) val);
            str.append(" ").append(std::to_string(*(char*) val)).append(",");
            break;
        case DBUS_TYPE_INT16:
            dbus_message_iter_get_basic(iter, (dbus_int16_t*) val);
            str.append(" ").append(std::to_string(*(dbus_int16_t*) val)).append(",");
            break;
        case DBUS_TYPE_INT32:
            dbus_message_iter_get_basic(iter, (dbus_int32_t*) val);
            str.append(" ").append(std::to_string(*(dbus_int32_t*) val)).append(",");
            break;
        case DBUS_TYPE_INT64:
            dbus_message_iter_get_basic(iter, (dbus_int64_t*) val);
            str.append(" ").append(std::to_string(*(dbus_int64_t*) val)).append(",");
            break;
        case DBUS_TYPE_UINT16:
            dbus_message_iter_get_basic(iter, (dbus_uint16_t*) val);
            str.append(" ").append(std::to_string(*(dbus_uint16_t*) val)).append(",");
            break;
        case DBUS_TYPE_UINT64:
            dbus_message_iter_get_basic(iter, (dbus_uint64_t*) val);
            str.append(" ").append(std::to_string(*(dbus_uint64_t*) val)).append(",");
            break;
        case DBUS_TYPE_STRING:
            dbus_message_iter_get_basic(iter, &val_str);
            str.append(" \"").append(val_str).append("\",");
            val_str = nullptr;
            break;
        case DBUS_TYPE_DOUBLE:
            dbus_message_iter_get_basic(iter, (double*) val);
            str.append(" ").append(std::to_string(*(double*) val)).append(",");
            break;
        case DBUS_TYPE_VARIANT:
        case DBUS_TYPE_ARRAY:
            tostring_array(iter, str);
            str.append(",");
            break;
        case DBUS_TYPE_DICT_ENTRY:
            tostring_dict(iter, str);
            str.append(",");
            break;
        case DBUS_TYPE_OBJECT_PATH:
            tostring_object(iter, str);
            break;

    }

    if (dbus_message_iter_has_next(iter) && dbus_message_iter_next(iter))
    {
        tostring(iter, str);
    }
    else if (*(str.end() - 1) == ',')
    {
        str.erase(str.size() - 1);
    }
}

MusicfoxManager::MusicfoxManager()
{

}

MusicfoxManager::~MusicfoxManager()
{

}

bool MusicfoxManager::connect_dbus()
{
    DBusError dbus_error;
    dbus_error_init(&dbus_error);

    if ( nullptr == (dbus_conn = ::dbus_bus_get(DBUS_BUS_SESSION, &dbus_error)) ) {
        ::perror(dbus_error.name);
        ::perror(dbus_error.message);
        return false;
    }
    return true;
}

void MusicfoxManager::close()
{
    if (nullptr != dbus_conn)
    {
        dbus_connection_unref(dbus_conn);
        dbus_conn = nullptr;
    }
    this->listeners.clear();
}


bool MusicfoxManager::find_musicfox()
{
    int name_list_count;
    char** name_list = nullptr;
    char* name = nullptr;
    char* owner = nullptr;
    DBusError dbus_error;
    DBusMessage* dbus_msg = nullptr;
    DBusMessage* dbus_rpl = nullptr;

    dbus_error_init(&dbus_error);
    DCODE(std::cout << "call method: ListNames" << std::endl;)
    if (nullptr == (dbus_msg = dbus_message_new_method_call(DBUS_METHOD_LISTNAMES)))
    {
        dbus_connection_unref(this->dbus_conn);
        this->dbus_conn = nullptr;
        return false;
    }
    else if (nullptr == (dbus_rpl = dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT, &dbus_error)))
    {
        dbus_message_unref(dbus_msg);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return false;
    }
    else if (!dbus_message_get_args(dbus_rpl, &dbus_error, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &name_list, &name_list_count, DBUS_TYPE_INVALID))
    {
        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_rpl);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return false;
    }

    DCODE(std::cout << "Name list count is: " << name_list_count << std::endl;)
    while (--name_list_count > 0)
    {
        DCODE(std::cout << "name: " << name_list[name_list_count] << std::endl;)
        if (nullptr != std::strstr(name_list[name_list_count], "musicfox"))
        {
            this->found_musicfox = true;
            this->name = std::string(name_list[name_list_count]);
            char name_tmp[128] = "";
            strcpy(name_tmp, this->name.c_str());
            name = name_tmp;
            DCODE(
                std::cout << "musicfox dbus name: " << name << std::endl;
            )
            break;
        }
    }

    dbus_free_string_array(name_list);
    dbus_message_unref(dbus_msg);
    dbus_message_unref(dbus_rpl);
    dbus_msg = dbus_rpl = nullptr;


    if (!this->found_musicfox)
    {
        return false;
    }
    this->found_musicfox = false;

    DCODE(std::cout << "call method: GetNameOwner" << std::endl;)
    if (nullptr == (dbus_msg = dbus_message_new_method_call(DBUS_METHOD_GETNAMEOWNER)))
    {
        dbus_connection_unref(this->dbus_conn);
        this->dbus_conn = nullptr;
        return false;
    } else if (!dbus_message_append_args(dbus_msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID))
    {
        dbus_message_unref(dbus_msg);
        return false;
    } else if (nullptr == (dbus_rpl = dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT, &dbus_error)))
    {
        dbus_message_unref(dbus_msg);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return false;
    } else if (!dbus_message_get_args(dbus_rpl, &dbus_error, DBUS_TYPE_STRING, &owner, DBUS_TYPE_INVALID))
    {
        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_rpl);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return false;
    }
    if (owner != nullptr)
    {
        this->owner = std::string(owner);
        DCODE(
            std::cout << "musicfox dbus name owner: " << this->owner << std::endl;
        )
        this->found_musicfox = true;
    }

    dbus_message_unref(dbus_msg);
    dbus_message_unref(dbus_rpl);

    // call event
    for (auto begin = listeners.begin(), end = listeners.end(); begin != end; ++begin)
    {
        if (0 == strcmp((*begin)->property(), LISTENER_TYPE_FOUND_PLAYER))
        {
            (*begin)->apply(nullptr);
        }
    }
    return true;
}

void MusicfoxManager::add_listener(Listener* listener)
{
    this->listeners.push_back(listener);
}

void MusicfoxManager::listen(int timeout)
{
    if (!this->ready_lisen)
    {
        // init match
        DBusError dbus_error;
        dbus_error_init(&dbus_error);
        dbus_bus_add_match(dbus_conn, "member=PropertiesChanged", &dbus_error);
        dbus_connection_flush(dbus_conn);
        if (dbus_error_is_set(&dbus_error))
        {
            perror(dbus_error.name);
            perror(dbus_error.message);

            return;
        }
        this->ready_lisen = true;
        DCODE(std::cout << "Finished prepare lisen." << std::endl);

        // find player
        if (!this->found_musicfox)
        {
            find_musicfox();
        }
    }

    if (!dbus_connection_read_write(dbus_conn, timeout))
    {
        return;
    }

    DBusMessage* msg;
    while (nullptr != (msg = dbus_connection_pop_message(dbus_conn))) {
        const char* destination = dbus_message_get_destination(msg);
        const char* sender = dbus_message_get_sender(msg);
        const char* path = dbus_message_get_path(msg);
        // 检查是否是 musicfox 发出的或者接收的
        if (
            destination != nullptr && (0 == strcmp(destination, this->name.c_str()) || 0 == strcmp(destination, this->owner.c_str())) ||
            sender != nullptr && (0 == strcmp(sender, this->name.c_str()) || 0 == strcmp(sender, this->owner.c_str()))
        )
        {
            DCODE(
                std::cout << "hit lisen" << std::endl;
            );
            DBusMessageIter iter;
            dbus_message_iter_init(msg, &iter);

            DCODE(
                std::cout << dbus_message_get_path(msg) << ";" << dbus_message_get_sender(msg) << "->" << "; " << dbus_message_get_member(msg) << ": ";
                std::cout << "position: " << position() << " :";
                std::string str = "";
                tostring(&iter, str);
                int i = 0;
                while ((i = str.find("\n")) != std::string::npos)
                {
                    str = str.replace(i, 1, "\\n");
                }
                std::cout << str << std::endl;
                dbus_message_iter_init(msg, &iter);
            );

            if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter))
            {
                dbus_message_unref(msg);
                continue;
            }

            char* arg1;
            DBusMessageIter arg2, dics;
            dbus_message_iter_get_basic(&iter, &arg1);
            if (0 != strcmp(arg1, "org.mpris.MediaPlayer2.Player"))
            {
                dbus_message_unref(msg);
                continue;
            }

            dbus_message_iter_next(&iter);
            dbus_message_iter_recurse(&iter, &arg2);
            dbus_message_iter_recurse(&arg2, &dics);

            char* key;
            DBusMessageIter val;
            dbus_message_iter_get_basic(&dics, &key);
            dbus_message_iter_next(&dics);
            dbus_message_iter_recurse(&dics, &val);

            // TODO listener
            DCODE(
                std::cout << "wait compare listener: " << key << std::endl;
            );
            for (auto begin = listeners.begin(), end = listeners.end(); begin != end; ++begin)
            {
                if (0 == strcmp((*begin)->property(), key))
                {
                    DCODE(
                        std::cout << "apply listener: " << key << std::endl;
                    );
                    (*begin)->apply(&val);
                }
            }
        }
        else if (nullptr != path && nullptr != strstr(path, "mpris"))
        {
            this->found_musicfox = false;
            find_musicfox();
        }
        dbus_message_unref(msg);
    }
}


void MusicfoxManager::listen_loop()
{
    while (true)
    {
        listen(200);
    }
}

bool MusicfoxManager::get_property(const char* destination, const char* interface, const char* properties, DBusMessage*& dbus_msg, DBusMessage*& dbus_rpl)
{
    if (!found_musicfox) return false;

    DBusError dbus_error;

    dbus_error_init(&dbus_error);
    if (nullptr == (dbus_msg = dbus_message_new_method_call(destination, DBUS_METHOD_GET_MEDIA_PLAYER2)))
    {
        dbus_connection_unref(this->dbus_conn);
        this->dbus_conn = nullptr;
        this->found_musicfox = false;
        return false;
    }
    else if (!dbus_message_append_args(dbus_msg, DBUS_TYPE_STRING, &interface,
        DBUS_TYPE_STRING, &properties, DBUS_TYPE_INVALID))
    {
        dbus_message_unref(dbus_msg);
        return false;
    }
    else if (nullptr == (dbus_rpl = dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT, &dbus_error)))
    {
        dbus_message_unref(dbus_msg);
        perror(dbus_error.name);
        perror(dbus_error.message);
        this->found_musicfox = false;
        find_musicfox();
        return false;
    }
    return true;
}


int64_t MusicfoxManager::position()
{
    DBusMessage* dbus_msg = nullptr;
    DBusMessage* dbus_rpl = nullptr;
    int64_t position = -1;
    if (get_property(this->owner.c_str(), "org.mpris.MediaPlayer2.Player", "Position", dbus_msg, dbus_rpl))
    {
        DBusMessageIter iter, sub;
        dbus_message_iter_init(dbus_rpl, &iter);
        dbus_message_iter_recurse(&iter, &sub);
        dbus_message_iter_get_basic(&sub, &position);

        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_rpl);
    }

    return position;
}

MprisMetadata MusicfoxManager::metadata()
{
    MprisMetadata metadata;
    DBusMessage* dbus_msg = nullptr;
    DBusMessage* dbus_rpl = nullptr;

    if (get_property(this->owner.c_str(), "org.mpris.MediaPlayer2.Player", "Metadata", dbus_msg, dbus_rpl))
    {
        DBusMessageIter iter, sub;
        dbus_message_iter_init(dbus_rpl, &iter);
        dbus_message_iter_recurse(&iter, &sub);
        MprisMetadata::read(&sub, metadata);

        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_rpl);
    }

    return metadata;
}

std::string MusicfoxManager::playback_status()
{
    DBusMessage* dbus_msg = nullptr;
    DBusMessage* dbus_rpl = nullptr;

    if (get_property(this->owner.c_str(), "org.mpris.MediaPlayer2.Player", "PlaybackStatus", dbus_msg, dbus_rpl))
    {
        char* status = nullptr;
        DBusMessageIter iter, sub;
        dbus_message_iter_init(dbus_rpl, &iter);
        dbus_message_iter_recurse(&iter, &sub);
        dbus_message_iter_get_basic(&sub, &status);

        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_rpl);
        return std::string(std::move(status));
    }
    return "Paused";
}


MusicfoxManager::Listener::~Listener()
{

}
