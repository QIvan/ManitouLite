/* Copyright (C) 2004,2005 Daniel Vrit

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

#include "date.h"
#include <time.h>
#include <locale.h>

date::date(const QString date)
{
  if (date.isEmpty()) {
    m_null=true;
    return;
  }
  m_null=false;
  m_sYYYYMMDDHHMMSS=date;
  m_sDate=date.mid(6,2)+"/"+date.mid(4,2)+"/"+date.mid(0,4);

  m_day = date.mid(6,2).toInt();
  m_month = date.mid(4,2).toInt();
  m_year = date.mid(0,4).toInt();

  m_hour=date.mid(8,2).toInt();
  m_min=date.mid(10,2).toInt();
  m_sec=date.mid(12,2).toInt();
}

bool
date::operator<(const date& other) const
{
  return m_sYYYYMMDDHHMMSS.compare(other.FullOutput()) < 0;
}

QString
date::OutputHM(int date_format) const
{
  QString s;
  if (m_null)
    return QString("");

  if (date_format==1)		// european
    s.sprintf("%02d/%02d/%04d %02d:%02d", m_day, m_month, m_year, m_hour, m_min);
  else				// US
    s.sprintf("%04d/%02d/%02d %02d:%02d", m_year, m_month, m_day, m_hour, m_min);
  return s;
}

// returns a 24 character representation that is suitable for
// the From line that separates messages in the mbox format
QString
date::output_24() const
{
  if (m_null)
    return QString("");

  struct tm t;
  t.tm_year = m_year-1900;
  t.tm_mday = m_day;
  t.tm_mon = m_month-1;
  t.tm_hour = m_hour;
  t.tm_min = m_min;
  t.tm_sec = m_sec;
  t.tm_wday = 0;
  t.tm_yday = 0;
  t.tm_isdst = 0;
  // use a portable locale for the Date: field
  char* loc=setlocale(LC_TIME, "C");
  char* s = asctime (&t);
  if (loc)
    setlocale(LC_TIME, loc);
  return QString(s);
}
