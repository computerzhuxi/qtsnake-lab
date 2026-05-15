#include "core/replay/ReplayPlayer.h"
#include "core/replay/ReplaySerializer.h"
#include "core/engine/GameController.h"

namespace core {

ReplayPlayer::ReplayPlayer()
{
}

bool ReplayPlayer::loadFromFile(const std::string& filepath) {
    data_ = loadReplayData(filepath);
    return !data_.initialSnake.empty();  // valid replay has snake data
}

std::optional<Direction> ReplayPlayer::directionAt(uint64_t tick) const {
    auto it = data_.directionChanges.find(tick);
    if (it != data_.directionChanges.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ReplayPlayer::setupController(GameController& controller) const {
    controller.setRngSeed(data_.rngSeed);
    controller.setInitialState(
        data_.initialSnake,
        data_.initialFood,
        data_.initialDirection,
        data_.initialScore);
}

} // namespace core
