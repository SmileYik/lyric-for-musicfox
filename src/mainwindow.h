#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "setting.h"
#include "lyricwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication* app, QWidget *parent = nullptr);
    ~MainWindow();

private:
    QUdpSocket* server;
    Ui::MainWindow *ui;
    QApplication* application;
    LyricWidget* lyric;
    QString currentLyric;
    QFont defaultFont;
    qint64 index;
private:
    void closeEvent(QCloseEvent* event) override;

signals:
    void needReloadSetting();
    void reciveCommand(QString command);

private slots:
    void receiveLyric();
    void reloadSetting();
    void handleCommand(QString command);
};
#endif // MAINWINDOW_H
