#include "core/engine/Grid.h"

namespace core {

Grid::Grid(int width, int height)
    : width_(width)
    , height_(height)
    , cells_(width * height, false)
{
}

bool Grid::isInside(Point p) const {
    return p.x >= 0 && p.x < width_ && p.y >= 0 && p.y < height_;
}

bool Grid::isOccupied(Point p) const {
    return isInside(p) && cells_[index(p)];
}

void Grid::occupy(Point p) {
    if (isInside(p)) {
        cells_[index(p)] = true;
    }
}

void Grid::clear(Point p) {
    if (isInside(p)) {
        cells_[index(p)] = false;
    }
}

void Grid::clearAll() {
    std::fill(cells_.begin(), cells_.end(), false);
}

int Grid::index(Point p) const {
    return p.y * width_ + p.x;
}

} // namespace core
