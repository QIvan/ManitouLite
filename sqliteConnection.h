#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H
#include <sqlite3.h>
#include <libpq-fe.h>
#include "database.h"

class sqliteConnection : public database
{
public:
  sqliteConnection();
  virtual ~sqliteConnection() {
    logoff();
  }
  int logon(const char* conninfo);
  void logoff();
  bool reconnect();
  bool ping();
  PGconn * connection() {
    return NULL;
  }

private:
  sqlite3 *m_pSQLiteObj;
};

#endif // SQLITECONNECTION_H
