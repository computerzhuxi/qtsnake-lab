#include <QtTest/QtTest>
#include <QTcpServer>
#include <QTcpSocket>

class TestNetworkMinimal : public QObject {
    Q_OBJECT

private slots:
    void testLocalhostConnection() {
        QTcpServer server;
        QVERIFY2(server.listen(QHostAddress::Any, 15555),
                 qPrintable(server.errorString()));

        QTcpSocket client;
        client.connectToHost("127.0.0.1", 15555);
        QVERIFY2(client.waitForConnected(5000), qPrintable(client.errorString()));
        QCOMPARE(client.state(), QAbstractSocket::ConnectedState);

        QVERIFY2(server.waitForNewConnection(5000), "server waitForNewConnection failed");
        QTcpSocket* srvSocket = server.nextPendingConnection();
        QVERIFY(srvSocket != nullptr);
        QCOMPARE(srvSocket->state(), QAbstractSocket::ConnectedState);

        // Write from client
        const char* testData = "hello\n";
        qint64 w = client.write(testData, 6);
        QVERIFY2(w == 6, qPrintable(client.errorString()));

        // Wait for data on server — use QEventLoop with timeout
        bool dataArrived = false;
        QTimer::singleShot(3000, &server, [&]() { dataArrived = false; });
        QEventLoop loop;
        connect(srvSocket, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        QTimer::singleShot(3000, &loop, &QEventLoop::quit);
        loop.exec();

        QByteArray received = srvSocket->readAll();
        QVERIFY2(!received.isEmpty(),
                 ("Server received nothing, bytesAvail=" + std::to_string(srvSocket->bytesAvailable())).c_str());
        QCOMPARE(QString(received.trimmed()), QString("hello"));

        srvSocket->disconnectFromHost();
        client.disconnectFromHost();
    }
};

QTEST_MAIN(TestNetworkMinimal)
#include "test_network_minimal.moc"
