#ifndef CORE_ENGINE_GAMEOBSERVER_H
#define CORE_ENGINE_GAMEOBSERVER_H

#include "GameEvent.h"

namespace core {

class GameObserver {
public:
    virtual ~GameObserver() = default;
    virtual void onEvent(const GameEvent& event) = 0;
};

} // namespace core

#endif // CORE_ENGINE_GAMEOBSERVER_H
