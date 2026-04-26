#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <QDateTime>

struct AuthData
{
    QString token;
    QDateTime expiresAt;
};

struct Task
{
    int task_id = -1;
    QString task_text;
    QString correct_answer;
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
        QString setStats(QString login, int task_id, bool is_correct);
        Task getTask(QString task_name);
        QString executeQuery(QString q);
        bool checkAuth(QString login, QString token);

    private:
        QMap<QString, AuthData> authBuffer; // хранит пары логин-токен для авторизованных пользователей
        DBManager();
        ~DBManager();
        QString generateToken();
        QSqlDatabase db;
};