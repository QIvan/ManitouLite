#ifndef TESTTRANSACTION_H
#define TESTTRANSACTION_H
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "testBaseTestDB.h"

class test_Transaction : public testBaseTestDB
{
    CPPUNIT_TEST_SUB_SUITE(test_Transaction, testBaseTestDB);
    CPPUNIT_TEST(transactionCommit);
    CPPUNIT_TEST(transactionRollback);
    CPPUNIT_TEST_SUITE_END();
public:
    void transactionCommit();
    void transactionRollback();
};



#endif // TESTTRANSACTION_H
