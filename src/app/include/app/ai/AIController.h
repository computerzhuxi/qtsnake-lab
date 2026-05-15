#ifndef APP_AI_AICONTROLLER_H
#define APP_AI_AICONTROLLER_H

#include <QObject>
#include <QPoint>
#include <QThread>
#include <QTimer>
#include <QVector>

#include "core/engine/Direction.h"

class AIWorker;
class GameControllerAdapter;

class AIController : public QObject {
    Q_OBJECT

public:
    explicit AIController(GameControllerAdapter* adapter,
                          QObject* parent = nullptr);
    ~AIController() override;

    void start();
    void stop();
    bool isRunning() const;

signals:
    void requestCompute(QPoint head, QPoint food,
                        QVector<QPoint> snakeBody,
                        int currentDir,
                        int gridWidth, int gridHeight);

private slots:
    void onMoveComputed(core::Direction dir);
    void onTick();

private:
    GameControllerAdapter* adapter_;
    QThread* workerThread_;
    AIWorker* worker_;
    QTimer* tickTimer_;
    bool running_ = false;
};

#endif // APP_AI_AICONTROLLER_H
