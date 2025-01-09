#include "mainwindow.h"
#include <QApplication>
#include "settingwindow.h"
#include "setting.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Setting setting;

    if (setting.isEmpty() || argc == 2 && strcmp(argv[1], "setting") == 0)
    {
        SettingWindow w;
        w.show();
        if (a.exec() == 1) return 0;
    }
    MainWindow w(&a);
    w.show();
    return a.exec();
}
