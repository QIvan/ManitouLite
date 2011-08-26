/* Copyright (C) 2004-2007 Daniel Vit

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

#include "sqlquery.h"
#include "main.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WITH_PGSQL
#include <libpq-fe.h>
#endif

enum Coding
{
  latin1,
  utf8,
  local8bit
};

QString escapeString(const QString value, const Coding coding )
{

  QByteArray qba;
  if (coding == latin1)
    qba = value.toLatin1();
  else if (coding == utf8)
    qba = value.toUtf8();
  else if (coding == local8bit)
    qba = value.toLocal8Bit();

  char* dest = qba.data();
  bool alloc = false;
#ifdef WITH_PGSQL
  char local_value[1024];
  int value_len=value.length();
  if (value_len*2<(int)sizeof(local_value)-1)
    dest=local_value;
  else
  {
    alloc = true;
    dest=(char*)malloc(1+value_len*2);
  }
  PQescapeString(dest, qba.constData(), value_len); // FIXME ENCODING*/
#endif
  QString result;
  if (coding == latin1)
    result = QString::fromLatin1(dest);
  else if (coding == utf8)
    result = QString::fromUtf8(dest);
  else if (coding == local8bit)
    result = QString::fromLocal8Bit(dest);

  if (alloc)
    free(dest);
  return result;
}

void
sql_query::add_clause(const QString expr)
{
  if (m_num_clauses++>0) {
    m_where += " AND ";
  }
  m_where+=expr;
}

// field=<integer value>
void
sql_query::add_clause(const QString field, int value)
{
  char valbuf[12];
  sprintf(valbuf, "%d", value);
  add_clause(field, valbuf);
}

// field=<string value>
void sql_query::add_clause(const QString field, const QString value)
{
  add_clause(field,value,"=");
}

// field cmp <string value>
void
sql_query::add_clause(const QString field, const QString value, const char *cmp)
{
  if (m_num_clauses++>0) {
    m_where += " AND ";
  }
  QString dest = escapeString(value, latin1);
  m_where += field + QString(cmp) + QString("'") + dest + QString("'");
}

/*
  Returns the number of the table in the list. Right now it's useless, but
  it will probably be used in the future.
*/
int
sql_query::add_table(const QString tablename)
{
  // Add the table to the query only if it hasn't been done already
  if (m_tables_set.find(tablename) == m_tables_set.end()) {
    // Add it to the query's text
    if (m_num_tables++ > 0)
      m_tables += ",";
    m_tables += tablename;

    // Add it to the set of tables
    m_tables_set.insert(tablename);
  }
  return m_num_tables-1;
}

void
sql_query::add_final(const QString sql)
{
  m_end += sql;
}

void
sql_query::start(const QString sql)
{
  m_start = sql;
}

QString
sql_query::get()
{
  QString s=m_start + " FROM " + m_tables;
  if (!m_where.isEmpty())
    s += " WHERE " + m_where;
  if (!m_end.isEmpty())
    s += " " + m_end;
  return s;
}

QString
sql_query::subquery(const QString select_list)
{
  QString s = QString("SELECT ") + select_list + " FROM " + m_tables;
  if (!m_where.isEmpty())
    s += " WHERE " + m_where;
  // no m_end
  return s;
}

sql_write_fields::sql_write_fields(const QString encoding) : m_fields_nb(0)
{
  m_utf8 = (encoding=="UTF8");
}

void
sql_write_fields::add(const char* field, int value)
{
  char valbuf[12];
  sprintf(valbuf, "%d", value);
  add_no_quote(field, valbuf);
}

void
sql_write_fields::add(const char* field, const QString value, int maxlength)
{
  bool alloc=false;
  if (m_fields_nb++>0) {
    m_values+=",";
    m_fields+=",";
  }
  const char *trunc_value;
  QByteArray qvalue;
  if (m_utf8)
    qvalue=value.toUtf8();
  else
    qvalue=value.toLocal8Bit();

  trunc_value=qvalue.constData();

  if (maxlength) {
    if (strlen(trunc_value) > (uint)maxlength) {
      char* p = (char*)malloc(maxlength+1);
      alloc=true;
      strncpy (p, trunc_value, maxlength);
      p[maxlength] = '\0';
      trunc_value = p;
    }
  }

  if (m_utf8)
    m_values+=QString("'") + escapeString(trunc_value, utf8) + QString("'");
  else
    m_values+=QString("'") + escapeString(trunc_value, local8bit) + QString("'");


  m_fields+=field;
  if (alloc)
    free ((void*)trunc_value);
}

void
sql_write_fields::add_no_quote(const char* field, const QString value)
{
  if (m_fields_nb++>0) {
    m_values+=",";
    m_fields+=",";
  }
  m_values+=value;
  m_fields+=field;
}

void
sql_write_fields::add(const char* field, sql_null n _UNUSED_)
{
  add(field,"null");
}

void
sql_write_fields::add_if_not_empty(const char* field, const QString value,
				   int maxlength)
{
  if (!value.isEmpty()) {
    add(field, value, maxlength);
  }
}

void
sql_write_fields::add_if_not_zero(const char* field, int value)
{
  if (value) {
    add(field,value);
  }
}

const QString&
sql_write_fields::values()
{
  return m_values;
}

const QString&
sql_write_fields::fields()
{
  return m_fields;
}

