#ifndef SNAKE_APP_SETTINGSDIALOG_H
#define SNAKE_APP_SETTINGSDIALOG_H

#include <QDialog>

/// \brief 设置更改确认弹窗
/// \details TRON 暗色风格：绿色边框 + "!" 图标 + 标题 + 描述 + 两个按钮。
///          用 setWindowFlags(FramelessWindowHint | Dialog) + setModal(true)。

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

signals:
    void restartClicked();
    void laterClicked();
};

#endif // SNAKE_APP_SETTINGSDIALOG_H
