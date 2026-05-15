#ifndef CORE_REPLAY_REPLAYPLAYER_H
#define CORE_REPLAY_REPLAYPLAYER_H

#include "core/replay/ReplayData.h"
#include <optional>
#include <string>

namespace core {

class GameController;

class ReplayPlayer {
public:
    ReplayPlayer();

    bool loadFromFile(const std::string& filepath);

    const ReplayData& data() const { return data_; }
    GameConfig config() const { return data_.config; }
    uint64_t totalTicks() const { return data_.totalTicks; }

    std::optional<Direction> directionAt(uint64_t tick) const;

    void setupController(GameController& controller) const;

private:
    ReplayData data_;
};

} // namespace core

#endif // CORE_REPLAY_REPLAYPLAYER_H
