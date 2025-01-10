#ifndef SETTING_H
#define SETTING_H

#define SETTING_FILE_NAME QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/.lyric-for-musicfox.setting"

#define KEY_POS_X               "POS_X"                 // 窗口 X 坐标
#define KEY_POS_Y               "POS_Y"                 // 窗口 Y 坐标
#define KEY_WIDTH               "WIDTH"                 // 窗口宽度
#define KEY_HEIGHT              "HEIGHT"                // 窗口高度
#define KEY_FONT_COLOR          "FONT_COLOR"            // 字体颜色
#define KEY_FONT_OUTLINE_COLOR  "FONT_OUTLINE_COLOR"    // 字体描边颜色
#define KEY_FONT_OUTLINE_WIDTH  "FONT_OUTLINE_WIDTH"    // 字体描边大小
#define KEY_FONT_SIZE           "FONT_SIZE"             // 字体大小
#define KEY_FONT_FAMILY         "FONT_FIMALY"           // 字体家族名
#define KEY_FONT_BOLD           "FONT_BOLD"             // 是否加粗
#define KEY_FONT_ITALIC         "FONT_ITALIC"           // 是否斜体
#define KEY_FLAGS_STAY_ON_TOP   "STAY_ON_TOP"           // 窗口是否置顶
#define KEY_FRAME_LESS          "FRAME_LESS"            // 窗口是否无边框
#define KEY_ENABLE_MPRIS        "ENABLE_MPRIS"          // 是否启用 MPRIS
#define KEY_RECEIVE_PORT        "RECEIVE_PORT"          // 接收歌词(或指令)的UDP端口
#define KEY_LYRIC_AUTO_TICK     "LYRIC_AUTO_TICK"       // 自动激发 Tick 方法间隔，正整数，0禁用

#include <string>
#include <map>
#include <QApplication>
#include <QStandardPaths>
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
