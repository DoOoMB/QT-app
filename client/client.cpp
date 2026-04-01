#include "client.h"
#include <QObject>

Client* Client::p_instance = nullptr;
ClientDestroyer Client::destroyer;

ClientDestroyer::~ClientDestroyer() { 
    delete p_instance; 
}

Client* Client::getInstance() {
    if (!p_instance) {
        p_instance = new Client();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

Client::Client(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

Client::~Client() {
    if (socket->isOpen()) socket->close();
}

void Client::connectToServer(const QString &host, quint16 port) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket->connectToHost(host, port);
    }
}

// Формат: auth&LOGIN&PASSWORD
void Client::auth(const QString &login, const QString &password) {
    m_login = login; // Запоминаем для будущих запросов
    QString cmd = QString("auth&%1&%2").arg(login, password);
    socket->write(cmd.toUtf8());
}

// Формат: reg&LOGIN&PASSWORD
void Client::reg(const QString &login, const QString &password) {
    QString cmd = QString("reg&%1&%2").arg(login, password);
    socket->write(cmd.toUtf8());
}

// Формат: stats&LOGIN&TOKEN (только если есть токен)
void Client::getStats() {
    if (m_token.isEmpty()) {
        qDebug() << "Cannot get stats: No token!";
        return;
    }
    QString cmd = QString("stats&%1&%2").arg(m_login, m_token);
    socket->write(cmd.toUtf8());
}

void Client::onReadyRead() {
    QByteArray data = socket->readAll();
    QString response = QString::fromUtf8(data).trimmed();
    qDebug() << "Onreadyread";

    // Разбираем ответ сервера по разделителю &
    QStringList parts = response.split('&');
    if (parts.isEmpty()) return;

    QString type = parts[0];

    if (type == "auth_success") {
        m_token = parts[1]; // Сохраняем присланный TOKEN
        qDebug() << "Received token:" << m_token;
        emit authSuccess();
    } 
    else if (type == "auth_error") {
        emit authError();
    }
    else if (type == "reg_success") {
        emit regSuccess();
    }
    else if (type == "stats" && parts.size() > 1) {
        emit statsReceived(parts[1]);
    }
}