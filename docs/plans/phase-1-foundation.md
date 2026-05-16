# Phase 1: Foundation + Single Player Classic Mode

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 构建可玩的单人经典贪吃蛇——完整的 CMake 骨架、游戏核心模型、QGraphicsView 渲染、控制器循环、应用外壳浮层（主菜单/暂停/结算/设置）。

**Architecture:** 每个模块编译为静态库，顶层 CMake 链接为 SnakeArena.exe。纯 C++ 的 core 不依赖 Qt，ui/controller/app 依赖 Qt5。游戏画面始终处于底层 QGraphicsView，所有界面以浮层形式覆盖。

**Tech Stack:** C++17, Qt5 (Widgets, Network, Linguist), CMake 3.16+, Google Test (FetchContent)

---

## 文件结构一览

```
SnakeArena/
├── CMakeLists.txt                    # 顶层
├── src/
│   ├── main.cpp                      # 客户端入口
│   ├── core/
│   │   ├── CMakeLists.txt
│   │   ├── Point.h
│   │   ├── Direction.h
│   │   ├── Snake.h / Snake.cpp
│   │   ├── Board.h / Board.cpp
│   │   └── Food.h
│   ├── logging/
│   │   ├── CMakeLists.txt
│   │   └── Logger.h / Logger.cpp
│   ├── ui/
│   │   ├── CMakeLists.txt
│   │   ├── GameScene.h / GameScene.cpp
│   │   ├── GameView.h / GameView.cpp
│   │   ├── SnakeItem.h / SnakeItem.cpp
│   │   └── FoodItem.h / FoodItem.cpp
│   ├── controller/
│   │   ├── CMakeLists.txt
│   │   └── GameController.h / GameController.cpp
│   ├── app/
│   │   ├── CMakeLists.txt
│   │   ├── AppShell.h / AppShell.cpp
│   │   ├── MainMenuWidget.h / MainMenuWidget.cpp
│   │   ├── PauseWidget.h / PauseWidget.cpp
│   │   ├── GameOverWidget.h / GameOverWidget.cpp
│   │   └── SettingsWidget.h / SettingsWidget.cpp
│   ├── audio/
│   │   ├── CMakeLists.txt
│   │   └── AudioManager.h / AudioManager.cpp
│   └── resources/
│       ├── resources.qrc
│       └── style/main.qss
└── tests/
    └── CMakeLists.txt
```

---

### Task 1: 顶层 CMake + 项目骨架

**Files:**
- Create: `CMakeLists.txt`
- Create: `src/core/CMakeLists.txt`
- Create: `src/logging/CMakeLists.txt`
- Create: `src/ui/CMakeLists.txt`
- Create: `src/controller/CMakeLists.txt`
- Create: `src/app/CMakeLists.txt`
- Create: `src/audio/CMakeLists.txt`
- Create: `tests/CMakeLists.txt`

