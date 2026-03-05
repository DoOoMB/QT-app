#include <QObject>
#include "serverfunctions.h"

QMap<QString, IServerFunc*> ServerFunctions::funcs;
QString ServerFunctions::parse(QString str)
{
    if (!funcs.contains(str))
    {
        return "Command was not found. Echo: " + str + "\r\n";
    }
    return "\r\n";
}