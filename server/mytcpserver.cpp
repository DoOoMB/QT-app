#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>
#include "serverfunctionsmanager.h"

MyTcpServer::~MyTcpServer()
{
    for (auto client : clients) // завершаем все соединения с клиентами
    {
        QString client_addr = client->peerAddress().toString();
        client->close();
        client->deleteLater();
        qDebug() << "Client with address" << client_addr << "disconnected successfully.";
    }
    clients.clear(); // очищаем буфер сокетов клиентов
    mTcpServer->close();
    //server_status=0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection); // подключаем инстанс сервера и назначаем нужные слоты для сигналов

    if(!mTcpServer->listen(QHostAddress::Any, 44444)){ // слушаем порт 44444
        qDebug() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
   //if(server_status==1){
        QTcpSocket* clientSocket = mTcpServer->nextPendingConnection(); // читаем новое соединение
        clients.append(clientSocket); // добавляем клиента в буфер

        clientSocket->write("Hello, World!!! I am echo server!\r\n"); // приветственное сообщение для клиента
        connect(clientSocket, &QTcpSocket::readyRead,this,&MyTcpServer::slotServerRead); // подключаем новый сокет клиента и назначаем нужные слоты для сигналов
        connect(clientSocket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);

        qDebug() << "New client connected:" << clientSocket->peerAddress().toString(); // выводим адрес клиента в консоль 
   //}
}

void MyTcpServer::slotServerRead(){
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender()); // достаём сокет клиента, что отправил сигнал

    if (!clientSocket) // проверяем, существует ли он, если нет, ничего не делаем
    {
        qDebug() << "Error: client's socket not found";
        return;
    }

    QByteArray data = clientSocket->readAll(); // читаем read buffer
    QString command = QString::fromUtf8(data).trimmed(); // достаём текст команды
    QString client_addr = clientSocket->peerAddress().toString(); // достаём адрес клиента
    if (command.isEmpty())
    {
        qDebug() << "Server received an empty command from client" << client_addr;
    }

    QString resp = ServerFunctionsManager::parse(command); // парсим команду
    qDebug() << "Sending response to client" << client_addr;
    if (clientSocket->state() == QAbstractSocket::ConnectedState) // проверям, активен ли клиент
    {
        clientSocket->write(resp.toUtf8()); // отправляем ответ клиенту
        clientSocket->flush(); // принудительно отправляем все данные из буфера
    } else {
        qDebug() << "Response was not sended. Client" << client_addr << "is already disconnected.";
    }
}

void MyTcpServer::slotClientDisconnected(){
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender()); // достаём сокет клиента, что отправил сигнал
    if (clientSocket)
    {
        QString client_addr = clientSocket->peerAddress().toString();
        clientSocket->close(); // завершаем соединение
        clients.removeOne(clientSocket); // удаляем сокет из буфера
        clientSocket->deleteLater(); // освобождаем память
        qDebug() << "Client with address" << client_addr << "disconnected successfully.";
    }
}
