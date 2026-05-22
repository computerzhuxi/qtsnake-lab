#include "Food.h"
#include "Board.h"

Food::Food(int points)
    : m_points(points)
{
}

Point Food::position() const { return m_position; }
int Food::points() const { return m_points; }
bool Food::isEaten() const { return m_eaten; }

void Food::placeAt(Point pos, Board& board) {
    m_position = pos;
    board.setFoodFlag(pos, true);
}

void Food::remove(Board& board) {
    board.setFoodFlag(m_position, false);
}

void Food::markEaten() { m_eaten = true; }
void Food::clearEaten() { m_eaten = false; }
