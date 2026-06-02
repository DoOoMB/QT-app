#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>


class Client;

/**
 * @brief Helper to destroy the singleton `Client` on program exit.
 */
class ClientDestroyer {
private:
    Client * p_instance;
public:
    ~ClientDestroyer();
    void initialize(Client * p) { p_instance = p; }

};

/**
 * @class Client
 * @brief Singleton TCP client used by the GUI to communicate with server.
 *
 * Provides convenience methods for authentication, registration,
 * requesting tasks and submitting results. Emits signals for async
 * responses coming from the server.
 */
class Client : public QObject {
    Q_OBJECT
private:
    static Client * p_instance;
    static ClientDestroyer destroyer;

    QTcpSocket *socket;
    QString m_token;
    QString m_login;

    QByteArray m_buffer;
    /** Send a raw command to the server. */
    void sendCommand(const QString &cmd);
    /** Process a server response stored in text form. */
    void processResponse(const QString &response);


protected:
    Client(QObject *parent = nullptr);
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client();
    friend class ClientDestroyer;

public:
    /** Return singleton instance (creates it on first call). */
    static Client* getInstance();

    /** Connect to a TCP server. */
    void connectToServer(const QString &host, quint16 port);
    /** Authenticate with login/password. Emits `authSuccess` on success. */
    void auth(const QString &login, const QString &password);
    /** Register a new user. Emits `regSuccess` on success. */
    void reg(const QString &login, const QString &password);
    /** Request current user statistics (uses stored token). */
    void getStats(); // Использует сохраненный токен и логин

    /** Request a task by command name/type. */
    void requestTask(const QString &taskCommand);
    /** Submit a completed task's result. */
    void setTaskResult(int taskId, bool isCorrect);
    /** Logout current user and clear token. */
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
    /** Handle data available on the socket. */
    void onReadyRead();
};

#endif // CLIENT_H
