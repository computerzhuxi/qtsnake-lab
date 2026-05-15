#ifndef APP_NETWORK_NETWORKMANAGER_H
#define APP_NETWORK_NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <memory>

#include "core/engine/Direction.h"

class GameServer;
class GameClient;
class Heartbeat;
class GameControllerAdapter;

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);

    bool hostGame(quint16 port, GameControllerAdapter& adapter);
    void stopHosting();

    void joinGame(const QString& host, quint16 port,
                  const QString& playerName, GameControllerAdapter& adapter);
    void leaveGame();

    bool isHosting() const { return isHost_; }
    bool isClient() const { return isClient_; }

    void sendInput(core::Direction d);

signals:
    void hostingStarted(quint16 port);
    void hostingStopped();
    void joined();
    void left();
    void errorOccurred(const QString& error);
    void gameStateUpdated(); // client: controller state changed → repaint

private slots:
    void onServerMessage(QTcpSocket* client, const QJsonObject& msg);
    void onClientMessage(const QJsonObject& msg);
    void onBroadcastTick();
    void onHeartbeatTimeout();

private:
    GameServer* server_ = nullptr;
    GameClient* client_ = nullptr;
    Heartbeat* heartbeat_ = nullptr;
    GameControllerAdapter* adapter_ = nullptr;

    bool isHost_ = false;
    bool isClient_ = false;
};

#endif // APP_NETWORK_NETWORKMANAGER_H
