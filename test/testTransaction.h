#ifndef TESTTRANSACTION_H
#define TESTTRANSACTION_H
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "testBaseTestDB.h"
#include <QDebug>
#include "sqlstream.h"


class testTransaction : public testBaseTestDB
{
    CPPUNIT_TEST_SUB_SUITE(testTransaction, testBaseTestDB);
    CPPUNIT_TEST(transactionCommit);
    CPPUNIT_TEST(transactionRollback);
    CPPUNIT_TEST_SUITE_END();
public:
    void transactionCommit();
    void transactionRollback();
};



#endif // TESTTRANSACTION_H
