#include <QDebug>
#include <QString>
#include <QStringList>
#include "stdio.h"
#include "sqliteConnection.h"
#include "main.h"
#include "db_listener.h"


sqliteConnection::sqliteConnection() : m_pSQLiteObj(NULL)
{
}

int
sqliteConnection::logon(const char* conninfo)
{
  QString db_name = service_f::GetNameDb(conninfo);
  int isErorr = sqlite3_open(db_name.toLocal8Bit().constData(), &m_pSQLiteObj);
  set_encoding("UTF8");
  QString tmp("Connection error: ");
  tmp.append(QString("%1").arg(isErorr));
  qDebug() << tmp.toUtf8().constData();
  return isErorr ? 0 : 1;
}

void
sqliteConnection::logoff()
{
  if (m_pSQLiteObj) {
    sqlite3_close(m_pSQLiteObj);
    m_pSQLiteObj=NULL;
  }
}

bool
sqliteConnection::reconnect()
{
  if (m_pSQLiteObj) {
    /// @todo: найти функцию reconnect в sqlite3
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
sqliteConnection::ping()
{
  if (!m_pSQLiteObj)
    return false;

  char* err_msg;
  int res = sqlite3_exec(m_pSQLiteObj, "SELECT 1", NULL, 0, &err_msg);
  if (res != SQLITE_OK) {
    DBG_PRINTF(3, err_msg);
    sqlite3_free(err_msg);
    return false;
  }
  else
    return true;
}

void
sqliteConnection::cancelRequest()
{

}
