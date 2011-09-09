#ifndef TESTBASETESTDB_H
#define TESTBASETESTDB_H
#include <QString>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <tr1/memory>

class db_cnx;
class db_excpt;
class sql_stream;

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
    void DebugExept_(db_excpt& e);
    void InsertString_(int count = 1, QString tableName = "test_table");
    void ThrowQuery_(QString tableName = "test_table");
    bool TableIsEmpty_(QString tableName = "test_table");
    int  CountString_(QString tableName = "test_table");
    QString AddSpace_(const QString& str, int count);

    void CheckText_(sql_stream &stream);
    void CheckInts_(sql_stream &stream);

};

#endif // TESTBASETESTDB_H
