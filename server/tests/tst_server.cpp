#include <QtTest/QtTest>

#include "serverfunctionsmanager.h"

class ServerFunctionsTest : public QObject
{
    Q_OBJECT

private slots:
    void unknownCommandReturnsUnknown();
    void authArgsErrorWhenWrongNumber();
    void registrationArgsErrorWhenWrongNumber();
    void setStatsArgsErrorWhenBadInts();
    void findNodesByDepthArgsErrorWhenWrongNumber();
};

void ServerFunctionsTest::unknownCommandReturnsUnknown()
{
    QString res = ServerFunctionsManager::parse("no_such_command&arg1");
    QCOMPARE(res, QString("Unknown_command\r\n"));
}

void ServerFunctionsTest::authArgsErrorWhenWrongNumber()
{
    QString res1 = ServerFunctionsManager::parse("auth&only_one_arg");
    QCOMPARE(res1, QString("auth_args_error\r\n"));

    QString res2 = ServerFunctionsManager::parse("auth");
    QCOMPARE(res2, QString("auth_args_error\r\n"));
}

void ServerFunctionsTest::registrationArgsErrorWhenWrongNumber()
{
    QString res = ServerFunctionsManager::parse("reg&only_one");
    QCOMPARE(res, QString("reg_args_error\r\n"));
}

void ServerFunctionsTest::setStatsArgsErrorWhenBadInts()
{
    QString res1 = ServerFunctionsManager::parse("set_stats&u&t&notint&1");
    QCOMPARE(res1, QString("set_stats_args_error\r\n"));

    QString res2 = ServerFunctionsManager::parse("set_stats&u&1&badflag");
    QCOMPARE(res2, QString("set_stats_args_error\r\n"));
}

void ServerFunctionsTest::findNodesByDepthArgsErrorWhenWrongNumber()
{
    QString res = ServerFunctionsManager::parse("findNodesByDepth&only_one_arg");
    QCOMPARE(res, QString("findNodesByDepth_args_error\r\n"));
}

QTEST_MAIN(ServerFunctionsTest)

#include "tst_server.moc"
