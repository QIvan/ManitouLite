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
#include <QMessageBox>
#include <QDebug>

#include "db.h"
#include "connection.h"
#include "main.h"
#include "PostgreSQL/psql_stream.h"

//================================== db_excpt ====================================//
db_excpt::db_excpt(const QString query,
			 const QString msg,
			 QString code/*=QString::null*/)
{
  m_query=query;
  m_err_msg=msg;
  m_err_code=code;
  DBG_PRINTF(3, "db_excpt: query='%s', err='%s'", m_query.toLocal8Bit().constData(), m_err_msg.toLocal8Bit().constData());
}

db_excpt::db_excpt(const QString query, db_cnx& d)
{
  m_query=query;
  char* pg_msg = PQerrorMessage(d.connection()->m_db->connection());
  if (pg_msg!=NULL)
    m_err_msg = QString::fromUtf8(pg_msg);
}

void DBEXCPT(db_excpt& p)
{
  //  std::cerr << p.query() << ":" << p.errmsg() << std::endl;
  QString err = p.query() + ":\n";
  err+=p.errmsg();
  QMessageBox::warning(NULL, QObject::tr("Database error"), err);
}
//___________________________________db_excpt______________________________________//



//======================== public ===============================
namespace service_f {
  void replace_random_param (QString &str)
  {
    str = str.replace("%", "\1");
  }
  void return_random_param (QString &str)
  {
    str = str.replace("\1", "%");
  }
}

sql_stream::sql_stream (const QString query, db_cnx& db) :
  m_db(db), m_query(query)
{
  QByteArray qba = query.toUtf8();
  init(qba.constData());
}

sql_stream::sql_stream (const char *query, db_cnx& db) :
  m_db(db), m_query(query)
{
  init(query);
}

sql_stream::~sql_stream()
{
  if (m_pgRes)
    PQclear(m_pgRes);
  if(!m_bExecuted)
    DBG_PRINTF(1, "QUERY NOT EXECUTE!");
}


// operators <<
sql_stream&
sql_stream::operator<<(const QString &s)
{
  return next_param('\'' + s + '\'');
}

sql_stream&
sql_stream::operator<<(const char* s)
{
  return next_param('\'' + QString(s) + '\'');
}

sql_stream&
sql_stream::operator<<(sql_null n _UNUSED_)
{
  return next_param(QString("null"));
}

//operators >>
sql_stream&
sql_stream::operator>>(int& i)
{
  i = next_result().toInt();
  return *this;
}

sql_stream&
sql_stream::operator>>(unsigned int& i)
{
  i = next_result().toUInt();
  return *this;
}

sql_stream&
sql_stream::operator>>(char& c)
{
  c = next_result().at(0).cell();
  return *this;
}

sql_stream&
sql_stream::operator>>(char* p)
{
  strcpy(next_result().toLocal8Bit().data(), p);
  return *this;
}

sql_stream&
sql_stream::operator>>(QString& s)
{
  s = next_result();
  return *this;
}
//______________end operators

namespace service_f {
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
sql_stream::execute()
{
  if (m_nArgPos < m_nArgCount)
    throw db_excpt(m_query, "Not all variables bound");

  DBG_PRINTF(5,"execute: %s", m_query.toLocal8Bit().constData());

  service_f::return_random_param(m_query);
  m_pgRes=PQexec(m_db.connection()->m_db->connection(), m_query.toLocal8Bit().constData());

  check_results();
  m_affected_rows = service_f::count_affected_rows(m_pgRes);

  m_rowNumber=0;
  m_colNumber=0;
  m_bExecuted=true;
}

int
sql_stream::isEmpty()
{
  if (!m_bExecuted)
    execute();
  return m_rowNumber >= PQntuples(m_pgRes);
}

int
sql_stream::affected_rows() const
{
  return m_affected_rows;
}


//======================== private ===============================
void
sql_stream::init (const char *query)
{
  m_nArgCount = 0;
  m_nArgPos = 0;
  m_pgRes = NULL;
  m_bExecuted = false;

  service_f::replace_random_param(m_query);
  find_and_replace_param(query);

  if(m_nArgCount == 0)
    execute();
}

void
sql_stream::find_and_replace_param(const char* query)
{
  QString sQuery = query;
  int pos = sQuery.indexOf(':');
  while (pos != -1)
  {
    if (sQuery.at(pos+1) != ':')
    {
      int rightPos = pos;
      while ((++rightPos < sQuery.size()) && sQuery.at(rightPos).isLetterOrNumber());
      int replaceCount = rightPos - pos;
      sQuery.replace(pos, replaceCount,
                     "%" + QString::number(m_nArgCount));
      ++m_nArgCount;
    }
    else
      pos += 2;
    pos = sQuery.indexOf(':', pos);
  }
  m_query = sQuery;
}

void
sql_stream::check_params() const
{
  if (m_nArgPos >= m_nArgCount)
    throw db_excpt(m_query, "Mismatch between bound variables and query");
}

sql_stream&
sql_stream::next_param(QString value)
{
  check_params();
  m_query = m_query.arg(value);
  ++m_nArgPos;
  return *this;
}

void
sql_stream::check_end_of_stream()
{
  if (isEmpty())
    throw db_excpt(m_query, "End of stream reached");
}

QString
sql_stream::next_result()
{
  QString result;
  check_end_of_stream();
  bool isNull = PQgetisnull(m_pgRes, m_rowNumber, m_colNumber);
  if (!isNull)
  {
    result = QString(PQgetvalue(m_pgRes, m_rowNumber, m_colNumber));
  }
  m_colNumber = ((m_colNumber+1) % PQnfields(m_pgRes));
  if (m_colNumber==0)
    m_rowNumber++;
  return result;
}

void
sql_stream::check_results()
{
  bool returns_rows = (m_query.indexOf("SELECT",0, Qt::CaseInsensitive) != -1);

  if (!m_pgRes)
    throw db_excpt(m_query, PQerrorMessage(m_db.connection()->m_db->connection()));

  if ((returns_rows && PQresultStatus(m_pgRes)!=PGRES_TUPLES_OK)
      || (!returns_rows && PQresultStatus(m_pgRes)!=PGRES_COMMAND_OK))
  {
    throw db_excpt(m_query, PQresultErrorMessage(m_pgRes),
       QString(PQresultErrorField(m_pgRes, PG_DIAG_SQLSTATE)));
  }
}
