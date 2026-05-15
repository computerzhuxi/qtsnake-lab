#include "app/network/NetworkManager.h"
#include "app/network/GameServer.h"
#include "app/network/GameClient.h"
#include "app/network/Heartbeat.h"
#include "app/network/Protocol.h"
#include "app/GameControllerAdapter.h"
#include "core/logging/Logger.h"

#include "core/engine/GameController.h"
#include "core/engine/Snake.h"
#include "core/engine/Food.h"

#include <QTimer>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
{
}

bool NetworkManager::hostGame(quint16 port, GameControllerAdapter& adapter) {
    LOG_INFO("NetMgr: hosting on port " + std::to_string(port));
    server_ = new GameServer(this);
    adapter_ = &adapter;

    connect(server_, &GameServer::messageReceived,
            this, &NetworkManager::onServerMessage);
    connect(server_, &GameServer::errorOccurred,
            this, &NetworkManager::errorOccurred);

    if (!server_->start(port)) return false;

    // Broadcast timer: poll controller state every 50ms
    auto* broadcastTimer = new QTimer(this);
    connect(broadcastTimer, &QTimer::timeout, this, &NetworkManager::onBroadcastTick);
    broadcastTimer->start(50);

    heartbeat_ = new Heartbeat(2000, 5000, this);
    connect(heartbeat_, &Heartbeat::pingNeeded, this, [this](int seq) {
        server_->broadcast(net::makeHeartbeat(seq));
    });
    connect(heartbeat_, &Heartbeat::timeout,
            this, &NetworkManager::onHeartbeatTimeout);
    // Heartbeat only after first client connects

    isHost_ = true;
    emit hostingStarted(port);
    LOG_INFO("NetMgr: hosting started");
    return true;
}

void NetworkManager::stopHosting() {
    LOG_INFO("NetMgr: stopping host");
    if (server_) server_->stop();
    if (heartbeat_) heartbeat_->stop();
    isHost_ = false;
    emit hostingStopped();
}

void NetworkManager::sendInput(core::Direction d) {
    if (isClient_ && client_ && adapter_) {
        client_->send(net::makeInput(d, adapter_->controller().tickNumber()));
    }
}

void NetworkManager::joinGame(const QString& host, quint16 port,
                               const QString& playerName,
                               GameControllerAdapter& adapter) {
    LOG_INFO("NetMgr: joining " + host.toStdString()
             + ":" + std::to_string(port) + " as " + playerName.toStdString());
    client_ = new GameClient(this);
    adapter_ = &adapter;

    connect(client_, &GameClient::messageReceived,
            this, &NetworkManager::onClientMessage);
    connect(client_, &GameClient::disconnected,
            this, [this]() { leaveGame(); });
    connect(client_, &GameClient::connectionError,
            this, &NetworkManager::errorOccurred);

    heartbeat_ = new Heartbeat(2000, 5000, this);
    connect(heartbeat_, &Heartbeat::pingNeeded, this, [this](int seq) {
        client_->send(net::makeHeartbeat(seq));
    });
    connect(heartbeat_, &Heartbeat::timeout,
            this, &NetworkManager::onHeartbeatTimeout);

    connect(client_, &GameClient::connected, this, [this]() {
        LOG_INFO("NetMgr: client TCP connected, waiting for JoinResponse");
        isClient_ = true;
    });

    client_->connectToHost(host, port, playerName);
}

void NetworkManager::leaveGame() {
    LOG_INFO("NetMgr: leaving game");
    if (client_) client_->disconnect();
    if (heartbeat_) heartbeat_->stop();
    isClient_ = false;
    emit left();
}

void NetworkManager::onBroadcastTick() {
    if (!isHost_ || !adapter_ || !server_) return;
    server_->poll(); // Accept any pending connections
    if (server_->clientCount() == 0) return;
    const auto& ctrl = adapter_->controller();
    auto body = ctrl.snake().body();
    std::vector<core::Point> snake(body.begin(), body.end());
    auto msg = net::makeGameState(snake, ctrl.food().position(),
                                   ctrl.score(), ctrl.tickNumber());
    server_->broadcast(msg);
}

void NetworkManager::onServerMessage(QTcpSocket* client,
                                      const QJsonObject& msg) {
    auto type = net::parseType(msg);
    switch (type) {
    case net::MsgType::JoinRequest: {
        LOG_INFO("NetMgr: received JoinRequest, sending JoinResponse");
        auto& ctrl = adapter_->controller();
        QJsonObject resp = net::makeJoinResponse(true,
            ctrl.grid().width(), ctrl.grid().height());
        server_->sendTo(client, resp);
        // Start heartbeat now that we have a client
        heartbeat_->start();
        break;
    }
    case net::MsgType::Input:
        adapter_->handleInput(net::parseDirection(msg));
        break;
    case net::MsgType::Heartbeat:
        server_->sendTo(client,
            net::makeHeartbeatAck(msg["seq"].toInt()));
        break;
    case net::MsgType::HeartbeatAck:
        heartbeat_->onPongReceived();
        break;
    default:
        break;
    }
}

void NetworkManager::onClientMessage(const QJsonObject& msg) {
    auto type = net::parseType(msg);
    LOG_INFO("NetMgr: client got msg type=" + std::to_string(static_cast<int>(type)));

    switch (type) {
    case net::MsgType::JoinResponse: {
        auto& ctrl = adapter_->controller();
        int w = msg["gridW"].toInt();
        int h = msg["gridH"].toInt();
        LOG_INFO("NetMgr: JoinResponse grid=" + std::to_string(w)
                 + "x" + std::to_string(h));
        if (w > 0 && h > 0) {
            core::GameConfig cfg = ctrl.config();
            cfg.gridWidth = w;
            cfg.gridHeight = h;
            adapter_->setConfig(cfg);
        }
        heartbeat_->start();
        emit joined();
        break;
    }
    case net::MsgType::GameState: {
        auto& ctrl = adapter_->controller();
        auto snake = net::parseSnake(msg);
        auto food = net::parseFood(msg);
        if (!snake.empty()) {
            if (ctrl.isPaused() || ctrl.isGameOver()) {
                ctrl.start();
            }
            std::deque<core::Point> body(snake.begin(), snake.end());
            auto dir = ctrl.snake().direction();
            ctrl.snake().setBody(body, dir);
            ctrl.food().setPosition(food);
            ctrl.grid().clearAll();
            for (const auto& seg : body) {
                ctrl.grid().occupy(seg);
            }
            emit gameStateUpdated();
        }
        break;
    }
    case net::MsgType::Heartbeat:
        client_->send(net::makeHeartbeatAck(msg["seq"].toInt()));
        break;
    case net::MsgType::HeartbeatAck:
        heartbeat_->onPongReceived();
        break;
    default:
        break;
    }
}

void NetworkManager::onHeartbeatTimeout() {
    LOG_ERROR("NetMgr: heartbeat timeout");
    emit errorOccurred("Connection lost (heartbeat timeout)");
    if (isHost_) stopHosting();
    if (isClient_) leaveGame();
}
