#ifndef SETTING_H
#define SETTING_H

#define SETTING_FILE_NAME QApplication::applicationDirPath() + "/.netease-lyric.setting"

#define KEY_POS_X               "POS_X"
#define KEY_POS_Y               "POS_Y"
#define KEY_WIDTH               "WIDTH"
#define KEY_HEIGHT              "HEIGHT"
#define KEY_FONT_COLOR          "FONT_COLOR"
#define KEY_FONT_SIZE           "FONT_SIZE"
#define KEY_FONT_FAMILY         "FONT_FIMALY"
#define KEY_FONT_BOLD           "FONT_BOLD"
#define KEY_FONT_ITALIC         "FONT_ITALIC"
#define KEY_POS_H               "POS_H"
#define KEY_POS_V               "POS_V"
#define KEY_FLAGS_STAY_ON_TOP   "STAY_ON_TOP"
#define KEY_FRAME_LESS          "FRAME_LESS"

#define VALUE_LEFT "LEFT"
#define VALUE_CENTER "CENTER"
#define VALUE_RIGHT "RIGHT"

#include <string>
#include <map>
#include <QApplication>

class Setting
{
public:
    Setting();
    const bool has(const std::string& key) const;
    const std::string get(const std::string& key);
    const std::string get(const std::string& key, const std::string& defaultValue);
    const bool getBool(const std::string& key);
    void putBool(const std::string& key, const bool value);
    void put(const std::string& key, const std::string& value);
    void store();
    bool isEmpty() const;
private:
    std::map<std::string, std::string> keyValues;
};

#endif // SETTING_H
