#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "debug.h"
#include <QFontMetrics>

MainWindow::MainWindow(QApplication* app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , application(app)
    , lyric(new LyricWidget(this))
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
    setGeometry(0, 0, width(), QFontMetrics(font).height() * 2 + 10);
    defaultFont = QFont(font);

    lyric->setFont(defaultFont);
    lyric->setColor(QString::fromStdString(setting.get(KEY_FONT_COLOR)));
    lyric->setSpeed(11);
    lyric->setBaseSize(width(), height());

    // ui->labelLyric->setFont(font);
    // ui->labelLyric->setStyleSheet(QString("color: %1").arg(setting.get(KEY_FONT_COLOR, "#000000").c_str()));

    server = new QUdpSocket(this);
    connect(server, SIGNAL(readyRead()), this, SLOT(receiveLyric()));
    server->bind(QHostAddress::LocalHost, 16501);
    ui->verticalLayout->addWidget(lyric);
}

MainWindow::~MainWindow()
{
    delete ui;
    server->close();
    delete server;
    delete lyric;
}

void MainWindow::receiveLyric()
{

    QByteArray buffer;
    buffer.resize(server->pendingDatagramSize());
    server->readDatagram(buffer.data(), buffer.size());
    QString incoming = QString::fromStdString(buffer.toStdString());
    if (incoming != currentLyric)
    {
        currentLyric = incoming;
        lyric->setText(currentLyric);
        index = 0;
    }
    lyric->tick();
    // DEBUG << currentLyric;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    DEBUG << "Closing";
    application->exit(0);
}
