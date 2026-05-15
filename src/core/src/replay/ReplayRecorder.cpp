#include "core/replay/ReplayRecorder.h"
#include "core/engine/GameController.h"
#include "core/engine/Snake.h"
#include "core/engine/Food.h"

namespace core {

ReplayRecorder::ReplayRecorder()
{
}

void ReplayRecorder::start(const GameController& controller) {
    data_ = ReplayData{};
    data_.config = controller.config();
    data_.rngSeed = controller.rngSeed();
    data_.initialSnake = controller.snake().body();
    data_.initialFood = controller.food().position();
    data_.initialDirection = controller.snake().direction();
    data_.initialScore = controller.score();
    recording_ = true;
}

void ReplayRecorder::recordDirection(uint64_t effectiveTick, Direction d) {
    if (!recording_) return;
    data_.directionChanges[effectiveTick] = d;
}

ReplayData ReplayRecorder::finish(const GameController& controller) {
    recording_ = false;
    data_.totalTicks = controller.tickNumber();
    data_.finalScore = controller.score();
    return data_;
}

} // namespace core
