#ifndef APP_REPLAY_REPLAYMANAGER_H
#define APP_REPLAY_REPLAYMANAGER_H

#include <QObject>
#include <QTimer>

#include "core/engine/Direction.h"
#include "core/replay/ReplayData.h"
#include "core/replay/ReplayRecorder.h"
#include "core/replay/ReplayPlayer.h"

class GameControllerAdapter;

class ReplayManager : public QObject {
    Q_OBJECT

public:
    explicit ReplayManager(QObject* parent = nullptr);

    void startRecording(GameControllerAdapter& adapter);
    void stopRecording();
    bool isRecording() const { return recorder_.isRecording(); }

    bool saveToFile(const QString& filepath);

    bool loadFromFile(const QString& filepath);
    void startPlayback(GameControllerAdapter& adapter);
    void stopPlayback();
    bool isPlaying() const { return playing_; }

    const core::ReplayPlayer& player() const { return player_; }

signals:
    void recordingStarted();
    void recordingStopped();
    void playbackStarted();
    void playbackFinished();

private slots:
    void onDirectionInputted(core::Direction d);
    void onPlaybackTick();

private:
    core::ReplayRecorder recorder_;
    core::ReplayPlayer player_;

    bool playing_ = false;
    uint64_t playbackTick_ = 0;
    QTimer* playbackTimer_ = nullptr;
    GameControllerAdapter* playbackAdapter_ = nullptr;
    GameControllerAdapter* recordingAdapter_ = nullptr;
};

#endif // APP_REPLAY_REPLAYMANAGER_H
