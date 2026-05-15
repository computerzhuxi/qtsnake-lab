#ifndef APP_NETWORK_GAMESERVER_H
#define APP_NETWORK_GAMESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>

class GameServer : public QObject {
    Q_OBJECT

public:
    explicit GameServer(QObject* parent = nullptr);

    bool start(quint16 port);
    void stop();
    bool isListening() const;
    void poll();

    void broadcast(const QJsonObject& msg);
    void sendTo(QTcpSocket* client, const QJsonObject& msg);
    int clientCount() const { return static_cast<int>(clients_.size()); }

signals:
    void clientConnected(QTcpSocket* client, const QString& name);
    void clientDisconnected(QTcpSocket* client);
    void messageReceived(QTcpSocket* client, const QJsonObject& msg);
    void errorOccurred(const QString& error);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpServer* server_;
    QHash<QTcpSocket*, QString> clients_;
    QHash<QTcpSocket*, QByteArray> buffers_;
};

#endif // APP_NETWORK_GAMESERVER_H
