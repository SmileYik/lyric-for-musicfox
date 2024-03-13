#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "setting.h"

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
    QString currentLyric;
    Setting setting;
private:
    void closeEvent(QCloseEvent* event) override;
private slots:
    void receiveLyric();
};
#endif // MAINWINDOW_H
