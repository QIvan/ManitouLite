#ifndef TESTSQLWRITEFIELDS_H
#define TESTSQLWRITEFIELDS_H
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <tr1/memory>

class db_cnx;

class test_sql_write_fields : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(test_sql_write_fields);
    CPPUNIT_TEST(insert);
    CPPUNIT_TEST_SUITE_END();
private:
    std::tr1::shared_ptr<db_cnx> m_DB;
public:
    test_sql_write_fields();
    void insert();
    void tearDown();
private:
    void assertNumerateString (int num);
};

#endif // TESTSQLWRITEFIELDS_H
