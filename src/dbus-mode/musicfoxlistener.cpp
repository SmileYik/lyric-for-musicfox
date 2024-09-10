// SPDX-FileCopyrightText: 2024 <copyright holder> <email>
// SPDX-License-Identifier: MIT

#include "musicfoxlistener.h"
#include <iostream>
#include "../debug.h"
#include "mprismetadata.h"

#define DBUS_METHOD_LISTNAMES "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames"
#define DBUS_METHOD_GETNAMEOWNER "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "GetNameOwner"

MusicfoxListener::MusicfoxListener()
{

}

MusicfoxListener::~MusicfoxListener()
{

}

void MusicfoxListener::connect_dbus()
{
    DBusError dbus_error;
    dbus_error_init(&dbus_error);

    if ( nullptr == (dbus_conn = ::dbus_bus_get(DBUS_BUS_SESSION, &dbus_error)) ) {
        ::perror(dbus_error.name);
        ::perror(dbus_error.message);
        return;
    }
}

void MusicfoxListener::close()
{
    if (nullptr != dbus_conn)
    {
        dbus_connection_unref(dbus_conn);
        dbus_conn = nullptr;
    }
}


void MusicfoxListener::find_musicfox()
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
        return;
    }
    else if (nullptr == (dbus_rpl = dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT, &dbus_error)))
    {
        dbus_message_unref(dbus_msg);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return;
    }
    else if (!dbus_message_get_args(dbus_rpl, &dbus_error, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &name_list, &name_list_count, DBUS_TYPE_INVALID))
    {
        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_rpl);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return;
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
        return;
    }
    this->found_musicfox = false;

    DCODE(std::cout << "call method: GetNameOwner" << std::endl;)
    if (nullptr == (dbus_msg = dbus_message_new_method_call(DBUS_METHOD_GETNAMEOWNER)))
    {
        dbus_connection_unref(this->dbus_conn);
        this->dbus_conn = nullptr;
        return;
    } else if (!dbus_message_append_args(dbus_msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID))
    {
        dbus_message_unref(dbus_msg);
        return;
    } else if (nullptr == (dbus_rpl = dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT, &dbus_error)))
    {
        dbus_message_unref(dbus_msg);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return;
    } else if (!dbus_message_get_args(dbus_rpl, &dbus_error, DBUS_TYPE_STRING, &owner, DBUS_TYPE_INVALID))
    {
        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_rpl);
        perror(dbus_error.name);
        perror(dbus_error.message);
        return;
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



    DCODE(std::cout << "finished" << std::endl);
    dbus_bus_add_match(dbus_conn, "member=PropertiesChanged", &dbus_error);
    dbus_connection_flush(dbus_conn);
    if (dbus_error_is_set(&dbus_error))
    {
        ::perror(dbus_error.name);
        ::perror(dbus_error.message);

        return;
    }
    DCODE(std::cout << "Listening" << std::endl);
    while (true)
    {
        if (!dbus_connection_read_write(dbus_conn, 0))
        {
            continue;
        }
        DBusMessage* msg;
        while (nullptr != (msg = dbus_connection_pop_message(dbus_conn))) {
            const char* destination = dbus_message_get_destination(msg);
            const char* sender = dbus_message_get_sender(msg);
            bool hit = false;
            if (
                destination != nullptr && (0 == strcmp(destination, this->name.c_str()) || 0 == strcmp(destination, this->owner.c_str())) ||
                sender != nullptr && (0 == strcmp(sender, this->name.c_str()) || 0 == strcmp(sender, this->owner.c_str()))
            )
            {
                DBusMessageIter iter;
                dbus_message_iter_init(msg, &iter);
                if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter)) {dbus_message_unref(msg); continue;}

                char* arg1;
                DBusMessageIter arg2, dics;
                dbus_message_iter_get_basic(&iter, &arg1);
                if (0 != strcmp(arg1, "org.mpris.MediaPlayer2.Player")) {dbus_message_unref(msg); continue;}
                dbus_message_iter_next(&iter);
                dbus_message_iter_recurse(&iter, &arg2);
                dbus_message_iter_recurse(&arg2, &dics);

                char* key;
                DBusMessageIter val;
                dbus_message_iter_get_basic(&dics, &key);
                if (0 != strcmp(key, "Metadata")) {dbus_message_unref(msg); continue;}
                dbus_message_iter_next(&dics);
                dbus_message_iter_recurse(&dics, &val);

                MprisMetadata metadata;
                if (MprisMetadata::read(&val, metadata))
                {
                    DCODE(std::cout << "SUCCESS READED!" << std::endl);
                    metadata.to_string();
                }
            }
            dbus_message_unref(msg);
        }
    }
}

