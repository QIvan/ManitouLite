#ifndef CREATECONNECTION_H
#define CREATECONNECTION_H

#include <list>
#include <QMutex>
#ifdef WITH_PGSQL
#include "database.h"
#endif


class creatorConnection
{
  friend class db_cnx;
private:
  static creatorConnection* m_impl;
  creatorConnection(const char* connect_string);
  ~creatorConnection();

  static const int MAX_CNX=5;
  std::list<db_cnx_elt> m_cnx_list;
  QString m_connect_string;
  QMutex m_mutex;

  static db_cnx_elt m_main_cnx;

public:
  static int initialled(const char* connect_string, QString* errstr);
private:
  static creatorConnection& getInstance();
  static void unInstance();

  static db_cnx_elt* getMainConnection();
  db_cnx_elt* getNewConnection();
  bool idle();

};

#endif // CREATECONNECTION_H
