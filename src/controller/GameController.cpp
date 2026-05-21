#include "GameController.h"
#include "InputComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "FoodSpawner.h"
#include "RenderComponent.h"
#include "RngService.h"
#include "GameScene.h"
#include "Logger.h"
#include <QDateTime>

GameController::GameController(GameScene* scene)
    : QObject(nullptr)
    , m_timer(new QTimer(this))
    , m_countdownTimer(new QTimer(this))
    , m_input(std::make_unique<InputComponent>())
    , m_move(std::make_unique<MoveComponent>())
    , m_collision(std::make_unique<CollisionComponent>())
    , m_rng(std::make_unique<RngService>())
    , m_food(std::make_unique<FoodSpawner>(m_rng.get()))
    , m_render(std::make_unique<RenderComponent>(scene))
{
    connect(m_timer, &QTimer::timeout, this, &GameController::update);
}

GameController::~GameController() = default;

void GameController::startGame(int boardW, int boardH, int speedMs) {
    m_boardW = boardW;
    m_boardH = boardH;
    m_speedMs = speedMs;
    m_timer->setInterval(speedMs);

    m_state = GameState{};
    m_state.board.setSize(boardW, boardH);

    m_kills = 0;
    m_gameStartMs = 0;
    m_elapsedSec = 0;

    Point spawn(m_rng->intInRange(0, boardW - 1), m_rng->intInRange(0, boardH - 1));

    struct { Direction d; int ox; int oy; } checks[] = {
        {Direction::Right, -1, 0}, {Direction::Left, 1, 0},
        {Direction::Down, 0, -1}, {Direction::Up, 0, 1}
    };
    Direction safe[4]; int cnt = 0;
    for (auto& c : checks) {
        if (spawn.x + c.ox * 2 >= 0 && spawn.x + c.ox * 2 < boardW &&
            spawn.y + c.oy * 2 >= 0 && spawn.y + c.oy * 2 < boardH)
            safe[cnt++] = c.d;
    }
    Direction dir = cnt > 0 ? safe[m_rng->intInRange(0, cnt - 1)] : Direction::Right;

    m_state.snakes.emplace_back(spawn, dir);
    m_state.board.initFromSnakes(m_state.snakes);

    initComponents();
    m_phase = State::Ready;
    emit stateChanged(m_phase);
    emit scoreChanged(m_state.score);

    m_render->update(m_state);

    LOG_INFO("GameController", "State: Idle -> Ready");
}

void GameController::initComponents() {
    m_input->init(m_state);
    m_food->init(m_state);
}

void GameController::setSeed(unsigned s) {
    if (m_phase != State::Idle) {
        LOG_WARN("GameController", "setSeed ignored: not in Idle");
        return;
    }
    m_rng->seed(s);
    LOG_INFO("GameController", "setSeed: " + std::to_string(s));
}

void GameController::handleReadyKey() {
    if (m_phase != State::Ready) return;

    LOG_INFO("GameController", "State: Ready -> Countdown");
    m_phase = State::Countdown;
    m_countdownValue = 3;
    emit stateChanged(m_phase);
    emit countdownTick(m_countdownValue);

    m_countdownTimer->stop();
    disconnect(m_countdownTimer, &QTimer::timeout, nullptr, nullptr);
    m_countdownTimer->setInterval(700);
    connect(m_countdownTimer, &QTimer::timeout, this, [this]() {
        m_countdownValue--;
        if (m_countdownValue > 0) {
            emit countdownTick(m_countdownValue);
        } else {
            emit countdownTick(0);
            m_countdownTimer->stop();
            m_phase = State::Playing;
            emit stateChanged(m_phase);
            m_timer->start();
            LOG_INFO("GameController", "State: Countdown -> Playing");
        }
    });
    m_countdownTimer->start();
}

void GameController::pause() {
    if (m_phase != State::Playing) return;
    LOG_INFO("GameController", "State: Playing -> Paused");
    m_timer->stop();
    m_phase = State::Paused;
    emit stateChanged(m_phase);
}

void GameController::resume() {
    if (m_phase != State::Paused) return;
    LOG_INFO("GameController", "State: Paused -> Playing");
    m_phase = State::Playing;
    emit stateChanged(m_phase);
    m_timer->start();
}

void GameController::reset() {
    LOG_INFO("GameController", "State: -> Idle (reset)");
    m_timer->stop();
    m_countdownTimer->stop();
    m_state = GameState{};
    m_phase = State::Idle;
    emit stateChanged(m_phase);
}

void GameController::update() {
    if (m_phase != State::Playing) return;

    m_render->update(m_state);
    m_input->update(m_state);
    m_move->update(m_state);

    auto reports = m_collision->detect(m_state);
    processCollisions(reports);

    if (m_state.food.isEaten())
        m_food->spawn(m_state);

    // 每 tick 更新耗时
    if (m_gameStartMs == 0) {
        m_gameStartMs = QDateTime::currentMSecsSinceEpoch();
    }
    m_elapsedSec = static_cast<int>(
        (QDateTime::currentMSecsSinceEpoch() - m_gameStartMs) / 1000);

    // Emit stats for UI
    int len = m_state.snakes.empty() ? 0
              : static_cast<int>(m_state.snakes[0].body().size());
    emit statsUpdated(m_state.score, len, m_elapsedSec,
                      m_speedMs, m_kills, 1, 1);

    if (m_state.gameOver) {
        m_timer->stop();
        m_phase = State::GameOver;
        emit stateChanged(m_phase);
        emit scoreChanged(m_state.score);
        LOG_INFO("GameController", "State: Playing -> GameOver, score=" + std::to_string(m_state.score));
    }
}

void GameController::processCollisions(const std::vector<CollisionReport>& reports) {
    std::vector<bool> alive(m_state.snakes.size(), true);
    bool anyoneAte = false;

    for (auto& r : reports) {
        auto& snake = m_state.snakes[r.snakeIndex];

        switch (r.type) {
        case CollisionType::Wall:
        case CollisionType::SelfBody:
        case CollisionType::OtherBody:
        case CollisionType::Obstacle:
            alive[r.snakeIndex] = false;
            break;
        case CollisionType::HeadToHead:
            alive[r.snakeIndex] = false;
            if (r.otherSnakeIndex >= 0)
                alive[r.otherSnakeIndex] = false;
            break;
        case CollisionType::Food:
            if (alive[r.snakeIndex]) {
                snake.grow();
                m_state.score += m_state.food.points();
                anyoneAte = true;
            }
            break;
        }
    }

    bool allDead = true;
    for (bool a : alive)
        if (a) { allDead = false; break; }
    if (allDead) m_state.gameOver = true;

    if (anyoneAte)
        m_state.food.markEaten();

    // 统计击杀：OtherBody 中 otherSnakeIndex 是身体拥有者（击杀方）
    for (auto& r : reports) {
        if (r.type == CollisionType::OtherBody && r.otherSnakeIndex == 0)
            m_kills++;
    }
}
