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
    if (args.length() != 2) return "auth_error\r\n";
    QString resp = DBManager::Instance().auth(args[0], args[1]);
    qDebug() << resp;
    if (resp != "connection_error" && resp != "query_error" && resp != "auth_failed")
        return "auth_success&" + resp + "\r\n";
    return "auth_error\r\n";
}

// принимает login, pass
QString registration(QStringList args)
{
    if (args.length() != 2) return "reg_error\r\n";
    QString resp = DBManager::Instance().registration(args[0], args[1]);
    qDebug() << resp;
    if (resp != "connection_error" && resp != "query_error" && resp != "reg_error")
        return "reg_success\r\n";
    return "reg_error\r\n";
}

// принимает login, token
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

// принимает login, token, stats
QString setStats(QStringList args)
{
    if (args.length() != 3) return "set_stats_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";
    QString resp = DBManager::Instance().setStats(args[0], args[2]);
    qDebug() << resp;
    if (resp != "query_error")
        return "stats&"+resp+"\r\n";
    return "set_stats_error\r\n";
}

// TODO
// -Сделать запись статистики-, заглушки для функций сокомандников

struct Node
{
    int id;
    QVector<QVector<int>> neighbours; // сначала id связанной ноды, после вес ребра, соединяющих ноды
};

Node* FindNode(QVector<Node*> vec, int node_id) // возвращает указатель на ноду по её id
{
    for (Node* n : vec)
    {
        if (n->id == node_id) return n;
    }
    return NULL;
}

void FindNodesByDepth(int start_id, QVector<Node*> vec, int i, int depth, QVector<int> &solution) // Ищет ноды, в которые можно добраться за depth шагов
{
    if (i == depth){
        solution.append(start_id);
        return;
    } 
    else
    {
        Node* start_node = FindNode(vec, start_id);
        qDebug() << start_node->neighbours;
        for (QVector<int> node : start_node->neighbours)
        {
            FindNodesByDepth(node[0], vec, i+1, depth, solution);
        }
    }
}

/*принимает login, token, id начального узла и граф в виде строки JSON:
[
    [<id узла>, [[<id соединённого с ним узла>, <вес ребра между ними>], ...] ],
    ...
]
Пример:
[ [1, [[2, 2], [3, 3]] ], [2, [[1, 2], [3, 1]] ], [3, [[1, 3], [2, 1]] ] ]
(ставьте вес 1, если граф невзвешенный)
*/
QString findNodesByDepth(QStringList args)
{
    if (args.length() != 4) return "findNodesByDepth_error\r\n";
    if (!DBManager::Instance().checkAuth(args[0], args[1])) return "token_expired\r\n";

    QByteArray raw_str = args[3].toUtf8();

    QJsonParseError json_err;
    QJsonDocument doc = QJsonDocument::fromJson(raw_str, &json_err);
    if (json_err.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON parse error:" << json_err.errorString();
        return "findNodesByDepth_error\r\n";
    }
    if (!doc.isArray())
    {
        qDebug() << "incorrect graph formatting";
        return "findNodesByDepth_error\r\n";
    }
    // try{
        QJsonArray rootArray = doc.array();

        QVector<Node*> nodes;

        for (const QJsonValue &nodeValue : rootArray) {
            
            QJsonArray nodeArray = nodeValue.toArray();

            int nodeId = nodeArray[0].toInt();

            QJsonArray neighboursArray = nodeArray[1].toArray();

            Node *newNode = new Node;
            newNode->id = nodeId;

            for (const QJsonValue &edgeValue : neighboursArray) {
                
                QJsonArray edgeArray = edgeValue.toArray();

                int neighbourId = edgeArray[0].toInt();
                int weight = edgeArray[1].toInt();

                newNode->neighbours.push_back({neighbourId, weight});
            }

            nodes.append(newNode);
        }
        QVector<int> res;
        FindNodesByDepth(args[2].toInt(), nodes, 0, 2, res);
        QString resp = "";
        for (int i : res)
        {
            resp = resp + QString::number(i) + "&";
        }
        QJsonArray stats_to_save;
        QJsonArray res_stats;
        for (int i : res)
        {
            res_stats.append(i);
        }
        stats_to_save.append(rootArray);
        stats_to_save.append(args[2].toInt());
        stats_to_save.append(res_stats);
        QJsonDocument stats_doc(stats_to_save);
        QByteArray stats_byte_arr = stats_doc.toJson(QJsonDocument::Compact);
        DBManager::Instance().setStats(args[0], QString::fromUtf8(stats_byte_arr));

        return "findNodesByDepth&" + resp.mid(0, resp.length()-1) + "\r\n";
    // } catch (const std::exception &e)
    // {
    //     qDebug() << e.what();
    //     return "findNodesByDepth_error\r\n";
    // }
}
QJsonDocument stats_doc;
QMap<QString, serverFunc>ServerFunctionsManager::funcs = {
    {"auth", auth},
    {"reg", registration},
    {"stats", getStats},
    {"set_stats", setStats},
    {"findNodesByDepth", findNodesByDepth}
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

