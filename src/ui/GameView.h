#pragma once
#include <QGraphicsView>

/// \brief 游戏视图：配置抗锯齿、无滚动条、暗色背景、全视口刷新
class GameView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GameView(QWidget* parent = nullptr);
};
