#include "connection.h"
#include "db_excpt.h"
#include "database.h"
#include "libpq-fe.h"


db_excpt::db_excpt(const QString query, const QString msg, QString code/*=QString::null*/)
          : m_query(query) , m_err_msg(msg) , m_err_code(code)
{
DBG_PRINTF(3, "db_excpt: query='%s', err='%s'",
   m_query.toLocal8Bit().constData(), m_err_msg.toLocal8Bit().constData());
}


db_excpt::db_excpt(const QString query, db_cnx& d)
{
  m_query=query;
  char* pg_msg = PQerrorMessage(d.connection()->m_db->connection());
  if (pg_msg!=NULL)
    m_err_msg = QString::fromUtf8(pg_msg);
}
