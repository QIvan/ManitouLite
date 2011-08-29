#ifndef PGCONNECTION_H
#define PGCONNECTION_H
#include "database.h"
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>


class pgConnection : public database
{
public:
  pgConnection();
  virtual ~pgConnection() {
    logoff();
  }
  int logon(const char* conninfo);
  void logoff();
  bool reconnect();
  bool ping();
  void cancelRequest();
  PGconn* connection();
private:
  PGconn* m_pgConn;
};

#endif // PGCONNECTION_H
