#ifndef SNAKE_APP_SETTINGSWIDGET_H
#define SNAKE_APP_SETTINGSWIDGET_H

#include <QWidget>

/// \brief 设置浮层：操作标签（键位/语言）+ 游戏标签（速度/大小/音量）
class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget* parent = nullptr);

signals:
    void backClicked();
};

#endif // SNAKE_APP_SETTINGSWIDGET_H
