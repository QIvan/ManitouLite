#include "testTransaction.h"
#include "RegistryTest.h"

std::string TestNames::transaction() { return "Transaction";}
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testTransaction, TestNames::transaction());


void testTransaction::transactionCommit()
{
    CPPUNIT_ASSERT(TableIsEmpty_());
    db_cnx db;
    try
    {
        db.begin_transaction();

        InsertString_();
        InsertString_();
        db.commit_transaction();
        CPPUNIT_ASSERT(2 == CountString_());

    }
    catch (db_excpt e)
    {
        db.rollback_transaction();
        CPPUNIT_ASSERT(0 == CountString_());
        qDebug() << e.query();
        qDebug() << e.errmsg();
        CPPUNIT_FAIL("Database Exept!");
    }
}

void testTransaction::transactionRollback()
{
    CPPUNIT_ASSERT(TableIsEmpty_());
    db_cnx db;
    try
    {
        db.begin_transaction();
        InsertString_();
        InsertString_();
        ThrowQuery();
        db.commit_transaction();
    }
    catch (db_excpt e)
    {
        db.rollback_transaction();
        CPPUNIT_ASSERT(0 == CountString_());
    }
}