#ifndef APP_NETWORK_HEARTBEAT_H
#define APP_NETWORK_HEARTBEAT_H

#include <QObject>
#include <QTimer>

class Heartbeat : public QObject {
    Q_OBJECT

public:
    explicit Heartbeat(int intervalMs, int timeoutMs,
                       QObject* parent = nullptr);

    void start();
    void stop();
    void onPongReceived();

signals:
    void timeout();
    void pingNeeded(int seq);

private slots:
    void onTimer();

private:
    QTimer* timer_;
    int timeoutMs_;
    int seq_ = 0;
    bool waitingForPong_ = false;
};

#endif // APP_NETWORK_HEARTBEAT_H
