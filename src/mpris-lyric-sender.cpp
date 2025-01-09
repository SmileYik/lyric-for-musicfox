#include <cstdio>
#include <QCoreApplication>
#include "dbus-mode/lyricnetworkcontroller.h"

int main(int argc, char** argv) 
{
    if (argc != 3) 
    {
        printf("Usage: %s [host] [port]\n", argv[0]);
        return 0;
    }
    QHostAddress address = QHostAddress(QString::fromStdString(std::string(argv[1])));
    quint16 port = QString::fromStdString(std::string(argv[2])).toInt();

    QCoreApplication app(argc, argv);
    LyricNetworkController controller(address, port, 30, 6000);
    return app.exec();
}