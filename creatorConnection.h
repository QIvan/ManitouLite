#ifndef CREATECONNECTION_H
#define CREATECONNECTION_H

#include <list>
#include <QMutex>
#ifdef WITH_PGSQL
#include "database.h"
#endif


class pgConnection;

class creatorConnection
{
private:
  static creatorConnection* m_impl;
  creatorConnection(const char* connect_string);
  ~creatorConnection();

  static const int MAX_CNX=5;
  std::list<db_cnx_elt> m_cnx_list;
  QString m_connect_string;
  QMutex m_mutex;

  static pgConnection pgDb;
public:
  static int initialled(const char* connect_string, QString* errstr);
  static creatorConnection& getInstance();
  static void unInstance();

#ifdef WITH_PGSQL
  static pgConnection* getMainConnection();
#endif
  db_cnx_elt* getNewConnection();

};

#endif // CREATECONNECTION_H
