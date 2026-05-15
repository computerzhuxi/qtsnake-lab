#ifndef APP_MAINWINDOW_H
#define APP_MAINWINDOW_H

#include <QMainWindow>
#include <QSize>
#include <memory>

class GameControllerAdapter;
class GameWidget;
class ReplayManager;
class AIController;
class NetworkManager;
class ScreenshotManager;
struct ScoreEntry;
class LeaderboardData;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void setupMenus();
    void setupWidgets();
    void setupConnections();

    void updateTitle();
    void toggleFullscreen();
    void startNewGameWithRecording();

    QSize windowSizeForDisplay() const;

    GameControllerAdapter* adapter_;
    GameWidget* gameWidget_;
    ReplayManager* replayManager_;
    AIController* aiController_;
    NetworkManager* networkManager_;
    ScreenshotManager* screenshotManager_;
    std::unique_ptr<LeaderboardData> leaderboard_;

    QSize windowedSize_;

    // Actions
    QAction* newGameAction_;
    QAction* pauseAction_;
    QAction* startRecordAction_;
    QAction* saveReplayAction_;
    QAction* loadReplayAction_;
    QAction* quitAction_;
};

#endif // APP_MAINWINDOW_H
