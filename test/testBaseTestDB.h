#ifndef TESTBASETESTDB_H
#define TESTBASETESTDB_H
#include <QString>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <tr1/memory>

class db_cnx;

class testBaseTestDB : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(testBaseTestDB);
    CPPUNIT_TEST_SUITE_END();
protected:
    std::tr1::shared_ptr<db_cnx> m_DB;
public:
    testBaseTestDB();
    void setUp();
    void tearDown();
protected:
    void InsertString_(QString tableName = "test_table");
    void ThrowQuery(QString tableName = "test_table");
    bool TableIsEmpty_(QString tableName = "test_table");
    int  CountString_(QString tableName = "test_table");

};

#endif // TESTBASETESTDB_H
