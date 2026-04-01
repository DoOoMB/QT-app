#include <QObject>

class IServerFunc
{
    public:
        IServerFunc();
        virtual QString run(QStringList args);
};