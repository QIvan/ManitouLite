#include <QDebug>
#include "testSqlWriteFields.h"
#include "RegistryTest.h"
#include "db.h"
#include "date.h"
#include "database.h"
#include "db_excpt.h"
#include "sqlstream.h"


std::string TestNames::sql_wirte_fields() { return "sql_wirte_fields"; }
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(test_sql_write_fields, TestNames::sql_wirte_fields());

void test_sql_write_fields::insert()
{
    try{
        m_DB->reset_seq("sequence_id");
        //первый запрос
        sql_write_fields fiedls(m_DB->cdatab()->encoding());
        assertNumerateString (0);
        int next_id;
        CPPUNIT_ASSERT(m_DB->next_seq_val("sequence_id", &next_id));
        fiedls.add("test_id", next_id);
        fiedls.add("test_text", "Insert Text 1");
        fiedls.add_if_not_empty("test_char200", "Insert Text 1", 9); //Insert Te
        fiedls.add_no_quote("test_date", ":now:");
        fiedls.add_if_not_zero("test_int_not_null", 0);
        fiedls.add_if_not_zero("test_int_not_null", 1);
        QString str =  QString("INSERT INTO test_table(%1) VALUES(%2)")
                .arg(fiedls.fields())
                .arg(fiedls.values());
        sql_stream insert(str,*m_DB);
        QString dbTime;
        CPPUNIT_ASSERT(m_DB->datab()->fetchServerDate(dbTime));
        assertNumerateString (1);

        //второй запрос
        db_cnx db(true);
        sql_write_fields fiedls2(db.cdatab()->encoding());
        fiedls2.add_if_not_empty("test_text", "Insert Text 2", 9);//Insert Te
        fiedls2.add("test_char200", "Insert Text 2");
        fiedls2.add_no_quote("test_date", ":now:");
        fiedls2.add_if_not_zero("test_int_not_null", 0);
        fiedls2.add_if_not_zero("test_int_not_null", 2);
        QString str2 =  QString("INSERT INTO test_table(%1) VALUES(%2)")
                .arg(fiedls2.fields())
                .arg(fiedls2.values());
        sql_stream insert2(str2,*m_DB);
        QString dbTime2;
        CPPUNIT_ASSERT(m_DB->datab()->fetchServerDate(dbTime2));
        assertNumerateString (2);

        //проверяем что добавилось
        /// @todo: было так: TO_CHAR(test_date,'YYYYMMDDHH24MISS') !!!
        sql_stream check("SELECT test_id, test_text, test_char200, test_date, test_int_not_null FROM test_table",*m_DB);
        int id, int_not_null;
        QString text, char200, dateStr;
        check >> id >> text >> char200 >> dateStr >> int_not_null;
        date dateDate (dateStr);
        CPPUNIT_ASSERT(id != 0);
        CPPUNIT_ASSERT(text == "Insert Text 1");
        CPPUNIT_ASSERT(char200 == AddSpace_("Insert Te", 200));
        CPPUNIT_ASSERT(!dateDate.is_null());
        CPPUNIT_ASSERT(int_not_null == 1);
        QString dateStr2;
        check >> id >> text >> char200 >> dateStr2 >> int_not_null;
        date dateDate2 (dateStr2);
        CPPUNIT_ASSERT(id != 0);
        CPPUNIT_ASSERT(text == "Insert Te");
        CPPUNIT_ASSERT(char200 == AddSpace_("Insert Text 2", 200));
        /// @todo: если свалится на этом ассерте, то просто запусти тест ещё раз
        CPPUNIT_ASSERT(dateDate.OutputHM(1) == dateDate2.OutputHM(1));
        dbTime.truncate(dbTime.indexOf('.', dbTime.indexOf(' ')));
        dbTime2.truncate(dbTime2.indexOf('.', dbTime2.indexOf(' ')));
        CPPUNIT_ASSERT_MESSAGE("Если этот assert свалился - перезапусти тест",
                               dbTime == dbTime2);
        CPPUNIT_ASSERT(int_not_null == 2);

    }
    catch(db_excpt e)
    {
        DebugExept_(e);
    }

}

void test_sql_write_fields::insert_throw()
{
    sql_write_fields fiedls(m_DB->cdatab()->encoding());
    assertNumerateString (0);
    fiedls.add("test_id", 1);
    fiedls.add_if_not_zero("test_int_not_null", 0);
    QString str =  QString("INSERT INTO test_table(%1) VALUES(%2)")
            .arg(fiedls.fields())
            .arg(fiedls.values());
    sql_stream insert(str,*m_DB);
}

void test_sql_write_fields::assertNumerateString(int num)
{
    sql_stream query_count("Select count(*) From test_table", *m_DB);
    int count = 0;
    query_count >> count;
    CPPUNIT_ASSERT(count == num);
}
