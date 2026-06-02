#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <QDateTime>

/**
 * @brief Authentication token data.
 *
 * Stores a session token and its expiration time for a user.
 */
struct AuthData
{
    QString token;
    QDateTime expiresAt;
};

/**
 * @brief Task description returned from the database.
 *
 * Contains an id, visible text and the correct answer.
 */
struct Task
{
    int task_id = -1;
    QString task_text;
    QString correct_answer;
};

/**
 * @class DBManager
 * @brief Simple singleton wrapper around the application database.
 *
 * Manages authentication tokens, user statistics and task retrieval.
 */
class DBManager : public QObject
{
    public:
        /**
         * @brief Get singleton instance.
         */
        static DBManager& Instance()
        {
            static DBManager instance;
            return instance;
        }

        /** Authenticate a user and return a token (or error string). */
        QString auth(QString login, QString password);
        /** Register a new user and return result string. */
        QString registration(QString login, QString password);
        /** Retrieve statistics for a user in a simple string format. */
        QString getStats(QString login);
        /** Save a task result for a user and return status string. */
        QString setStats(QString login, int task_id, bool is_correct);
        /** Get task data by name. */
        Task getTask(QString task_name);
        /** Execute a raw query and return result as string (internal use). */
        QString executeQuery(QString q);
        /** Check if a given login/token pair is valid. */
        bool checkAuth(QString login, QString token);

    private:
        QMap<QString, AuthData> authBuffer; // хранит пары логин-токен для авторизованных пользователей
        DBManager();
        ~DBManager();
        /** Generate a new random token. */
        QString generateToken();
        QSqlDatabase db;
};