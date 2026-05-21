#include "AppShell.h"
#include "MainMenuWidget.h"
#include "GamePage.h"
#include "SettingsWidget.h"
#include "InputComponent.h"
#include "Logger.h"
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QKeyEvent>

AppShell::AppShell(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Snake Arena");
    setMinimumSize(800, 600);

    setupUI();

    // 主菜单信号
    connect(m_mainMenu, &MainMenuWidget::singlePlayerClicked,
            this, &AppShell::startSinglePlayer);
    connect(m_mainMenu, &MainMenuWidget::settingsClicked,
            this, &AppShell::showSettings);
    connect(m_mainMenu, &MainMenuWidget::exitClicked,
            this, &QWidget::close);

    // 游戏页信号
    connect(m_gamePage, &GamePage::resumeClicked, this, [this]() {
        if (m_controller) m_controller->resume();
        m_gamePage->hideAllOverlays();
    });
    connect(m_gamePage, &GamePage::restartClicked, this, [this]() {
        m_gamePage->hideAllOverlays();
        startSinglePlayer();
    });
    connect(m_gamePage, &GamePage::menuClicked, this, [this]() {
        showMenu();
    });
    connect(m_gamePage, &GamePage::settingsClicked, this, &AppShell::showSettings);
    connect(m_gamePage, &GamePage::playAgainClicked, this, [this]() {
        m_gamePage->hideAllOverlays();
        startSinglePlayer();
    });

    // 设置页信号
    connect(m_settingsWidget, &SettingsWidget::backClicked, this, [this]() {
        m_settingsWidget->hide();
    });

    LOG_INFO("AppShell", "AppShell initialized");
    showMenu();
    setFocus();
}

void AppShell::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);
    m_stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_mainMenu = new MainMenuWidget(this);
    m_gamePage = new GamePage(this);

    m_stack->addWidget(m_mainMenu);  // page 0
    m_stack->addWidget(m_gamePage);  // page 1

    layout->addWidget(m_stack);

    m_settingsWidget = new SettingsWidget(this);
}

void AppShell::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_settingsWidget->setGeometry(0, 0, width(), height());
}

void AppShell::showMenu() {
    m_gamePage->exit();
    m_controller.reset();
    m_stack->setCurrentIndex(0);
    m_mainMenu->enter();
    m_settingsWidget->hide();
}

void AppShell::showGame() {
    m_mainMenu->exit();
    m_gamePage->enter();
    m_stack->setCurrentIndex(1);
    m_settingsWidget->hide();
}

void AppShell::showSettings() {
    m_settingsWidget->show();
    m_settingsWidget->raise();
}

void AppShell::startSinglePlayer() {
    showGame();

    m_controller = std::make_unique<GameController>(m_gamePage->scene());

    connect(m_controller.get(), &GameController::stateChanged,
            this, &AppShell::onControllerStateChanged);
    connect(m_controller.get(), &GameController::countdownTick,
            this, &AppShell::onCountdownTick);
    connect(m_controller.get(), &GameController::scoreChanged, this, [this](int score) {
        m_currentScore = score;
    });
    connect(m_controller.get(), &GameController::statsUpdated,
            this, &AppShell::onStatsUpdated);

    m_controller->startGame(20, 20, 100);
    m_gamePage->updateStats(0, 3, 0, m_controller->speedMs(), 0, 1, 1);
    m_gamePage->view()->fitInView(m_gamePage->scene()->sceneRect(), Qt::KeepAspectRatio);
    setFocus();
}

void AppShell::keyPressEvent(QKeyEvent* event) {
    if (!m_controller) {
        QWidget::keyPressEvent(event);
        return;
    }

    const auto state = m_controller->state();

    if (state == GameController::State::Ready) {
        m_controller->handleReadyKey();
        return;
    }

    if (state == GameController::State::Playing) {
        switch (event->key()) {
            case Qt::Key_Up:    m_controller->input()->setDirection(Direction::Up);    break;
            case Qt::Key_Down:  m_controller->input()->setDirection(Direction::Down);  break;
            case Qt::Key_Left:  m_controller->input()->setDirection(Direction::Left);  break;
            case Qt::Key_Right: m_controller->input()->setDirection(Direction::Right); break;
            case Qt::Key_Escape:
                m_controller->pause();
                m_gamePage->showPause();
                break;
        }
    }

    if (state == GameController::State::Paused) {
        if (event->key() == Qt::Key_Escape) {
            m_controller->resume();
            m_gamePage->hideAllOverlays();
        }
    }

    QWidget::keyPressEvent(event);
}

void AppShell::onControllerStateChanged(GameController::State state) {
    switch (state) {
        case GameController::State::Ready:
            m_gamePage->showCountdown(-1);
            break;
        case GameController::State::GameOver: {
            const auto& gs = m_controller->gameState();
            int len = gs.snakes.empty() ? 0 : static_cast<int>(gs.snakes[0].body().size());
            m_gamePage->showGameOver(m_currentScore, len,
                                     m_controller->kills(),
                                     m_controller->elapsedSec());
            break;
        }
        case GameController::State::Playing:
            m_gamePage->hideAllOverlays();
            break;
        default:
            break;
    }
}

void AppShell::onCountdownTick(int number) {
    m_gamePage->showCountdown(number);
}

void AppShell::onStatsUpdated(int score, int length, int timeSec,
                               int speedMs, int kills, int rank, int totalPlayers) {
    m_currentScore = score;
    m_gamePage->updateStats(score, length, timeSec, speedMs, kills, rank, totalPlayers);
}
