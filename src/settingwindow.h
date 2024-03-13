#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QMainWindow>
#include "setting.h"
#include <QApplication>

namespace Ui {
class SettingWindow;
}

class SettingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

private slots:
    void on_pushButtonChooseColor_clicked();

    void on_radioButtonHLeft_clicked(bool checked);

    void on_radioButtonHCenter_clicked(bool checked);

    void on_radioButtonHRight_clicked(bool checked);

    void on_radioButtonVLeft_clicked(bool checked);

    void on_radioButtonVCenter_clicked(bool checked);

    void on_radioButtonVRight_clicked(bool checked);

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_checkBoxBold_clicked(bool checked);

    void on_checkBoxItalic_clicked(bool checked);

    void on_lineEditSize_editingFinished();

    void on_pushButtonPreview_clicked();

    void on_pushButtonApply_clicked();

private:
    Ui::SettingWindow *ui;
    QApplication* application;
    Setting setting;
    void previewLabel();
    QFont buildFondFromSetting();
    void initialSetting();
    void applySettingToForm();
};

#endif // SETTINGWINDOW_H
