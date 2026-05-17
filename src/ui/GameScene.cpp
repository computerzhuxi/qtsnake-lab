#include "GameScene.h"

GameScene::GameScene(QObject* parent) : QGraphicsScene(parent) {
    setBackgroundBrush(QColor("#06060f"));
}

void GameScene::syncFromBoard(const Board& board, const std::vector<Snake*>& snakes) {
    for (size_t i = 0; i < snakes.size(); ++i) {
        int key = static_cast<int>(i);
        const auto& body = snakes[i]->body();
        auto& items = m_snakeItems[key];

        // 增长：body 比 item 多
        while (items.size() < body.size()) {
            auto* item = new SnakeItem(false);
            addItem(item);
            items.append(item);
        }

        // 缩减（其他蛇死亡）：body 比 item 少
        while (items.size() > body.size()) {
            auto* item = items.takeLast();
            removeItem(item);
            delete item;
        }

        // 更新位置和头/身状态
        for (size_t j = 0; j < body.size(); ++j) {
            items[j]->setPos(cellToPixel(body[j].x), cellToPixel(body[j].y));
            items[j]->setIsHead(j == 0);
        }
    }

    // 清理已消失的蛇
    for (auto it = m_snakeItems.begin(); it != m_snakeItems.end(); ) {
        int key = it.key();
        if (key >= static_cast<int>(snakes.size())) {
            for (auto* item : it.value()) {
                removeItem(item);
                delete item;
            }
            it = m_snakeItems.erase(it);
        } else {
            ++it;
        }
    }

    // 食物：首次创建，后续只移动
    Food food = board.food();
    if (!m_foodItem) {
        m_foodItem = new FoodItem();
        addItem(m_foodItem);
    }
    m_foodItem->setPos(cellToPixel(food.position.x), cellToPixel(food.position.y));
}

void GameScene::clearAll() {
    for (auto& items : m_snakeItems) {
        for (auto* item : items) {
            removeItem(item);
            delete item;
        }
    }
    m_snakeItems.clear();

    if (m_foodItem) {
        removeItem(m_foodItem);
        delete m_foodItem;
        m_foodItem = nullptr;
    }
}

int GameScene::cellToPixel(int cell) const {
    return cell * SnakeItem::cellSize + SnakeItem::cellSize / 2;
}
