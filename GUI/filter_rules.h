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

#ifndef INC_FILTER_RULES_H
#define INC_FILTER_RULES_H

#include <QString>
#include <list>

// filter_action table
class filter_action
{
public:
  filter_action();
  virtual ~filter_action();
  /* Convert an action argument from db format to internal format.
     This is currently used for tag hierarchies for which the separator
     differs between the UI and DB. */
  void set_action_string_from_db(const QString arg);

  /* Convert an action argument from internal format to db.
     See set_action_string_from_db() comment. */
  QString action_string_to_db();

  int m_action_order;
  int m_action_type;
  QString m_str_atype;
  QString m_action_string;
};

class filter_expr
{
public:
  filter_expr();
  virtual ~filter_expr();

  int m_expr_id;
  QString m_expr_name;
  QString m_element;		// body,header:From, header:To, etc.
  QString m_value;
  QString m_expr_text;
  char m_direction;
  int m_match_type;
  bool m_dirty;
  bool m_delete;
  bool m_new;
  std::list<filter_action> m_actions;
};

class expr_list : public std::list<filter_expr>
{
public:
  expr_list();
  virtual ~expr_list();
  bool fetch();
  bool update_db();
  // find an expr entry by its (unique) name
  filter_expr* find_name(const QString& name);
};


#endif