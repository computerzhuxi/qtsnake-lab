#ifndef CORE_REPLAY_REPLAYSERIALIZER_H
#define CORE_REPLAY_REPLAYSERIALIZER_H

#include "core/replay/ReplayData.h"
#include <string>

namespace core {

bool saveReplayData(const ReplayData& data, const std::string& filepath);
ReplayData loadReplayData(const std::string& filepath);

} // namespace core

#endif // CORE_REPLAY_REPLAYSERIALIZER_H
