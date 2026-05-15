#ifndef CORE_ENGINE_GRID_H
#define CORE_ENGINE_GRID_H

#include "Point.h"
#include <vector>

namespace core {

class Grid {
public:
    Grid(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }

    bool isInside(Point p) const;
    bool isOccupied(Point p) const;
    void occupy(Point p);
    void clear(Point p);
    void clearAll();

private:
    int width_;
    int height_;
    std::vector<bool> cells_;
    int index(Point p) const;
};

} // namespace core

#endif // CORE_ENGINE_GRID_H
