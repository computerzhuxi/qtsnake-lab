#ifndef SNAKE_UI_GAMEVIEW_H
#define SNAKE_UI_GAMEVIEW_H

#include <QGraphicsView>

/// \brief 游戏视图
/// \details 预配置：抗锯齿、隐藏滚动条、黑色背景、全视口刷新、无边框。
///          仅负责视口渲染配置，不涉及数据逻辑。

class GameView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GameView(QWidget* parent = nullptr);
};

#endif // SNAKE_UI_GAMEVIEW_H
