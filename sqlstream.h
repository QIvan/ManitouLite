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

#include "sqlquery.h"
#include "database.h"
#include "db.h"
#ifdef WITH_PGSQL
  #include "PostgreSQL/psql_stream.h"
#else
  //#include "SQLite/sqlite_stream.h"
#endif


#endif // INC_SQLSTREAM_H
