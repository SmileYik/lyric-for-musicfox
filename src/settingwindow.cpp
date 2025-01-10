#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QColorDialog>
#include <QMessageBox>
#include <QUdpSocket>
#include <QPlainTextEdit>
#include <QThread>
#include "debug.h"
#include "config.h"

SettingWindow::SettingWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    ui->plainTextEditPreviewText->textChanged();
    connect(ui->plainTextEditPreviewText, &QPlainTextEdit::textChanged, this, &SettingWindow::previewLabelText);
#   ifndef LINUX
    ui->checkBoxEnableMpris->setEnabled(false);
#   endif

    initialSetting();
    applySettingToForm();
    previewLabel();
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

void SettingWindow::previewLabel()
{
    QFont font = buildFondFromSetting();
    ui->labelPreview->setFont(font);
    ui->labelPreview->setStyleSheet(QString("color: %1").arg(setting.get(KEY_FONT_COLOR, "#000000").c_str()));
}

QFont SettingWindow::buildFondFromSetting()
{
    QFont font(QString::fromStdString(setting.get(KEY_FONT_FAMILY)));
    font.setBold(setting.getBool(KEY_FONT_BOLD));
    font.setItalic(setting.getBool(KEY_FONT_ITALIC));
    font.setPointSize(QString::fromStdString(setting.get(KEY_FONT_SIZE)).toInt());
    return font;
}

void SettingWindow::initialSetting()
{

    if (setting.get(KEY_FONT_COLOR) == "")
    {
        setting.put(KEY_FONT_COLOR, "#000000");
    }
    if (setting.get(KEY_FONT_FAMILY) == "")
    {
        setting.put(KEY_FONT_FAMILY, ui->labelPreview->font().family().toStdString());
    }
    if (setting.get(KEY_FONT_SIZE) == "")
    {
        setting.put(KEY_FONT_SIZE, QString("%1").arg(ui->labelPreview->font().pointSize()).toStdString());
    }
#   ifdef LINUX
    if (!setting.has(KEY_ENABLE_MPRIS))
    {
        setting.putBool(KEY_ENABLE_MPRIS, true);
        ui->checkBoxEnableMpris->setChecked(true);
    }
#   endif
}

void SettingWindow::applySettingToForm()
{
    DEBUG(QString::fromStdString(setting.get(KEY_FONT_COLOR)));
    ui->labelColorPreview->setText(QString::fromStdString(setting.get(KEY_FONT_COLOR)));
    ui->labelColorPreview->setStyleSheet(QString("background-color: %1").arg(setting.get(KEY_FONT_COLOR).c_str()));
    if (setting.has(KEY_FONT_OUTLINE_COLOR))
    {
        ui->labelOutlineColorPreview->setText(QString::fromStdString(setting.get(KEY_FONT_OUTLINE_COLOR)));
        ui->labelOutlineColorPreview->setStyleSheet(QString("background-color: %1").arg(setting.get(KEY_FONT_OUTLINE_COLOR).c_str()));
    } else {
        ui->labelOutlineColorPreview->setText("no outline");
    }

    ui->fontComboBox->setCurrentFont(buildFondFromSetting());
    ui->lineEditSize->setText(QString::fromStdString(setting.get(KEY_FONT_SIZE)));
    ui->checkBoxBold->setChecked(setting.getBool(KEY_FONT_BOLD));
    ui->checkBoxItalic->setChecked(setting.getBool(KEY_FONT_ITALIC));
    ui->checkBoxFrameLess->setChecked(setting.getBool(KEY_FRAME_LESS));
    ui->checkBoxStayOnTop->setChecked(setting.getBool(KEY_FLAGS_STAY_ON_TOP));
    ui->checkBoxEnableMpris->setChecked(setting.getBool(KEY_ENABLE_MPRIS));

    port = setting.has(KEY_RECEIVE_PORT) ? QString::fromStdString(setting.get(KEY_RECEIVE_PORT)).toInt() : PORT;
}

