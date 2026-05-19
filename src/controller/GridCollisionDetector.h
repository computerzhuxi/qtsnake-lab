#ifndef SNAKE_CONTROLLER_GRIDCOLLISIONDETECTOR_H
#define SNAKE_CONTROLLER_GRIDCOLLISIONDETECTOR_H

#include "ICollisionDetector.h"
#include <vector>

struct CellInfo {
    int headCount = 0;
    int firstHeadIdx = -1;
    int secondHeadIdx = -1;
    int bodySnakeIdx = -1;
    int totalSegs = 0;
};

class GridCollisionDetector : public ICollisionDetector {
public:
    explicit GridCollisionDetector(int boardWidth, int boardHeight);
    std::vector<CollisionReport> detect(const GameState& state) const override;

private:
    void ensureSize(int width, int height) const;

    mutable int m_width;
    mutable int m_height;
    mutable int m_epoch = 0;
    mutable std::vector<std::vector<CellInfo>> m_grid;
    mutable std::vector<std::vector<int>> m_epochGrid;
};

#endif
