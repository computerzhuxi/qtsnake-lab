#include "Snake.h"

Snake::Snake(Point start, Direction initialDir) {
    m_direction = initialDir;
    m_body.push_back(start);

    Point offset;
    switch (initialDir) {
        case Direction::Right: offset = {-1, 0}; break;
        case Direction::Left:  offset = { 1, 0}; break;
        case Direction::Down:  offset = { 0,-1}; break;
        case Direction::Up:    offset = { 0, 1}; break;
    }

    m_body.push_back({start.x + offset.x,     start.y + offset.y});
    m_body.push_back({start.x + offset.x * 2, start.y + offset.y * 2});
}

void Snake::setDirection(Direction dir) {
    if ((m_direction == Direction::Up    && dir == Direction::Down)  ||
        (m_direction == Direction::Down  && dir == Direction::Up)    ||
        (m_direction == Direction::Left  && dir == Direction::Right) ||
        (m_direction == Direction::Right && dir == Direction::Left)) {
        return;
    }
    m_direction = dir;
}

Direction Snake::direction() const {
    return m_direction;
}

Point Snake::nextHead() const {
    Point h = head();
    switch (m_direction) {
        case Direction::Up:    h.y -= 1; break;
        case Direction::Down:  h.y += 1; break;
        case Direction::Left:  h.x -= 1; break;
        case Direction::Right: h.x += 1; break;
    }
    return h;
}

void Snake::move() {
    m_body.insert(m_body.begin(), nextHead());
    if (m_growNext) {
        // 增长时不 pop_back；m_growNext 由后续 growPending() 消费
    } else {
        m_body.pop_back();
    }
}

void Snake::grow() {
    m_growNext = true;
}

const std::vector<Point>& Snake::body() const {
    return m_body;
}

Point Snake::head() const {
    return m_body.front();
}

Point Snake::tail() const {
    return m_body.back();
}