void SettingWindow::on_pushButtonChooseColor_clicked()
{
    QColor color = QColorDialog::getColor(QColor(QString::fromStdString(setting.get(KEY_FONT_COLOR))), this);
    setting.put(KEY_FONT_COLOR, color.name().toStdString());
    applySettingToForm();
    previewLabel();
}

void SettingWindow::on_pushButtonChooseOutlineColor_clicked()
{
    QColor color = setting.has(KEY_FONT_OUTLINE_COLOR) ? QColorDialog::getColor(QColor(QString::fromStdString(setting.get(KEY_FONT_OUTLINE_COLOR))), this) : QColor(Qt::transparent);
    setting.put(KEY_FONT_OUTLINE_COLOR, color.name().toStdString());
    applySettingToForm();
    previewLabel();
}

void SettingWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    setting.put(KEY_FONT_FAMILY, f.family().toStdString());
    previewLabel();
}


void SettingWindow::on_checkBoxBold_clicked(bool checked)
{
    setting.putBool(KEY_FONT_BOLD, checked);
    previewLabel();
}


void SettingWindow::on_checkBoxItalic_clicked(bool checked)
{
    setting.putBool(KEY_FONT_ITALIC, checked);
    previewLabel();
}

void SettingWindow::on_checkBoxFrameLess_clicked(bool checked)
{
    // setting.putBool(KEY_FRAME_LESS, checked);
    QUdpSocket s;
    s.writeDatagram(QString("LYRIC_CONFIG_FRAME_LESS %1").arg(checked ? 1 : 0).toUtf8(), QHostAddress::LocalHost, port);
}

void SettingWindow::on_checkBoxStayOnTop_clicked(bool checked)
{
    // setting.putBool(KEY_FLAGS_STAY_ON_TOP, checked);
    QUdpSocket s;
    s.writeDatagram(QString("LYRIC_CONFIG_STAY_ON_TOP %1").arg(checked ? 1 : 0).toUtf8(), QHostAddress::LocalHost, port);
}

void SettingWindow::on_checkBoxEnableMpris_clicked(bool checked)
{
    setting.putBool(KEY_ENABLE_MPRIS, checked);
    previewLabel();
}

void SettingWindow::on_lineEditSize_editingFinished()
{
    setting.put(KEY_FONT_SIZE, ui->lineEditSize->text().toStdString());
    previewLabel();
}

void SettingWindow::previewLabelText()
{
    ui->labelPreview->setText(ui->plainTextEditPreviewText->toPlainText());
}

void SettingWindow::on_pushButtonPreview_clicked()
{
    previewLabel();
    QUdpSocket s;
    s.writeDatagram(
        QString("LYRIC_CONFIG_PREVIEW ")
            .append(ui->plainTextEditPreviewText->toPlainText()).toUtf8(),
        QHostAddress::LocalHost, PORT
    );
}

void SettingWindow::on_pushButtonApply_clicked()
{
    setting.putBool(KEY_FRAME_LESS, ui->checkBoxFrameLess->isChecked());
    setting.putBool(KEY_FLAGS_STAY_ON_TOP, ui->checkBoxStayOnTop->isChecked());
    setting.store();
    QMessageBox::information(this, "Information", "If you want configurate in one day, "
                                                  "you can use command 'lyric-for-musicfox setting'");
    this->close();
    application->exit(0);
}

void SettingWindow::on_pushButtonReload_clicked()
{
    setting.putBool(KEY_FRAME_LESS, ui->checkBoxFrameLess->isChecked());
    setting.putBool(KEY_FLAGS_STAY_ON_TOP, ui->checkBoxStayOnTop->isChecked());
    setting.store();

    QUdpSocket s;
    s.writeDatagram(QString("LYRIC_CONFIG_RELOAD").toUtf8(), QHostAddress::LocalHost, port);
}

void SettingWindow::on_pushButtonSaveSelf_clicked()
{
    QUdpSocket s;
    s.writeDatagram(QString("LYRIC_CONFIG_SAVE_SELF").toUtf8(), QHostAddress::LocalHost, port);
    QThread::msleep(1000);
    this->setting = Setting();
    applySettingToForm();
}

void SettingWindow::closeEvent(QCloseEvent* event)
{
    application->exit(1);
}
