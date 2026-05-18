#include "AppShell.h"
#include "MainMenuWidget.h"
#include "GamePage.h"
#include "SettingsWidget.h"
#include "components/InputComponent.h"
#include "Logger.h"
#include <QVBoxLayout>
#include <QKeyEvent>

AppShell::AppShell(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Snake Arena");
    setMinimumSize(800, 600);

    m_controller = nullptr;

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

AppShell::~AppShell() {
    delete m_controller;
}

void AppShell::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);

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
    delete m_controller;
    m_controller = nullptr;
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

    delete m_controller;
    m_controller = new GameController(m_gamePage->scene(), this);
    m_inputComponent = m_controller->input();

    connect(m_controller, &GameController::stateChanged,
            this, &AppShell::onControllerStateChanged);
    connect(m_controller, &GameController::countdownTick,
            this, &AppShell::onCountdownTick);
    connect(m_controller, &GameController::scoreChanged, this, [this](int score) {
        m_currentScore = score;
    });

    m_controller->startGame(20, 20, 100);
    m_gamePage->view()->fitInView(m_gamePage->scene()->sceneRect(), Qt::KeepAspectRatio);
    setFocus();
}

void AppShell::keyPressEvent(QKeyEvent* event) {
    if (!m_controller) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (m_controller->state() == GameController::State::Ready) {
        m_controller->handleReadyKey();
        return;
    }

    if (m_controller->state() == GameController::State::Playing) {
        switch (event->key()) {
            case Qt::Key_Up:    m_inputComponent->setDirection(Direction::Up);    break;
            case Qt::Key_Down:  m_inputComponent->setDirection(Direction::Down);  break;
            case Qt::Key_Left:  m_inputComponent->setDirection(Direction::Left);  break;
            case Qt::Key_Right: m_inputComponent->setDirection(Direction::Right); break;
            case Qt::Key_Escape:
                m_controller->pause();
                m_gamePage->showPause();
                break;
        }
    }

    if (m_controller->state() == GameController::State::Paused) {
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
            m_gamePage->showGameOver(m_currentScore, len, 0, 0);
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
