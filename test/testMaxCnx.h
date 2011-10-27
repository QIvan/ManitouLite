#ifndef TESTMAXCNX_H
#define TESTMAXCNX_H
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "testBaseTestDB.h"

class test_max_db_cnx : public testBaseTestDB
{
    CPPUNIT_TEST_SUB_SUITE(test_max_db_cnx, testBaseTestDB);
    CPPUNIT_TEST(run);
    CPPUNIT_TEST_SUITE_END();
public:
    void run();
};



#endif // TESTMAXCNX_H
