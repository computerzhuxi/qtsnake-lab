#include "app/ai/AIController.h"
#include "app/ai/AIWorker.h"
#include "app/GameControllerAdapter.h"

#include "core/ai/AStarPathfinder.h"
#include "core/engine/GameController.h"
#include "core/engine/Snake.h"
#include "core/engine/Food.h"

AIController::AIController(GameControllerAdapter* adapter, QObject* parent)
    : QObject(parent)
    , adapter_(adapter)
    , workerThread_(new QThread(this))
    , worker_(new AIWorker())
    , tickTimer_(new QTimer(this))
{
    worker_->setPathfinder(std::make_unique<core::AStarPathfinder>());
    worker_->moveToThread(workerThread_);

    connect(workerThread_, &QThread::finished, worker_, &QObject::deleteLater);

    // Controller requests → worker computes (queued, cross-thread)
    connect(this, &AIController::requestCompute,
            worker_, &AIWorker::computeNextMove,
            Qt::QueuedConnection);

    // Worker result → controller applies (queued, back to main thread)
    connect(worker_, &AIWorker::moveComputed,
            this, &AIController::onMoveComputed,
            Qt::QueuedConnection);

    connect(tickTimer_, &QTimer::timeout, this, &AIController::onTick);

    workerThread_->start();
}

AIController::~AIController() {
    stop();
    workerThread_->quit();
    workerThread_->wait();
}

void AIController::start() {
    running_ = true;
    tickTimer_->start(50); // Compute path every 50ms
}

void AIController::stop() {
    running_ = false;
    tickTimer_->stop();
}

bool AIController::isRunning() const {
    return running_;
}

void AIController::onTick() {
    if (!running_) return;
    if (adapter_->stateMachine().currentType() != core::StateType::Playing) return;

    const auto& ctrl = adapter_->controller();
    const auto& snake = ctrl.snake();

    QPoint qhead(snake.head().x, snake.head().y);
    QPoint qfood(ctrl.food().position().x, ctrl.food().position().y);

    QVector<QPoint> qbody;
    const auto& body = snake.body();
    for (size_t i = 1; i < body.size(); ++i) {
        qbody.append(QPoint(body[i].x, body[i].y));
    }

    emit requestCompute(qhead, qfood, qbody,
                        static_cast<int>(snake.direction()),
                        ctrl.grid().width(), ctrl.grid().height());
}

void AIController::onMoveComputed(core::Direction dir) {
    if (!running_) return;
    adapter_->handleInput(dir);
}
