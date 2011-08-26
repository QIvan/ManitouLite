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

#ifndef INC_DATABASE_H
#define INC_DATABASE_H
#include <QString>
#include <QThread>
#include <QMutex>
#include <QList>
#include <list>

class db_cnx;
class db_listener;

/*
  Top-level database class.
  Implementations for different RDBMS inherit from this class and
  override the appropriate functions depending on the database
  capabilities and API
*/
class database
{
public:
  database();
  virtual ~database();
  // fetch the current date and time in DD/MM/YYYY HH:MI:SS format
  static bool fetchServerDate(QString&);
  virtual void begin_transaction();
  virtual void commit_transaction();
  virtual void rollback_transaction();
  virtual int logon(const char* conninfo)=0;
  virtual void logoff()=0;
  virtual bool reconnect()=0;
  virtual bool ping()=0;
  void end_transaction();
  int open_transactions_count() const;
  const QString& encoding() const {
    return m_encoding;
  }
  void add_listener(db_listener* listener);
protected:
  int set_encoding(const QString encoding) {
    m_encoding=encoding;
    return 0;
  }
  QList<db_listener*> m_listeners;
private:
  /* Number of open transactions.

     This is used to allow for nested beginTransaction/endTransactions
     pairs in a RDBMS where commit blocks are not nested (i.e. commit
     or rollback is applied to all changes that occurred after the
     last commit or rollback, without savepoints like in Oracle).
     commit or rollback issued in non-top level transactions blocks
     will be ignored; only the top level transaction commit or
     rollback will be sent to the db backend, thus commiting or
     rolling back all the changes made in the nested transactions as
     well as in the top level block.  */
  int m_open_trans_count;
  // currently SQL_ASCII or UNICODE
  QString m_encoding;
};


/// sql Exception class
class db_excpt
{
public:
  db_excpt() {}
  db_excpt(const QString query, db_cnx& d);
  // defined in db.cpp
  db_excpt(const QString query, const QString msg, QString code=QString::null);
  virtual ~db_excpt() {}
  QString query() const { return m_query; }
  QString errmsg() const { return m_err_msg; }
  QString errcode() const { return m_err_code; }
  bool unique_constraint_violation() const {
    return m_err_code=="23505";
  }
private:
  QString m_query;
  QString m_err_msg;
  QString m_err_code;
};

void DBEXCPT(db_excpt& p);	// db.cpp






//---------------------------- NOT USING IN CLASS ------------------------------//


// Transaction object. The destructor issues a rollback if commit has
// not been called and we're a top level transaction within our
// connection
class db_transaction
{
public:
//  db_transaction(database& db);
  db_transaction(db_cnx& d);
  virtual ~db_transaction();
  void commit();
  void rollback();
private:
  db_cnx* m_pDb;
  bool m_commit_done;
};

#endif // INC_DATABASE_H
