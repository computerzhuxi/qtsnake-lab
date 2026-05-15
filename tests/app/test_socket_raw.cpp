#include <winsock2.h>
#include <ws2tcpip.h>
#include <QtTest/QtTest>

class TestSocketRaw : public QObject {
    Q_OBJECT

private slots:
    void testRawSocketLoopback() {
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);

        SOCKET server = ::socket(AF_INET, SOCK_STREAM, 0);
        QVERIFY(server != INVALID_SOCKET);

        sockaddr_in addr {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(16666);

        QVERIFY(::bind(server, (sockaddr*)&addr, sizeof(addr)) == 0);
        QVERIFY(::listen(server, 1) == 0);

        SOCKET client = ::socket(AF_INET, SOCK_STREAM, 0);
        QVERIFY(client != INVALID_SOCKET);

        sockaddr_in caddr = addr;
        QVERIFY2(::connect(client, (sockaddr*)&caddr, sizeof(caddr)) == 0,
                 "Raw socket connect also failed - OS level issue");

        sockaddr_in from {};
        int fromLen = sizeof(from);
        SOCKET accepted = ::accept(server, (sockaddr*)&from, &fromLen);
        QVERIFY2(accepted != INVALID_SOCKET, "accept failed");

        const char* msg = "hello";
        ::send(client, msg, 5, 0);
        char buf[64] = {};
        ::recv(accepted, buf, 64, 0);
        QCOMPARE(QString(buf), QString("hello"));

        ::closesocket(accepted);
        ::closesocket(client);
        ::closesocket(server);
        WSACleanup();
    }
};

QTEST_MAIN(TestSocketRaw)
#include "test_socket_raw.moc"
