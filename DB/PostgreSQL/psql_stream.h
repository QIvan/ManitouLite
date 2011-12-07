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
#include <QString>
#include "IStreamExec.h"

class db_cnx;

#ifdef NO_PG
#error wrong headers included
#endif

/**
   sql_stream class. Allows the parametrized execution of a query
   and easy retrieval of results
*/
class psql_stream : public IStreamExec
{
public:
  psql_stream(db_cnx& db): m_db(db), m_pgRes(NULL) { }
  virtual ~psql_stream();

  void execute(QString query);
  int isEmpty(); //no const
  int affected_rows() const;
  QString next_result();
private:
  void check_results(QString query);
  void increment_position();

  db_cnx& m_db;
  // results
  bool m_bExecuted;
  PGresult* m_pgRes;
  int m_rowNumber;		/* last row read from the stream */
  int m_colNumber;		/* last column read from the stream */
  int m_status;
  int m_affected_rows;
};

#endif // INC_PSQL_SQLSTREAM_H
