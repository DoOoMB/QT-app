#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <QDateTime>

struct AuthData
{
    QString token;
    QDateTime expiresAt;
};

class DBManager : public QObject
{
    public:
        static DBManager& Instance()
        {
            static DBManager instance;
            return instance;
        }

        QString auth(QString login, QString password);
        QString registration(QString login, QString password);
        QString getStats(QString login);
        QString executeQuery(QString q);
        bool checkAuth(QString login, QString token);

    private:
        QMap<QString, AuthData> authBuffer; // хранит пары логин-токен для авторизованных пользователей
        DBManager();
        ~DBManager();
        QString generateToken();
        QSqlDatabase db;
};