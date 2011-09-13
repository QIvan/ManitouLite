#include "pgConnection.h"
#include "main.h"
#include "db_listener.h"
#include "sqlstream.h"


pgConnection::pgConnection(): m_pgConn(NULL)
{}

int
pgConnection::logon(const char* conninfo)
{
  m_pgConn = PQconnectdb(conninfo);
  if (!m_pgConn) {
    throw db_excpt("connect", "not enough memory");
  }
  DBG_PRINTF(5,"logon m_pgConn=0x%p", m_pgConn);
  if (PQstatus(m_pgConn) == CONNECTION_BAD) {
    throw db_excpt("connect", PQerrorMessage(m_pgConn));
  }

  /* If the user has set PGCLIENTENCODING in its environment, then we decide
     to do no translation behind the postgresql client layer, since we
     assume that the user knows what he's doing. In the future, we may
     decide for a fixed encoding (that would be unicode/utf8, most
     probably) and override PGCLIENTENCODING. */
  if (!getenv("PGCLIENTENCODING")) {
    PGresult* res=PQexec(m_pgConn, "SELECT pg_encoding_to_char(encoding) FROM pg_database WHERE datname=current_database()");
    if (res && PQresultStatus(res)==PGRES_TUPLES_OK) {
      const char* enc=(const char*)PQgetvalue(res,0,0);
      // pgsql versions under 8.1 return 'UNICODE', >=8.1 return 'UTF8'
      // we keep UTF8
      if (!strcmp(enc,"UNICODE"))
        enc="UTF8";
      set_encoding(enc);
    }
    if (res)
      PQclear(res);
  }
  PQclear(PQexec(m_pgConn, "SET standard_conforming_strings=on"));
  return 1;
}

void
pgConnection::logoff()
{
  if (m_pgConn) {
    PQfinish(m_pgConn);
    m_pgConn=NULL;
  }
}

bool
pgConnection::reconnect()
{
  if (m_pgConn) {
    PQreset(m_pgConn);
    if (PQstatus(m_pgConn)!=CONNECTION_OK)
      return false;
  }
  for (int i=0; i<m_listeners.size(); i++) {
    /* Reinitialize listeners. It is necessary if the db backend
       process went down, and if the socket changed */
    m_listeners.at(i)->setup_notification();
    m_listeners.at(i)->setup_db();
  }
  return true;
}

bool
pgConnection::ping()
{
  if (!m_pgConn)
    return false;
  PGresult* res = PQexec(m_pgConn, "SELECT 1");
  if (res) {
    bool ret = (PQresultStatus(res)==PGRES_TUPLES_OK);
    PQclear(res);
    return ret;
  }
  else
    return false;
}

void
pgConnection::cancelRequest()
{
  PQrequestCancel(m_pgConn);
}

PGconn*
pgConnection::connection() {
  return m_pgConn;
}
