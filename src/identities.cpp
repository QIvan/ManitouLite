/* Copyright (C) 2004-2007 Daniel Vrit

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

#include "main.h"
#include "app_config.h"
#include "identities.h"
#include "db.h"
#include "sqlstream.h"

mail_identity::mail_identity() : m_internal_id(0), m_dirty(false), m_fetched(false)
{
}

mail_identity::~mail_identity()
{
}

bool
identities::fetch(bool force/*=false*/)
{
  if (!force && m_fetched)
    return true;

  int start_id=1;
  db_cnx db;
  const QString default_email=get_config().get_string("default_identity");
  try {
    sql_stream s("SELECT email_addr,username,xface,signature FROM identities ORDER BY email_addr", db);
    while (!s.eof()) {
      mail_identity id;
      s >> id.m_email_addr >> id.m_name >> id.m_xface >> id.m_signature;
      if (id.m_email_addr.isEmpty()) {
	// Ignore identities with no email to be safe. This shouldn't happen.
	continue;
      }
      id.m_internal_id=start_id++; // unused, remove later. FIXME
      id.m_fetched=true;
      id.m_orig_email_addr = id.m_email_addr;
      id.m_is_default = (id.m_email_addr == default_email);	
      (*this)[id.m_email_addr]=id;
    }
  }
  catch(db_excpt& p) {
    DBEXCPT(p);
    clear();
    return false;
  }
  return true;
}

bool
mail_identity::update_db()
{
  db_cnx db;
  try {
    sql_stream ss("SELECT 1 FROM identities WHERE email_addr=:p1", db);
    ss << m_orig_email_addr;
    if (!ss.eof()) {
      sql_stream su("UPDATE identities SET email_addr=:p1, username=:p2, xface=:p3, signature=:p4 WHERE email_addr=:p5", db);
      su << m_email_addr << m_name << m_xface << m_signature;
      su << m_orig_email_addr;
//      DBG_PRINTF(5,"updated identity '%s'\n", m_email_addr.latin1());
    }
    else {
      sql_stream si("INSERT INTO identities(email_addr,username,xface,signature) VALUES (:p1,:p2,:p3,:p4)", db);
      si << m_email_addr << m_name << m_xface << m_signature;
//      DBG_PRINTF(5,"inserted identity '%s'\n", m_email_addr.latin1());
    }
    m_orig_email_addr = m_email_addr;
  }
  catch(db_excpt& p) {
    DBEXCPT(p);
    return false;
  }
  return true;
}

mailbox::mailbox() : m_id(0)
{
}

mailbox::~mailbox()
{
}

bool
mailboxes::fetch(bool force/*=false*/)
{
  if (!force && m_fetched)
    return true;

  db_cnx db;
  try {
    sql_stream s("SELECT mbox_id, name FROM mailboxes", db);
    while (!s.eof()) {
      mailbox m;
      s >> m.m_id >> m.m_email;
      (*this)[m.m_email]=m;
    }
    m_fetched=true;
  }
  catch(db_excpt& p) {
    DBEXCPT(p);
    clear();
    return false;
  }
  return true;
}

/*
  Given a mailbox id, return its name
*/
const QString&
mailboxes::name_by_id(int id)
{
  static QString null = QString::null;
  if (!fetch(false)) return null;
  mailboxes::const_iterator iter = begin();
  while (iter!=end()) {
    if (iter->second.m_id==id)
      return iter->first;
    ++iter;
  }
  return null;
}

