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

#ifndef INC_SQLSTREAM_H
#define INC_SQLSTREAM_H

#ifdef WITH_PGSQL
  #include "PostgreSQL/psql_stream.h"
  typedef psql_stream sql_stream_exec;
#else
  #include "SQLite/sqlite_stream.h"
  typedef sqlite_stream sql_stream_exec;
#endif

#include <QString>
#include "db.h"
#include "database.h"
#include "sqlquery.h"
#include "dbtypes.h"
#include "db_excpt.h"

class db_cnx;

/**
   sql_stream class. Allows the parametrized execution of a query
   and easy retrieval of results
*/
class sql_stream
{
public:
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
  void find_param();
  void find_key_word();
  void check_params() const;
  sql_stream& next_param(QString value);
  void check_end_of_stream();
  QString next_result();

  QString m_query;
  // query parametrs
  int m_nArgPos;
  int m_nArgCount;
  bool m_bExecuted;

private:
  sql_stream_exec m_sqlite;
};






#endif // INC_SQLSTREAM_H
