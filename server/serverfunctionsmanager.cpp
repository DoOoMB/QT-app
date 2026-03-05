#include <QObject>
#include "serverfunctionsmanager.h"

QMap<QString, IServerFunc*> ServerFunctionsManager::funcs;
QString ServerFunctionsManager::parse(QString str)
{
    if (!funcs.contains(str))
    {
        return "Command was not found. Echo: " + str + "\r\n";
    }
    return "\r\n";
}