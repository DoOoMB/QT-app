#include "QCoreApplication"
#include "QDebug"
#include "mytcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInfo() << "hhe";
    MyTcpServer server;
    return a.exec();
}