#include "NaiveCollisionDetector.h"
#include "GameState.h"

std::vector<CollisionReport> NaiveCollisionDetector::detect(const GameState& state) const {
    std::vector<CollisionReport> reports;
    const auto& snakes = state.snakes;
    const auto& board = state.board;

    for (size_t i = 0; i < snakes.size(); ++i) {
        Point head = snakes[i].head();

        // 1. Wall
        if (head.x < 0 || head.x >= board.width ||
            head.y < 0 || head.y >= board.height) {
            reports.push_back({static_cast<int>(i), CollisionType::Wall, head});
            continue;
        }

        // 2. Food
        if (head == board.foodPos) {
            reports.push_back({static_cast<int>(i), CollisionType::Food, head});
        }

        // 3. Obstacle
        if (board.obstacles.count(head)) {
            reports.push_back({static_cast<int>(i), CollisionType::Obstacle, head});
        }

        // 4. Body / HeadToHead
        for (size_t j = 0; j < snakes.size(); ++j) {
            const auto& body = snakes[j].body();
            size_t start = (i == j) ? 1 : 0;
            for (size_t k = start; k < body.size(); ++k) {
                if (head == body[k]) {
                    if (i != j && k == 0) {
                        reports.push_back({static_cast<int>(i), CollisionType::HeadToHead, head, static_cast<int>(j)});
                    } else if (i == j) {
                        reports.push_back({static_cast<int>(i), CollisionType::SelfBody, head});
                    } else {
                        reports.push_back({static_cast<int>(i), CollisionType::OtherBody, head, static_cast<int>(j)});
                    }
                    goto next_snake;
                }
            }
        }
        next_snake: ;
    }

    return reports;
}
