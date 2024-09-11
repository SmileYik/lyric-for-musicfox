// SPDX-FileCopyrightText: 2024 <copyright holder> <email>
// SPDX-License-Identifier: MIT

#ifndef LYRIC_H
#define LYRIC_H

#include <string>
#include <map>

/**
 * @todo write docs
 */
class Lyric
{
public:

    typedef struct _LyricInfo {
        const std::string* prev_lyric = nullptr;
        const std::string* lyric = nullptr;
        const std::string* next_lyric = nullptr;
        int64_t time_to_prev;
        int64_t time_to_next;
    } LyricInfo;

    Lyric();

    Lyric(const std::string& lyric_text);

    ~Lyric();

    void rebuild_from(const std::string& lyric_text);

    const std::string* get_lyric(int64_t timestamp) const;

    const LyricInfo get_lyric_info(int64_t timestamp) const;
private:
    std::map<int64_t, std::string> timestamp_map;

};

#endif // LYRIC_H
