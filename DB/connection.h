#ifndef CONNECTION_H
#define CONNECTION_H

#include <tr1/memory>
#ifdef WITH_PGSQL
#include "PostgreSQL/pgConnection.h"
typedef pgConnection ConnectionClass;
#else
#include "sqliteConnection.h"
typedef sqliteConnection ConnectionClass;
#endif

template <class T>
class connection
{
protected:
  typedef T typeDB;
  virtual ~connection(){}
};

/// Connection class
class db_cnx_elt : public connection<ConnectionClass>
{
public:
  db_cnx_elt() :
    m_db(new typeDB),
    m_available(true),
    m_connected(false)
  {  }
  virtual ~db_cnx_elt() {
    m_available=false;
    m_connected=false;
  }
  std::tr1::shared_ptr<typeDB> m_db;
  bool m_available;
  bool m_connected;
};


#endif // CONNECTION_H
