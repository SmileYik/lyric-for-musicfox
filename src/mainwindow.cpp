#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFontMetrics>
#include <QHostAddress>
#include <qnamespace.h>
#include "debug.h"
#include "config.h"

#ifdef LINUX
#include "dbus-mode/lyricnetworkcontroller.h"
#endif

MainWindow::MainWindow(QApplication* app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , application(app)
    , lyric(new LyricWidget(this))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(lyric);

    setAttribute(Qt::WA_TranslucentBackground);
    // 在 Windows 下, 一开始设置鼠标穿透属性会直接导致后续无法移动窗口
#ifdef LINUX
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool | Qt::WindowTransparentForInput);
#else
    connect(this->stayOnTopTimer, &QTimer::timeout, this, &MainWindow::handleRefreshStayOnTopFlag);
#endif

    connect(this, &MainWindow::needReloadSetting, this, &MainWindow::reloadSetting);
    connect(this, &MainWindow::reciveCommand, this, &MainWindow::handleCommand);

    emit needReloadSetting();
}

MainWindow::~MainWindow()
{
    delete ui;

    if (server) {
        server->close();
        delete server;
    }

    delete lyric;
#   ifdef LINUX
    if (nullptr != this->mprisLyricController)
    {
        disconnect(this->mprisLyricController);
        this->mprisLyricController->deleteLater();
    }
#   endif
}

void MainWindow::reloadSetting()
{
    DEBUG("reload setting");
    Setting setting;

    int w = QString::fromStdString(setting.get(KEY_WIDTH, "800")).toInt();
    int h = QString::fromStdString(setting.get(KEY_HEIGHT, "80")).toInt();

    setBaseSize(w, h);
    if (setting.has(KEY_POS_X))
    {
        setGeometry(
            QString::fromStdString(setting.get(KEY_POS_X)).toInt(),
            QString::fromStdString(setting.get(KEY_POS_Y)).toInt(),
            w, h
        );
    }

    QFont font(QString::fromStdString(setting.get(KEY_FONT_FAMILY)));
    font.setBold(setting.getBool(KEY_FONT_BOLD));
    font.setItalic(setting.getBool(KEY_FONT_ITALIC));
    font.setPointSize(QString::fromStdString(setting.get(KEY_FONT_SIZE)).toInt());
    // setGeometry(0, 0, width(), QFontMetrics(font).height() * 2 + 10);
    defaultFont = QFont(font);

    lyric->setFont(defaultFont);
    lyric->setColor(QString::fromStdString(setting.get(KEY_FONT_COLOR)));
    lyric->setSpeed(11);
    lyric->setBaseSize(width(), height());
    if (setting.has(KEY_FONT_OUTLINE_COLOR))
    {
        lyric->setOutlineColor(QString::fromStdString(setting.get(KEY_FONT_OUTLINE_COLOR)));
    }
    if (setting.has(KEY_FONT_OUTLINE_WIDTH))
    {
        lyric->setOutlineWidth(QString::fromStdString(setting.get(KEY_FONT_OUTLINE_WIDTH)).toInt());
    }

    setWindowFlag(Qt::WindowStaysOnTopHint, setting.getBool(KEY_FLAGS_STAY_ON_TOP));
    setWindowFlag(Qt::FramelessWindowHint, setting.getBool(KEY_FRAME_LESS));
    setWindowFlag(Qt::WindowTransparentForInput, setting.getBool(KEY_FRAME_LESS));
    setAttribute(Qt::WA_TransparentForMouseEvents, setting.getBool(KEY_FRAME_LESS));

#   ifndef LINUX
    this->stayOnTop = setting.getBool(KEY_FLAGS_STAY_ON_TOP);
    if (this->stayOnTop)
    {
        this->stayOnTopTimer->start(1000);
    } else
    {
        this->stayOnTopTimer->stop();
    }
#   endif

    // 自动tick
    int autoTick = QString::fromStdString(setting.get(KEY_LYRIC_AUTO_TICK, "0")).toInt();
    quint16 port = setting.has(KEY_RECEIVE_PORT) ? QString::fromStdString(setting.get(KEY_RECEIVE_PORT)).toInt() : PORT;
#   ifdef LINUX
    if (setting.getBool(KEY_ENABLE_MPRIS))
    {
        if (nullptr == this->mprisLyricController)
        {
            this->mprisLyricController = new LyricNetworkController(QHostAddress::LocalHost, port, 30, 6000);
            this->lyric->enableAutoTick(autoTick > 0 ? autoTick : 10);
        }
    }
#  else
    if (autoTick > 0)
    {
        this->lyric->enableAutoTick(autoTick);
    }
    else {
        this->lyric->pauseAutoTick(true);
    }
#  endif

    // setup server
    if (nullptr != server) {
        disconnect(server, SIGNAL(readyRead()), this, SLOT(receiveLyric()));
        server->close();
        delete server;
        server = nullptr;
    }
    server = new QUdpSocket(this);
    if (!server->bind(QHostAddress::Any, port))
    {
        qInfo() << "已经有个实例正在运行了！";
        server->close();
        delete server;
        server = nullptr;
        this->application->exit(0);
        return;
    }
    DEBUG("Running at port" << PORT);
    connect(server, SIGNAL(readyRead()), this, SLOT(receiveLyric()));

    setVisible(true);
}

