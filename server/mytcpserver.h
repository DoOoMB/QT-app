#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <QtNetwork>
#include <QByteArray>
#include <QDebug>

#include <QList>

/**
 * @class MyTcpServer
 * @brief Lightweight TCP server handling multiple client sockets.
 *
 * Accepts new connections, reads data and handles client disconnects.
 */
class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    /** Create server and prepare to listen. */
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();
public slots:
    /** Handle a newly accepted client connection. */
    void slotNewConnection();
    /** Handle a client disconnecting. */
    void slotClientDisconnected();
    /** Read incoming data from clients. */
    void slotServerRead();
private:
    QTcpServer * mTcpServer;
    QList<QTcpSocket*> clients;
    //int server_status;
};
#endif // MYTCPSERVER_H







