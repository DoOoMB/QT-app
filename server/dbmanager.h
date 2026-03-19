#include <QObject>
#include <QSqlDatabase>
#include <QMap>

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
        QString getStats(QString login, QString token);
        QString executeQuery(QString q);

    private:
        QMap<QString, QString> authBuffer; // хранит пары логин-токен для авторизованных пользователей
        DBManager();
        ~DBManager();
        QString generateToken();
        QSqlDatabase db;
};