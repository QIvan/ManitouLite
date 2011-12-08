#include <QDebug>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QProcess>
#include <QDateTime>
#include <cppunit/TextTestResult.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "db.h"
//#include "main.h"
#include "RegistryTest.h"
#include "testBaseTestDB.h"
#include "sqlstream.h"


std::string TestNames::db_api() { return "DB_API";}

CPPUNIT_REGISTRY_ADD_TO_DEFAULT(TestNames::db_api());

//int global_debug_level;

int main()
{
  //global_debug_level=1;
    //global_debug_level = 3;
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

/*
void
debug_printf(int level, const char* file, int line, const char *fmt, ...)
{
  if (level<=global_debug_level) {
    fprintf(stderr, "%s, line %d: ", file, line);
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    fflush(stderr);
  }

}

void
err_printf(const char* file, int line, const char *fmt, ...)
{
  fprintf(stderr, "%s, line %d: ERR: ", file, line);
  va_list ap;
  va_start(ap,fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  fflush(stderr);
}


QString service_f::toCodingDb(const QString &s)
{
    /// @todo разобраться с кодировкой
    /*QByteArray qb;
    db_cnx db;
    if (db.cdatab()->encoding()=="UTF8") {
      qb = s.toUtf8();
    }
    else {
      qb = s.toLocal8Bit();
    }*//*
    return s;
}

QString service_f::GetNameDb(const char* conninfo)
{
  QString str = conninfo;
  QStringList list = str.split(QChar(' '));
  list = list.first().split(QChar('='));
  QString db_name = list[1];
  db_name.append(".db");
  return db_name;
}
*/
