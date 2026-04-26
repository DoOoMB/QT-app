#include <QObject>
#include "serverfunctionsmanager.h"
#include "QStringList"
#include "QDebug"
#include "dbmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

// принимает login, pass возвращает token для использования функций
QString auth(QStringList args)
{
    if (args.length() != 2) return "auth_args_error\r\n";
    QString resp = DBManager::Instance().auth(args[0], args[1]);
    qDebug() << resp;
    if (resp != "connection_error" && resp != "query_error" && resp != "auth_failed")
        return "auth_success&" + resp + "\r\n";
    return "auth_error\r\n";
}

// принимает login, pass
QString registration(QStringList args)
{
    if (args.length() != 2) return "reg_args_error\r\n";
    QString resp = DBManager::Instance().registration(args[0], args[1]);
    qDebug() << resp;
    if (resp != "connection_error" && resp != "query_error" && resp != "reg_error")
        return "reg_success\r\n";
    return "reg_error\r\n";
}

// принимает login, token
// возвращает stats&[всего заданий выполнено]&[из них верно]&[из них неверно]
QString getStats(QStringList args)
{
    if (args.length() != 2) return "get_stats_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";
    QString resp = DBManager::Instance().getStats(args[0]);
    qDebug() << resp;
    if (resp != "query_error")
        return "stats&"+resp+"\r\n";
    return "get_stats_error\r\n";
}

// принимает login, token, task_id, is_correct (1, 0)
QString setStats(QStringList args)
{
    if (args.length() != 4 || !(args[3] == "1" || args[3] == "0")) return "set_stats_args_error\r\n";
    bool isInt;
    args[2].toInt(&isInt);
    if (!isInt) return "set_stats_args_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";
    QString resp = DBManager::Instance().setStats(args[0], args[2].toInt(), args[3] == "1" ? true : false);
    qDebug() << resp;
    if (resp != "query_error")
        return "stats&"+resp+"\r\n";
    return "set_stats_error\r\n";
}

// возвращают [тип задачи]&[id задачи]&[текст задачи]&[правильный ответ]
QString findNodesByDepth(QStringList args)
{
    if (args.length() != 2) return "findNodesByDepth_args_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";
    Task resp = DBManager::Instance().getTask("findNodesByDepth");
    if (resp.task_id == -1)
        return "findNodesByDepth_error\r\n";
    return QString("findNodesByDepth&%1&%2&%3\r\n").arg(resp.task_id).arg(resp.task_text).arg(resp.correct_answer);
}

QString isPathShortest(QStringList args)
{
    if (args.length() != 2) return "isPathShortest_args_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";
    Task resp = DBManager::Instance().getTask("isPathShortest");
    if (resp.task_id == -1)
        return "isPathShortest_error\r\n";
    return QString("isPathShortest&%1&%2&%3\r\n").arg(resp.task_id).arg(resp.task_text).arg(resp.correct_answer);
}

QString primesAlgorithm(QStringList args)
{
    if (args.length() != 2) return "primesAlgorithm_args_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";
    Task resp = DBManager::Instance().getTask("primesAlgorithm");
    if (resp.task_id == -1)
        return "primesAlgorithm_error\r\n";
    return QString("primesAlgorithm&%1&%2&%3\r\n").arg(resp.task_id).arg(resp.task_text).arg(resp.correct_answer);
}

QString kruskalsAlgorithm(QStringList args)
{
    if (args.length() != 2) return "kruskalssAlgorithm_args_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";
    Task resp = DBManager::Instance().getTask("kruskalsAlgorithm");
    if (resp.task_id == -1)
        return "kruskalsAlgorithm_error\r\n";
    return QString("kruskalsAlgorithm&%1&%2&%3\r\n").arg(resp.task_id).arg(resp.task_text).arg(resp.correct_answer);
}


QJsonDocument stats_doc;
QMap<QString, serverFunc>ServerFunctionsManager::funcs = {
    {"auth", auth},
    {"reg", registration},
    {"stats", getStats},
    {"set_stats", setStats},
    {"findNodesByDepth", findNodesByDepth},
    {"isPathShortest", isPathShortest},
    {"primesAlgorithm", primesAlgorithm},
    {"kruskalsAlgorithm", kruskalsAlgorithm}
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
        return "Unknown_command""\r\n";
    }
    return funcs[command](args);
}