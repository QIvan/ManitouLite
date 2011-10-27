#include "testDbListener.h"
#include "RegistryTest.h"

std::string TestNames::listener(){ return "LISTENER"; }

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(test_db_listener, TestNames::listener());

Notifer::Notifer(QObject *parent) :
    QObject(parent)
{
}
