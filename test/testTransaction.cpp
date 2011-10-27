#include <QDebug>
#include "testTransaction.h"
#include "RegistryTest.h"
#include "sqlstream.h"
#include "db.h"

std::string TestNames::transaction() { return "Transaction";}
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(test_Transaction, TestNames::transaction());


void test_Transaction::transactionCommit()
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
        DebugExept_(e);
    }
}

void test_Transaction::transactionRollback()
{
    CPPUNIT_ASSERT(TableIsEmpty_());
    db_cnx db;
    try
    {
        db.begin_transaction();
        InsertString_();
        InsertString_();
        ThrowQuery_();
        db.commit_transaction();
    }
    catch (db_excpt e)
    {
        db.rollback_transaction();
        CPPUNIT_ASSERT(0 == CountString_());
    }
}
