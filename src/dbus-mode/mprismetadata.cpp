#include <iostream>
#include "mprismetadata.h"
#include "../debug.h"

typedef struct _MprisMetadataFieldReadRule {
    const char* key;
    void (*apply)(MprisMetadata&, DBusMessageIter*);
} MprisMetadataFieldReadRule;


const static MprisMetadataFieldReadRule MPRIS_METADATA_FIELD_READ_RULE_ARRAY[] = {
    {
        "mpris:length",
        [] (MprisMetadata& metadata, DBusMessageIter* iter) {
            MprisMetadata::read_basic(iter, &metadata.mpris_length);
        }
    },
    {
        "mpris:trackid",
        [] (MprisMetadata& metadata, DBusMessageIter* iter) {
            MprisMetadata::read_object_path(iter, metadata.mpris_trackid);
        }
    },
    {
        "mpris:artUrl",
        [] (MprisMetadata& metadata, DBusMessageIter* iter) {
            MprisMetadata::read_string(iter, metadata.mpris_art_url);
        }
    },
    {
        "xesam:album",
        [] (MprisMetadata& metadata, DBusMessageIter* iter) {
            MprisMetadata::read_string(iter, metadata.xesam_album);
        }
    },
    {
        "xesam:title",
        [] (MprisMetadata& metadata, DBusMessageIter* iter) {
            MprisMetadata::read_string(iter, metadata.xesam_title);
        }
    },
    {
        "xesam:asText",
        [] (MprisMetadata& metadata, DBusMessageIter* iter) {
            MprisMetadata::read_string(iter, metadata.xesam_as_text);
        }
    },
    {
        "xesam:artist",
        [] (MprisMetadata& metadata, DBusMessageIter* iter) {
            MprisMetadata::read_string_array(iter, metadata.xesam_artist);
        }
    }
};


bool MprisMetadata::read(DBusMessageIter* iter, MprisMetadata& metadata)
{
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(iter))
    {
        DCODE(printf("[read] not array\n"));
        return false;
    }

    DCODE(std::cout << "[read] start read mpris metadata" << std::endl);

    DBusMessageIter elems;
    dbus_message_iter_recurse(iter, &elems);
    do {
        char* key;
        DBusMessageIter dic, var;

        dbus_message_iter_recurse(&elems, &dic);
        dbus_message_iter_get_basic(&dic, &key);
        DCODE(std::cout << "read key: " << key << std::endl);
        dbus_message_iter_next(&dic);
        dbus_message_iter_recurse(&dic, &var);

        for (const MprisMetadataFieldReadRule & rule : MPRIS_METADATA_FIELD_READ_RULE_ARRAY)
        {
            if (0 == strcmp(rule.key, key))
            {
                DCODE(std::cout << "match key: " << key << std::endl);
                rule.apply(metadata, &var);
            }
        }
    } while (dbus_message_iter_has_next(&elems) && dbus_message_iter_next(&elems));

    return true;
}

void MprisMetadata::read_string(DBusMessageIter* iter, std::string& value)
{
    char* val = nullptr;
    dbus_message_iter_get_basic(iter, &val);
    value = std::move(val);
}

void MprisMetadata::read_string_array(DBusMessageIter* iter, std::vector<std::string>& value)
{
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(iter))
    {
        DCODE(std::cout << ("[read_string_array] not array") << std::endl);
        return;
    }

    DBusMessageIter elems;
    dbus_message_iter_recurse(iter, &elems);
    value.clear();
    do {
        std::string str;
        read_string(&elems, str);
        value.push_back(std::move(str));
    } while (dbus_message_iter_has_next(&elems) && dbus_message_iter_next(&elems));
}


void MprisMetadata::read_basic(DBusMessageIter* iter, void* value)
{
    dbus_message_iter_get_basic(iter, value);
}

void MprisMetadata::read_object_path(DBusMessageIter* iter, std::string& value)
{
    read_string(iter, value);
}

std::string MprisMetadata::to_string()
{
    auto string_array_to_string = [] (const std::vector<std::string>& array) {
        if (array.empty()) return std::string("[]");
        std::string str = "[";
        bool first = true;
        for (const std::string& elem : array)
        {
            if (first)
            {
                first = false;
                str += elem;
            } else {
                str += ", " + elem;
            }
        }
        return str + "]";
    };

    DCODE(std::cout << "{"
                << "mpris_trackid: " << mpris_trackid << ", "
                << "mpris_length: " << mpris_length << ", "
                << "mpris_art_url: " << mpris_art_url << ", "
                << "xesam_album: " << xesam_album << ", "
                << "xesam_album_artist: " << string_array_to_string(xesam_album_artist) << ", "
                << "xesam_artist: " << string_array_to_string(xesam_artist) << ", "
                << "xesam_as_text: " << xesam_as_text << ", "
                << "xesam_audio_bpm: " << xesam_audio_bpm << ", "
                << "xesam_auto_rating: " << xesam_auto_rating << ", "
                << "xesam_comment: " << string_array_to_string(xesam_comment) << ", "
                << "xesam_composer: " << string_array_to_string(xesam_composer) << ", "
                << "xesam_content_created: " << xesam_content_created << ", "
                << "xesam_disc_number: " << xesam_disc_number << ", "
                << "xesam_first_used: " << xesam_first_used << ", "
                << "xesam_genre: " << string_array_to_string(xesam_genre) << ", "
                << "xesam_last_used: " << xesam_last_used << ", "
                << "xesam_lyricist: " << string_array_to_string(xesam_lyricist) << ", "
                << "xesam_title: " << xesam_title << ", "
                << "xesam_track_number: " << xesam_track_number << ", "
                << "xesam_url: " << xesam_url << ", "
                << "xesam_use_count: " << xesam_use_count << ", "
                << "xesam_user_rating: " << xesam_user_rating << "}" << std::endl;);
    return "";
}


