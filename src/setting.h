#ifndef SETTING_H
#define SETTING_H

#define SETTING_FILE_NAME QApplication::applicationDirPath() + "/.netease-lyric.setting"

#define KEY_FONT_COLOR "FONT_COLOR"
#define KEY_FONT_SIZE "FONT_SIZE"
#define KEY_FONT_FAMILY "FONT_FIMALY"
#define KEY_FONT_BOLD "FONT_BOLD"
#define KEY_FONT_ITALIC "FONT_ITALIC"
#define KEY_POS_H "POS_H"
#define KEY_POS_V "POS_V"

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
