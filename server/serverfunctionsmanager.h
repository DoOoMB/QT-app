#include <QObject>
#include <QMap>
#include "serverfunc.h"

class ServerFunctionsManager
{
    static QMap<QString, IServerFunc*> funcs;
    public:
        static QString parse(QString str);
};