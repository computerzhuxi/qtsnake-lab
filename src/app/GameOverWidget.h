#ifndef SNAKE_APP_GAMEOVERWIDGET_H
#define SNAKE_APP_GAMEOVERWIDGET_H

#include <QWidget>

/// \brief 结算浮层
/// \details 半透明遮罩 + 居中弹窗。显示最终分数/长度/击杀/时间，
///          三个按钮：再来一局/保存回放（阶段 2 启用）/返回主菜单。

class GameOverWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameOverWidget(QWidget* parent = nullptr);

    /// \brief 设置显示的分数和统计信息
    void setScore(int score, int length, int kills, int seconds);

signals:
    void playAgainClicked();
    void saveReplayClicked();
    void menuClicked();
};

#endif // SNAKE_APP_GAMEOVERWIDGET_H
