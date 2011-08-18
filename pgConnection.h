#ifndef PGCONNECTION_H
#define PGCONNECTION_H
#include "database.h"
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>

class pgConnection : public database
{
public:
  pgConnection(): m_pgConn(NULL) {}
  virtual ~pgConnection() {
    logoff();
  }
  int logon(const char* conninfo);
  void logoff();
  bool reconnect();
  bool ping();
  PGconn* connection() {
    return m_pgConn;
  }
private:
  PGconn* m_pgConn;
};

#endif // PGCONNECTION_H
