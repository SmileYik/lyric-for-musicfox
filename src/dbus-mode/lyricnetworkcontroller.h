#ifndef LYRIC_NETWORK_CONTROLLER_H
#define LYRIC_NETWORK_CONTROLLER_H

#include <QObject>
#include <QHostAddress>
#include <qhostaddress.h>
#include <sys/types.h>
#include "lyriccontroller.h"
#include "../config.h"

/**
 * @brief 歌词网络控制器，用于歌词更新时，向外部发送单句歌词信息
 * 
 */
class LyricNetworkController : public QObject
{
public:
    LyricNetworkController(QHostAddress host = QHostAddress::LocalHost, u_int16_t port = PORT, int interval = 10, int positionUpdateInterval = 5000);
    ~LyricNetworkController();

private slots:
    void play();
    void pause();
    void lyricChanged(const Lyric::LyricInfo lyricInfo);

private:
    QHostAddress host;
    u_int16_t port;
    LyricController* controller;
};

#endif // LYRIC_NETWORK_CONTROLLER_H
