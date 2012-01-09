/* Copyright (C) 2004-2008 Daniel V– ‚Ññrit– ‚Ññ

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

#include <QDateTime>
#include <QDebug>
#include <QRegExp>

#include "main.h"
#include "sqlstream.h"


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
  m_query(query), m_nArgPos(0), m_nArgCount(0),
  m_bExecuted(false), m_sqlite(db)
{
  service_f::replace_random_param(m_query);
  find_key_word();
  find_and_replace_param();

  if(m_nArgCount == 0)
    execute();
}

sql_stream::~sql_stream()
{
  if(!m_bExecuted)
  {
/*#ifndef QT_DEBUG
    throw db_excpt(m_query, "QUERY NOT EXECUTE!");
#else*/
    DBG_PRINTF(1, "QUERY NOT EXECUTE!");
//#endif
  }
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

void
sql_stream::execute()
{
  if (m_nArgPos < m_nArgCount)
    throw new db_excpt(m_query, "Not all variables bound");

  DBG_PRINTF(5,"execute: %s", m_query.toLocal8Bit().constData());

  service_f::return_random_param(m_query);
  try{
    m_sqlite.execute(m_query);
    m_bExecuted = true;
  }
  catch(db_excpt e)
  {
    m_bExecuted = false;
    throw e;
  }
}

int
sql_stream::isEmpty()
{
  if (!m_bExecuted)
    m_sqlite.execute(m_query);
  return m_sqlite.isEmpty();
}

int
sql_stream::affected_rows() const
{
  return m_sqlite.affected_rows();
}


//======================== private ===============================

namespace service_f {
bool isBetweenQuote(QString str, int pos)
{
  QList<int> quoteList;
  int quotePos = str.indexOf('\'');
  while (quotePos != -1)
  {
    quoteList.append(quotePos);
    quotePos = str.indexOf('\'', quotePos+1);
  }

  bool result = false;
  while (!quoteList.isEmpty())
  {
    int firstQuote = quoteList.takeFirst();
    if (quoteList.isEmpty())
      throw new db_excpt(str, "bad query!");
    int secondQuote = quoteList.takeFirst();
    if ((firstQuote < pos) && (pos < secondQuote))
      result = true;
  }
  return result;
}
}

void
sql_stream::find_key_word()
{
  QString sQuery = m_query;
  const QString NOW = ":now:";
  int pos_now = sQuery.indexOf(NOW);
  if (pos_now != -1)
  {
    sQuery = sQuery.replace(NOW,
                            '\'' + QDateTime::currentDateTime().
                                   toString("dd.MM.yyyy hh::mm::ss.zzzz")
                            + '\'');
  }
  m_query = sQuery;
}

void
sql_stream::find_and_replace_param()
{
  QString sQuery = m_query;
  QRegExp reg(":[A-Za-z]{,255}[0-9]{,10}");
  int pos = 0;
  while (reg.indexIn(sQuery, pos) != -1)
  {
    QString param = reg.cap();
    if (!service_f::isBetweenQuote(sQuery, sQuery.indexOf(param)))
    {
      sQuery.replace(param, "%"+QString::number(m_nArgCount));
      ++m_nArgCount;
    }

    pos += reg.matchedLength();
  }
  sQuery.replace("::", ":");
  m_query = sQuery;
}

void
sql_stream::check_params() const
{
  if (m_nArgPos >= m_nArgCount)
    throw new db_excpt(m_query, "Mismatch between bound variables and query");
}

sql_stream&
sql_stream::next_param(QString value)
{
  check_params();
  m_query = m_query.arg(value);
  ++m_nArgPos;
  if (m_nArgPos == m_nArgCount)
    execute();
  return *this;
}

void
sql_stream::check_end_of_stream()
{
  if (isEmpty())
    throw new db_excpt(m_query, "End of stream reached");
}

QString
sql_stream::next_result()
{
  check_end_of_stream();
  return m_sqlite.next_result();
}



