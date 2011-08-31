#include <QDebug>
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
      qDebug() << e.errcode();
      qDebug() << e.query();
      qDebug() << e.errmsg();
      throw  e;
    }
}

void testBaseTestDB::tearDown()
{
    try{
      sql_stream query("DELETE FROM test_table", *m_DB);
    }
    catch(db_excpt e)
    {
      qDebug() << e.errcode();
      qDebug() << e.query();
      qDebug() << e.errmsg();
      throw  e;
    }
}

void testBaseTestDB::InsertString_(int last_field, QString tableName)
{
    sql_write_fields fiedls(m_DB->cdatab()->encoding());
    fiedls.add("test_text", "Test Text");
    fiedls.add("test_char200", "Test Text");
    fiedls.add_no_quote("test_date", "datetime('now')");
    fiedls.add("test_int_not_null", last_field);
    QString str =  QString("INSERT INTO %1(%2) VALUES(%3)")
            .arg(tableName)
            .arg(fiedls.fields())
            .arg(fiedls.values());
    sql_stream insert(str, *m_DB);
}

void testBaseTestDB::ThrowQuery(QString tableName)
{
    sql_stream insert(QString("SELECT error_column FROM %1").arg(tableName), *m_DB);
}

bool testBaseTestDB::TableIsEmpty_(QString tableName)
{
    db_cnx db;
    sql_stream stream("SELECT * FROM " + tableName, db);
    return stream.eof();
}

int testBaseTestDB::CountString_(QString tableName)
{
    db_cnx db;
    sql_stream stream("SELECT count(*) FROM " + tableName, db);
    int count = 0;
    stream >> count;
    return count;
}