void MainWindow::handleCommand(QString command)
{
    if (command == "LYRIC_CONFIG_RELOAD")
    {
        emit needReloadSetting();
        return;
    }
    else if (command == "LYRIC_CONFIG_SAVE_SELF")
    {
        Setting s;
        s.put(KEY_FONT_SIZE, QString("%1").arg(defaultFont.pointSize()).toStdString());
        s.put(KEY_WIDTH, QString("%1").arg(width()).toStdString());
        s.put(KEY_HEIGHT, QString("%1").arg(height()).toStdString());
        s.put(KEY_POS_X, QString("%1").arg(x()).toStdString());
        s.put(KEY_POS_Y, QString("%1").arg(y()).toStdString());
        s.store();
        this->lyric->setText("Saved!");
        this->lyric->tick();
        return;
    }
    else if (command == "LYRIC_CONFIG_STOP_TICK")
    {
        this->lyric->pauseAutoTick(true);
        return;
    }
    else if (command == "LYRIC_CONFIG_START_TICK")
    {
        this->lyric->pauseAutoTick(false);
        return;
    }

    if (command.startsWith("LYRIC_CONFIG_PREVIEW")) {
        this->lyric->setText(command.remove(0, 21));
        this->lyric->tick();
        return;
    }

    QStringList commands = command.split(" ");
    if (commands.isEmpty()) return;

    if (commands[0] == "LYRIC_CONFIG_FRAME_LESS" && commands.size() == 2)
    {
        setWindowFlag(Qt::FramelessWindowHint, commands[1] == "1");
        setVisible(true);
        return;
    }
    else if (commands[0] == "LYRIC_CONFIG_STAY_ON_TOP" && commands.size() == 2)
    {
        setWindowFlag(Qt::WindowStaysOnTopHint, commands[1] == "1");
        setVisible(true);
        return;
    }
    else if (commands[0] == "LYRIC_ENABLE_AUTO_TICK")
    {
        this->lyric->enableAutoTick(commands[1].toInt());
    }
}

void MainWindow::handleRefreshStayOnTopFlag()
{
#   ifndef LINUX
    setWindowFlag(Qt::WindowStaysOnTopHint, !this->stayOnTop);
    setWindowFlag(Qt::WindowStaysOnTopHint, this->stayOnTop);
    setWindowFlag(Qt::Tool, true);
    setVisible(true);
#   endif
}

void MainWindow::receiveLyric()
{
    QByteArray buffer;
    buffer.resize(server->pendingDatagramSize());
    server->readDatagram(buffer.data(), buffer.size());
    QString incoming = QString::fromStdString(buffer.toStdString());

    // handle command
    if (incoming.startsWith("LYRIC_CONFIG"))
    {
        DEBUG("Recived config signal.");
        emit reciveCommand(incoming);
        return;
    }
    else if(incoming.startsWith("LYRIC"))
    {
        QStringList list = incoming.split(" ");
        int timeToNextLine = list[1].toInt();
        QString line = incoming.remove(0, 6 + list[1].size());
        this->lyric->setText(line, timeToNextLine);
        return;
    }

    if (nullptr != this->mprisLyricController) return;
    // set lyric
    if (incoming != currentLyric)
    {
        currentLyric = incoming;
        lyric->setText(currentLyric);
        index = 0;
    }
    lyric->tick();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    this->close();
    if (nullptr != this->mprisLyricController)
    {
        delete this->mprisLyricController;
        this->mprisLyricController = nullptr;
    }
    application->exit(0);
}
