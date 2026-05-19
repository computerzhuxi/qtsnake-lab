#ifndef SNAKE_CONTROLLER_NAIVECOLLISIONDETECTOR_H
#define SNAKE_CONTROLLER_NAIVECOLLISIONDETECTOR_H

#include "ICollisionDetector.h"

class NaiveCollisionDetector : public ICollisionDetector {
public:
    std::vector<CollisionReport> detect(const GameState& state) const override;
};

#endif
