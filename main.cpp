#include <QApplication>
#include <stdlib.h>
#include <QStringList>
#include "main.h"

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
    }*/
    return s;
}


QString service_f::GetNameDb(const char *conninfo)
{
  QString str = conninfo;
  QStringList list = str.split(QChar(' '));
  list = list.first().split(QChar('='));
  QString db_name = list[1];
  db_name.append(".db");
  return db_name;
}

int main()
{
  qDebug() << "Is release Manitou database module";
  return 0;
}
