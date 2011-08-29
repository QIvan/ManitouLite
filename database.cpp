#include "main.h"

#include "db.h"
#include <stdio.h>

#ifdef _WINDOWS
#include <windows.h>
#endif

#include <sys/stat.h>
#include <locale.h>
#include <iostream>
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>

#include <QByteArray>
#include <QMessageBox>
#include <QRegExp>
#include <QStringList>
#include <QTextCodec>

#include "database.h"
#include "db_listener.h"
#include "creatorConnection.h"
#include "sqlstream.h"
#include "sqlquery.h"


//============================== database =======================================//
database::database() : m_open_trans_count(0)
{
}

database::~database()
{
}

void database::begin_transaction()
{
  m_open_trans_count++;
}

void database::commit_transaction()
{
}

void database::rollback_transaction()
{
}

int database::open_transactions_count() const
{
  return m_open_trans_count;
}

void
database::add_listener(db_listener* listener)
{
  m_listeners.append(listener);
}

void
database::end_transaction()
{
  m_open_trans_count--;
  DBG_PRINTF(4, "new m_open_trans_count=%d", m_open_trans_count);
  if (m_open_trans_count<0) {
    fprintf(stderr, "Fatal error: m_open_trans_count<0\n");
    exit(1);
  }
}

// fetch the current date and time in DD/MM/YYYY HH:MI:SS format
//static
bool
database::fetchServerDate(QString& date)
{
  bool result=true;
  try {
  db_cnx db;
  sql_stream query("SELECT to_char(now(),\'DD/MM/YYYY HH24::MI::SS\')", db);
  query >> date;
  }
  catch (db_excpt e) {
    DBEXCPT(e);
    result=false;
  }
  return result;
}
//_______________________________database____________________________________________//


