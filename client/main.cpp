#include "loginwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Client::getInstance()->connectToServer("127.0.0.1", 44444);

    // Создание окна авторизации
    LoginWindow w;
    w.show();


    return a.exec();
}
