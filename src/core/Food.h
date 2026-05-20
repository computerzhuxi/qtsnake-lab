#ifndef SNAKE_CORE_FOOD_H
#define SNAKE_CORE_FOOD_H

#include "Point.h"

class Board;

class Food {
public:
    Food(int points = 1);

    Point position() const;
    int points() const;
    bool isEaten() const;

    void placeAt(Point pos, Board& board);
    void remove(Board& board);
    void markEaten();
    void clearEaten();

private:
    Point m_position;
    int m_points = 1;
    bool m_eaten = false;
};

#endif
