#include <QString>
#include "testConnectDisconnect.h"
#include "RegistryTest.h"
#include "db.h"

//std::string TestNames::connect(){ return "Connect"; }

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testConnect, TestNames::connect());
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testDisconnect, "Disconnect");

/*============================ testConnect ====================================*/
void testConnect::connect()
{
    QString errstr;
    CPPUNIT_ASSERT (ConnectDb("dbname=test user=ivan", &errstr));
}


/*============================ testDisconnect ====================================*/
void testDisconnect::disconnect()
{
    DisconnectDb();
}
