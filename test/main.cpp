#include <QApplication>
#include <QDebug>
#include <cppunit/TextTestResult.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "db.h"
#include "main.h"
#include "RegistryTest.h"
#include "testBaseTestDB.h"
#include "sqlstream.h"



std::string TestNames::db_api() { return "DB_API";}

/*CPPUNIT_REGISTRY_ADD(TestNames::connect(), "temp");
CPPUNIT_REGISTRY_ADD(TestNames::transaction(), "temp");
CPPUNIT_REGISTRY_ADD(TestNames::disconnect(), "temp");*/

CPPUNIT_REGISTRY_ADD_TO_DEFAULT(TestNames::db_api());



class testTransaction : public testBaseTestDB
{
    CPPUNIT_TEST_SUB_SUITE(testTransaction, testBaseTestDB);
    CPPUNIT_TEST(transactionCommit);
    CPPUNIT_TEST(transactionRollback);
    CPPUNIT_TEST_SUITE_END();
public:
    void transactionCommit()
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
    void transactionRollback()
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
};

std::string TestNames::transaction() { return "Transaction";}
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testTransaction, TestNames::transaction());



int main(int argn, char** args)
{
    QApplication a(argn, args);
 //   global_debug_level = 3;
    CPPUNIT_NS :: TestResult testresult;

    // Listener zum Sammeln der Testergebnisse registrieren
    CPPUNIT_NS :: TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // Listener zur Ausgabe der Ergebnisse einzelner Tests
    CPPUNIT_NS :: BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // Test-Suite ueber die Registry im Test-Runner einfuegen
    CPPUNIT_NS :: TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS :: TestFactoryRegistry :: getRegistry ().makeTest ());
    testrunner.run (testresult);

    // Resultate im Compiler-Format ausgeben
    CPPUNIT_NS :: CompilerOutputter compileroutputter (&collectedresults, std::cerr);
    compileroutputter.write ();

    // Rueckmeldung, ob Tests erfolgreich waren
    return collectedresults.wasSuccessful () ? 0 : 1;


    /*// Get the top level suite from the registry
    CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CPPUNIT_NS::TextUi::TestRunner runner;
    runner.addTest( suite );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CPPUNIT_NS::CompilerOutputter( &runner.result(),
                                                            CPPUNIT_NS::stdCOut() ) );
    // Run the test.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;*/
}

