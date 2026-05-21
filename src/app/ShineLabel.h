#ifndef SNAKE_APP_SHINELABEL_H
#define SNAKE_APP_SHINELABEL_H

#include <QLabel>

class QTimer;

/// \brief 反光文字标签
/// \details paintEvent 中用 QLinearGradient + QTimer 驱动亮带从左向右扫过文字。
///          stopShine() 后重绘为静止暗色。

class ShineLabel : public QLabel {
    Q_OBJECT
public:
    explicit ShineLabel(QWidget* parent = nullptr);

    /// \brief 启动反光动画（循环）
    void startShine();

    /// \brief 停止动画，重绘为静止暗色文字
    void stopShine();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QTimer* m_timer;
    qreal m_phase;
    bool m_active;
};

#endif // SNAKE_APP_SHINELABEL_H
