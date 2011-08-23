#include <QDebug>
#include "RegistryTest.h"
#include "testSqlQuery.h"
#include "sqlstream.h"

std::string TestNames::sql_query() { return "Sql Query";}
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(test_sqlquery, TestNames::sql_query());
void test_sqlquery::setUp()
{
  InsertString_(1);
  InsertString_(2);
  InsertString_(3);
}

sql_query test_sqlquery::build_query_str()
{
  sql_query query;
  query.start("SELECT test_text");
  query.add_table("test_table");
  query.add_clause("test_int_not_null", 2);
  query.add_final("ORDER BY test_id");
  return query;
}
sql_query test_sqlquery::build_query_int()
{
  sql_query query;
  query.start("SELECT test_int_not_null");
  query.add_table("test_table");
  query.add_clause("test_text", "Test Text");
  query.add_final("ORDER BY test_id");
  return query;
}

void test_sqlquery::exec_querys()
{
  try {
    sql_query q_str = build_query_str();
    sql_stream exec_str(q_str.get(),*m_DB);
    QString text;
    exec_str >> text;
    CPPUNIT_ASSERT(text == "Test Text");

    sql_query q_int = build_query_int();
    sql_stream exec_int(q_int.get(),*m_DB);
    int num1, num2, num3;
    exec_int >> num1 >> num2 >> num3;
    CPPUNIT_ASSERT((num1==1) && (num2==2) && (num3==3));
  }
  catch(db_excpt e)
  {
    qDebug() << e.errcode();
    qDebug() << e.query();
    qDebug() << e.errmsg();
    throw  e;
  }
}
