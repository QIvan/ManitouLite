/* Copyright (C) 2004-2008 Daniel Vrit

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

#ifndef INC_PSQL_SQLSTREAM_H
#define INC_PSQL_SQLSTREAM_H

#include <libpq-fe.h>
#include <vector>
#include <QVector>
#include <QString>
#include "dbtypes.h"

class db_cnx;

/// sql Exception class
class db_excpt
{
public:
  db_excpt() {}
  db_excpt(const QString query, db_cnx& d);
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

/**
   sql_stream class. Allows the parametrized execution of a query
   and easy retrieval of results
*/
class sql_stream
{
public:
  sql_stream(const char* query, db_cnx& db);
  sql_stream(const QString query, db_cnx& db);
  virtual ~sql_stream();

  template<typename T>
  sql_stream& operator<<(T param)
  {
    return next_param(QString("%1").arg(param));
  }
  sql_stream& operator<<(const QString& );
  sql_stream& operator<<(const char* );
  sql_stream& operator<<(sql_null);

  sql_stream& operator>>(int&);
  sql_stream& operator>>(unsigned int&);
  sql_stream& operator>>(char*);
  sql_stream& operator>>(char&);
  sql_stream& operator>>(QString&);

  /** send the query to the server */
  void execute();
  /** true if there are no more results to read from the stream */
  int isEmpty(); //no const
  int affected_rows() const;
private:
  void init(const char* query);
  void find_and_replace_param(const char* query);
  void check_params() const;
  sql_stream& next_param(QString value);
  void check_end_of_stream();
  QString next_result();
  void check_results();

  db_cnx& m_db;
  QString m_query;
  // query parametrs
  int m_nArgPos;
  int m_nArgCount;
  // results
  bool m_bExecuted;
  PGresult* m_pgRes;
  int m_rowNumber;		/* last row read from the stream */
  int m_colNumber;		/* last column read from the stream */
  bool m_val_null;
  int m_affected_rows;
};

#endif // INC_PSQL_SQLSTREAM_H
