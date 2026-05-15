#include "app/replay/ReplayManager.h"
#include "app/GameControllerAdapter.h"

#include "core/engine/GameController.h"

ReplayManager::ReplayManager(QObject* parent)
    : QObject(parent)
    , playbackTimer_(new QTimer(this))
{
    connect(playbackTimer_, &QTimer::timeout,
            this, &ReplayManager::onPlaybackTick);
}

// ---- Recording ----

void ReplayManager::startRecording(GameControllerAdapter& adapter) {
    const auto& ctrl = adapter.controller();
    recorder_.start(ctrl);

    recordingAdapter_ = &adapter;
    connect(&adapter, &GameControllerAdapter::directionInputted,
            this, &ReplayManager::onDirectionInputted);

    emit recordingStarted();
}

void ReplayManager::stopRecording() {
    if (recordingAdapter_) {
        disconnect(recordingAdapter_, &GameControllerAdapter::directionInputted,
                   this, &ReplayManager::onDirectionInputted);
        recorder_.finish(recordingAdapter_->controller());
        recordingAdapter_ = nullptr;
    }
    emit recordingStopped();
}

void ReplayManager::onDirectionInputted(core::Direction d) {
    if (!recorder_.isRecording() || !recordingAdapter_) return;
    uint64_t effectiveTick = recordingAdapter_->controller().tickNumber() + 1;
    recorder_.recordDirection(effectiveTick, d);
}

// ---- Save / Load ----

bool ReplayManager::saveToFile(const QString& filepath) {
    return core::saveReplayData(recorder_.data(), filepath.toStdString());
}

bool ReplayManager::loadFromFile(const QString& filepath) {
    return player_.loadFromFile(filepath.toStdString());
}

// ---- Playback ----

void ReplayManager::startPlayback(GameControllerAdapter& adapter) {
    adapter.setConfig(player_.config());
    player_.setupController(adapter.controller());
    adapter.controller().start();

    // Sync RNG state: the original game consumed 2 RNG draws when
    // respawn() was called during reset(). setInitialState() skipped
    // those draws, so we consume them here and restore the correct
    // food position.
    {
        auto savedFood = player_.data().initialFood;
        auto& ctrl = adapter.controller();
        ctrl.food().respawn(ctrl.grid(), ctrl.snake()); // consume 2 draws
        ctrl.food().setPosition(savedFood);              // restore
    }

    adapter.stopTickTimer();

    playbackAdapter_ = &adapter;
    playbackTick_ = 0;
    playing_ = true;

    playbackTimer_->setInterval(player_.config().initialSpeed);
    playbackTimer_->start();

    emit playbackStarted();
}

void ReplayManager::stopPlayback() {
    playbackTimer_->stop();
    playing_ = false;
    playbackAdapter_ = nullptr;
    emit playbackFinished();
}

void ReplayManager::onPlaybackTick() {
    if (!playing_ || !playbackAdapter_) return;

    ++playbackTick_;
    auto& ctrl = playbackAdapter_->controller();

    auto dir = player_.directionAt(playbackTick_);
    if (dir.has_value()) {
        ctrl.setDirection(dir.value());
    }
    ctrl.tick();

    if (ctrl.isGameOver() || playbackTick_ >= player_.totalTicks()) {
        stopPlayback();
    }
}
