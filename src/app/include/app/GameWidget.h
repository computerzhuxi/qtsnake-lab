#ifndef APP_GAMEWIDGET_H
#define APP_GAMEWIDGET_H

#include <QWidget>
#include <QColor>

#include "core/engine/Direction.h"
#include "core/engine/Point.h"
#include "app/settings/KeyBindings.h"

class GameControllerAdapter;

class GameWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameWidget(GameControllerAdapter* adapter, QWidget* parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setKeyBindings(const KeyBindings& bindings) { keyBindings_ = bindings; }
    const KeyBindings& keyBindings() const { return keyBindings_; }

    void setIntegerScaling(bool on) { integerScaling_ = on; }
    bool integerScaling() const { return integerScaling_; }

    void setPlaybackActive(bool active) { playbackActive_ = active; }
    void setNetworkClient(bool on) { networkClient_ = on; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    GameControllerAdapter* adapter_;
    KeyBindings keyBindings_;
    bool integerScaling_ = true;
    bool playbackActive_ = false;
    bool networkClient_ = false;

    void drawBackground(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawSnake(QPainter& painter);
    void drawFood(QPainter& painter);
    void drawScore(QPainter& painter);
    void drawOverlay(QPainter& painter);

    QRectF cellRect(int x, int y) const;
    double cellSize() const;

    // Colors
    QColor bgColor_      { 30,  30,  30 };
    QColor gridColor_    { 50,  50,  50 };
    QColor snakeHead_    { 0,   200, 0   };
    QColor snakeBody_    { 0,   150, 0   };
    QColor foodColor_    { 220, 50,  50  };
    QColor overlayColor_ { 0,   0,   0,  160 };
    QColor textColor_    { 255, 255, 255 };
};

#endif // APP_GAMEWIDGET_H
