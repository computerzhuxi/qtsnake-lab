#include "GameController.h"
#include "GameScene.h"
#include "Logger.h"
#include <random>

GameController::GameController(GameScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &GameController::tick);
}

void GameController::startGame(int boardW, int boardH, int speedMs) {
    delete m_board;
    delete m_playerSnake;

    m_board = new Board(boardW, boardH);
    m_score = 0;
    m_timer->setInterval(speedMs);

    // 随机出生位置
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, boardW - 1);
    std::uniform_int_distribution<int> distY(0, boardH - 1);

    Point spawn(distX(gen), distY(gen));

    // 智能选方向：排除会让身体越界的方向
    Direction possible[4] = { Direction::Right, Direction::Left, Direction::Down, Direction::Up };
    Direction safeDirs[4];
    int safeCount = 0;

    for (auto dir : possible) {
        int seg2x = spawn.x, seg2y = spawn.y;
        int seg3x = spawn.x, seg3y = spawn.y;
        switch (dir) {
            case Direction::Right: seg2x--; seg3x -= 2; break;
            case Direction::Left:  seg2x++; seg3x += 2; break;
            case Direction::Down:  seg2y--; seg3y -= 2; break;
            case Direction::Up:    seg2y++; seg3y += 2; break;
        }
        if (seg3x >= 0 && seg3x < boardW && seg3y >= 0 && seg3y < boardH) {
            safeDirs[safeCount++] = dir;
        }
    }

    Direction initialDir = Direction::Right;
    if (safeCount > 0) {
        initialDir = safeDirs[gen() % safeCount];
    }

    m_playerSnake = new Snake(spawn, initialDir);
    m_board->spawnFood(m_playerSnake);
    m_state = State::Ready;

    emit stateChanged(m_state);
    emit scoreChanged(m_score);

    std::vector<Snake*> snakes = {m_playerSnake};
    m_scene->syncFromBoard(*m_board, snakes);

    LOG_INFO("GameController", "Game started at (" + std::to_string(spawn.x)
             + "," + std::to_string(spawn.y) + ") dir=" + std::to_string(static_cast<int>(initialDir)));
}

void GameController::handleReadyKey() {
    if (m_state != State::Ready) return;

    m_state = State::Countdown;
    m_countdownValue = 3;
    emit stateChanged(m_state);
    emit countdownTick(m_countdownValue);

    auto* countdownTimer = new QTimer(this);
    countdownTimer->setInterval(700);
    connect(countdownTimer, &QTimer::timeout, this, [this, countdownTimer]() {
        m_countdownValue--;
        if (m_countdownValue > 0) {
            emit countdownTick(m_countdownValue);
        } else {
            emit countdownTick(0);
            countdownTimer->stop();
            countdownTimer->deleteLater();
            m_state = State::Playing;
            emit stateChanged(m_state);
            m_timer->start();
            LOG_INFO("GameController", "Game playing");
        }
    });
    countdownTimer->start();
}

void GameController::pause() {
    if (m_state != State::Playing) return;
    m_timer->stop();
    m_state = State::Paused;
    emit stateChanged(m_state);
}

void GameController::resume() {
    if (m_state != State::Paused) return;
    m_state = State::Playing;
    emit stateChanged(m_state);
    m_timer->start();
}

void GameController::reset() {
    m_timer->stop();
    delete m_board;
    delete m_playerSnake;
    m_board = nullptr;
    m_playerSnake = nullptr;
    m_state = State::Idle;
    emit stateChanged(m_state);
}

void GameController::handleKeyPress(Direction dir) {
    if (m_state == State::Playing && m_playerSnake) {
        m_playerSnake->setDirection(dir);
    }
}

GameController::State GameController::state() const { return m_state; }
Board* GameController::board() { return m_board; }
Snake* GameController::playerSnake() { return m_playerSnake; }
int GameController::score() const { return m_score; }

void GameController::tick() {
    if (!m_board || !m_playerSnake) return;
    if (m_state != State::Playing) return;

    m_playerSnake->move();

    if (m_board->checkWallCollision(*m_playerSnake) ||
        m_playerSnake->checkSelfCollision()) {
        m_timer->stop();
        m_state = State::GameOver;
        emit stateChanged(m_state);
        LOG_INFO("GameController", "Game over, score=" + std::to_string(m_score));
        return;
    }

    if (m_board->checkFoodCollision(*m_playerSnake)) {
        m_playerSnake->grow();
        m_score += m_board->food().points;
        emit scoreChanged(m_score);
        m_board->spawnFood(m_playerSnake);
    }

    std::vector<Snake*> snakes = {m_playerSnake};
    m_scene->syncFromBoard(*m_board, snakes);
}
