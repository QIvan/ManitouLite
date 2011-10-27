#ifndef TESTCONNECTDISCONNECT_H
#define TESTCONNECTDISCONNECT_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


class testConnect : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(testConnect);
    CPPUNIT_TEST(connect);
    CPPUNIT_TEST(ping);
    CPPUNIT_TEST_SUITE_END();
public:
    void connect();
    void ping();
};


class testDisconnect : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(testDisconnect);
    CPPUNIT_TEST(disconnect);
    CPPUNIT_TEST_FAIL(ping);
    CPPUNIT_TEST_SUITE_END();
public:
    void disconnect();
    void ping();
};

#endif // TESTCONNECTDISCONNECT_H
