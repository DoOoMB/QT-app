#include "dbmanager.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QSqlError>
#include <QUuid>
#include <QDateTime>
#include <QVariant>
#include <QDebug>

DBManager::DBManager()
{
    this->db = QSqlDatabase::addDatabase("QPSQL");
    
    db.setHostName("localhost");
    db.setPort(5433);

    db.setDatabaseName("doomb");
    db.setUserName("postgres");
    db.setPassword("pass");

    if (!db.open())
    {
        qDebug() << "Database connection refused. Something went wrong.";
        return;
    }

    qDebug() << "Connected to the database succesfully";

}

DBManager::~DBManager()
{
    if (db.isOpen())
    {
        db.close();
        qDebug() << "Database connection was closed.";
    }
}

QString DBManager::auth(QString login, QString password)
{
    if (!db.isOpen())
    {
        qDebug() << "Database connection is not open. Something went wrong.";
        return "connection_error";        
    }
    db.transaction();
    QSqlQuery query;
    query.prepare("SELECT * FROM USERS WHERE login = ? AND password = ?");
    query.addBindValue(login);
    query.addBindValue(password);

    if (!query.exec())
    {
        db.rollback();
        qDebug() << "Database query execution error: " << query.lastError().text();
        return "query_error";
    }
    if (query.next())
    {
        qDebug() << "User" << query.value("login").toString() << "authorized successfully.";
        QString token = generateToken();
        authBuffer[login].token = token;
        authBuffer[login].expiresAt = QDateTime::currentDateTime().addSecs(1800);
        db.commit();
        return token;
    }
    else
    {
        db.rollback();
        return "auth_failed";
    }
}

QString DBManager::registration(QString login, QString password)
{
    if (!db.isOpen())
    {
        qDebug() << "Database connection is not open. Something went wrong.";
        return "connection_error";        
    }
    db.transaction();
    QSqlQuery query;
    query.prepare("SELECT * FROM USERS WHERE login = ?");
    query.addBindValue(login);

    if (!query.exec())
    {
        db.rollback();
        qDebug() << "Database query execution error: " << query.lastError().text();
        return "query_error";
    }
    if (query.next())
    {
        db.rollback();
        qDebug() << "User with login" << login << "already exists.";
        return "reg_error";
    } else {
        query.prepare("INSERT INTO users (login, password) VALUES (?, ?)");
        query.addBindValue(login);
        query.addBindValue(password);
        if (!query.exec())
        {
            db.rollback();
            qDebug() << "Database query execution error: " << query.lastError().text();
            return "query_error";
        }
        db.commit();
        qDebug() << "User" << login << "authorized successfully.";
        return "reg_success";
    }
}

QString DBManager::getStats(QString login)
{
    
    db.transaction();
    QSqlQuery query_correct_task_count;
    QSqlQuery query_failed_task_count;
    query_correct_task_count.prepare("select COUNT(*) FROM stats s \
                                    JOIN users u ON u.user_id = s.user_id \
                                    WHERE u.login = ? \
                                    group by s.IS_CORRECT \
                                    having s.IS_CORRECT = true");
    query_correct_task_count.addBindValue(login);

    query_failed_task_count.prepare("select COUNT(*) FROM stats s \
                                    JOIN users u ON u.user_id = s.user_id \
                                    WHERE u.login = ? \
                                    group by s.IS_CORRECT \
                                    having s.IS_CORRECT = false");
    query_failed_task_count.addBindValue(login);
    if (!query_correct_task_count.exec() || !query_failed_task_count.exec())
    {
        db.rollback();
        qDebug() << "Database query correct task execution error: " << query_correct_task_count.lastError().text();
        qDebug() << "Database query failed task execution error: " << query_failed_task_count.lastError().text();
        return "query_error";
    }

    int correct_tasks = 0;
    if (query_correct_task_count.next())
        correct_tasks = query_correct_task_count.value(0).toInt();

    int failed_tasks = 0;
    if (query_failed_task_count.next())
        failed_tasks = query_failed_task_count.value(0).toInt();

    db.commit();
    return QString("%1&%2&%3").arg(correct_tasks+failed_tasks).arg(correct_tasks).arg(failed_tasks);
}

QString DBManager::setStats(QString login, int task_id, bool is_correct)
{
    db.transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO stats (task_id, is_correct, date, user_id) VALUES (?, ?, ?, (SELECT user_id FROM users u WHERE u.login = ? ))");
    query.addBindValue(task_id); // id задания
    query.addBindValue(is_correct); // статус (1 - верно, 0 - не верно)
    query.addBindValue(QDateTime::currentDateTime()); // текущее время
    query.addBindValue(login); // логин
    if (!query.exec())
    {
        db.rollback();
        qDebug() << "Database query execution error: " << query.lastError().text();
        return "query_error";
    }
    db.commit();
    return "set_stats_success";
}

Task DBManager::getTask(QString task_name)
{
    db.transaction();
    QSqlQuery query;
    query.prepare("select * from tasks \
                where tasks.task_name = ? \
                order by random() \
                limit 1;");
    query.addBindValue(task_name);
    if (!query.exec())
    {
        db.rollback();
        qDebug() << "Database query execution error: " << query.lastError().text();
        return Task();
    }
    if (query.next())
    {
        Task res;
        res.task_id = query.value("task_id").toInt();
        res.task_text = query.value("task_text").toString();
        res.correct_answer = query.value("correct_answer").toString();
        return res;
    }
    return Task();
}

QString DBManager::executeQuery(QString q)
{
    if(!db.open())
    {
        qDebug() << "Database connection is not open. Something went wrong.";
        return "connection_error";  
    }
    db.transaction();
    QSqlQuery query;
    if(!query.exec(q))
    {
        db.rollback();
        qDebug() << "Database query execution error: " << query.lastError().text();
        return "query_error";
    }
    db.commit();
    return "success";
}

bool DBManager::checkAuth(QString login, QString token)
{
    if (!authBuffer.contains(login) || authBuffer[login].token != token)
    {
        return 0;
    }
    if (QDateTime::currentDateTime() > authBuffer[login].expiresAt)
    {
        authBuffer.remove(login);
        return 0;
    }
    return 1;
}

QString DBManager::generateToken()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
