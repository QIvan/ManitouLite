/* Copyright (C) 2004-2010 Daniel Verite

   This file is part of Manitou-Mail (see http://www.manitou-mail.org)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifdef _WINDOWS
#include <windows.h>
#endif
#include <QMessageBox>
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
#include <stdio.h>
#include "sqlstream.h"
#include "creatorConnection.h"
#include "db.h"
#include "main.h"
#include "connection.h"


int
ConnectDb(const char* cnx_string, QString* errstr)
{
  return creatorConnection::ConnectDb(cnx_string, errstr);
}

void
DisconnectDb()
{
  // close primary connection
  creatorConnection::DisconnectDb();
}


//=============================== db_cnx ============================================//
db_cnx::db_cnx(bool other_thread) : m_cnx(NULL), m_other_thread(other_thread)
{
  try{
    if (!other_thread) {
      // just use the main connection for the main thread
      m_cnx = creatorConnection::getMainConnection();
      return;
    }
    else {
      m_cnx = creatorConnection::getInstance().getNewConnection();
    }
  }
  catch(std::exception)
  {
    DBG_PRINTF(1, "Not initialled creatorConnection!");
  }
}

db_cnx::~db_cnx()
{
  if (m_other_thread)
    if (m_cnx)
    {
      m_cnx->m_available = true;
      m_cnx->m_db->logoff();
      m_cnx->m_connected = false;
    }
}

db_cnx_elt*
db_cnx::connection() {
  return m_cnx;
}
database*
db_cnx::datab() {
  return m_cnx->m_db.get();
}
const database*
db_cnx::cdatab() const {
  return m_cnx->m_db.get();
}

//int
//db_cnx::lo_creat(int mode)
//{
//    //return ::lo_creat(this->connection()->m_db->connection(), mode);
//}

//int
//db_cnx::lo_open(uint lobjId, int mode)
//{
//    //return ::lo_open(this->connection()->m_db->connection(), lobjId, mode);
//}

//int
//db_cnx::lo_read(int fd, char *buf, size_t len)
//{
//    //return ::lo_read(this->connection()->m_db->connection(), fd, buf, len);
//}

//int
//db_cnx::lo_write(int fd, const char *buf, size_t len)
//{
//    //return ::lo_write(this->connection()->m_db->connection(), fd, buf, len);
//}

//int
//db_cnx::lo_import(const char *filename)
//{
//    //return ::lo_import(this->connection()->m_db->connection(), filename);
//}

//int
//db_cnx::lo_close(int fd)
//{
//    //return ::lo_close(this->connection()->m_db->connection(), fd);
//}

void
db_cnx::cancelRequest()
{
  this->connection()->m_db->cancelRequest();
}

bool
db_cnx::next_seq_val(const char* seqName, int* id)
{
  try {
    QString query = QString("SELECT nextval('%1')").arg(seqName);
    sql_stream s(query, *this);
    s >> *id;
  }
  catch(db_excpt& p) {
    DBEXCPT(p);
    return false;
  }
  return true;
}

bool
db_cnx::next_seq_val(const char* seqName, unsigned int* id)
{
  try {
    QString query = QString("SELECT nextval('%1')").arg(seqName);
    sql_stream s(query, *this);
    s >> *id;
  }
  catch(db_excpt& p) {
    DBEXCPT(p);
    return false;
  }
  return true;
}

void
db_cnx::begin_transaction()
{
  m_cnx->m_db->begin_transaction();
  // don't use nested transactions
  DBG_PRINTF(5,"open_transactions_count()=%d", datab()->open_transactions_count());
  if (datab()->open_transactions_count()==1) {
    sql_stream s("BEGIN", *this);
  }
}

void
db_cnx::commit_transaction()
{
  end_transaction();
  if (datab()->open_transactions_count()==0) {
    sql_stream s("COMMIT", *this);
  }
}

void
db_cnx::rollback_transaction()
{
  end_transaction();
  if (datab()->open_transactions_count()==0) {
    sql_stream s("ROLLBACK", *this);
  }
}

void
db_cnx::end_transaction() {
  m_cnx->m_db->end_transaction();
}

void
db_cnx::handle_exception(db_excpt& e)
{
  if (m_alerts_enabled) {
    DBEXCPT(e);
  }
}

bool
db_cnx::ping()
{
  if (!m_cnx) return false;
  return m_cnx->m_db->ping();
}

/* idle(): Return false if at least one non-primary connection is in
   use, meaning that we're probably running a query in a sub-thread.
   Should be called to avoid hitting the db with multiple simultaneous
   queries whenever possible */
// static
bool
db_cnx::idle()
{
  return creatorConnection::getInstance().idle();
}

// static
const QString&
db_cnx::dbname()
{
	return creatorConnection::getInstance().m_dbname;
}
//___________________________________db_cnx______________________________________//



db_transaction::db_transaction(db_cnx& db): m_commit_done(false)
{
  m_pDb=&db;
  db.begin_transaction();
}

db_transaction::~db_transaction()
{
  if (m_pDb->datab()->open_transactions_count()==1 && !m_commit_done)
    m_pDb->rollback_transaction();
//  m_pDb->datab()->end_transaction();
}

void
db_transaction::commit()
{
  m_commit_done=true;
  if (m_pDb->datab()->open_transactions_count()==1) {
    m_pDb->commit_transaction();
  }
}

void
db_transaction::rollback()
{
  if (m_pDb->datab()->open_transactions_count()==1) {
    m_pDb->rollback_transaction();
  }
}

