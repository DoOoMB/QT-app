#include <QtTest/QtTest>
#include <QHostAddress>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>

#include "client.h"

class ClientTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void authSuccessStoresTokenAndTokenExpiredClearsIt();
    void taskResponseEmitsParsedTask();

private:
    static void resetClient();
    static QTcpSocket *connectToFakeServer(QTcpServer &server);
    static void writeServerResponse(QTcpSocket *socket, const QByteArray &response);
};

void ClientTest::init()
{
    resetClient();
}

void ClientTest::cleanup()
{
    resetClient();
}

void ClientTest::resetClient()
{
    Client *client = Client::getInstance();
    client->logout();

    QTcpSocket *socket = client->findChild<QTcpSocket *>();
    if (socket != nullptr) {
        socket->abort();
    }
}

QTcpSocket *ClientTest::connectToFakeServer(QTcpServer &server)
{
    if (!server.listen(QHostAddress::LocalHost, 0)) {
        return nullptr;
    }

    QSignalSpy newConnectionSpy(&server, &QTcpServer::newConnection);
    Client::getInstance()->connectToServer("127.0.0.1", server.serverPort());

    if (!newConnectionSpy.wait(1000) && !server.hasPendingConnections()) {
        return nullptr;
    }

    return server.nextPendingConnection();
}

void ClientTest::writeServerResponse(QTcpSocket *socket, const QByteArray &response)
{
    QCOMPARE(socket->write(response), static_cast<qint64>(response.size()));
    QVERIFY(socket->flush());
}

void ClientTest::authSuccessStoresTokenAndTokenExpiredClearsIt()
{
    QTcpServer server;
    QTcpSocket *serverSocket = connectToFakeServer(server);
    QVERIFY(serverSocket != nullptr);
    Client *client = Client::getInstance();

    QSignalSpy authSuccessSpy(client, &Client::authSuccess);
    QSignalSpy tokenExpiredSpy(client, &Client::tokenExpired);

    writeServerResponse(serverSocket, "auth_success&test-token\r\n");

    QVERIFY(authSuccessSpy.wait(1000));
    QCOMPARE(authSuccessSpy.count(), 1);
    QVERIFY(client->isAuthorized());
    QCOMPARE(client->getToken(), QString("test-token"));

    writeServerResponse(serverSocket, "token_expired\r\n");

    QVERIFY(tokenExpiredSpy.wait(1000));
    QCOMPARE(tokenExpiredSpy.count(), 1);
    QVERIFY(!client->isAuthorized());
    QCOMPARE(client->getToken(), QString());
}

void ClientTest::taskResponseEmitsParsedTask()
{
    QTcpServer server;
    QTcpSocket *serverSocket = connectToFakeServer(server);
    QVERIFY(serverSocket != nullptr);
    Client *client = Client::getInstance();

    QSignalSpy taskReceivedSpy(client, &Client::taskReceived);

    writeServerResponse(serverSocket, "findNodesByDepth&42&task text&answer\r\n");

    QVERIFY(taskReceivedSpy.wait(1000));
    QCOMPARE(taskReceivedSpy.count(), 1);

    const QList<QVariant> arguments = taskReceivedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString("findNodesByDepth"));
    QCOMPARE(arguments.at(1).toInt(), 42);
    QCOMPARE(arguments.at(2).toString(), QString("task text"));
    QCOMPARE(arguments.at(3).toString(), QString("answer"));
}

QTEST_MAIN(ClientTest)

#include "tst_client.moc"
