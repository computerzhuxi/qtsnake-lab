#ifndef CORE_REPLAY_REPLAYRECORDER_H
#define CORE_REPLAY_REPLAYRECORDER_H

#include "core/replay/ReplayData.h"

namespace core {

class GameController;

class ReplayRecorder {
public:
    ReplayRecorder();

    void start(const GameController& controller);
    void recordDirection(uint64_t effectiveTick, Direction d);
    ReplayData finish(const GameController& controller);

    const ReplayData& data() const { return data_; }
    bool isRecording() const { return recording_; }

private:
    ReplayData data_;
    bool recording_ = false;
};

} // namespace core

#endif // CORE_REPLAY_REPLAYRECORDER_H
