#include <QDebug>
#include "testSqlStream.h"
#include "RegistryTest.h"
#include "sqlstream.h"

std::string TestNames::sql_stream() { return "Sql Stream";}
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testSqlStream, TestNames::sql_stream());


void testSqlStream::empty()
{
    sql_stream empty("SELECT * FROM test_table where test_int_not_null='0'", *m_DB);
    CPPUNIT_ASSERT(empty.eof());
}
void testSqlStream::noEmpty()
{
    InsertString_();
    InsertString_();
    sql_stream empty("SELECT * FROM test_table", *m_DB);
    CPPUNIT_ASSERT(!empty.eof());
}

void testSqlStream::withVariable()
{
    try {
        InsertString_(1);
        InsertString_(2);
        InsertString_(3);
        sql_stream query_int("SELECT test_text FROM test_table WHERE test_int_not_null=:p1",
                             *m_DB);
        query_int << 1;
        QString text;
        query_int >> text;
        CPPUNIT_ASSERT (text == "Test Text");

        sql_stream query_text("SELECT test_int_not_null FROM test_table WHERE test_text=:p1",
                             *m_DB);
        query_text << "Test Text";
        int int1, int2, int3;
        query_text >> int1 >> int2 >> int3;
        CPPUNIT_ASSERT (int1 == 1);
        CPPUNIT_ASSERT (int2 == 2);
        CPPUNIT_ASSERT (int3 == 3);
    }
    catch(db_excpt e)
    {
        DebugExept(e);
    }
}
