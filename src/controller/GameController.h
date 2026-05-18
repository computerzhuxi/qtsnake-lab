#ifndef SNAKE_CONTROLLER_GAMECONTROLLER_H
#define SNAKE_CONTROLLER_GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <memory>
#include "GameState.h"

class GameScene;
class InputComponent;
class MoveComponent;
class CollisionComponent;
class FoodComponent;
class RenderComponent;

/// \brief 游戏控制器（循环驱动器）
/// \details 持有 QTimer + 5 个游戏组件。
///          状态机：Idle → Ready → Countdown → Playing → GameOver / Paused。
///          startGame() 创建 GameState 并初始化组件，
///          update() 每帧按序调用各组件（输入→移动→碰撞→食物→渲染）。
///          pause()/resume() 控制 QTimer 启停。
///          不包含游戏规则细节——规则由各组件独立处理。

class GameController : public QObject {
    Q_OBJECT
public:
    enum class State { Idle, Ready, Countdown, Playing, Paused, GameOver };

    explicit GameController(GameScene* scene, QObject* parent = nullptr);

    /// \brief 开始新一局：创建 GameState + 随机出生蛇 + 初始化组件
    void startGame(int boardW = 20, int boardH = 20, int speedMs = 100);

    /// \brief 暂停（停止 QTimer）
    void pause();

    /// \brief 恢复（重新启动 QTimer）
    void resume();

    /// \brief 重置：停止 timer + 清空 GameState → Idle
    void reset();

    /// \brief 处理准备键（Ready 状态 → 启动 3-2-1 倒计时）
    void handleReadyKey();

    State state() const { return m_phase; }
    const GameState& gameState() const { return m_state; }
    int score() const { return m_state.score; }

    /// \brief 输入组件指针（AppShell 用 setDirection() 转发方向键）
    InputComponent* input() const { return m_input.get(); }

signals:
    void stateChanged(State newState);
    void countdownTick(int number);
    void scoreChanged(int score);

private slots:
    /// \brief QTimer 回调：按序调用各组件 update()
    void update();

private:
    /// \brief 初始化需要首次调用的组件（MoveComponent + FoodComponent）
    void initComponents();

    QTimer* m_timer;                                ///< 主循环定时器

    std::unique_ptr<InputComponent> m_input;         ///< 键盘输入
    std::unique_ptr<MoveComponent> m_move;           ///< 蛇移动 + 空闲格
    std::unique_ptr<CollisionComponent> m_collision; ///< 碰撞检测
    std::unique_ptr<FoodComponent> m_food;           ///< 食物管理
    std::unique_ptr<RenderComponent> m_render;       ///< 渲染同步

    GameState m_state;                               ///< 当前游戏状态
    State m_phase = State::Idle;                     ///< 当前阶段
    int m_countdownValue = 3;                        ///< 倒计时计数
    int m_speedMs = 100;                             ///< tick 间隔
    int m_boardW = 20;                               ///< 棋盘宽度（重生用）
    int m_boardH = 20;                               ///< 棋盘高度（重生用）
};

#endif