- [ ] **Step 1: 写顶层 CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.16)
project(SnakeArena VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network LinguistTools)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src/core)
add_subdirectory(src/logging)
add_subdirectory(src/ui)
add_subdirectory(src/controller)
add_subdirectory(src/audio)
add_subdirectory(src/app)
add_subdirectory(tests)
```

- [ ] **Step 2: 写各模块占位 CMakeLists.txt**

`src/core/CMakeLists.txt`:
```cmake
add_library(snake_core STATIC
  Snake.cpp
  Board.cpp
)
target_include_directories(snake_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

`src/logging/CMakeLists.txt`:
```cmake
add_library(snake_logging STATIC
  Logger.cpp
)
target_include_directories(snake_logging PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

`src/ui/CMakeLists.txt`:
```cmake
add_library(snake_ui STATIC
  GameScene.cpp
  GameView.cpp
  SnakeItem.cpp
  FoodItem.cpp
)
target_include_directories(snake_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snake_ui PUBLIC snake_core Qt5::Widgets)
```

`src/controller/CMakeLists.txt`:
```cmake
add_library(snake_controller STATIC
  GameController.cpp
)
target_include_directories(snake_controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snake_controller PUBLIC snake_core snake_ui snake_logging Qt5::Widgets)
```

`src/audio/CMakeLists.txt`:
```cmake
add_library(snake_audio STATIC
  AudioManager.cpp
)
target_include_directories(snake_audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

`src/app/CMakeLists.txt`:
```cmake
add_library(snake_app STATIC
  AppShell.cpp
  MainMenuWidget.cpp
  PauseWidget.cpp
  GameOverWidget.cpp
  SettingsWidget.cpp
)
target_include_directories(snake_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snake_app PUBLIC snake_controller snake_ui snake_audio snake_logging Qt5::Widgets)
```

`tests/CMakeLists.txt`:
```cmake
enable_testing()

add_executable(test_core
  test_board.cpp
  test_snake.cpp
)
target_link_libraries(test_core PRIVATE snake_core gtest_main)
add_test(NAME test_core COMMAND test_core)
```

- [ ] **Step 3: 写空的 .cpp 文件让 CMake 能通过**

`src/core/Snake.cpp`:
```cpp
#include "Snake.h"
```

`src/core/Board.cpp`:
```cpp
#include "Board.h"
```

`src/logging/Logger.cpp`:
```cpp
#include "Logger.h"
```

`src/ui/GameScene.cpp`:
```cpp
#include "GameScene.h"
```

`src/ui/GameView.cpp`:
```cpp
#include "GameView.h"
```

`src/ui/SnakeItem.cpp`:
```cpp
#include "SnakeItem.h"
```

`src/ui/FoodItem.cpp`:
```cpp
#include "FoodItem.h"
```

`src/controller/GameController.cpp`:
```cpp
#include "GameController.h"
```

`src/audio/AudioManager.cpp`:
```cpp
#include "AudioManager.h"
```

`src/app/AppShell.cpp`:
```cpp
#include "AppShell.h"
```

`src/app/MainMenuWidget.cpp`:
```cpp
#include "MainMenuWidget.h"
```

`src/app/PauseWidget.cpp`:
```cpp
#include "PauseWidget.h"
```

`src/app/GameOverWidget.cpp`:
```cpp
#include "GameOverWidget.h"
```

`src/app/SettingsWidget.cpp`:
```cpp
#include "SettingsWidget.h"
```

`tests/test_board.cpp`:
```cpp
#include <gtest/gtest.h>

TEST(Board, Placeholder) {
    EXPECT_TRUE(true);
}
```

`tests/test_snake.cpp`:
```cpp
#include <gtest/gtest.h>

TEST(Snake, Placeholder) {
    EXPECT_TRUE(true);
}
```

- [ ] **Step 4: 写空的 .h 文件**

每个模块创建对应的头文件（骨架声明，暂不添加具体成员）：

`src/core/Point.h`:
```cpp
#pragma once

struct Point {
    int x;
    int y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};
```

`src/core/Direction.h`:
```cpp
#pragma once

enum class Direction { Up, Down, Left, Right };
```

`src/core/Food.h`:
```cpp
#pragma once
#include "Point.h"

struct Food {
    Point position;
    int points = 1;
};
```

`src/core/Snake.h`:
```cpp
#pragma once

class Snake {
public:
    Snake();
};
```

`src/core/Board.h`:
```cpp
#pragma once

class Board {
public:
    Board();
};
```

其余 .h 文件创建空的类声明即可（GameScene、GameView、SnakeItem、FoodItem、GameController、AudioManager、AppShell、MainMenuWidget、PauseWidget、GameOverWidget、SettingsWidget、Logger）。

- [ ] **Step 5: 构建验证**

```bash
mkdir -p build && cd build && cmake .. && cmake --build .
```

期望：CMake 配置成功，编译通过（所有 .cpp 只是 include 头文件，无实际逻辑）。

- [ ] **Step 6: 运行测试验证 Google Test 就绪**

```bash
cd build && ctest --output-on-failure
```

期望：1/1 test passed（placeholder 测试）。

- [ ] **Step 7: 提交**

```bash
git add -A
git commit -m "build: CMake skeleton with empty module stubs and Google Test"
```

---

### Task 2: core/Point + Direction 类型

**Files:**
- Create: `src/core/Point.h`（已创建，现在更新）
- Create: `src/core/Direction.h`（已创建，现在更新）

Point.h 和 Direction.h 已经在 Task 1 中写了完整内容，无需额外步骤。跳到 Task 3。

---

### Task 3: core/Snake 模型（TDD）

**Files:**
- Modify: `src/core/Snake.h`
- Modify: `src/core/Snake.cpp`
- Modify: `tests/test_snake.cpp`

- [ ] **Step 1: 写 Snake 测试**

替换 `tests/test_snake.cpp`:

```cpp
#include <gtest/gtest.h>
#include "Snake.h"
#include "Point.h"
#include "Direction.h"

TEST(Snake, initialLengthIsThree) {
    Snake snake;
    EXPECT_EQ(snake.body().size(), 3);
}

TEST(Snake, initialDirectionIsRight) {
    Snake snake;
    EXPECT_EQ(snake.direction(), Direction::Right);
}

TEST(Snake, initialHeadPosition) {
    Snake snake(Point{5, 5});
    EXPECT_EQ(snake.head(), Point(5, 5));
}

TEST(Snake, moveUpdatesPositions) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Right);
    snake.move();
    EXPECT_EQ(snake.head(), Point(6, 5));
}

TEST(Snake, moveUp) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Up);
    snake.move();
    EXPECT_EQ(snake.head(), Point(5, 4));
}

TEST(Snake, moveLeft) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Left);
    snake.move();
    EXPECT_EQ(snake.head(), Point(4, 5));
}

TEST(Snake, moveDown) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Down);
    snake.move();
    EXPECT_EQ(snake.head(), Point(5, 6));
}

TEST(Snake, bodyFollowsHeadAfterMoves) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Right);
    snake.move(); // head→(6,5), segment1→(5,5), segment2→(4,5)
    EXPECT_EQ(snake.body()[0], Point(6, 5));
    EXPECT_EQ(snake.body()[1], Point(5, 5));
    EXPECT_EQ(snake.body()[2], Point(4, 5));
}

TEST(Snake, cannotReverseToOppositeDirection) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Up);
    EXPECT_EQ(snake.direction(), Direction::Up);
    snake.setDirection(Direction::Down);
    EXPECT_EQ(snake.direction(), Direction::Up); // 不应反转
}

TEST(Snake, growIncreasesLength) {
    Snake snake(Point{5, 5});
    snake.setDirection(Direction::Right);
    size_t oldLen = snake.body().size();
    snake.grow();
    snake.move();
    EXPECT_EQ(snake.body().size(), oldLen + 1);
}
```

- [ ] **Step 2: 运行测试（应该大量 FAIL）**

```bash
cd build && cmake .. && cmake --build . && ctest --output-on-failure
```

- [ ] **Step 3: 实现 Snake**

替换 `src/core/Snake.h`:

```cpp
#pragma once
#include <vector>
#include "Point.h"
#include "Direction.h"

class Snake {
public:
    explicit Snake(Point start = Point{3, 3});

    void setDirection(Direction dir);
    Direction direction() const;
    void move();
    void grow();
    const std::vector<Point>& body() const;
    Point head() const;

private:
    std::vector<Point> m_body;
    Direction m_direction = Direction::Right;
    bool m_growNext = false;
};
```

替换 `src/core/Snake.cpp`:

```cpp
#include "Snake.h"

Snake::Snake(Point start) {
    m_body.push_back(start);
    m_body.push_back({start.x - 1, start.y});
    m_body.push_back({start.x - 2, start.y});
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

void Snake::move() {
    Point newHead = head();
    switch (m_direction) {
        case Direction::Up:    newHead.y -= 1; break;
        case Direction::Down:  newHead.y += 1; break;
        case Direction::Left:  newHead.x -= 1; break;
        case Direction::Right: newHead.x += 1; break;
    }
    m_body.insert(m_body.begin(), newHead);
    if (m_growNext) {
        m_growNext = false;
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
```

- [ ] **Step 4: 运行测试验证**

```bash
cd build && cmake --build . && ctest --output-on-failure
```

期望：All 9 tests passed for test_snake。

- [ ] **Step 5: 提交**

```bash
git add src/core/Snake.h src/core/Snake.cpp tests/test_snake.cpp
git commit -m "feat: Snake model with movement, growth, and direction control"
```

---

### Task 4: core/Board 模型（TDD）

**Files:**
- Modify: `src/core/Board.h`
- Modify: `src/core/Board.cpp`
- Modify: `tests/test_board.cpp`

- [ ] **Step 1: 写 Board 测试**

替换 `tests/test_board.cpp`:

```cpp
#include <gtest/gtest.h>
#include "Board.h"
#include "Snake.h"
#include "Point.h"
#include "Direction.h"

class BoardTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board(20, 20);
        snake = new Snake(Point{10, 10});
    }
    void TearDown() override {
        delete board;
        delete snake;
    }
    Board* board;
    Snake* snake;
};

TEST_F(BoardTest, initializesWithGivenSize) {
    EXPECT_EQ(board->width(), 20);
    EXPECT_EQ(board->height(), 20);
}

TEST_F(BoardTest, snakeWithinBoundsIsAlive) {
    EXPECT_FALSE(board->checkWallCollision(*snake));
}

TEST_F(BoardTest, snakeHitsLeftWall) {
    snake->setDirection(Direction::Left);
    for (int i = 0; i < 12; ++i) snake->move();
    EXPECT_TRUE(board->checkWallCollision(*snake));
}

TEST_F(BoardTest, snakeHitsSelf) {
    snake->grow();
    snake->grow();
    snake->grow();
    snake->grow();
    // 蛇长大后再检测自撞
    for (int i = 0; i < 10; ++i) snake->move();
    EXPECT_FALSE(board->checkSelfCollision(*snake));
}

TEST_F(BoardTest, foodPlacedWithinBoard) {
    board->spawnFood();
    Food food = board->food();
    EXPECT_GE(food.position.x, 0);
    EXPECT_LT(food.position.x, board->width());
    EXPECT_GE(food.position.y, 0);
    EXPECT_LT(food.position.y, board->height());
}

TEST_F(BoardTest, foodNotOnSnakeBody) {
    snake->move(); snake->move(); snake->move();
    board->spawnFood();
    Food food = board->food();
    for (const auto& seg : snake->body()) {
        EXPECT_FALSE(food.position == seg);
    }
}

TEST_F(BoardTest, checkFoodCollisionWhenHeadOnFood) {
    board->spawnFood();
    Food f = board->food();
    Snake s(Point{f.position.x, f.position.y});
    EXPECT_TRUE(board->checkFoodCollision(s));
}
```

- [ ] **Step 2: 运行测试（大量 FAIL）**

```bash
cd build && cmake --build . && ctest --output-on-failure -R test_board
```

- [ ] **Step 3: 实现 Board**

替换 `src/core/Board.h`:

```cpp
#pragma once
#include <random>
#include "Point.h"
#include "Snake.h"
#include "Food.h"

class Board {
public:
    Board(int width = 20, int height = 20, unsigned int seed = 0);

    int width() const;
    int height() const;
    Food food() const;

    void setSeed(unsigned int seed);
    void spawnFood();
    bool checkWallCollision(const Snake& snake) const;
    bool checkSelfCollision(const Snake& snake) const;
    bool checkFoodCollision(const Snake& snake) const;

private:
    int m_width;
    int m_height;
    Food m_food;
    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_distX;
    std::uniform_int_distribution<int> m_distY;

    Point randomPoint();
};
```

替换 `src/core/Board.cpp`:

```cpp
#include "Board.h"
#include <algorithm>

Board::Board(int width, int height, unsigned int seed)
    : m_width(width)
    , m_height(height)
    , m_rng(seed == 0 ? std::random_device{}() : seed)
    , m_distX(0, width - 1)
    , m_distY(0, height - 1)
{
}

int Board::width() const { return m_width; }
int Board::height() const { return m_height; }
Food Board::food() const { return m_food; }

void Board::setSeed(unsigned int seed) {
    m_rng.seed(seed == 0 ? std::random_device{}() : seed);
}

void Board::spawnFood() {
    m_food.position = randomPoint();
}

Point Board::randomPoint() {
    return {m_distX(m_rng), m_distY(m_rng)};
}

bool Board::checkWallCollision(const Snake& snake) const {
    Point h = snake.head();
    return h.x < 0 || h.x >= m_width || h.y < 0 || h.y >= m_height;
}

bool Board::checkSelfCollision(const Snake& snake) const {
    Point h = snake.head();
    const auto& body = snake.body();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == h) return true;
    }
    return false;
}

bool Board::checkFoodCollision(const Snake& snake) const {
    return snake.head() == m_food.position;
}
```

- [ ] **Step 4: 更新 test_board.cpp 的 CMakeLists，因为它需要 board + snake 的测试**

`tests/CMakeLists.txt` 已经包含了 test_board 和 test_snake，无需更改。

- [ ] **Step 5: 运行测试验证**

```bash
cd build && cmake --build . && ctest --output-on-failure
```

期望：All tests pass（test_snake 9 个 + test_board 7 个 = 16 个）。

- [ ] **Step 6: 提交**

```bash
git add src/core/Board.h src/core/Board.cpp tests/test_board.cpp
git commit -m "feat: Board model with wall/self/food collision and food spawning"
```

---

### Task 5: logging/Logger 模块

**Files:**
- Modify: `src/logging/Logger.h`
- Modify: `src/logging/Logger.cpp`

- [ ] **Step 1: 实现 Logger**

替换 `src/logging/Logger.h`:

```cpp
#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

enum class LogLevel { Debug, Info, Warning, Error };

class Logger {
public:
    static Logger& instance();

    void setFile(const std::string& filepath);
    void setMinLevel(LogLevel level);

    void log(LogLevel level, const std::string& tag, const std::string& msg);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    LogLevel m_minLevel = LogLevel::Debug;
    std::ofstream m_file;
    bool m_hasFile = false;

    std::string levelStr(LogLevel level);
    std::string timestamp();
};

#define LOG_DEBUG(tag, msg) Logger::instance().log(LogLevel::Debug, tag, msg)
#define LOG_INFO(tag, msg)  Logger::instance().log(LogLevel::Info,  tag, msg)
#define LOG_WARN(tag, msg)  Logger::instance().log(LogLevel::Warning, tag, msg)
#define LOG_ERROR(tag, msg) Logger::instance().log(LogLevel::Error, tag, msg)
```

替换 `src/logging/Logger.cpp`:

```cpp
#include "Logger.h"

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::setFile(const std::string& filepath) {
    m_file.open(filepath, std::ios::app);
    m_hasFile = m_file.is_open();
}

void Logger::setMinLevel(LogLevel level) {
    m_minLevel = level;
}

void Logger::log(LogLevel level, const std::string& tag, const std::string& msg) {
    if (level < m_minLevel) return;
    std::string line = timestamp() + " [" + levelStr(level) + "] [" + tag + "] " + msg;
    std::cout << line << std::endl;
    if (m_hasFile) {
        m_file << line << std::endl;
    }
}

std::string Logger::levelStr(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
    }
    return "???";
}

std::string Logger::timestamp() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
```

- [ ] **Step 2: 编译验证**

```bash
cd build && cmake --build .
```

- [ ] **Step 3: 提交**

```bash
git add src/logging/Logger.h src/logging/Logger.cpp
git commit -m "feat: Logger with level filtering and console/file output"
```

---

### Task 6: ui/SnakeItem + FoodItem 渲染

**Files:**
- Modify: `src/ui/SnakeItem.h`
- Modify: `src/ui/SnakeItem.cpp`
- Modify: `src/ui/FoodItem.h`
- Modify: `src/ui/FoodItem.cpp`

- [ ] **Step 1: 实现 SnakeItem**

替换 `src/ui/SnakeItem.h`:

```cpp
#pragma once
#include <QGraphicsItem>
#include <QColor>

class SnakeItem : public QGraphicsItem {
public:
    SnakeItem(bool isHead, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    static constexpr int cellSize = 24;
    static constexpr int blockSize = 20;
    static constexpr int headSize = 20;

private:
    bool m_isHead;
};
```

替换 `src/ui/SnakeItem.cpp`:

```cpp
#include "SnakeItem.h"
#include <QPainter>

SnakeItem::SnakeItem(bool isHead, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_isHead(isHead)
{
}

QRectF SnakeItem::boundingRect() const {
    int s = cellSize;
    return QRectF(-s / 2, -s / 2, s, s);
}

void SnakeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    int s = blockSize;
    int r = 4; // 小圆角

    if (m_isHead) {
        painter->setPen(QPen(QColor("#44ffaa"), 1));
        painter->setBrush(QColor("#00ff88"));
        painter->drawRoundedRect(QRectF(-s / 2.0, -s / 2.0, s, s), r, r);
    } else {
        painter->setPen(QPen(QColor("#22cc66"), 1));
        painter->setBrush(QColor("#00bb55"));
        painter->drawRoundedRect(QRectF(-s / 2.0, -s / 2.0, s, s), r, r);
    }
}
```

- [ ] **Step 2: 实现 FoodItem**

替换 `src/ui/FoodItem.h`:

```cpp
#pragma once
#include <QGraphicsItem>
#include <QColor>

class FoodItem : public QGraphicsItem {
public:
    FoodItem(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    static constexpr int cellSize = 24;
    static constexpr int radius = 6;
};
```

替换 `src/ui/FoodItem.cpp`:

```cpp
#include "FoodItem.h"
#include <QPainter>

FoodItem::FoodItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
}

QRectF FoodItem::boundingRect() const {
    int s = cellSize;
    return QRectF(-s / 2, -s / 2, s, s);
}

void FoodItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    int r = radius;
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#ff4466"));
    painter->drawEllipse(QPointF(0, 0), r, r);
}
```

- [ ] **Step 3: 编译**

```bash
cd build && cmake --build .
```

- [ ] **Step 4: 提交**

```bash
git add src/ui/SnakeItem.h src/ui/SnakeItem.cpp src/ui/FoodItem.h src/ui/FoodItem.cpp
git commit -m "feat: SnakeItem and FoodItem QGraphicsItem rendering"
```

---

### Task 7: ui/GameScene + GameView

**Files:**
- Modify: `src/ui/GameScene.h`
- Modify: `src/ui/GameScene.cpp`
- Modify: `src/ui/GameView.h`
- Modify: `src/ui/GameView.cpp`

- [ ] **Step 1: 实现 GameScene**

替换 `src/ui/GameScene.h`:

```cpp
#pragma once
#include <QGraphicsScene>
#include <QMap>
#include "SnakeItem.h"
#include "FoodItem.h"
#include "Snake.h"
#include "Board.h"

class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene(QObject* parent = nullptr);

    void syncFromBoard(const Board& board, const std::vector<Snake*>& snakes);
    void clearAll();

private:
    QMap<int, QVector<SnakeItem*>> m_snakeItems;
    FoodItem* m_foodItem = nullptr;

    int cellToPixel(int cell) const;
};
```

替换 `src/ui/GameScene.cpp`:

```cpp
#include "GameScene.h"

GameScene::GameScene(QObject* parent) : QGraphicsScene(parent) {
    setBackgroundBrush(QColor("#06060f"));
}

void GameScene::syncFromBoard(const Board& board, const std::vector<Snake*>& snakes) {
    // 清除旧的蛇渲染
    for (auto& items : m_snakeItems) {
        for (auto* item : items) {
            removeItem(item);
            delete item;
        }
    }
    m_snakeItems.clear();

    // 绘制蛇
    for (size_t i = 0; i < snakes.size(); ++i) {
        QVector<SnakeItem*> items;
        const auto& body = snakes[i]->body();
        for (size_t j = 0; j < body.size(); ++j) {
            auto* item = new SnakeItem(j == 0);
            item->setPos(cellToPixel(body[j].x), cellToPixel(body[j].y));
            addItem(item);
            items.append(item);
        }
        m_snakeItems[static_cast<int>(i)] = items;
    }

    // 食物
    if (m_foodItem) { removeItem(m_foodItem); delete m_foodItem; }
    Food food = board.food();
    m_foodItem = new FoodItem();
    m_foodItem->setPos(cellToPixel(food.position.x), cellToPixel(food.position.y));
    addItem(m_foodItem);
}

void GameScene::clearAll() {
    clear();
    m_snakeItems.clear();
    m_foodItem = nullptr;
}

int GameScene::cellToPixel(int cell) const {
    return cell * SnakeItem::cellSize + SnakeItem::cellSize / 2;
}
```

- [ ] **Step 2: 实现 GameView**

替换 `src/ui/GameView.h`:

```cpp
#pragma once
#include <QGraphicsView>

class GameView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GameView(QWidget* parent = nullptr);
};
```

替换 `src/ui/GameView.cpp`:

```cpp
#include "GameView.h"

GameView::GameView(QWidget* parent) : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setBackgroundBrush(QColor("#06060f"));
    setFrameStyle(QFrame::NoFrame);
}
```

- [ ] **Step 3: 编译**

```bash
cd build && cmake --build .
```

- [ ] **Step 4: 提交**

```bash
git add src/ui/GameScene.h src/ui/GameScene.cpp src/ui/GameView.h src/ui/GameView.cpp
git commit -m "feat: GameScene sync from Board, GameView with dark background"
```

---

### Task 8: controller/GameController

**Files:**
- Modify: `src/controller/GameController.h`
- Modify: `src/controller/GameController.cpp`

- [ ] **Step 1: 实现 GameController**

替换 `src/controller/GameController.h`:

```cpp
#pragma once
#include <QObject>
#include <QTimer>
#include "Board.h"
#include "Snake.h"
#include "Direction.h"

class GameScene;

class GameController : public QObject {
    Q_OBJECT
public:
    enum class State { Idle, Ready, Countdown, Playing, Paused, GameOver };

    explicit GameController(GameScene* scene, QObject* parent = nullptr);

    void startGame(int boardW = 20, int boardH = 20, int speedMs = 100);
    void pause();
    void resume();
    void reset();
    void handleKeyPress(Direction dir);
    void handleReadyKey();

    State state() const;
    Board* board();
    Snake* playerSnake();
    int score() const;

signals:
    void stateChanged(State newState);
    void countdownTick(int number);  // 3, 2, 1, 0=GO
    void scoreChanged(int score);

private slots:
    void tick();

private:
    GameScene* m_scene;
    QTimer* m_timer;
    Board* m_board = nullptr;
    Snake* m_playerSnake = nullptr;
    State m_state = State::Idle;
    int m_score = 0;
    int m_countdownValue = 3;
};
```

替换 `src/controller/GameController.cpp`:

```cpp
#include "GameController.h"
#include "GameScene.h"
#include "Logger.h"

GameController::GameController(GameScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &GameController::tick);
}

void GameController::startGame(int boardW, int boardH, int speedMs) {
    delete m_board;
    delete m_playerSnake;

    m_board = new Board(boardW, boardH);
    m_playerSnake = new Snake(Point{boardW / 2, boardH / 2});
    m_score = 0;
    m_board->spawnFood();
    m_state = State::Ready;
    emit stateChanged(m_state);
    emit scoreChanged(m_score);

    std::vector<Snake*> snakes = {m_playerSnake};
    m_scene->syncFromBoard(*m_board, snakes);

    LOG_INFO("GameController", "Game started, waiting for ready key");
}

void GameController::handleReadyKey() {
    if (m_state != State::Ready) return;
    m_state = State::Countdown;
    m_countdownValue = 3;
    emit stateChanged(m_state);
    emit countdownTick(m_countdownValue);

    // 倒计时用 QTimer::singleShot 链式调用
    auto* countdownTimer = new QTimer(this);
    countdownTimer->setInterval(700);
    connect(countdownTimer, &QTimer::timeout, this, [this, countdownTimer]() {
        m_countdownValue--;
        if (m_countdownValue > 0) {
            emit countdownTick(m_countdownValue);
        } else {
            emit countdownTick(0); // GO
            countdownTimer->stop();
            countdownTimer->deleteLater();
            m_state = State::Playing;
            emit stateChanged(m_state);
            m_timer->start(m_board ? 100 : 100); // 使用默认速度
            LOG_INFO("GameController", "Game playing");
        }
    });
    countdownTimer->start();
}

void GameController::pause() {
    if (m_state != State::Playing) return;
    m_timer->stop();
    m_state = State::Paused;
    emit stateChanged(m_state);
}

void GameController::resume() {
    if (m_state != State::Paused) return;
    m_state = State::Playing;
    emit stateChanged(m_state);
    m_timer->start();
}

void GameController::reset() {
    m_timer->stop();
    m_state = State::Idle;
    emit stateChanged(m_state);
}

void GameController::handleKeyPress(Direction dir) {
    if (m_state == State::Playing && m_playerSnake) {
        m_playerSnake->setDirection(dir);
    }
}

GameController::State GameController::state() const { return m_state; }
Board* GameController::board() { return m_board; }
Snake* GameController::playerSnake() { return m_playerSnake; }
int GameController::score() const { return m_score; }

void GameController::tick() {
    if (!m_board || !m_playerSnake) return;
    if (m_state != State::Playing) return;

    m_playerSnake->move();

    if (m_board->checkWallCollision(*m_playerSnake) ||
        m_board->checkSelfCollision(*m_playerSnake)) {
        m_timer->stop();
        m_state = State::GameOver;
        emit stateChanged(m_state);
        LOG_INFO("GameController", "Game over, final score: " + std::to_string(m_score));
        return;
    }

    if (m_board->checkFoodCollision(*m_playerSnake)) {
        m_playerSnake->grow();
        m_score += m_board->food().points;
        emit scoreChanged(m_score);
        m_board->spawnFood();
    }

    std::vector<Snake*> snakes = {m_playerSnake};
    m_scene->syncFromBoard(*m_board, snakes);
}
```

- [ ] **Step 2: 编译**

```bash
cd build && cmake --build .
```

- [ ] **Step 3: 提交**

```bash
git add src/controller/GameController.h src/controller/GameController.cpp
git commit -m "feat: GameController with tick loop, countdown, and game state machine"
```

---

### Task 9: audio/AudioManager 空壳

**Files:**
- Modify: `src/audio/AudioManager.h`
- Modify: `src/audio/AudioManager.cpp`

替换 `src/audio/AudioManager.h`:

```cpp
#pragma once

enum class SoundEffect { Eat, Die, MenuClick, Countdown, Go };

class AudioManager {
public:
    static AudioManager& instance();
    void play(SoundEffect effect);
    void setVolume(int vol); // 0-100
    int volume() const;

private:
    AudioManager() = default;
    int m_volume = 50;
};
```

替换 `src/audio/AudioManager.cpp`:

```cpp
#include "AudioManager.h"

AudioManager& AudioManager::instance() {
    static AudioManager inst;
    return inst;
}

void AudioManager::play(SoundEffect) {
    // 阶段 4 对接 QSoundEffect
}

void AudioManager::setVolume(int vol) {
    m_volume = (vol < 0) ? 0 : (vol > 100 ? 100 : vol);
}

int AudioManager::volume() const { return m_volume; }
```

编译后提交：

```bash
cd build && cmake --build .
git add src/audio/AudioManager.h src/audio/AudioManager.cpp
git commit -m "feat: AudioManager shell interface"
```

---

### Task 10: resources/QSS + .qrc

**Files:**
- Create: `src/resources/resources.qrc`
- Create: `src/resources/style/main.qss`

- [ ] **Step 1: 写 QSS 全局样式**

`src/resources/style/main.qss`:

```css
/* 全局 */
QWidget {
    background-color: #06060f;
    color: #ccc;
    font-family: "Consolas", "Courier New", monospace;
}

/* 按钮 */
QPushButton {
    background-color: #0d0d28;
    border: 1px solid #222255;
    border-radius: 5px;
    color: #ccc;
    padding: 9px 0;
    font-size: 14px;
    min-width: 200px;
}
QPushButton:hover {
    border-color: #00ff88;
    color: #00ff88;
}
QPushButton:pressed {
    background-color: #0a0a1e;
}
QPushButton#btnDanger {
    border-color: #332233;
    color: #ff6688;
}
QPushButton#btnDanger:hover {
    border-color: #ff6688;
}
QPushButton#btnPrimary {
    color: #00ff88;
    border-color: #225544;
}
QPushButton#btnPrimary:hover {
    border-color: #00ff88;
}

/* 标签 */
QLabel {
    color: #ccc;
    background: transparent;
}
QLabel#titleLabel {
    color: #e0e0ff;
    font-size: 28px;
    letter-spacing: 8px;
}
QLabel#subtitleLabel {
    color: #556;
    font-size: 11px;
}
QLabel#scoreLabel {
    color: #00ff88;
    font-size: 36px;
    font-weight: bold;
}

/* 输入框 */
QLineEdit {
    background-color: #08081a;
    border: 1px solid #1a1a4e;
    border-radius: 4px;
    color: #0ff;
    padding: 7px 12px;
    font-size: 14px;
}
QLineEdit:focus {
    border-color: #00ff88;
}

/* 浮层面板 */
QWidget#overlay {
    background-color: rgba(6, 6, 15, 240);
}
QWidget#panel {
    background-color: #0a0a1a;
    border: 1px solid #1a1a4e;
    border-radius: 10px;
}
```

- [ ] **Step 2: 写 .qrc 资源文件**

`src/resources/resources.qrc`:

```xml
<RCC>
    <qresource prefix="/">
        <file>style/main.qss</file>
    </qresource>
</RCC>
```

- [ ] **Step 3: 更新顶层 CMakeLists.txt 加入 resources**

在顶层 CMakeLists.txt 的 `add_subdirectory(tests)` 之前添加：

```cmake
set(RESOURCE_FILE src/resources/resources.qrc)
```

稍后在 main.cpp 中通过 Qt resource 系统加载 QSS。

- [ ] **Step 4: 提交**

```bash
git add src/resources/
git commit -m "feat: Global QSS stylesheet and Qt resource file"
```

---

### Task 11: app/AppShell + 浮层窗口

**Files:**
- Modify: `src/app/AppShell.h`
- Modify: `src/app/AppShell.cpp`
- Create: `src/main.cpp`

- [ ] **Step 1: 实现 AppShell**

替换 `src/app/AppShell.h`:

```cpp
#pragma once
#include <QWidget>
#include <QStackedLayout>
#include <QKeyEvent>
#include "GameController.h"
#include "GameView.h"
#include "GameScene.h"

class MainMenuWidget;
class PauseWidget;
class GameOverWidget;
class SettingsWidget;

class AppShell : public QWidget {
    Q_OBJECT
public:
    explicit AppShell(QWidget* parent = nullptr);
    ~AppShell();

    void showMainMenu();
    void showPause();
    void showGameOver();
    void showSettings();
    void hideAllOverlays();

    void startSinglePlayer();
    void startAIGame(int aiCount, int difficulty);
    void resetGame();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onControllerStateChanged(GameController::State state);

private:
    void setupUI();

    GameScene* m_scene;
    GameView* m_view;
    GameController* m_controller;

    MainMenuWidget* m_mainMenu;
    PauseWidget* m_pauseWidget;
    GameOverWidget* m_gameOver;
    SettingsWidget* m_settingsWidget;
};
```

替换 `src/app/AppShell.cpp`:

```cpp
#include "AppShell.h"
#include "MainMenuWidget.h"
#include "PauseWidget.h"
#include "GameOverWidget.h"
#include "SettingsWidget.h"
#include "AudioManager.h"
#include "Logger.h"
#include <QVBoxLayout>
#include <QApplication>

AppShell::AppShell(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Snake Arena");
    setMinimumSize(800, 600);

    m_scene = new GameScene(this);
    m_view = new GameView(this);
    m_view->setScene(m_scene);

    m_controller = new GameController(m_scene, this);

    m_mainMenu = new MainMenuWidget(this);
    m_pauseWidget = new PauseWidget(this);
    m_gameOver = new GameOverWidget(this);
    m_settingsWidget = new SettingsWidget(this);

    setupUI();

    connect(m_controller, &GameController::stateChanged,
            this, &AppShell::onControllerStateChanged);

    // 连接浮层信号
    connect(m_mainMenu, &MainMenuWidget::singlePlayerClicked,
            this, &AppShell::startSinglePlayer);
    connect(m_mainMenu, &MainMenuWidget::settingsClicked,
            this, &AppShell::showSettings);
    connect(m_pauseWidget, &PauseWidget::resumeClicked,
            this, [this]() { m_controller->resume(); hideAllOverlays(); });
    connect(m_pauseWidget, &PauseWidget::restartClicked,
            this, [this]() { hideAllOverlays(); startSinglePlayer(); });
    connect(m_pauseWidget, &PauseWidget::menuClicked,
            this, [this]() { m_controller->reset(); showMainMenu(); });
    connect(m_pauseWidget, &PauseWidget::settingsClicked,
            this, &AppShell::showSettings);
    connect(m_gameOver, &GameOverWidget::playAgainClicked,
            this, [this]() { hideAllOverlays(); startSinglePlayer(); });
    connect(m_gameOver, &GameOverWidget::menuClicked,
            this, [this]() { m_controller->reset(); showMainMenu(); });

    connect(m_settingsWidget, &SettingsWidget::backClicked,
            this, [this]() { hideAllOverlays(); showMainMenu(); });

    LOG_INFO("AppShell", "AppShell initialized");
    showMainMenu();
}

AppShell::~AppShell() {
    delete m_controller;
}

void AppShell::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);

    // 所有浮层覆盖在 game view 上方
    m_mainMenu->setGeometry(0, 0, width(), height());
    m_pauseWidget->setGeometry(0, 0, width(), height());
    m_gameOver->setGeometry(0, 0, width(), height());
    m_settingsWidget->setGeometry(0, 0, width(), height());

    m_mainMenu->raise();
}

void AppShell::showMainMenu() {
    hideAllOverlays();
    m_mainMenu->show();
    m_mainMenu->raise();
}

void AppShell::showPause() {
    hideAllOverlays();
    m_pauseWidget->show();
    m_pauseWidget->raise();
}

void AppShell::showGameOver() {
    hideAllOverlays();
    m_gameOver->show();
    m_gameOver->raise();
}

void AppShell::showSettings() {
    hideAllOverlays();
    m_settingsWidget->show();
    m_settingsWidget->raise();
}

void AppShell::hideAllOverlays() {
    m_mainMenu->hide();
    m_pauseWidget->hide();
    m_gameOver->hide();
    m_settingsWidget->hide();
}

void AppShell::startSinglePlayer() {
    hideAllOverlays();
    m_controller->startGame(20, 20, 100);
}

void AppShell::startAIGame(int, int) {
    // 阶段 2 实现
}

void AppShell::resetGame() {
    m_controller->reset();
    showMainMenu();
}

void AppShell::keyPressEvent(QKeyEvent* event) {
    if (m_controller->state() == GameController::State::Ready) {
        if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
            m_controller->handleReadyKey();
            return;
        }
    }

    if (m_controller->state() == GameController::State::Playing) {
        switch (event->key()) {
            case Qt::Key_Up:    m_controller->handleKeyPress(Direction::Up);    break;
            case Qt::Key_Down:  m_controller->handleKeyPress(Direction::Down);  break;
            case Qt::Key_Left:  m_controller->handleKeyPress(Direction::Left);  break;
            case Qt::Key_Right: m_controller->handleKeyPress(Direction::Right); break;
            case Qt::Key_Escape: showPause(); break;
        }
    }

    if (m_controller->state() == GameController::State::Paused) {
        if (event->key() == Qt::Key_Escape) {
            m_controller->resume();
            hideAllOverlays();
        }
    }

    QWidget::keyPressEvent(event);
}

void AppShell::onControllerStateChanged(GameController::State state) {
    switch (state) {
        case GameController::State::GameOver:
            showGameOver();
            break;
        case GameController::State::Ready:
        case GameController::State::Playing:
            hideAllOverlays();
            break;
        default:
            break;
    }
}
```

- [ ] **Step 2: 实现 main.cpp**

`src/main.cpp`:

```cpp
#include <QApplication>
#include <QFile>
#include "AppShell.h"
#include "Logger.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 加载 QSS
    QFile qss(":/style/main.qss");
    if (qss.open(QFile::ReadOnly)) {
        app.setStyleSheet(qss.readAll());
        qss.close();
    } else {
        LOG_WARN("main", "Failed to load QSS stylesheet");
    }

    AppShell shell;
    shell.show();

    return app.exec();
}
```

更新顶层 CMakeLists.txt，在 `add_subdirectory(tests)` 之前添加：

```cmake
add_executable(SnakeArena
    src/main.cpp
    ${RESOURCE_FILE}
)
target_link_libraries(SnakeArena PRIVATE snake_app snake_controller snake_ui snake_audio snake_logging Qt5::Widgets)
```

- [ ] **Step 3: 编译并运行**

```bash
cd build && cmake .. && cmake --build .
```

期望：编译通过，双击 SnakeArena.exe 能看到主菜单，点击单人经典能进入游戏。

- [ ] **Step 4: 提交**

```bash
git add src/app/AppShell.h src/app/AppShell.cpp src/main.cpp CMakeLists.txt
git commit -m "feat: AppShell with overlay management and main.cpp entry point"
```

---

### Task 12: app/MainMenuWidget

**Files:**
- Modify: `src/app/MainMenuWidget.h`
- Modify: `src/app/MainMenuWidget.cpp`

替换 `src/app/MainMenuWidget.h`:

```cpp
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>

class MainMenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainMenuWidget(QWidget* parent = nullptr);

signals:
    void singlePlayerClicked();
    void aiBattleClicked();
    void multiplayerClicked();
    void replayClicked();
    void settingsClicked();
    void exitClicked();
};
```

替换 `src/app/MainMenuWidget.cpp`:

```cpp
#include "MainMenuWidget.h"
#include <QVBoxLayout>

MainMenuWidget::MainMenuWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // 标题装饰线
    auto* lineWidget = new QWidget(this);
    lineWidget->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 transparent,stop:0.5 #00ff8866,stop:1 transparent); height: 1px;");

    // 标题
    auto* title = new QLabel("SNAKE ARENA", this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);

    auto* subtitle = new QLabel("MULTI-MODE BATTLE PLATFORM", this);
    subtitle->setObjectName("subtitleLabel");
    subtitle->setAlignment(Qt::AlignCenter);

    auto* hint = new QLabel("SELECT MODE", this);
    hint->setStyleSheet("color: #0ff; font-size: 10px; border: 1px solid #1a1a3e; border-radius: 10px; padding: 3px 12px;");
    hint->setAlignment(Qt::AlignCenter);

    // 按钮
    auto* btnSingle = new QPushButton(tr("单人经典模式"), this);
    auto* btnAI = new QPushButton(tr("AI 对战模式"), this);
    auto* btnMulti = new QPushButton(tr("联机对战模式"), this);
    auto* btnReplay = new QPushButton(tr("对局回放"), this);

    connect(btnSingle, &QPushButton::clicked, this, &MainMenuWidget::singlePlayerClicked);
    connect(btnAI, &QPushButton::clicked, this, &MainMenuWidget::aiBattleClicked);
    connect(btnMulti, &QPushButton::clicked, this, &MainMenuWidget::multiplayerClicked);
    connect(btnReplay, &QPushButton::clicked, this, &MainMenuWidget::replayClicked);

    // 底部链接
    auto* bottomLayout = new QHBoxLayout();
    auto* btnSettings = new QPushButton(tr("[ 设置 ]"), this);
    auto* btnExit = new QPushButton(tr("[ 退出 ]"), this);
    btnSettings->setStyleSheet("QPushButton { background: transparent; border: none; color: #556; font-size: 11px; min-width: 0; padding: 0; } QPushButton:hover { color: #00ff88; }");
    btnExit->setStyleSheet("QPushButton { background: transparent; border: none; color: #556; font-size: 11px; min-width: 0; padding: 0; } QPushButton:hover { color: #ff6688; }");
    connect(btnSettings, &QPushButton::clicked, this, &MainMenuWidget::settingsClicked);
    connect(btnExit, &QPushButton::clicked, this, &MainMenuWidget::exitClicked);
    bottomLayout->addStretch();
    bottomLayout->addWidget(btnSettings);
    bottomLayout->addWidget(btnExit);
    bottomLayout->addStretch();

    layout->addWidget(lineWidget);
    layout->addSpacing(16);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(12);
    layout->addWidget(hint, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(btnSingle, 0, Qt::AlignCenter);
    layout->addWidget(btnAI, 0, Qt::AlignCenter);
    layout->addWidget(btnMulti, 0, Qt::AlignCenter);
    layout->addWidget(btnReplay, 0, Qt::AlignCenter);
    layout->addSpacing(24);
    layout->addLayout(bottomLayout);
    layout->addStretch();

    hide();
}
```

编译提交：

```bash
cd build && cmake --build .
git add src/app/MainMenuWidget.h src/app/MainMenuWidget.cpp
git commit -m "feat: MainMenuWidget with mode buttons and settings/exit links"
```

---

### Task 13: app/PauseWidget + GameOverWidget + SettingsWidget

**Files:**
- Modify: `src/app/PauseWidget.h` / `.cpp`
- Modify: `src/app/GameOverWidget.h` / `.cpp`
- Modify: `src/app/SettingsWidget.h` / `.cpp`

这三个组件比较相似，做在一个 Task 里。

- [ ] **Step 1: PauseWidget**

`src/app/PauseWidget.h`:

```cpp
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>

class PauseWidget : public QWidget {
    Q_OBJECT
public:
    explicit PauseWidget(QWidget* parent = nullptr);

signals:
    void resumeClicked();
    void restartClicked();
    void settingsClicked();
    void menuClicked();
};
```

`src/app/PauseWidget.cpp`:

```cpp
#include "PauseWidget.h"
#include <QVBoxLayout>

PauseWidget::PauseWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(tr("游戏暂停"), this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);

    auto* btnResume = new QPushButton(tr("继续游戏"), this);
    btnResume->setObjectName("btnPrimary");

    auto* btnRestart = new QPushButton(tr("重新开始"), this);
    auto* btnSettings = new QPushButton(tr("设置"), this);

    auto* btnMenu = new QPushButton(tr("返回主菜单"), this);
    btnMenu->setObjectName("btnDanger");

    connect(btnResume, &QPushButton::clicked, this, &PauseWidget::resumeClicked);
    connect(btnRestart, &QPushButton::clicked, this, &PauseWidget::restartClicked);
    connect(btnSettings, &QPushButton::clicked, this, &PauseWidget::settingsClicked);
    connect(btnMenu, &QPushButton::clicked, this, &PauseWidget::menuClicked);

    layout->addWidget(title);
    layout->addSpacing(20);
    layout->addWidget(btnResume, 0, Qt::AlignCenter);
    layout->addWidget(btnRestart, 0, Qt::AlignCenter);
    layout->addWidget(btnSettings, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(btnMenu, 0, Qt::AlignCenter);
    layout->addStretch();

    hide();
}
```

- [ ] **Step 2: GameOverWidget**

`src/app/GameOverWidget.h`:

```cpp
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>

class GameOverWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameOverWidget(QWidget* parent = nullptr);
    void setScore(int score, int length, int kills, int seconds);

signals:
    void playAgainClicked();
    void saveReplayClicked();
    void menuClicked();
};
```

`src/app/GameOverWidget.cpp`:

```cpp
#include "GameOverWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

GameOverWidget::GameOverWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(tr("游戏结束"), this);
    title->setStyleSheet("color: #ff6688; font-size: 18px; letter-spacing: 6px;");

    auto* scoreLabel = new QLabel(this);
    scoreLabel->setObjectName("scoreLabel");
    scoreLabel->setAlignment(Qt::AlignCenter);

    auto* statsLabel = new QLabel(this);
    statsLabel->setObjectName("subtitleLabel");
    statsLabel->setAlignment(Qt::AlignCenter);

    auto* btnAgain = new QPushButton(tr("再来一局"), this);
    btnAgain->setObjectName("btnPrimary");

    auto* btnSave = new QPushButton(tr("保存回放"), this);
    auto* btnMenu = new QPushButton(tr("返回主菜单"), this);
    btnMenu->setObjectName("btnDanger");

    connect(btnAgain, &QPushButton::clicked, this, &GameOverWidget::playAgainClicked);
    connect(btnSave, &QPushButton::clicked, this, &GameOverWidget::saveReplayClicked);
    connect(btnMenu, &QPushButton::clicked, this, &GameOverWidget::menuClicked);

    layout->addWidget(title);
    layout->addSpacing(8);
    layout->addWidget(scoreLabel);
    layout->addWidget(statsLabel);
    layout->addSpacing(16);
    layout->addWidget(btnAgain, 0, Qt::AlignCenter);
    layout->addWidget(btnSave, 0, Qt::AlignCenter);
    layout->addSpacing(12);
    layout->addWidget(btnMenu, 0, Qt::AlignCenter);
    layout->addStretch();

    hide();
}

void GameOverWidget::setScore(int score, int length, int kills, int seconds) {
    auto* scoreLabel = findChild<QLabel*>("scoreLabel");
    if (scoreLabel) scoreLabel->setText(QString::number(score));

    int min = seconds / 60;
    int sec = seconds % 60;
    auto* statsLabel = findChild<QLabel*>("subtitleLabel");
    if (statsLabel) statsLabel->setText(
        tr("长度: %1  |  击杀: %2  |  时间: %3:%4")
            .arg(length).arg(kills).arg(min).arg(sec, 2, 10, QChar('0')));
}
```

- [ ] **Step 3: SettingsWidget（仅操作标签部分，游戏部分后续补充）**

`src/app/SettingsWidget.h`:

```cpp
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget* parent = nullptr);

signals:
    void backClicked();
};
```

`src/app/SettingsWidget.cpp`:

```cpp
#include "SettingsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QKeySequenceEdit>
#include <QComboBox>

SettingsWidget::SettingsWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("overlay");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(tr("设置"), this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);

    auto* tabWidget = new QTabWidget(this);

    // === 操作标签 ===
    auto* controlTab = new QWidget(this);
    auto* controlLayout = new QVBoxLayout(controlTab);

    auto* keyLabel = new QLabel(tr("移动键位:"), controlTab);
    auto* keyCombo = new QComboBox(controlTab);
    keyCombo->addItem(tr("方向键"));
    keyCombo->addItem("WASD");

    auto* langLabel = new QLabel(tr("语言 / Language:"), controlTab);
    auto* langCombo = new QComboBox(controlTab);
    langCombo->addItem(tr("中文"));
    langCombo->addItem("English");

    controlLayout->addWidget(keyLabel);
    controlLayout->addWidget(keyCombo);
    controlLayout->addWidget(langLabel);
    controlLayout->addWidget(langCombo);
    controlLayout->addStretch();

    // === 游戏标签 ===
    auto* gameTab = new QWidget(this);
    auto* gameLayout = new QVBoxLayout(gameTab);

    auto* speedLabel = new QLabel(tr("游戏速度:"), gameTab);
    auto* speedCombo = new QComboBox(gameTab);
    speedCombo->addItems({tr("慢"), tr("中"), tr("快")});
    speedCombo->setCurrentIndex(1);

    auto* sizeLabel = new QLabel(tr("棋盘大小:"), gameTab);
    auto* sizeCombo = new QComboBox(gameTab);
    sizeCombo->addItems({tr("小 (15x15)"), tr("中 (20x20)"), tr("大 (30x30)")});
    sizeCombo->setCurrentIndex(1);

    auto* volLabel = new QLabel(tr("音量:"), gameTab);
    auto* volSlider = new QSlider(Qt::Horizontal, gameTab);
    volSlider->setRange(0, 100);
    volSlider->setValue(50);

    gameLayout->addWidget(speedLabel);
    gameLayout->addWidget(speedCombo);
    gameLayout->addWidget(sizeLabel);
    gameLayout->addWidget(sizeCombo);
    gameLayout->addWidget(volLabel);
    gameLayout->addWidget(volSlider);
    // 网格开关和食物闪烁后续 phase 补充
    gameLayout->addStretch();

    tabWidget->addTab(controlTab, tr("操作"));
    tabWidget->addTab(gameTab, tr("游戏"));

    auto* btnBack = new QPushButton(tr("返回"), this);

    connect(btnBack, &QPushButton::clicked, this, &SettingsWidget::backClicked);

    mainLayout->addWidget(title);
    mainLayout->addSpacing(16);
    mainLayout->addWidget(tabWidget, 0, Qt::AlignCenter);
    mainLayout->addSpacing(16);
    mainLayout->addWidget(btnBack, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    hide();
}
```

- [ ] **Step 4: 编译**

```bash
cd build && cmake --build .
```

- [ ] **Step 5: 提交**

```bash
git add src/app/PauseWidget.h src/app/PauseWidget.cpp src/app/GameOverWidget.h src/app/GameOverWidget.cpp src/app/SettingsWidget.h src/app/SettingsWidget.cpp
git commit -m "feat: PauseWidget, GameOverWidget and SettingsWidget overlays"
```

---

### Task 14: 整合测试 + 可玩验证

- [ ] **Step 1: 编译运行**

```bash
cd build && cmake .. && cmake --build .
./SnakeArena    # Windows: SnakeArena.exe
```

手动验证流程：
1. 启动程序 → 看到主菜单（黑底霓虹风）
2. 点击"单人经典模式" → 主菜单消失，棋盘出现
3. 按空格或回车 → 3-2-1-GO 倒计时
4. 方向键操控蛇 → 吃食物得分
5. 撞墙或撞自己 → 结算弹窗显示分数
6. 点"再来一局"重新开始
7. 游戏中 ESC → 暂停弹窗

- [ ] **Step 2: 运行单元测试**

```bash
cd build && ctest --output-on-failure
```

期望：test_snake (9 passed) + test_board (7 passed) + test_core (1 passed) = 17 passed。

- [ ] **Step 3: 提交**

```bash
git add -A && git commit -m "feat: Complete Phase 1 - playable single player with full UI flow"
```
