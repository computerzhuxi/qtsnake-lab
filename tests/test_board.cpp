#include <gtest/gtest.h>
#include "Board.h"
#include "Point.h"

TEST(Board, defaultSizeIs20x20) {
    Board board;
    EXPECT_EQ(board.width(), 20);
    EXPECT_EQ(board.height(), 20);
}

TEST(Board, customSize) {
    Board board(15, 10);
    EXPECT_EQ(board.width(), 15);
    EXPECT_EQ(board.height(), 10);
}

TEST(Board, freeCellsEmptyByDefault) {
    Board board;
    EXPECT_TRUE(board.freeCells().empty());
}
