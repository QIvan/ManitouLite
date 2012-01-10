#include <QDebug>
#include "testSqlStream.h"
#include "RegistryTest.h"
#include "sqlstream.h"

std::string TestNames::sql_stream() { return "Sql Stream";}
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testSqlStream, TestNames::sql_stream());


void testSqlStream::empty()
{
    sql_stream empty("SELECT * FROM test_table where test_int_not_null='0'", *m_DB);
    CPPUNIT_ASSERT(empty.isEmpty());
}
void testSqlStream::noEmpty()
{
    InsertString_(2);
    sql_stream noEmpty("SELECT * FROM test_table", *m_DB);
    CPPUNIT_ASSERT(!noEmpty.isEmpty());
}

void testSqlStream::withVariable()
{
    try {
        InsertString_(3);
        sql_stream query_int(QString ("SELECT test_text FROM test_table WHERE ")
                             + "test_int_not_null=:p1 AND "
                             + "test_int_not_null=:p2 AND "
                             + "test_int_not_null=:p3 AND "
                             + "test_int_not_null=:p4 AND "
                             + "test_int_not_null=:p5 AND "
                             + "test_int_not_null=:p6"
                             , *m_DB);
        int nInt = 1;
        unsigned int nUnInt = 1;
        short nShort = 1;
        unsigned short nUnShort = 1;
        long nLong = 1;
        unsigned long nUnLong = 1;
        query_int << nInt << nUnInt << nShort << nUnShort << nLong << nUnLong;
        CPPUNIT_ASSERT_THROW(query_int << 1, db_excpt);
        CheckText_(query_int);

        sql_stream query_text("   SELECT test_int_not_null FROM test_table WHERE test_text=:p1",
                              *m_DB);
        query_text << "Test Text";
        CheckInts_(query_text);

        sql_stream query_text_like("SELECT test_int_not_null FROM test_table WHERE test_text like :p1",
                                   *m_DB);
        query_text_like << QString ("Test%");
        CheckInts_(query_text_like);

        sql_stream query_text_param("SELECT test_int_not_null FROM test_table WHERE test_text like :p1",
                                    *m_DB);
        query_text_param << "%1";
        CPPUNIT_ASSERT(query_text_param.isEmpty());

        sql_stream query_big_param ("Select * from test_table where test_id=:verybigtestparam", *m_DB);
        query_big_param << 1;
        CPPUNIT_ASSERT(!query_big_param.isEmpty());

    }
    catch(db_excpt e)
    {
        DebugExept_(e);
    }
}

void testSqlStream::with2Variable()
{
    try {
        InsertString_(3);
        sql_stream query_2_param ("SELECT test_int_not_null FROM test_table \
                                   WHERE (test_text like :p1||'%') \
                                   AND \
                                   (test_char200 like :p1||'%')" ,*m_DB);
        query_2_param << "Test";
        CheckInts_(query_2_param);

    }
    catch(db_excpt e)
    {
        DebugExept_(e);
    }
}

void testSqlStream::hardQuery()
{
  try {
    InsertString_(2);
    sql_stream query_char_star("SELECT test_text FROM test_table", *m_DB);
    char ch = 0;
    char str[10] = "Test Text";
    query_char_star >> ch >> str;
    CPPUNIT_ASSERT (ch == 'T');
    CPPUNIT_ASSERT (strcmp(str, "Test Text\\0"));
  }
  catch (db_excpt e)
  {
    DebugExept_(e);
  }
}

void testSqlStream::throwQuote()
{
  sql_stream bad_query ("Select * from test_table where test_text='Test", *m_DB);
}
