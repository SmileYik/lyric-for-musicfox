#include "lyricnetworkcontroller.h"
#include <QUdpSocket>
#include "../debug.h"

LyricNetworkController::LyricNetworkController(QHostAddress _host, u_int16_t _port, int interval, int positionUpdateInterval)
    : QObject(),
    host(_host),
    port(_port),
    controller(new LyricController(interval, positionUpdateInterval))
{
    connect(this->controller, &LyricController::lyricChanged, this, &LyricNetworkController::lyricChanged);
    connect(this->controller, &LyricController::play, this, &LyricNetworkController::play);
    connect(this->controller, &LyricController::pause, this, &LyricNetworkController::pause);
}

LyricNetworkController::~LyricNetworkController()
{
    disconnect(this->controller);
    delete this->controller;
}

void LyricNetworkController::lyricChanged(const Lyric::LyricInfo lyricInfo)
{
    DEBUG("current lyric: " << (lyricInfo.lyric ? QString::fromStdString(*lyricInfo.lyric) : "NULL"));
    if (lyricInfo.lyric)
    {
        std::string lyric = std::string(*lyricInfo.lyric);
        int left = lyric.find_last_of("["), right = lyric.find_last_of("]");
        QString newLyric = "%1\n%2";

        if (std::string::npos != right && std::string::npos != left)
        {
            newLyric = newLyric.arg(QString::fromStdString(lyric.substr(0, left)))
                                .arg(QString::fromStdString(lyric.substr(left + 1, right - left - 1)));
        }
        else
        {
            newLyric = QString::fromStdString(lyric);
        }


        QUdpSocket().writeDatagram(
            QString("LYRIC %1 %2").arg(lyricInfo.time_to_next)
                                  .arg(newLyric).toUtf8(),
            host, port
        );
    }
}

void LyricNetworkController::pause()
{
    DEBUG("paused");
    QUdpSocket().writeDatagram(QString("LYRIC_CONFIG_STOP_TICK").toUtf8(), host, port);
}

void LyricNetworkController::play()
{
    DEBUG("playing");
    QUdpSocket().writeDatagram(QString("LYRIC_CONFIG_START_TICK").toUtf8(), host, port);
}

