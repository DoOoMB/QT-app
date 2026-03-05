#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>

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

    if(!mTcpServer->listen(QHostAddress::Any, 44444)){ // слушаем порт 3333
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
    QString res = "";

    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender()); // достаём сокет клиента, что отправил сигнал

    if (!clientSocket) // проверяем, существует ли он, если нет, ничего не делаем
    {
        qDebug() << "Error: client's socket not found";
        return;
    }

    while(clientSocket->bytesAvailable()>0) // пока в буфере есть данные для чтения
    {
        QByteArray array =clientSocket->readAll(); // читаем данные из read buffer
        qDebug()<<array<<"\n";
        if(array=="\x01") // если попадается конец строки
        {
            clientSocket->write(res.toUtf8()); // выводим тоже самое клиенту
            res = "";
        }
        else
            res.append(array); // иначе отправляем байт в res
    }
    clientSocket->write(res.toUtf8()); // отправляем res клиенту
    clientSocket->flush(); // принудительно отправляем все данные из буфера
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
