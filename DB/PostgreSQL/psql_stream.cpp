/* Copyright (C) 2004-2008 Daniel Vйritй

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

#include <string>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <QDebug>
#include <QDateTime>

#include "db.h"
#include "connection.h"
#include "main.h"
#include "db_excpt.h"
#include "PostgreSQL/psql_stream.h"


//======================== public ===============================
psql_stream::~psql_stream()
{
  if (m_pgRes)
    PQclear(m_pgRes);
}



namespace execute {
  int count_affected_rows(PGresult* res)
  {
    int result = 0;
    const char* t = PQcmdTuples(res);
    if (t && *t)
      result = atoi(t);
    return result;
  }
}
void
psql_stream::execute(QString query)
{
  m_pgRes=PQexec(m_db.connection()->m_db->connection(), query.toLocal8Bit().constData());

  check_results(query);
  m_affected_rows = execute::count_affected_rows(m_pgRes);

  m_rowNumber=0;
  m_colNumber=0;
  m_bExecuted=true;
}

int
psql_stream::isEmpty()
{
  return m_rowNumber >= PQntuples(m_pgRes);
}

int
psql_stream::affected_rows() const
{
  return m_affected_rows;
}


QString
psql_stream::next_result()
{
  QString result;
  bool isNull = PQgetisnull(m_pgRes, m_rowNumber, m_colNumber);
  if (!isNull)
  {
    result = QString(PQgetvalue(m_pgRes, m_rowNumber, m_colNumber));
  }
  increment_position();
  return result;
}

//======================== private ===============================
void
psql_stream::check_results(QString query)
{
  bool returns_rows = (query.indexOf("SELECT",0, Qt::CaseInsensitive) != -1);

  if (!m_pgRes)
    throw db_excpt(query, PQerrorMessage(m_db.connection()->m_db->connection()));

  if ((returns_rows && PQresultStatus(m_pgRes)!=PGRES_TUPLES_OK)
      || (!returns_rows && PQresultStatus(m_pgRes)!=PGRES_COMMAND_OK))
  {
    throw db_excpt(query, PQresultErrorMessage(m_pgRes),
       QString(PQresultErrorField(m_pgRes, PG_DIAG_SQLSTATE)));
  }
}

void psql_stream::increment_position()
{
  m_colNumber = ((m_colNumber+1) % PQnfields(m_pgRes));
  if (m_colNumber==0)
    m_rowNumber++;
}
