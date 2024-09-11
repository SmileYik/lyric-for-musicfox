// SPDX-FileCopyrightText: 2024 <copyright holder> <email>
// SPDX-License-Identifier: MIT

#include "lyric.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <map>
#include "../debug.h"

const static std::regex TIME_REGEX(
    "\\[([0-9]{0,2}):([0-9]{0,2})\\.([0-9]{0,2})\\]"
);

Lyric::Lyric()
{

}


Lyric::Lyric(const std::string& lyric_text)
{
    rebuild_from(lyric_text);
}

Lyric::~Lyric()
{
    timestamp_map.clear();
}

void Lyric::rebuild_from(const std::string& lyric_text)
{
    DCODE(
        std::cout << "start build lyric" << std::endl;
    );

    timestamp_map.clear();

    std::istringstream iss(lyric_text);
    std::string line;

    std::smatch time_match;
    while (std::getline(iss, line))
    {
        if (std::regex_search(line, time_match, TIME_REGEX))
        {
            if (time_match.size() != 4) continue;
            // DCODE(
            //     for (int i = 0, e = time_match.size(); i != e; ++i)
            //     {
            //         std::cout << time_match[i] << ", ";
            //     }
            // );
            int mm = atoi(time_match[1] == "" ? "0" : std::string(time_match[1]).c_str());
            int ss = atoi(time_match[2] == "" ? "0" : std::string(time_match[2]).c_str());
            int xx = atoi(time_match[3] == "" ? "0" : std::string(time_match[3]).c_str());
            int64_t timestamp = ((mm * 60 + ss) * 100 + xx) * 10000;
            // DCODE(
            //     std::cout << "timestamp: " << timestamp << "; lyric: " << time_match.suffix() << std::endl;
            // );
            if (timestamp_map.count(timestamp))
            {
                timestamp_map[timestamp] += "\n" + std::string(time_match.suffix());
            }
            else {
                timestamp_map[timestamp] = std::string(time_match.suffix());
            }
        }
    }
    DCODE(
        std::cout << "finished build lyric" << std::endl;
    );
}


const std::string* Lyric::get_lyric(int64_t timestamp) const
{
    auto pos = timestamp_map.lower_bound(timestamp);

    if (pos != timestamp_map.begin())
    {
        --pos;
    }

    if (pos == timestamp_map.end())
    {
        return nullptr;
    }
    return &pos->second;
}

const Lyric::LyricInfo Lyric::get_lyric_info(int64_t timestamp) const
{
    LyricInfo info;
    auto pos = timestamp_map.lower_bound(timestamp);
    if (pos != timestamp_map.begin())
    {
        --pos;
    }

    if (pos == timestamp_map.end())
    {
        return info;
    }

    info.lyric = &pos->second;

    auto tmp = pos;
    if (++tmp != timestamp_map.end())
    {
        info.time_to_next = tmp->first - timestamp;
        info.next_lyric = &tmp->second;
    }

    tmp = pos;
    if (pos != timestamp_map.begin())
    {
        --tmp;
        info.time_to_prev = timestamp - tmp->first;
        info.prev_lyric = &tmp->second;
    }

    return info;
}
