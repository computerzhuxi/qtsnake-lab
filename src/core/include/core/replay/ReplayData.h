#ifndef CORE_REPLAY_REPLAYDATA_H
#define CORE_REPLAY_REPLAYDATA_H

#include "core/engine/Direction.h"
#include "core/engine/GameConfig.h"
#include "core/engine/Point.h"
#include <cstdint>
#include <deque>
#include <map>
#include <string>

namespace core {

struct ReplayData {
    GameConfig config;
    std::deque<Point> initialSnake;
    Point  initialFood;
    Direction initialDirection = Direction::Right;
    int    initialScore = 0;
    unsigned int rngSeed = 0;

    // tick -> new direction (only when direction changes)
    std::map<uint64_t, Direction> directionChanges;

    // Metadata (not used during playback)
    uint64_t totalTicks = 0;
    int      finalScore = 0;
};

bool saveReplayData(const ReplayData& data, const std::string& filepath);
ReplayData loadReplayData(const std::string& filepath);

} // namespace core

#endif // CORE_REPLAY_REPLAYDATA_H
