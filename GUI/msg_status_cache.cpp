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

#include "msg_status_cache.h"
#include "db.h"
#include "message.h"
#include "message_port.h"
#include "main.h"
#include "app_config.h"

// static members
msg_status_map
msg_status_cache::global_status_map;

msg_status_cache* msg_status_cache::m_this;

mail_id_t
msg_status_cache::m_max_mail_id;



void
msg_status_cache::reset()
{
  global_status_map.clear();
}

uint
msg_status_cache::unread_count()
{
  uint cnt=0;
  for (msg_status_map::const_iterator iter = global_status_map.begin();
       iter != global_status_map.end();
       ++iter)
    {
      if ((iter->second & c_mask_unread) == 0)
	cnt++;
    }
  return cnt;  
}

uint
msg_status_cache::unprocessed_count()
{
  uint cnt=0;
  for (msg_status_map::const_iterator iter = global_status_map.begin();
       iter != global_status_map.end();
       ++iter)
    {
      if ((iter->second & c_mask_unprocessed) == 0)
	cnt++;
    }
  return cnt;    
}

bool
msg_status_cache::has_unread_messages()
{
  for (msg_status_map::const_iterator iter = global_status_map.begin();
       iter != global_status_map.end();
       ++iter)
    {
      if (((iter->second) & c_mask_unread) == 0)
	return true;
    }
  return false;
}

/* Set up the notification listener for new messages */
// static
void
msg_status_cache::init_db()
{
  m_this = new msg_status_cache();
  db_cnx db;
  db_listener* listener = new db_listener(db, "new_message");
  connect(listener, SIGNAL(notified()), m_this, SLOT(db_new_mail_notif()));

  /*
  message_port::connect_sender(m_this, SIGNAL(new_mail_notified(mail_id_t)),
			       SLOT(broadcast_new_mail(mail_id_t)));
  */
}

void
msg_status_cache::db_new_mail_notif()
{
  DBG_PRINTF(2, "We have NEW MAIL!");
  db_cnx db;
  try {
    sql_stream s("SELECT mail_id,status FROM mail_status WHERE mail_id>:p1", db);
    s << m_max_mail_id;
    mail_id_t mail_id;
    int status;
    int count=0;
    while (!s.isEmpty()) {
      s >> mail_id >> status;
      count++;
      DBG_PRINTF(3, "Seen mail_id %d with status %d", mail_id, status);
      update(mail_id, status);
      //      emit new_mail_notified(mail_id); // for the message port
      message_port::instance()->broadcast_new_mail(mail_id);
    }
    if (count>0 && get_config().get_bool("fetch/auto_incorporate_new_results", false)) {
      message_port::instance()->broadcast_list_refresh_request();
    }
  }
  catch(db_excpt& p) {
    DBEXCPT(p);
  }
}
