#include <QString>
#include "testConnectDisconnect.h"
#include "RegistryTest.h"
#include "db.h"

std::string TestNames::connect(){ return "Connect"; }
std::string TestNames::disconnect(){ return "Disconnect"; }

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testConnect, TestNames::connect());
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testDisconnect, TestNames::disconnect());

/*============================ testConnect ====================================*/
void testConnect::connect()
{
    QString errstr;
    /// @todo: в этой функции утечка
    CPPUNIT_ASSERT (ConnectDb("dbname=test_db_api", &errstr));
}

void testConnect::ping()
{
    db_cnx db;
    CPPUNIT_ASSERT_ASSERTION_PASS(db.ping());
}

/*============================ testDisconnect ====================================*/
void testDisconnect::disconnect()
{
    /// @todo: в этой функции утечка
    DisconnectDb();
}

void testDisconnect::ping()
{
    db_cnx db;
    CPPUNIT_ASSERT_ASSERTION_FAIL(db.ping());
}
