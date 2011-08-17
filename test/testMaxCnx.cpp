#include "testMaxCnx.h"
#include "RegistryTest.h"
#include "database.h"
#include "sqlstream.h"


std::string TestNames::max_db_cnx() { return "Query Thread";}
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(test_max_db_cnx, TestNames::max_db_cnx());

void test_max_db_cnx::run()
{
    CPPUNIT_ASSERT(db_cnx::idle());
    db_cnx db_insert1(true);
    sql_stream insert1("INSERT INTO table_for_thread(field) VALUES(1)", db_insert1);
    db_cnx db_insert2(true);
    sql_stream insert2("INSERT INTO table_for_thread(field) VALUES(1)", db_insert2);
    db_cnx db_insert3(true);
    sql_stream insert3("INSERT INTO table_for_thread(field) VALUES(1)", db_insert3);
    db_cnx db_select(true);
    sql_stream select("SELECT * FROM table_for_thread", db_select);
    db_cnx db_del(true);
    sql_stream del("DELETE FROM table_for_thread", db_del);
    CPPUNIT_ASSERT_THROW(db_cnx db_throw(true), db_excpt);
    CPPUNIT_ASSERT(!db_cnx::idle());
}









