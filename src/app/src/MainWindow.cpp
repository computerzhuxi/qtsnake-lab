#include "app/MainWindow.h"
#include "app/GameControllerAdapter.h"
#include "app/GameWidget.h"
#include "app/settings/AppSettings.h"
#include "app/dialogs/SettingsDialog.h"
#include "app/replay/ReplayManager.h"
#include "app/ai/AIController.h"
#include "app/ai/AIWorker.h"
#include "app/network/NetworkManager.h"
#include "app/dialogs/MultiplayerDialog.h"
#include "app/leaderboard/LeaderboardData.h"
#include "app/dialogs/LeaderboardDialog.h"
#include "app/screenshot/ScreenshotManager.h"

#include "core/state/StateMachine.h"

#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QResizeEvent>
#include <QShortcut>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto gameConfig = AppSettings::instance().loadGameConfig();
    auto keyBindings = AppSettings::instance().loadKeyBindings();
    bool integerScaling = AppSettings::instance().loadIntegerScaling();
    bool startFullscreen = AppSettings::instance().loadFullscreen();

    windowedSize_ = AppSettings::instance().loadWindowSize();

    adapter_ = new GameControllerAdapter(gameConfig, this);
    replayManager_ = new ReplayManager(this);
    aiController_ = new AIController(adapter_, this);
    networkManager_ = new NetworkManager(this);
    screenshotManager_ = new ScreenshotManager(this);

    leaderboard_ = std::make_unique<LeaderboardData>();
    leaderboard_->loadFromFile("leaderboard.json");

    setupWidgets();
    setupMenus();
    setupConnections();

    gameWidget_->setKeyBindings(keyBindings);
    gameWidget_->setIntegerScaling(integerScaling);

    if (startFullscreen) {
        showFullScreen();
    } else {
        resize(windowedSize_);
    }

    updateTitle();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupWidgets() {
    gameWidget_ = new GameWidget(adapter_, this);
    setCentralWidget(gameWidget_);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (!isMaximized() && !isFullScreen()) {
        windowedSize_ = event->size();
        AppSettings::instance().saveWindowSize(windowedSize_);
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (replayManager_->isRecording()) {
        replayManager_->stopRecording();
    }
    if (replayManager_->isPlaying()) {
        replayManager_->stopPlayback();
    }
    event->accept();
}

void MainWindow::toggleFullscreen() {
    if (isFullScreen()) {
        showNormal();
        resize(windowedSize_);
        AppSettings::instance().saveFullscreen(false);
    } else {
        windowedSize_ = size();
        AppSettings::instance().saveWindowSize(windowedSize_);
        showFullScreen();
        AppSettings::instance().saveFullscreen(true);
    }
}

QSize MainWindow::windowSizeForDisplay() const {
    return windowedSize_;
}

void MainWindow::startNewGameWithRecording() {
    if (replayManager_->isPlaying()) {
        replayManager_->stopPlayback();
    }
    if (replayManager_->isRecording()) {
        replayManager_->stopRecording();
    }

    // Start game first (this calls reset() inside PlayingState::onEnter)
    adapter_->startGame();
    // Then capture the post-reset initial state
    replayManager_->startRecording(*adapter_);
}

void MainWindow::setupMenus() {
    // Game menu
    QMenu* gameMenu = menuBar()->addMenu(tr("&Game"));

    newGameAction_ = gameMenu->addAction(tr("&New Game"), this,
        [this]() { adapter_->startGame(); }, QKeySequence(Qt::Key_F2));
    pauseAction_ = gameMenu->addAction(tr("&Pause"), this,
        [this]() { adapter_->togglePause(); }, QKeySequence(Qt::Key_P));

    QAction* aiAction = gameMenu->addAction(tr("&AI Auto-Play"));
    aiAction->setCheckable(true);
    aiAction->setShortcut(QKeySequence(Qt::Key_F8));
    connect(aiAction, &QAction::toggled, this, [this](bool on) {
        if (on) {
            aiController_->start();
        } else {
            aiController_->stop();
        }
    });

    gameMenu->addSeparator();
    quitAction_ = gameMenu->addAction(tr("&Quit"), this,
        &QWidget::close, QKeySequence::Quit);

    // Replay menu
    QMenu* replayMenu = menuBar()->addMenu(tr("&Replay"));

    startRecordAction_ = replayMenu->addAction(tr("&Record Game"), this,
        [this]() { startNewGameWithRecording(); }, QKeySequence(Qt::Key_F5));

    saveReplayAction_ = replayMenu->addAction(tr("&Save Replay..."), this, [this]() {
        if (replayManager_->isRecording()) {
            replayManager_->stopRecording();
        }
        QString replayDir = AppSettings::instance().loadReplayDir();
        QString path = QFileDialog::getSaveFileName(
            this, tr("Save Replay"), replayDir, tr("Replay Files (*.snake_replay)"));
        if (!path.isEmpty()) {
            replayManager_->saveToFile(path);
        }
    });
    saveReplayAction_->setEnabled(false);

    replayMenu->addSeparator();

    loadReplayAction_ = replayMenu->addAction(tr("&Load Replay..."), this, [this]() {
        QString replayDir = AppSettings::instance().loadReplayDir();
        QString path = QFileDialog::getOpenFileName(
            this, tr("Load Replay"), replayDir, tr("Replay Files (*.snake_replay)"));
        if (path.isEmpty()) return;

        if (replayManager_->isPlaying()) {
            replayManager_->stopPlayback();
        }
        if (replayManager_->isRecording()) {
            replayManager_->stopRecording();
        }

        if (!replayManager_->loadFromFile(path)) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Failed to load replay file."));
            return;
        }

        replayManager_->startPlayback(*adapter_);
    });

    // Network menu
    QMenu* netMenu = menuBar()->addMenu(tr("&Network"));
    netMenu->addAction(tr("&Host/Join..."), this, [this]() {
        MultiplayerDialog dialog(this);
        if (dialog.exec() != QDialog::Accepted) return;

        bool hosting = dialog.isHosting();
        quint16 port = dialog.port();

        if (hosting) {
            if (!networkManager_->hostGame(port, *adapter_)) {
                QMessageBox::warning(this, tr("Error"),
                    tr("Failed to start server on port %1").arg(port));
                return;
            }
            statusBar()->showMessage(
                tr("Hosting on port %1").arg(port), 3000);
            adapter_->startGame();
        } else {
            networkManager_->joinGame(dialog.hostAddress(), port,
                                       dialog.playerName(), *adapter_);
            statusBar()->showMessage(
                tr("Connecting to %1:%2...").arg(dialog.hostAddress()).arg(port), 3000);
        }
    });
    netMenu->addAction(tr("&Disconnect"), this, [this]() {
        networkManager_->stopHosting();
        networkManager_->leaveGame();
        adapter_->returnToMenu();
        statusBar()->showMessage(tr("Disconnected."), 3000);
    });

    // Client: send direction input to host
    connect(adapter_, &GameControllerAdapter::directionInputted,
            this, [this](core::Direction d) {
        if (networkManager_->isClient()) {
            networkManager_->sendInput(d);
        }
    });

    // Client: repaint when game state arrives from host
    connect(networkManager_, &NetworkManager::gameStateUpdated,
            gameWidget_, QOverload<>::of(&QWidget::update));

    // Client mode: skip overlay
    connect(networkManager_, &NetworkManager::joined, this, [this]() {
        gameWidget_->setNetworkClient(true);
    });
    connect(networkManager_, &NetworkManager::left, this, [this]() {
        gameWidget_->setNetworkClient(false);
    });

    // F11 quick toggle
    new QShortcut(QKeySequence(Qt::Key_F11), this, [this]() { toggleFullscreen(); });

    // F12 screenshot
    new QShortcut(QKeySequence(Qt::Key_F12), this, [this]() {
        QString path;
        QString dir = AppSettings::instance().loadReplayDir(); // reuse replay dir
        if (screenshotManager_->capture(gameWidget_, dir, &path)) {
            statusBar()->showMessage(tr("Screenshot saved: %1").arg(path), 3000);
        }
    });

    // View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

    QAction* integerScaleAction = viewMenu->addAction(tr("&Integer Scaling"));
    integerScaleAction->setCheckable(true);
    integerScaleAction->setChecked(gameWidget_->integerScaling());
    connect(integerScaleAction, &QAction::toggled, this, [this](bool on) {
        gameWidget_->setIntegerScaling(on);
        AppSettings::instance().saveIntegerScaling(on);
        gameWidget_->update();
    });

    // Leaderboard menu
    QMenu* lbMenu = menuBar()->addMenu(tr("&Leaderboard"));
    lbMenu->addAction(tr("&View Leaderboard"), this, [this]() {
        LeaderboardDialog dialog(*leaderboard_, this);
        dialog.exec();
    });

    // Settings menu
    QMenu* settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(tr("&Preferences..."), this, [this]() {
        SettingsDialog dialog(this);
        dialog.setGameConfig(adapter_->config());
        dialog.setKeyBindings(gameWidget_->keyBindings());
        dialog.setWindowSize(windowSizeForDisplay());
        dialog.setFullscreen(isFullScreen());
        dialog.setReplayDir(AppSettings::instance().loadReplayDir());

        if (dialog.exec() == QDialog::Accepted) {
            AppSettings::instance().saveReplayDir(dialog.replayDir());

            core::GameConfig newConfig = dialog.gameConfig();
            AppSettings::instance().saveGameConfig(newConfig);
            adapter_->setConfig(newConfig);

            KeyBindings newBindings = dialog.keyBindings();
            AppSettings::instance().saveKeyBindings(newBindings);
            gameWidget_->setKeyBindings(newBindings);

            bool wantFullscreen = dialog.fullscreen();
            if (wantFullscreen != isFullScreen()) {
                if (wantFullscreen) {
                    windowedSize_ = size();
                    AppSettings::instance().saveWindowSize(windowedSize_);
                    showFullScreen();
                } else {
                    showNormal();
                    QSize newSize = dialog.windowSize();
                    if (newSize.isValid()) {
                        windowedSize_ = newSize;
                    }
                    resize(windowedSize_);
                }
                AppSettings::instance().saveFullscreen(wantFullscreen);
            } else if (!wantFullscreen) {
                QSize newSize = dialog.windowSize();
                if (newSize.isValid() && newSize != size()) {
                    windowedSize_ = newSize;
                    AppSettings::instance().saveWindowSize(newSize);
                    resize(newSize);
                }
            }
        }
    });
}

