#include <QDebug>
#include <QTime>
#include "testBaseTestDB.h"
#include "sqlstream.h"
#include "db.h"

testBaseTestDB::testBaseTestDB(): m_DB(new db_cnx)
{
}

void testBaseTestDB::setUp()
{
    try{
        sql_stream query("DELETE FROM test_table", *m_DB);
    }
    catch(db_excpt e)
    {
      DebugExept_(e);
    }
}

void testBaseTestDB::tearDown()
{
    try{
      sql_stream query("DELETE FROM test_table", *m_DB);
    }
    catch(db_excpt e)
    {
        DebugExept_(e);
    }
}

void testBaseTestDB::DebugExept_(db_excpt &e)
{
    qDebug() << e.errcode();
    qDebug() << e.query();
    qDebug() << e.errmsg();
    CPPUNIT_FAIL("Database Exeption!");
}



void testBaseTestDB::InsertString_(int count, QString tableName)
{
  try
  {
    for (int last_field=1; last_field<=count; ++last_field)
    {
      sql_write_fields fiedls(m_DB->cdatab()->encoding());
      fiedls.add("test_text", "Test Text");
      fiedls.add("test_char200", "Test Text");
      fiedls.add_no_quote("test_date", '\'' + QDateTime::currentDateTime().toString("yyyy-MM-dd") + '\'');
      fiedls.add("test_int_not_null", last_field);
      QString str =  QString("INSERT INTO %1(%2) VALUES(%3)")
              .arg(tableName)
              .arg(fiedls.fields())
              .arg(fiedls.values());
      sql_stream insert(str, *m_DB);
      CPPUNIT_ASSERT (insert.affected_rows() == 1);
    }
  }
  catch(db_excpt e)
  {
    DebugExept_(e);
  }
}

void testBaseTestDB::ThrowQuery_(QString tableName)
{
    sql_stream insert(QString("SELECT error_column FROM %1").arg(tableName), *m_DB);
}

bool testBaseTestDB::TableIsEmpty_(QString tableName)
{
    db_cnx db;
    sql_stream stream("SELECT * FROM " + tableName, db);
    return stream.isEmpty();
}

int testBaseTestDB::CountString_(QString tableName)
{
    db_cnx db;
    sql_stream stream("SELECT count(*) FROM " + tableName, db);
    int count = 0;
    stream >> count;
    return count;
}

QString testBaseTestDB::AddSpace_(const QString &str, int count)
{
    QString result = str;
    count = 10;
    /*for (int i=0; i<count-str.size(); ++i)
        result.append(' ');*/
    return result;
}

void testBaseTestDB::CheckText_(sql_stream &stream)
{
  QString text;
  CPPUNIT_ASSERT (!stream.isEmpty());
  stream >> text;
  CPPUNIT_ASSERT (stream.isEmpty());
  CPPUNIT_ASSERT (text == "Test Text");
}

void testBaseTestDB::CheckInts_(sql_stream &stream)
{
  int int1, int2, int3;
  stream >> int1 >> int2 >> int3;
  CPPUNIT_ASSERT (int1 == 1);
  CPPUNIT_ASSERT (int2 == 2);
  CPPUNIT_ASSERT (int3 == 3);
}

