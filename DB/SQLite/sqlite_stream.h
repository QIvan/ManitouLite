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

#ifndef INC_SQLITE_SQLSTREAM_H
#define INC_SQLITE_SQLSTREAM_H
#include <QVector>
#include <QPair>
#include <QString>
#include <sqlite3.h>
#include "IStreamExec.h"

class db_cnx;

#ifdef NO_SQLITE
#error wrong headers included
#endif

/**
   sqlite_stream class. Allows the parametrized execution of a query
   and easy retrieval of results
*/
class sqlite_stream : IStreamExec
{
public:
  sqlite_stream(db_cnx& db): m_db(db), m_sqlRes(NULL) { }
  virtual ~sqlite_stream();

  void execute(QString query);
  int isEmpty(); //no const
  int affected_rows() const;
  QString next_result();
private:
  void check_results(QString query, int code_error, const QString& errmsg);
  void increment_position();

  db_cnx& m_db;
  // results
  bool m_bExecuted;
  sqlite3_stmt* m_sqlRes;
  int m_rowNumber;		/* last row read from the stream */
  int m_colNumber;		/* last column read from the stream */
  int m_status;
  int m_affected_rows;
};


#endif // INC_SQLSTREAM_H
