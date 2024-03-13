#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QColorDialog>
#include <QMessageBox>
#include "debug.h"

SettingWindow::SettingWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
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
    if (VALUE_CENTER == setting.get(KEY_POS_H))
    {
        ui->labelPreview->setAlignment(Qt::AlignmentFlag::AlignHCenter);
    } else if (VALUE_LEFT == setting.get(KEY_POS_H))
    {
        ui->labelPreview->setAlignment(Qt::AlignmentFlag::AlignHorizontal_Mask | Qt::AlignLeft);
    } else
    {
        ui->labelPreview->setAlignment(Qt::AlignmentFlag::AlignHorizontal_Mask | Qt::AlignRight);
    }
    if (VALUE_CENTER == setting.get(KEY_POS_V))
    {
        ui->labelPreview->setAlignment(Qt::AlignmentFlag::AlignVCenter);
    } else if (VALUE_LEFT == setting.get(KEY_POS_V))
    {
        ui->labelPreview->setAlignment(Qt::AlignmentFlag::AlignVertical_Mask | Qt::AlignTop);
    } else
    {
        ui->labelPreview->setAlignment(Qt::AlignmentFlag::AlignVertical_Mask | Qt::AlignBottom);
    }
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
    if (setting.get(KEY_POS_H ) == "")
    {
        setting.put(KEY_POS_H, VALUE_CENTER);
    }
    if (setting.get(KEY_POS_V) == "")
    {
        setting.put(KEY_POS_V, VALUE_CENTER);
    }
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
}

void SettingWindow::applySettingToForm()
{
    // apply setting to form.
    if (VALUE_CENTER == setting.get(KEY_POS_H))
    {
        ui->radioButtonHCenter->setChecked(true);
    } else if (VALUE_LEFT == setting.get(KEY_POS_H))
    {
        ui->radioButtonHLeft->setChecked(true);
    } else
    {
        ui->radioButtonHRight->setChecked(true);
    }

    if (VALUE_CENTER == setting.get(KEY_POS_V))
    {
        ui->radioButtonVCenter->setChecked(true);
    } else if (VALUE_LEFT == setting.get(KEY_POS_V))
    {
        ui->radioButtonVLeft->setChecked(true);
    } else
    {
        ui->radioButtonVRight->setChecked(true);
    }

    DEBUG << QString::fromStdString(setting.get(KEY_FONT_COLOR));
    ui->labelColorPreview->setText(QString::fromStdString(setting.get(KEY_FONT_COLOR)));
    ui->labelColorPreview->setStyleSheet(QString("background-color: %1").arg(setting.get(KEY_FONT_COLOR).c_str()));

    ui->fontComboBox->setCurrentFont(buildFondFromSetting());
    ui->lineEditSize->setText(QString::fromStdString(setting.get(KEY_FONT_SIZE)));
    ui->checkBoxBold->setChecked(setting.getBool(KEY_FONT_BOLD));
    ui->checkBoxItalic->setChecked(setting.getBool(KEY_FONT_ITALIC));
}

void SettingWindow::on_pushButtonChooseColor_clicked()
{
    QColor color = QColorDialog::getColor(QColor::fromString(setting.get(KEY_FONT_COLOR)), this);
    setting.put(KEY_FONT_COLOR, color.name().toStdString());
    applySettingToForm();
    previewLabel();
}


void SettingWindow::on_radioButtonHLeft_clicked(bool checked)
{
    if (checked)
    {
        setting.put(KEY_POS_H, VALUE_LEFT);
        previewLabel();
    }
}


void SettingWindow::on_radioButtonHCenter_clicked(bool checked)
{
    if (checked)
    {
        setting.put(KEY_POS_H, VALUE_CENTER);
        previewLabel();
    }
}


void SettingWindow::on_radioButtonHRight_clicked(bool checked)
{
    if (checked)
    {
        setting.put(KEY_POS_H, VALUE_RIGHT);
        previewLabel();
    }
}

void SettingWindow::on_radioButtonVLeft_clicked(bool checked)
{
    if (checked)
    {
        setting.put(KEY_POS_V, VALUE_LEFT);
        previewLabel();
    }
}


void SettingWindow::on_radioButtonVCenter_clicked(bool checked)
{
    if (checked)
    {
        setting.put(KEY_POS_V, VALUE_CENTER);
        previewLabel();
    }
}


void SettingWindow::on_radioButtonVRight_clicked(bool checked)
{
    if (checked)
    {
        setting.put(KEY_POS_V, VALUE_RIGHT);
        previewLabel();
    }
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


void SettingWindow::on_lineEditSize_editingFinished()
{
    setting.put(KEY_FONT_SIZE, ui->lineEditSize->text().toStdString());
    previewLabel();
}


void SettingWindow::on_pushButtonPreview_clicked()
{
    previewLabel();
}


void SettingWindow::on_pushButtonApply_clicked()
{
    setting.store();
    QMessageBox::information(this, "Information", "If you want configurate in one day, "
                                                  "you can use command 'netease-lyric setting'");
    this->close();
}
