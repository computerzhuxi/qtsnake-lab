#include "ShineLabel.h"
#include <QPainter>
#include <QTimer>
#include <QLinearGradient>

ShineLabel::ShineLabel(QWidget* parent) : QLabel(parent), m_phase(0.0), m_active(false) {
    m_timer = new QTimer(this);
    m_timer->setInterval(33);

    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_phase += 1.0 / 120.0;
        if (m_phase >= 1.0)
            m_phase = 0.0;
        update();
    });
}

void ShineLabel::startShine() {
    m_active = true;
    m_phase = 0.0;
    m_timer->start();
}

void ShineLabel::stopShine() {
    m_active = false;
    m_timer->stop();
    update();
}

void ShineLabel::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect cr = contentsRect();

    if (!m_active) {
        painter.setPen(QColor(0x55, 0x66, 0x66));
        painter.setFont(font());
        painter.drawText(cr, alignment(), text());
        return;
    }

    qreal w = cr.width();
    qreal span = w * 3.0;
    qreal offset = m_phase * w - span * 0.47;

    QLinearGradient gradient(cr.left() + offset, 0,
                             cr.left() + offset + span, 0);
    gradient.setColorAt(0.00, QColor(0x55, 0x66, 0x66));
    gradient.setColorAt(0.40, QColor(0x55, 0x66, 0x66));
    gradient.setColorAt(0.44, QColor(0x1a, 0x3a, 0x2a));
    gradient.setColorAt(0.47, QColor(0x00, 0xff, 0x88));
    gradient.setColorAt(0.50, QColor(0x00, 0xff, 0x88));
    gradient.setColorAt(0.53, QColor(0x1a, 0x3a, 0x2a));
    gradient.setColorAt(0.56, QColor(0x55, 0x66, 0x66));
    gradient.setColorAt(1.00, QColor(0x55, 0x66, 0x66));

    painter.setPen(QPen(QBrush(gradient), 0));
    painter.setFont(font());
    painter.drawText(cr, alignment(), text());
}
