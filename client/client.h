#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class Client;

// Класс для автоматической очистки памяти (как в вашем примере)
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
    QString m_token; // Тот самый токен на 30 минут
    QString m_login; // Сохраняем логин для формирования запросов stats

protected:
    Client(QObject *parent = nullptr);
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client();
    friend class ClientDestroyer;

public:
    static Client* getInstance();

    // Основные команды согласно API_OVERVIEW.md
    void connectToServer(const QString &host, quint16 port);
    void auth(const QString &login, const QString &password);
    void reg(const QString &login, const QString &password);
    void getStats(); // Использует сохраненный токен и логин

    // Вспомогательные методы
    bool isAuthorized() const { return !m_token.isEmpty(); }
    QString getToken() const { return m_token; }

signals:
    void authSuccess();
    void authError();
    void regSuccess();
    void regError();
    void statsReceived(const QString &data);

private slots:
    void onReadyRead();
};

#endif // CLIENT_H
