#include <gtest/gtest.h>
#include <unordered_set>
#include "Point.h"

// ========== 对称对哈希不碰撞 ==========

TEST(PointHash, symmetricCoordinatesHaveDifferentHash) {
    EXPECT_NE(std::hash<Point>{}(Point{1, 2}),
              std::hash<Point>{}(Point{2, 1}));
    EXPECT_NE(std::hash<Point>{}(Point{3, 7}),
              std::hash<Point>{}(Point{7, 3}));
}

// ========== 20×20 网格碰撞率测试 ==========

TEST(PointHash, grid400LowCollision) {
    std::unordered_set<Point> s;
    for (int x = 0; x < 20; ++x)
        for (int y = 0; y < 20; ++y)
            s.insert(Point{x, y});

    EXPECT_EQ(s.size(), 400u);
    EXPECT_GT(s.bucket_count(), 0u);

    size_t maxBucket = 0;
    for (size_t i = 0; i < s.bucket_count(); ++i) {
        size_t bs = s.bucket_size(i);
        if (bs > maxBucket) maxBucket = bs;
    }
    EXPECT_LE(maxBucket, 8u);
}
