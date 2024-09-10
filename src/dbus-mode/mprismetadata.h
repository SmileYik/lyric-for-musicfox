#ifndef MPRIS_METADATA_H
#define MPRIS_METADATA_H

#include <vector>
#include <string>
#include <dbus/dbus.h>

class MprisMetadata
{
public:
    std::string to_string();

    static bool read(DBusMessageIter* iter, MprisMetadata& metadata);
    static void read_string(DBusMessageIter* iter, std::string& value);
    static void read_string_array(DBusMessageIter* iter, std::vector<std::string>& value);
    static void read_basic(DBusMessageIter* iter, void* value);
    static void read_object_path(DBusMessageIter* iter, std::string& value);

public:
    std::string mpris_trackid;
    int64_t mpris_length = -1;
    std::string mpris_art_url;

    std::string xesam_album;
    std::vector<std::string> xesam_album_artist;
    std::vector<std::string> xesam_artist;
    std::string xesam_as_text;
    int32_t xesam_audio_bpm = 0;
    float xesam_auto_rating = .0;
    std::vector<std::string> xesam_comment;
    std::vector<std::string> xesam_composer;
    std::string xesam_content_created;
    int32_t xesam_disc_number = 0;
    std::string xesam_first_used;
    std::vector<std::string> xesam_genre;
    std::string xesam_last_used;
    std::vector<std::string> xesam_lyricist;
    std::string xesam_title;
    int32_t xesam_track_number = 0;
    std::string xesam_url;
    int32_t xesam_use_count = 0;
    float xesam_user_rating = 1; // 0 ~ 1
};

#endif  // MPRIS_METADATA_H
