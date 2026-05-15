#include "app/network/GameClient.h"
#include "app/network/Protocol.h"
#include "core/logging/Logger.h"
#include <QJsonDocument>
#include <QTimer>

GameClient::GameClient(QObject* parent)
    : QObject(parent)
    , socket_(new QTcpSocket(this))
{
    LOG_INFO("Client: created");
    connect(socket_, &QTcpSocket::connected,
            this, &GameClient::onConnected);
    connect(socket_, &QTcpSocket::disconnected,
            this, &GameClient::onDisconnected);
    connect(socket_, &QTcpSocket::readyRead,
            this, &GameClient::onReadyRead);
    connect(socket_,
            static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &GameClient::onError);

    // Poll socket every 30ms as fallback (in case readyRead signal is lost)
    auto* pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, [this]() {
        if (socket_->bytesAvailable() > 0) {
            LOG_INFO("Client: poll found data, bytesAvail="
                     + std::to_string(socket_->bytesAvailable()));
            onReadyRead();
        }
    });
    pollTimer->start(30);
}

GameClient::~GameClient() {
    LOG_INFO("Client: destroyed");
}

void GameClient::connectToHost(const QString& host, quint16 port,
                                const QString& playerName) {
    LOG_INFO("Client: connecting to " + host.toStdString()
             + ":" + std::to_string(port));
    socket_->connectToHost(host, port);

    // Wait synchronously for connection (localhost is instant)
    if (!socket_->waitForConnected(5000)) {
        LOG_ERROR("Client: connect failed: " + socket_->errorString().toStdString());
        return;
    }

    LOG_INFO("Client: connected, sending JoinRequest as '"
             + playerName.toStdString() + "'");
    onConnected(); // emit connected signal for NetworkManager
    send(net::makeJoinRequest(playerName));

    // Wait synchronously for JoinResponse
    if (!socket_->waitForReadyRead(5000)) {
        LOG_ERROR("Client: no JoinResponse received after 5s: "
                  + socket_->errorString().toStdString());
        return;
    }

    LOG_INFO("Client: data arrived, processing...");
    onReadyRead(); // triggers messageReceived → onClientMessage
}

void GameClient::disconnect() {
    LOG_INFO("Client: disconnecting");
    socket_->disconnectFromHost();
}

bool GameClient::isConnected() const {
    return socket_->state() == QAbstractSocket::ConnectedState;
}

int GameClient::socketState() const {
    return static_cast<int>(socket_->state());
}

void GameClient::send(const QJsonObject& msg) {
    QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact) + '\n';
    qint64 written = socket_->write(data);
    if (written < 0) {
        LOG_ERROR("Client: send failed: " + socket_->errorString().toStdString());
    } else if (written != data.size()) {
        LOG_WARN("Client: send partial: " + std::to_string(written)
                 + "/" + std::to_string(data.size()));
    }
    socket_->flush();
}

void GameClient::onConnected() {
    LOG_INFO("Client: onConnected");
    emit connected();
}

void GameClient::onDisconnected() {
    LOG_INFO("Client: onDisconnected, state="
             + std::to_string(static_cast<int>(socket_->state()))
             + " error=" + socket_->errorString().toStdString());
    emit disconnected();
}

void GameClient::onReadyRead() {
    qint64 avail = socket_->bytesAvailable();
    LOG_INFO("Client: onReadyRead, bytesAvail=" + std::to_string(avail));
    buffer_.append(socket_->readAll());

    while (true) {
        int idx = buffer_.indexOf('\n');
        if (idx < 0) break;

        QByteArray line = buffer_.left(idx);
        buffer_.remove(0, idx + 1);

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError) {
            LOG_WARN("Client: JSON parse error: " + err.errorString().toStdString());
            continue;
        }

        QJsonObject obj = doc.object();
        LOG_INFO("Client received: " + obj["type"].toString().toStdString());
        emit messageReceived(obj);
    }
}

void GameClient::onError(QAbstractSocket::SocketError /*error*/) {
    LOG_ERROR("Client: socket error: " + socket_->errorString().toStdString());
    emit connectionError(socket_->errorString());
}
