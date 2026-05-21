#ifndef SNAKE_APP_INFOBAR_H
#define SNAKE_APP_INFOBAR_H

#include <QWidget>

class QLabel;

/// \brief 游戏顶部信息栏（HUD）
/// \details 六列数据：Score / Length / Time / Speed / Kills / Rank，
///          由 Separator 分隔。setMode(false) 隐藏 Kills 和 Rank 列。

class InfoBar : public QWidget {
    Q_OBJECT
public:
    explicit InfoBar(QWidget* parent = nullptr);

    void setScore(int score);
    void setLength(int length);
    void setTime(int seconds);
    void setSpeed(int speedMs);
    void setKills(int kills);
    void setRank(int rank, int total);

    /// \brief 设置模式：true=多人（显示全部），false=单人（隐藏 Kills + Rank）
    void setMode(bool multiplayer);

private:
    QLabel* m_scoreValue;
    QLabel* m_lengthValue;
    QLabel* m_timeValue;
    QLabel* m_speedValue;
    QLabel* m_killsLabel;
    QLabel* m_killsValue;
    QWidget* m_sepBeforeKills;
    QLabel* m_rankLabel;
    QLabel* m_rankValue;
    QWidget* m_sepBeforeRank;
};

#endif // SNAKE_APP_INFOBAR_H
