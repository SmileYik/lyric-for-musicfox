#include "setting.h"
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QDir>
Setting::Setting()
{
    QDir dir;
    if (!dir.exists(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)))
{
    dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
}
    QFile file(SETTING_FILE_NAME);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    QString line = nullptr;
    while ((line = in.readLine()) != nullptr)
    {
        QString key = line;
        if ((line = in.readLine()) == nullptr) break;
        QString value = line.replace("\\n", "\n");
        keyValues[key.toStdString()] = value.toStdString();
    }
    file.close();

}

const bool Setting::has(const std::string& key) const
{
    return keyValues.count(key);
}


const std::string Setting::get(const std::string& key)
{
    return keyValues.count(key) ? keyValues[key] : "";
}

const std::string Setting::get(const std::string &key, const std::string &defaultValue)
{
    return keyValues.count(key) ? keyValues[key] : defaultValue;
}

const bool Setting::getBool(const std::string &key)
{
    return get(key) == "TRUE";
}

void Setting::putBool(const std::string &key, const bool value)
{
    put(key, value ? std::string("TRUE") : std::string("FALSE"));
}

void Setting::put(const std::string& key, const std::string& value)
{
    keyValues[std::move(key)] = std::move(value);
}

void Setting::store()
{
    QFile file(SETTING_FILE_NAME);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        return;
    }

    for (auto begin = keyValues.cbegin(),
                end = keyValues.cend(); begin != end; ++begin)
    {
        file.write(QByteArray::fromStdString(begin->first));
        file.write(QByteArray::fromStdString("\n"));
        file.write(QByteArray::fromStdString(begin->second).replace("\n", "\\n"));
        file.write(QByteArray::fromStdString("\n"));
    }

    file.flush();
    file.close();
}

bool Setting::isEmpty() const
{
    return keyValues.empty();
}
