#include "creatorConnection.h"
#include "iostream"
#include "stdexcept"
#include "main.h"

creatorConnection* creatorConnection::m_impl = NULL;
pgConnection creatorConnection::pgDb;
///bool creatorConnection::m_initialized = false;

//========================= Private =================================//
creatorConnection::creatorConnection(const char* connect_string) :
  m_connect_string (connect_string)
{
  for (int i=0; i<MAX_CNX; i++) {
    db_cnx_elt p;
    m_cnx_list.push_back(p);
  }
}

creatorConnection::~creatorConnection()
{
  pgDb.logoff();
}
//____________________________________________________________________//

//static
int
creatorConnection::initialled(const char* connect_string, QString* errstr)
{
  try{
    pgDb.logon(connect_string);
    m_impl = new creatorConnection(connect_string);
  }
  catch(db_excpt& p) {
    QByteArray errmsg_bytes = p.errmsg().toLocal8Bit();
    std::cerr << errmsg_bytes.constData() << std::endl;
    if (errstr)
      *errstr = p.errmsg();
    return 0;
  }

  return true;
}

//======================== For Singlton ===============================//
//static
creatorConnection&
creatorConnection::getInstance()
{
  if (m_impl == NULL)
    throw std::exception();
  return *(m_impl);
}

//static
void
creatorConnection::unInstance()
{
  if (m_impl)
    delete m_impl, m_impl = NULL;

}
//_____________________________________________________________________//

#ifdef WITH_PGSQL
//static
pgConnection*
creatorConnection::getMainConnection()
{
  return &pgDb;
}
#endif

db_cnx_elt*
creatorConnection::getNewConnection()
{
  db_cnx_elt* result;
  m_mutex.lock();
  std::list<db_cnx_elt>::iterator it = m_cnx_list.begin();
  for (; it!=m_cnx_list.end(); it++) {
    if ((*it).m_available) {
      pgConnection* p;
      if (!(*it).m_connected) {
        p = new pgConnection;
        (*it).m_db = p;
        p->logon(m_connect_string.toLocal8Bit().constData());
        DBG_PRINTF(3, "Opening a new database connection");
        (*it).m_connected=true;
      }
      (*it).m_available=false;
      result = &(*it);
      break;
    }
  }
  m_mutex.unlock();
  if (it==m_cnx_list.end()) {
    DBG_PRINTF(2, "No database connection found");
    throw db_excpt(NULL, QObject::tr("The %1 database connections are already in use.").arg(MAX_CNX));
  }
  return result;
}

