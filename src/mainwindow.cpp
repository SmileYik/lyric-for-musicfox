#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "debug.h"

MainWindow::MainWindow(QApplication* app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , application(app)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setWindowFlag(Qt::Tool);
    setWindowFlag(Qt::FramelessWindowHint);

    QFont font(QString::fromStdString(setting.get(KEY_FONT_FAMILY)));
    font.setBold(setting.getBool(KEY_FONT_BOLD));
    font.setItalic(setting.getBool(KEY_FONT_ITALIC));
    font.setPointSize(QString::fromStdString(setting.get(KEY_FONT_SIZE)).toInt());

    ui->labelLyric->setFont(font);
    ui->labelLyric->setStyleSheet(QString("color: %1").arg(setting.get(KEY_FONT_COLOR, "#000000").c_str()));

    server = new QUdpSocket(this);
    connect(server, SIGNAL(readyRead()), this, SLOT(receiveLyric()));
    server->bind(QHostAddress::LocalHost, 16501);
}

MainWindow::~MainWindow()
{
    delete ui;
    server->close();
    delete server;
}

void MainWindow::receiveLyric()
{
    QByteArray buffer;
    buffer.resize(server->pendingDatagramSize());
    server->readDatagram(buffer.data(), buffer.size());
    currentLyric = QString::fromStdString(buffer.toStdString());
    DEBUG << currentLyric;
    if (currentLyric != ui->labelLyric->text())
    {
        ui->labelLyric->setText(currentLyric);
    }
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    DEBUG << "Closing";
    application->exit(0);
}
