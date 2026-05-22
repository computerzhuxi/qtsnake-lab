#ifndef SNAKE_APP_GAMEOVERWIDGET_H
#define SNAKE_APP_GAMEOVERWIDGET_H

#include <QWidget>
#include <QString>
#include <vector>

/// \brief 结算页单条玩家结果
/// \details 与 LeaderboardWidget.h 的 PlayerInfo 独立，不互相引用。

struct GameResult {
    QString name;
    int score = 0;
    int length = 0;
    int kills = 0;
    bool alive = true;
    bool isSelf = false;
};

class QLabel;
class QVBoxLayout;

/// \brief 结算浮层
/// \details 半透明遮罩 + 居中面板。单人模式显示大分数+统计行，
///          多人模式显示排名表。panel 内按钮填充面板宽度。

class GameOverWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameOverWidget(QWidget* parent = nullptr);

    /// \brief 单人结算：大分数 + 统计行
    void setSingleResult(int score, int length, int kills, int seconds);

    /// \brief 多人结算：排名表
    void setMultiResults(const std::vector<GameResult>& results);

signals:
    void playAgainClicked();
    void saveReplayClicked();
    void menuClicked();

private:
    void clearContent();
    void buildSingleContent(int score, int length, int kills, int seconds);
    void buildMultiContent(const std::vector<GameResult>& results);

    QWidget* m_panel;
    QVBoxLayout* m_contentLayout;
};

#endif // SNAKE_APP_GAMEOVERWIDGET_H
