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
    InsertString_(2);
    sql_stream noEmpty("SELECT * FROM test_table", *m_DB);
    CPPUNIT_ASSERT(!noEmpty.eof());
}

void testSqlStream::withVariable()
{
    try {
        InsertString_(3);
        sql_stream query_int("SELECT test_text FROM test_table WHERE test_int_not_null=:p1",
                             *m_DB);
        query_int << 1;
        CheckText_(query_int);

        sql_stream query_text("SELECT test_int_not_null FROM test_table WHERE test_text=:p1",
                             *m_DB);
        query_text << "Test Text";
        CheckInts_(query_text);

        sql_stream query_text_like("SELECT test_int_not_null FROM test_table WHERE test_text like :p1",
                             *m_DB);
        query_text_like << "Test%";
        CheckInts_(query_text_like);

        sql_stream query_text_int_null("SELECT test_int_not_null FROM test_table WHERE test_text like :p1",
                             *m_DB);
        query_text_int_null << "%1";
        CPPUNIT_ASSERT(query_text_int_null.eof());
    }
    catch(db_excpt e)
    {
        DebugExept_(e);
    }
}
