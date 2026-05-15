#ifndef APP_NETWORK_GAMECLIENT_H
#define APP_NETWORK_GAMECLIENT_H

#include <QObject>
#include <QTcpSocket>

class GameClient : public QObject {
    Q_OBJECT

public:
    explicit GameClient(QObject* parent = nullptr);
    ~GameClient() override;

    void connectToHost(const QString& host, quint16 port,
                       const QString& playerName);
    void disconnect();
    bool isConnected() const;

    void send(const QJsonObject& msg);
    int socketState() const;

signals:
    void connected();
    void disconnected();
    void messageReceived(const QJsonObject& msg);
    void connectionError(const QString& error);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    QTcpSocket* socket_;
    QByteArray buffer_;
};

#endif // APP_NETWORK_GAMECLIENT_H
