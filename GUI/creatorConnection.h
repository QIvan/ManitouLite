#ifndef CREATECONNECTION_H
#define CREATECONNECTION_H

#include <list>
#include <QMutex>
#include "db.h"


class creatorConnection
{
  friend class db_cnx;
private:
  static creatorConnection* m_impl;
  creatorConnection(const char* connect_string, QString dbname);
  ~creatorConnection();

  static const int MAX_CNX=5;
  std::list<db_cnx_elt> m_cnx_list;
  QString m_connect_string;
  QString m_dbname;
  QMutex m_mutex;

  static db_cnx_elt m_main_cnx;

public:
  static int ConnectDb(const char* connect_string, QString* errstr);
  static void DisconnectDb();
private:
  static creatorConnection& getInstance();
  static void unInstance();

  static db_cnx_elt* getMainConnection();
  db_cnx_elt* getNewConnection();
  void disconnect_all();
  bool idle();

};

#endif // CREATECONNECTION_H
