#ifndef SNAKE_APP_LEADERBOARDWIDGET_H
#define SNAKE_APP_LEADERBOARDWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>
#include <vector>

class QLabel;

/// \brief 单局排行榜数据
struct PlayerInfo {
    QString name;
    int score = 0;
    bool alive = true;
    bool isSelf = false;
};

/// \brief 右侧排行榜组件
/// \details 显示 "— RANK —" 标题 + 动态玩家行 + 存活计数。

class LeaderboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit LeaderboardWidget(QWidget* parent = nullptr);

    /// \brief 更新排行榜数据（清除旧行，重建列表）
    void updatePlayers(const std::vector<PlayerInfo>& players);

private:
    QWidget* makePlayerRow(int rank, const PlayerInfo& info);
    void clearRows();

    QList<QWidget*> m_rows;
    QLabel* m_aliveLabel;
};

#endif // SNAKE_APP_LEADERBOARDWIDGET_H
