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
  void cancelRequest();
  sqlite3* connection() {
    return m_pSQLiteObj;
  }

private:
  sqlite3 *m_pSQLiteObj;
};

#endif // SQLITECONNECTION_H
