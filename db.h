/* Copyright (C) 2004-2010 Daniel Verite

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

#ifndef INC_DB_H
#define INC_DB_H
#include <QString>
#include <QVariant>
#include <list>
//#include "sqlstream.h"
//#include "database.h"

class db_cnx_elt;
class db_excpt;
class database;

int ConnectDb(const char*, QString*);
void DisconnectDb();

class db_cnx
{
public:
  db_cnx(bool other_thread=false);
  virtual ~db_cnx();
  db_cnx_elt* connection();
  database* datab();
  const database* cdatab() const;
  int lo_creat(int mode);
  int lo_open(uint lobjId, int mode);
  int lo_read(int fd, char *buf, size_t len);
  int lo_write(int fd, const char *buf, size_t len);
  int lo_import(const char *filename);
  int lo_close(int fd);
  void cancelRequest();
  bool next_seq_val(const char*, int*);
  bool next_seq_val(const char*, unsigned int*);
  // overrides database's methods
  void begin_transaction();
  void commit_transaction();
  void rollback_transaction();
  void end_transaction();
  //void enable_user_alerts(bool); // return previous state
  bool ping();
  void handle_exception(db_excpt& e);

  static bool idle();
  static const QString& dbname();
private:
  db_cnx_elt* m_cnx;
  bool m_other_thread;
  bool m_alerts_enabled;
};


#endif // INC_DB_H
