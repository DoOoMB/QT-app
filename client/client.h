#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>


class Client;

class ClientDestroyer {
private:
    Client * p_instance;
public:
    ~ClientDestroyer();
    void initialize(Client * p) { p_instance = p; }


};

class Client : public QObject {
    Q_OBJECT
private:
    static Client * p_instance;
    static ClientDestroyer destroyer;

    QTcpSocket *socket;
    QString m_token;
    QString m_login;

    QByteArray m_buffer;
    void sendCommand(const QString &cmd);
    void processResponse(const QString &response);


protected:
    Client(QObject *parent = nullptr);
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client();
    friend class ClientDestroyer;

public:
    static Client* getInstance();

    void connectToServer(const QString &host, quint16 port);
    void auth(const QString &login, const QString &password);
    void reg(const QString &login, const QString &password);
    void getStats(); // Использует сохраненный токен и логин

    void requestTask(const QString &taskCommand);
    void setTaskResult(int taskId, bool isCorrect);
    void logout();

    // Вспомогательные методы
    bool isAuthorized() const { return !m_token.isEmpty(); }
    QString getToken() const { return m_token; }

signals:
    void authSuccess();
    void authError();
    void regSuccess();
    void regError();
    void statsReceived(const QString &data);
    void taskReceived(const QString &taskType, int taskId, const QString &taskText, const QString &correctAnswer);
    void taskResultSaved();
    void tokenExpired();
    void serverMessage(const QString &message);


private slots:
    void onReadyRead();
};

#endif // CLIENT_H
