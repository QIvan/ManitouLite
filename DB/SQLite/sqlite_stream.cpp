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
#include <QDateTime>
#include <QTextCodec>
#include <QDebug>

#include "db.h"
#include "connection.h"
#include "main.h"
#include "SQLite/sqlite_stream.h"
#include "db_excpt.h"




//================================== sqlite_stream ====================================//
//***************** Callback functions
/*int
sqlite_stream::callback (void* voidThis, int count, char** values, char** columnNames)
{
  sqlite_stream* This = static_cast<sqlite_stream*> (voidThis);
  return This->callback(count, values, columnNames);
}

int
sqlite_stream::callback(int count, char** values, char** columnNames)
{
  m_callback = true;
  if (count == 0)
    return SQLITE_ERROR;
  Rec rec;
  for (int i=0; i<count; ++i)
  {
    rec.insert(columnNames[i], values[i]);
  }
  m_resultData.append(rec);
  return 0;
}
*/
//======================== public ===============================
sqlite_stream::~sqlite_stream()
{
  if (m_sqlRes)
    sqlite3_finalize(m_sqlRes);
}

void
sqlite_stream::execute(QString query)
{
  const char *errmsg = 0;
  int rc = 0;
  bool isLocked = false;
  do
  {
    QTextCodec *Codec = QTextCodec::codecForName("UTF-8");
    rc = sqlite3_prepare(m_db.connection()->m_db->connection(),
                         Codec->fromUnicode(query).constData(),
                         -1, &m_sqlRes, &errmsg);
    m_status = sqlite3_step(m_sqlRes);
    isLocked = (rc == SQLITE_BUSY) || (m_status == SQLITE_LOCKED);
    if (isLocked)
      sleep(1);
  } while (isLocked);
  check_results(query, rc, errmsg);
  m_affected_rows = sqlite3_changes(m_db.connection()->m_db->connection());

  m_rowNumber=0;
  m_colNumber=0;
  m_bExecuted=true;
}

int
sqlite_stream::isEmpty()
{
  if (m_colNumber+1 < sqlite3_data_count(m_sqlRes))
    return false;
  return m_status == SQLITE_DONE;
}

int
sqlite_stream::affected_rows() const
{
  return m_affected_rows;
}


//======================== private ===============================

QString
sqlite_stream::next_result()
{
  QString result;
  if (m_status == SQLITE_ROW)
  {
    result = QString::fromUtf8(((const char*)sqlite3_column_text(m_sqlRes, m_colNumber)));
    increment_position();
  }
  return result;
}

void
sqlite_stream::check_results(QString query, int code_error, const QString& errmsg)
{
  if ((m_status == SQLITE_MISUSE) || (m_status == SQLITE_ERROR))
    throw db_excpt(query, m_db);

  if (code_error != SQLITE_OK)
    throw db_excpt(query, errmsg, QString::number(code_error));
}

void
sqlite_stream::increment_position()
{
  if (m_status != SQLITE_DONE)
  {
    m_colNumber = ((m_colNumber+1) % sqlite3_column_count(m_sqlRes));
    if (m_colNumber==0)
    {
      m_rowNumber++;
      m_status = sqlite3_step(m_sqlRes);
    }
  }
}
