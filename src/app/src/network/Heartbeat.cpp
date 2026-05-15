#include "app/network/Heartbeat.h"

Heartbeat::Heartbeat(int intervalMs, int timeoutMs, QObject* parent)
    : QObject(parent)
    , timer_(new QTimer(this))
    , timeoutMs_(timeoutMs)
{
    connect(timer_, &QTimer::timeout, this, &Heartbeat::onTimer);
}

void Heartbeat::start() {
    seq_ = 0;
    waitingForPong_ = false;
    timer_->start(timeoutMs_ / 3); // check 3x per timeout period
}

void Heartbeat::stop() {
    timer_->stop();
}

void Heartbeat::onPongReceived() {
    waitingForPong_ = false;
}

void Heartbeat::onTimer() {
    if (waitingForPong_) {
        emit timeout();
        return;
    }
    ++seq_;
    waitingForPong_ = true;
    emit pingNeeded(seq_);
}
