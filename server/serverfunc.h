#include <QObject>

/**
 * @brief Interface for server-side command handlers.
 *
 * Implementations should provide a `run` method that accepts string
 * arguments and returns a result string.
 */
class IServerFunc
{
    public:
        IServerFunc();
        /**
         * @brief Execute the server function with arguments.
         * @param args list of string arguments
         * @return result string (response to client)
         */
        virtual QString run(QStringList args);
};