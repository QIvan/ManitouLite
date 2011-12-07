#include "connection.h"
#include "db_excpt.h"
#include "database.h"
#include "sqlite3.h"



db_excpt::db_excpt(const QString query, db_cnx& d)
{
  m_query=query;
  const char* pg_msg = sqlite3_errmsg(d.connection()->m_db->connection());
  if (pg_msg!=NULL)
    m_err_msg = QString::fromUtf8(pg_msg);
}



