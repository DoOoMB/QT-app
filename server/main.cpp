#include "QCoreApplication"
#include "QDebug"
#include "mytcpserver.h"
#include "dbmanager.h"
#include <QStringList>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << QT_VERSION_STR;

    MyTcpServer server;
    DBManager::Instance();

    return a.exec();
}