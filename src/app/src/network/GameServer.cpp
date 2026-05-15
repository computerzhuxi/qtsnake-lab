#include "app/network/GameServer.h"
#include "core/logging/Logger.h"
#include <QJsonDocument>
#include <QJsonObject>

GameServer::GameServer(QObject* parent)
    : QObject(parent)
    , server_(new QTcpServer(this))
{
    connect(server_, &QTcpServer::newConnection,
            this, &GameServer::onNewConnection);
}

bool GameServer::start(quint16 port) {
    if (!server_->listen(QHostAddress::Any, port)) {
        LOG_ERROR("Server failed to listen on port " + std::to_string(port)
                  + ": " + server_->errorString().toStdString());
        emit errorOccurred(server_->errorString());
        return false;
    }
    LOG_INFO("Server listening on 127.0.0.1:" + std::to_string(port)
             + " serverAddress=" + server_->serverAddress().toString().toStdString());
    return true;
}

void GameServer::stop() {
    LOG_INFO("Server stopping, clients=" + std::to_string(clients_.size()));
    for (auto* client : clients_.keys()) {
        client->disconnectFromHost();
    }
    server_->close();
    clients_.clear();
    buffers_.clear();
}

bool GameServer::isListening() const {
    return server_->isListening();
}

void GameServer::poll() {
    static int pollCount = 0;
    ++pollCount;
    if (pollCount == 1 || pollCount % 40 == 0) { // log every ~2 seconds
        LOG_INFO("Server: poll #" + std::to_string(pollCount)
                 + " listening=" + std::to_string(server_->isListening())
                 + " pending=" + std::to_string(server_->hasPendingConnections())
                 + " clients=" + std::to_string(clients_.size()));
    }
    if (server_->hasPendingConnections()) {
        LOG_INFO("Server: poll found pending connections");
        onNewConnection();
    }
}

void GameServer::broadcast(const QJsonObject& msg) {
    QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact) + '\n';
    for (auto* client : clients_.keys()) {
        client->write(data);
    }
}

void GameServer::sendTo(QTcpSocket* client, const QJsonObject& msg) {
    QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact) + '\n';
    client->write(data);
}

void GameServer::onNewConnection() {
    while (server_->hasPendingConnections()) {
        QTcpSocket* client = server_->nextPendingConnection();
        LOG_INFO("Server: new client connected: "
                 + client->peerAddress().toString().toStdString()
                 + ":" + std::to_string(client->peerPort()));
        clients_.insert(client, QString());
        buffers_.insert(client, QByteArray());

        connect(client, &QTcpSocket::readyRead,
                this, &GameServer::onReadyRead);
        connect(client, &QTcpSocket::disconnected,
                this, &GameServer::onDisconnected);
    }
}

void GameServer::onReadyRead() {
    auto* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    buffers_[client].append(client->readAll());

    while (true) {
        int idx = buffers_[client].indexOf('\n');
        if (idx < 0) break;

        QByteArray line = buffers_[client].left(idx);
        buffers_[client].remove(0, idx + 1);

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError) {
            LOG_WARN("Server: JSON parse error: " + err.errorString().toStdString());
            continue;
        }

        QJsonObject msg = doc.object();
        QString type = msg["type"].toString();
        LOG_INFO("Server received: " + type.toStdString());

        if (type == "JoinRequest") {
            clients_[client] = msg["playerName"].toString();
            LOG_INFO("Server: player '" + clients_[client].toStdString() + "' joined");
            emit clientConnected(client, clients_[client]);
        }

        emit messageReceived(client, msg);
    }
}

void GameServer::onDisconnected() {
    auto* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    LOG_INFO("Server: client disconnected: " + clients_[client].toStdString());
    emit clientDisconnected(client);
    clients_.remove(client);
    buffers_.remove(client);
    client->deleteLater();
}
