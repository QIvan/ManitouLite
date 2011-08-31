#ifndef TESTSQLQUERY_H
#define TESTSQLQUERY_H
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "testBaseTestDB.h"

class sql_query;

class test_sqlquery : public testBaseTestDB
{
    CPPUNIT_TEST_SUB_SUITE(test_sqlquery, testBaseTestDB);
    CPPUNIT_TEST(exec_querys);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    sql_query build_query_str();
    sql_query build_query_int();

    void exec_querys();
};

#endif // TESTSQLQUERY_H
