#include <QObject>
#include <QMap>
#include "serverfunc.h"

using serverFunc = QString(*)(QStringList);
class ServerFunctionsManager
{
    static QMap<QString, serverFunc> funcs;
    public:
        static QString parse(QString str);
};