void MainWindow::setupConnections() {
    connect(adapter_, &GameControllerAdapter::stateChanged,
            this, [this](core::StateType) { updateTitle(); });

    // Auto-save score on game over
    connect(adapter_, &GameControllerAdapter::stateChanged,
            this, [this](core::StateType type) {
        if (type == core::StateType::GameOver) {
            ScoreEntry entry;
            entry.playerName = "Player";
            entry.score = adapter_->score();
            const auto& cfg = adapter_->config();
            entry.gridWidth = cfg.gridWidth;
            entry.gridHeight = cfg.gridHeight;
            entry.timestamp = QDateTime::currentSecsSinceEpoch();
            leaderboard_->addEntry(entry);
            leaderboard_->saveToFile("leaderboard.json");
        }
    });

    // Auto-save replay when recorded game ends
    connect(adapter_, &GameControllerAdapter::stateChanged,
            this, [this](core::StateType type) {
        if (type == core::StateType::GameOver && replayManager_->isRecording()) {
            replayManager_->stopRecording();

            QString dir = AppSettings::instance().loadReplayDir();
            QDir().mkpath(dir);
            QString filename = QString("snake_%1.snake_replay")
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
            QString path = QDir(dir).filePath(filename);

            if (replayManager_->saveToFile(path)) {
                statusBar()->showMessage(
                    tr("Replay saved: %1").arg(path), 5000);
            }
            saveReplayAction_->setEnabled(true);
        }
    });

    // Playback finished
    connect(replayManager_, &ReplayManager::playbackFinished, this, [this]() {
        statusBar()->showMessage(tr("Playback finished."), 3000);
        loadReplayAction_->setEnabled(true);
        gameWidget_->setPlaybackActive(false);
    });

    // Recording state UI
    connect(replayManager_, &ReplayManager::recordingStarted, this, [this]() {
        startRecordAction_->setEnabled(false);
        saveReplayAction_->setEnabled(false);
        loadReplayAction_->setEnabled(false);
    });

    connect(replayManager_, &ReplayManager::recordingStopped, this, [this]() {
        startRecordAction_->setEnabled(true);
        loadReplayAction_->setEnabled(true);
    });

    // Playback started
    connect(replayManager_, &ReplayManager::playbackStarted, this, [this]() {
        startRecordAction_->setEnabled(false);
        loadReplayAction_->setEnabled(false);
        gameWidget_->setPlaybackActive(true);
    });
}

void MainWindow::updateTitle() {
    auto type = adapter_->stateMachine().currentType();
    QString title = "QtSnake Lab";
    switch (type) {
    case core::StateType::Menu:    title += " - Menu"; break;
    case core::StateType::Playing: title += " - Playing"; break;
    case core::StateType::Paused:  title += " - Paused"; break;
    case core::StateType::GameOver:title += " - Game Over"; break;
    }
    if (replayManager_->isRecording()) {
        title += " [REC]";
    }
    if (replayManager_->isPlaying()) {
        title += " [PLAYBACK]";
    }
    setWindowTitle(title);
}
