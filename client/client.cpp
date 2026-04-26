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


void Client::sendCommand(const QString &cmd)
{
    if (socket->state() == QAbstractSocket::ConnectingState) {
        socket->waitForConnected(1000);
    }

    if (socket->state() != QAbstractSocket::ConnectedState) {
        emit serverMessage("Нет подключения к серверу.");
        return;
    }

    socket->write(cmd.toUtf8());
    socket->flush();
}

void Client::auth(const QString &login, const QString &password)
{
    m_login = login;
    sendCommand(QString("auth&%1&%2\r\n").arg(login, password));
}

void Client::reg(const QString &login, const QString &password)
{
    sendCommand(QString("reg&%1&%2\r\n").arg(login, password));
}

void Client::getStats()
{
    if (m_token.isEmpty()) {
        emit serverMessage("Сначала войдите в аккаунт.");
        return;
    }

    sendCommand(QString("stats&%1&%2\r\n").arg(m_login, m_token));
}

void Client::requestTask(const QString &taskCommand)
{
    if (m_token.isEmpty()) {
        emit serverMessage("Сначала войдите в аккаунт.");
        return;
    }

    sendCommand(QString("%1&%2&%3\r\n").arg(taskCommand, m_login, m_token));
}

void Client::setTaskResult(int taskId, bool isCorrect)
{
    if (m_token.isEmpty()) {
        emit serverMessage("Сначала войдите в аккаунт.");
        return;
    }

    if (taskId < 0) {
        emit serverMessage("Сервер не прислал task_id, статистику сохранить нельзя.");
        return;
    }

    sendCommand(QString("set_stats&%1&%2&%3&%4\r\n")
                    .arg(m_login, m_token)
                    .arg(taskId)
                    .arg(isCorrect ? 1 : 0));
}

void Client::logout()
{
    m_token.clear();
    m_login.clear();
    m_buffer.clear();
}

void Client::onReadyRead()
{
    m_buffer += socket->readAll();

    while (true) {
        qsizetype endIndex = m_buffer.indexOf("\r\n");
        if (endIndex == -1) {
            break;
        }

        QByteArray rawMessage = m_buffer.left(endIndex);
        m_buffer.remove(0, endIndex + 2);

        processResponse(QString::fromUtf8(rawMessage));
    }
}

void Client::processResponse(const QString &response)
{
    if (response.isEmpty()) {
        return;
    }

    qDebug() << "Server response:" << response;

    QStringList parts = response.split('&');
    if (parts.isEmpty()) {
        return;
    }

    const QString type = parts[0];

    if (type == "auth_success" && parts.size() >= 2) {
        m_token = parts[1];
        emit authSuccess();
        return;
    }

    if (type == "auth_error" || type == "auth_args_error") {
        emit authError();
        return;
    }

    if (type == "reg_success") {
        emit regSuccess();
        return;
    }

    if (type == "reg_error" || type == "reg_args_error") {
        emit regError();
        return;
    }

    if (type == "token_expired") {
        m_token.clear();
        emit tokenExpired();
        return;
    }

    if (type == "stats") {
        if (parts.size() == 4) {
            emit statsReceived(parts.mid(1).join("&"));
            return;
        }

        if (parts.size() >= 2 && parts[1] == "set_stats_success") {
            emit taskResultSaved();
            return;
        }
    }

    if (type == "findNodesByDepth" ||
        type == "isPathShortest" ||
        type == "primesAlgorithm" ||
        type == "kruskalsAlgorithm") {

        if (parts.size() >= 4) {
            bool ok = false;
            int taskId = parts[1].toInt(&ok);

            if (ok) {
                emit taskReceived(type, taskId, parts[2], parts[3]);
                return;
            }
        }

        if (parts.size() >= 3) {
            emit taskReceived(type, -1, parts[1], parts[2]);
            return;
        }

        emit serverMessage("Сервер прислал неполный ответ на задачу.");
        return;
    }

    emit serverMessage(response);
}
