#include <QApplication>
#include <stdlib.h>
#include <cppunit/TextTestResult.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "addresses.h"
#include "config.h"
#include "database.h"
#include "date.h"
#include "db.h"
#include "db_listener.h"
#include "main.h"
#include "dbtypes.h"
#include "sqlquery.h"
#include "sqlstream.h"


/* Debugging level, set with --debug-level command like option.
   DBG_PRINTF() macro calls cause a display if their
   level (first argument) is lower or equal than global_debug_level.
   Normally the values should be:
   1: signal something that reveals an inconsistency inside our code

   2: signal something that shouldn't normally happen but may happen under certain circumstances (for example severe violations of RFC in the data).

   3 and higher: used for actual printf-like debugging. Use higher
   values for the more mundane messages or those that cause a lot of output
*/
int global_debug_level;




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




int main()
{
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

