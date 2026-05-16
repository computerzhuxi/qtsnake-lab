#pragma once
#include <QWidget>

/// \brief 结算浮层：显示分数/统计，再来一局/保存回放/返回主菜单
class GameOverWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameOverWidget(QWidget* parent = nullptr);
    void setScore(int score, int length, int kills, int seconds);

signals:
    void playAgainClicked();
    void saveReplayClicked();
    void menuClicked();
};
