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

#include "filter_rules.h"
#include "db.h"
#include "sqlstream.h"
#include "users.h"
#include "tags.h"

filter_expr::filter_expr() : m_dirty(false), m_delete(false), m_new(false)
{
  m_expr_id=0;
  m_match_type=0;
}

filter_expr::~filter_expr()
{
}

filter_action::filter_action()
{
  m_action_order=0;
//  m_action_type=0;
}

filter_action::~filter_action()
{
}

void
filter_action::set_action_string_from_db(const QString arg)
{
  if (m_str_atype=="tag") {
    m_action_string = message_tag::convert_separator_from_ascii(arg);
  }
  else
    m_action_string = arg;
}

QString
filter_action::action_string_to_db()
{
  if (m_str_atype=="tag") {
    return message_tag::convert_separator_to_ascii(m_action_string);
  }
  else
    return m_action_string;
}

expr_list::expr_list()
{
}

expr_list::~expr_list()
{
}

bool
expr_list::fetch()
{
  bool result=true;
  db_cnx db;
  try {
    filter_expr* current=NULL;
    QString action_type, action_args, tmp;
    int expr_id;
    sql_stream s("SELECT fe.expr_id, fe.name, fe.expression, fe.direction, action_type,action_arg FROM filter_action fa RIGHT JOIN filter_expr fe ON fe.expr_id=fa.expr_id ORDER BY expr_id,action_order", db);
    while (!s.eof()) {
      s >> expr_id;
      if (!current || current->m_expr_id != expr_id) {
	// new expr
	filter_expr e;
	e.m_expr_id = expr_id;
	push_back(e);
	current = &(back());
	s >> current->m_expr_name >> current->m_expr_text >> current->m_direction;
      }
      else {
	char c;
	s >> tmp >> tmp >> c;
      }
      s >> action_type >> action_args;
      if (!action_type.isEmpty()) {
	filter_action a;
	a.m_str_atype = action_type;
	a.set_action_string_from_db(action_args);
	current->m_actions.push_back(a);
      }
    }
  }
  catch(db_excpt& p) {
    DBEXCPT(p);
    result=false;
  }
  return result;
}


bool
expr_list::update_db()
{
  bool result=true;
  db_cnx db;
  try {
    db.begin_transaction();
    sql_stream s_upd("UPDATE filter_expr SET name=:name,expression=:expr, user_lastmod=:user, last_update=now(),direction=':d' WHERE expr_id=:id", db);
    sql_stream s_del_expr("DELETE FROM filter_expr WHERE expr_id=:id", db);
    sql_stream s_ins("INSERT INTO filter_expr(expr_id,name,expression,direction) VALUES (:id,:name,:expr,':dir')", db);
    // TODO: use a sequence
    sql_stream s_seq("SELECT 1+coalesce(max(expr_id),0) FROM filter_expr", db);
    sql_stream s_del_act("DELETE FROM filter_action WHERE expr_id=:p1", db);
    sql_stream s_add_act("INSERT INTO filter_action(expr_id,action_arg,action_type,action_order) VALUES(:p1,:p2,:p3,:p4)", db);

    std::list<filter_expr>::iterator it = begin();
    for (; it != end(); ++it) {
      unsigned int db_expr_id=it->m_expr_id;
      if (!db_expr_id) {
	if (!it->m_delete) {
	  if (s_seq.eof())
	    s_seq.execute();
	  s_seq >> db_expr_id;
	  s_ins << db_expr_id;
	  if (it->m_expr_name.isEmpty())
	    s_ins << sql_null() << it->m_expr_text;
	  else
	    s_ins << it->m_expr_name << it->m_expr_text << it->m_direction;
	}
 	// else ignore the entry: it has been created then deleted.
      }
      else if (it->m_delete) {
	it->m_actions.clear(); // to avoid inserting actions below
	if (db_expr_id) {	// should always be true
	  s_del_act << db_expr_id;
	  s_del_expr << db_expr_id;
	}
      }
      else if (it->m_dirty) {
	if (it->m_expr_name.isEmpty())
	  s_upd << sql_null();
	else
	  s_upd << it->m_expr_name;
	s_upd << it->m_expr_text << user::current_user_id() << it->m_direction << db_expr_id;
	// on update, delete all old actions and insert the new ones below
	s_del_act << db_expr_id;
      }
      // insert actions
      if ((it->m_dirty || it->m_expr_id==0) && !it->m_delete) {
	int aorder=1;
	std::list<filter_action>::iterator ait;
	for (ait=it->m_actions.begin(); ait!=it->m_actions.end(); ++ait) {
	  s_add_act << db_expr_id << ait->action_string_to_db();
	  s_add_act << ait->m_str_atype << aorder++;
	}
      }
    } // for each expr
    db.commit_transaction();
  }
  catch(db_excpt& p) {
    db.rollback_transaction();
    DBEXCPT(p);
    result=false;
  }
  return result;
}

filter_expr*
expr_list::find_name(const QString& name)
{
  std::list<filter_expr>::iterator it = begin();
  for (; it != end(); ++it) {
    if ((*it).m_expr_name == name)
      return &(*it);
  }
  return NULL;
}
