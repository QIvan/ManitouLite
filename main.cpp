#include <QApplication>
#include <stdlib.h>
#include "db.h"
#ifndef QT_NO_DEBUG
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#endif

using namespace std;

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


QString gl_xpm_path; // used by FT_MAKE_ICON (icons.h)
QString gl_tr_path;
QString gl_help_path;

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


void
usage(const char* progname)
{
#ifdef Q_WS_WIN
  QMessageBox::critical(NULL, "Arguments error", "The possible arguments are --dbcnx followed by a connection string and --config followed by a configuration name.\nExample, --dbcnx \"dbname=mail user=mailadmin host=pgserver\" --config myconf");
#else
  fprintf(stderr, "Usage: %s [--debug-output=level] [--config=confname] \"connect string\".\nThe connect string looks like, for example, \"dbname=mail user=mailadmin host=pgserver\".\n",
          progname);
#endif
  exit(1);
}

#ifndef QT_NO_DEBUG



int main()
{
    // Get the top level suite from the registry
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
    return wasSucessful ? 0 : 1;
}
#else
int main()
{


    return 1;
}

#endif
