#ifndef TESTSQLWRITEFIELDS_H
#define TESTSQLWRITEFIELDS_H
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "testBaseTestDB.h"

class db_excpt;

class test_sql_write_fields : public testBaseTestDB
{
    CPPUNIT_TEST_SUB_SUITE(test_sql_write_fields, testBaseTestDB);
    CPPUNIT_TEST(insert);
    CPPUNIT_TEST_EXCEPTION(insert_throw, db_excpt);
    CPPUNIT_TEST_SUITE_END();
public:
    void insert();
    void insert_throw();
private:
    void assertNumerateString (int num);
};

#endif // TESTSQLWRITEFIELDS_H
