#include "app/GameWidget.h"
#include "app/GameControllerAdapter.h"

#include "core/engine/GameController.h"
#include "core/engine/Snake.h"
#include "core/engine/Grid.h"
#include "core/engine/Food.h"
#include "core/state/StateMachine.h"

#include <QKeyEvent>
#include <QPainter>
#include <QtMath>
#include <cmath>

GameWidget::GameWidget(GameControllerAdapter* adapter, QWidget* parent)
    : QWidget(parent)
    , adapter_(adapter)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(400, 300);

    connect(adapter_, &GameControllerAdapter::ticked,
            this, QOverload<>::of(&QWidget::update));
    connect(adapter_, &GameControllerAdapter::stateChanged,
            this, QOverload<>::of(&QWidget::update));
}

QSize GameWidget::minimumSizeHint() const {
    return QSize(400, 300);
}

QSize GameWidget::sizeHint() const {
    return QSize(800, 600);
}

double GameWidget::cellSize() const {
    const auto& config = adapter_->config();
    double cw = static_cast<double>(width())  / config.gridWidth;
    double ch = static_cast<double>(height()) / config.gridHeight;
    double cs = qMin(cw, ch);

    // Integer scaling: snap to whole pixels so each cell is exactly N pixels.
    // Prevents sub-pixel rendering blur on non-high-DPI displays.
    if (integerScaling_) {
        cs = std::floor(cs);
        if (cs < 1.0) cs = 1.0;
    }

    return cs;
}

QRectF GameWidget::cellRect(int x, int y) const {
    double cs = cellSize();
    double gridPixelW = cs * adapter_->config().gridWidth;
    double gridPixelH = cs * adapter_->config().gridHeight;
    double offsetX = (width()  - gridPixelW) / 2.0;
    double offsetY = (height() - gridPixelH) / 2.0;
    return QRectF(offsetX + x * cs, offsetY + y * cs, cs, cs);
}

void GameWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    drawBackground(painter);
    drawGrid(painter);
    drawFood(painter);
    drawSnake(painter);
    drawScore(painter);
    drawOverlay(painter);
}

void GameWidget::drawBackground(QPainter& painter) {
    painter.fillRect(rect(), bgColor_);
}

void GameWidget::drawGrid(QPainter& painter) {
    painter.setPen(QPen(gridColor_, 0.5));

    const auto& config = adapter_->config();
    double cs = cellSize();
    double gw = cs * config.gridWidth;
    double gh = cs * config.gridHeight;
    double ox = (width()  - gw) / 2.0;
    double oy = (height() - gh) / 2.0;

    // Vertical lines
    for (int x = 0; x <= config.gridWidth; ++x) {
        double px = ox + x * cs;
        painter.drawLine(QPointF(px, oy), QPointF(px, oy + gh));
    }
    // Horizontal lines
    for (int y = 0; y <= config.gridHeight; ++y) {
        double py = oy + y * cs;
        painter.drawLine(QPointF(ox, py), QPointF(ox + gw, py));
    }
}

void GameWidget::drawSnake(QPainter& painter) {
    const auto& body = adapter_->controller().snake().body();

    for (size_t i = 0; i < body.size(); ++i) {
        const core::Point& seg = body[i];
        QRectF r = cellRect(seg.x, seg.y);
        double margin = cellSize() * 0.05;
        r.adjust(margin, margin, -margin, -margin);

        if (i == 0) {
            painter.fillRect(r, snakeHead_);
        } else {
            painter.fillRect(r, snakeBody_);
        }
    }
}

void GameWidget::drawFood(QPainter& painter) {
    core::Point pos = adapter_->controller().food().position();
    QRectF r = cellRect(pos.x, pos.y);
    double margin = cellSize() * 0.15;
    r.adjust(margin, margin, -margin, -margin);

    painter.setBrush(foodColor_);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(r);
}

void GameWidget::drawScore(QPainter& painter) {
    auto type = adapter_->stateMachine().currentType();
    if (type != core::StateType::Playing &&
        type != core::StateType::Paused &&
        type != core::StateType::GameOver) {
        return;
    }

    painter.setPen(textColor_);
    painter.setFont(QFont("Consolas", 14));
    QString scoreText = QString("Score: %1").arg(adapter_->score());
    painter.drawText(10, 25, scoreText);
}

void GameWidget::drawOverlay(QPainter& painter) {
    if (playbackActive_ || networkClient_) return;

    auto type = adapter_->stateMachine().currentType();

    if (type == core::StateType::Menu) {
        painter.fillRect(rect(), overlayColor_);
        painter.setPen(textColor_);
        painter.setFont(QFont("Consolas", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "QtSnake Lab\n\nPress Enter to Start");
    } else if (type == core::StateType::Paused) {
        painter.fillRect(rect(), overlayColor_);
        painter.setPen(textColor_);
        painter.setFont(QFont("Consolas", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "PAUSED\n\nPress P to Resume");
    } else if (type == core::StateType::GameOver) {
        painter.fillRect(rect(), overlayColor_);
        painter.setPen(textColor_);
        painter.setFont(QFont("Consolas", 24, QFont::Bold));
        QString text = QString("GAME OVER\n\nScore: %1\n\nPress Enter to Restart")
            .arg(adapter_->score());
        painter.drawText(rect(), Qt::AlignCenter, text);
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    auto currentType = adapter_->stateMachine().currentType();

    // Direction input
    if (currentType == core::StateType::Playing) {
        if (key == keyBindings_.keyUp) {
            adapter_->handleInput(core::Direction::Up);
            return;
        }
        if (key == keyBindings_.keyDown) {
            adapter_->handleInput(core::Direction::Down);
            return;
        }
        if (key == keyBindings_.keyLeft) {
            adapter_->handleInput(core::Direction::Left);
            return;
        }
        if (key == keyBindings_.keyRight) {
            adapter_->handleInput(core::Direction::Right);
            return;
        }
    }

    // Confirm action
    if (key == keyBindings_.keyConfirm) {
        if (currentType == core::StateType::Menu ||
            currentType == core::StateType::GameOver) {
            adapter_->startGame();
            return;
        }
    }

    // Pause action
    if (key == keyBindings_.keyPause) {
        if (currentType == core::StateType::Playing ||
            currentType == core::StateType::Paused) {
            adapter_->togglePause();
            return;
        }
    }

    // Back action
    if (key == keyBindings_.keyBack) {
        if (currentType == core::StateType::Playing ||
            currentType == core::StateType::Paused ||
            currentType == core::StateType::GameOver) {
            adapter_->returnToMenu();
            return;
        }
    }

    QWidget::keyPressEvent(event);
}
