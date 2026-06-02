#include <QObject>
#include <QMap>
#include "serverfunc.h"

using serverFunc = QString(*)(QStringList);

/**
 * @class ServerFunctionsManager
 * @brief Registry and dispatcher for server-side functions.
 *
 * Maps textual commands to function pointers and parses incoming
 * command strings to invoke the appropriate handler.
 */
class ServerFunctionsManager
{
    static QMap<QString, serverFunc> funcs;
    public:
        /**
         * @brief Parse and execute a command string.
         * @param str raw command (name and arguments)
         * @return handler result string
         */
        static QString parse(QString str);
};