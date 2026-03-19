#include <QObject>
#include "serverfunctionsmanager.h"
#include "QStringList"
#include "QDebug"
#include "dbmanager.h"

QString auth(QStringList args)
{
    if (args.length() != 2) return "Authentification failed.\r\n";
    QString resp = DBManager::Instance().auth(args[0], args[1]);
    qDebug() << resp;
    if (resp != "connection_error" && resp != "query_error" && resp != "auth_failed")
        return "auth_completed&" + resp + "\r\n";
    return "Authentification failed.\r\n";
}

QString registration(QStringList args)
{
    if (args.length() != 2) return "Registration failed.\r\n";
    QString resp = DBManager::Instance().registration(args[0], args[1]);
    qDebug() << resp;
    if (resp != "connection_error" && resp != "query_error" && resp != "reg_error")
        return "reg_completed\r\n";
    return "Registration failed.\r\n";
}

QString getStats(QStringList args)
{
    if (args.length() != 2) return "Cannot fetch stats.\r\n";
    QString resp = DBManager::Instance().getStats(args[0], args[1]);
    qDebug() << resp;
    if (resp !="unauth_err" && resp != "query_error")
        return "stats&"+resp+"\r\n";
    return "Cannot fetch stats.\r\n";
}

QString calc(QStringList args)
{
    return "Successfully calculated.\r\n";
}

QMap<QString, serverFunc>ServerFunctionsManager::funcs = {
    {"auth", auth},
    {"reg", registration},
    {"stats", getStats},
    {"calc", calc}
};

QString ServerFunctionsManager::parse(QString str)
{
    QStringList str_spl = str.split(u'&');
    QString command = str_spl[0];
    QStringList args = str_spl.mid(1);
    qDebug() << command;
    for (auto i : args)
    {
        qDebug() << i;
    }
    if (!funcs.contains(command))
    {
        return "Command was not found. Echo: " + str + "\r\n";
    }
    return funcs[command](args);
}

