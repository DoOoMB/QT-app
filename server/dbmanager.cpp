#include "dbmanager.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QSqlError>
#include <QUuid>

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
    QSqlQuery query;
    query.prepare("SELECT s.stats, s.date FROM stats s JOIN users u ON u.user_id = s.user_id WHERE u.login = ?");
    query.addBindValue(login);
    if (!query.exec())
    {
        db.rollback();
        qDebug() << "Database query execution error: " << query.lastError().text();
        return "query_error";
    }
    QString stats = "";
    while (query.next())
    {
        stats = stats + query.value("stats").toString() + "," + query.value("date").toString() + "&";
    }
    db.commit();
    return stats.mid(0, stats.length()-1);
}

QString DBManager::setStats(QString login, QString stats)
{
    db.transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO stats (stats, date, user_id) VALUES (?, ?, (SELECT user_id FROM users u WHERE u.login = ? ))");
    query.addBindValue(stats); // статы
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
