#include "GameScene.h"
#include <QPen>
#include <QBrush>

GameScene::GameScene(QObject* parent) : QGraphicsScene(parent) {
    setBackgroundBrush(QColor("#06060f"));
}

void GameScene::syncFromState(const GameState& state) {
    const auto& board = state.board;
    m_boardW = board.width;
    m_boardH = board.height;
    int pixelW = m_boardW * SnakeItem::cellSize;
    int pixelH = m_boardH * SnakeItem::cellSize;
    setSceneRect(0, 0, pixelW, pixelH);

    if (m_borderItem) {
        removeItem(m_borderItem);
        delete m_borderItem;
        m_borderItem = nullptr;
    }
    m_borderItem = new QGraphicsRectItem(0, 0, pixelW, pixelH);
    m_borderItem->setPen(QPen(QColor("#00ff88"), 3));
    m_borderItem->setBrush(QColor("#0a0a14"));
    m_borderItem->setZValue(-1);
    addItem(m_borderItem);

    // 蛇
    for (size_t i = 0; i < state.snakes.size(); ++i) {
        int key = static_cast<int>(i);
        const auto& body = state.snakes[i].body();
        auto& items = m_snakeItems[key];

        while (items.size() < body.size()) {
            auto* item = new SnakeItem(false);
            addItem(item);
            items.append(item);
        }
        while (items.size() > body.size()) {
            auto* item = items.takeLast();
            removeItem(item);
            delete item;
        }
        for (size_t j = 0; j < body.size(); ++j) {
            items[j]->setPos(cellToPixel(body[j].x), cellToPixel(body[j].y));
            items[j]->setIsHead(j == 0);
        }
    }

    // 清理已消失的蛇
    for (auto it = m_snakeItems.begin(); it != m_snakeItems.end(); ) {
        if (it.key() >= static_cast<int>(state.snakes.size())) {
            for (auto* item : it.value()) {
                removeItem(item);
                delete item;
            }
            it = m_snakeItems.erase(it);
        } else {
            ++it;
        }
    }

    // 食物
    if (!m_foodItem) {
        m_foodItem = new FoodItem();
        addItem(m_foodItem);
    }
    m_foodItem->setPos(cellToPixel(board.foodPos.x), cellToPixel(board.foodPos.y));
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
