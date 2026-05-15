#ifndef APP_AI_AIWORKER_H
#define APP_AI_AIWORKER_H

#include <QObject>
#include <QPoint>
#include <QVector>
#include <memory>

#include "core/ai/IPathfinder.h"
#include "core/engine/Direction.h"

class AIWorker : public QObject {
    Q_OBJECT

public:
    explicit AIWorker(QObject* parent = nullptr);
    void setPathfinder(std::unique_ptr<core::IPathfinder> pathfinder);

public slots:
    void computeNextMove(QPoint head, QPoint food,
                         QVector<QPoint> snakeBody,
                         int currentDir,
                         int gridWidth, int gridHeight);

signals:
    void moveComputed(core::Direction direction);

private:
    std::unique_ptr<core::IPathfinder> pathfinder_;
};

#endif // APP_AI_AIWORKER_H
