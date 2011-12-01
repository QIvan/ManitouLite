#ifndef TESTSQLSTREAM_H
#define TESTSQLSTREAM_H
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "testBaseTestDB.h"

class testSqlStream : public testBaseTestDB
{
    CPPUNIT_TEST_SUB_SUITE(testSqlStream, testBaseTestDB);
    CPPUNIT_TEST(empty);
    CPPUNIT_TEST(noEmpty);
    CPPUNIT_TEST(withVariable);
    CPPUNIT_TEST(with2Variable);
    CPPUNIT_TEST(hardQuery);
    CPPUNIT_TEST_SUITE_END();
public:
    void empty();
    void noEmpty();
    void withVariable();
    void with2Variable();
    void hardQuery();
};


#endif // TESTSQLSTREAM_H
