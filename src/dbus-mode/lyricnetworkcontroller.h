#ifndef LYRIC_NETWORK_CONTROLLER_H
#define LYRIC_NETWORK_CONTROLLER_H

#include <QObject>
#include "lyriccontroller.h"

class LyricNetworkController : public QObject
{
public:
    LyricNetworkController(int interval = 10, int positionUpdateInterval = 5000);
    ~LyricNetworkController();

private slots:
    void play();
    void pause();
    void lyricChanged(const Lyric::LyricInfo lyricInfo);

private:
    LyricController* controller;
};

#endif // LYRIC_NETWORK_CONTROLLER_H
