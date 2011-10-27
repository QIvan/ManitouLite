/* Copyright (C) 2004,2005 Daniel Vérité

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

#ifndef INC_IDENTITIES_H
#define INC_IDENTITIES_H

#include <qstring.h>
#include <map>

/*
  An identity contains the mail address and associated information we
  use as a sender of messages.
*/
class mail_identity
{
public:
  mail_identity();
  virtual ~mail_identity();
  QString m_email_addr;
  QString m_name;
  QString m_signature;
  QString m_xface;
  QString m_orig_email_addr;	// m_email_addr at load time
  int m_internal_id;
  bool update_db();
  bool m_dirty;
  bool m_is_default;
  bool m_fetched;
};

class identities : public std::map<QString,mail_identity>
{
public:
  identities(): m_fetched(false) {}
  bool fetch(bool force=false);
private:
  bool m_fetched;
};

/*
  A mailbox identifies the recipient to which email is sent from
  outside.  It has a number and a name; the number is copied into
  mail.mbox_id when importing a message.
  Mailboxes basically are to incoming mail what identities are to
  outgoing mail.  In a typical simple setup, there will be one mailbox
  per identity, but if using aliases or catch-all mechanisms, a user
  may want to do otherwise.
*/
class mailbox
{
public:
  mailbox();
  virtual ~mailbox();
  QString m_email;
  int m_id;
};

class mailboxes: public std::map<QString,mailbox>
{
public:
  mailboxes(): m_fetched(false) {}
  bool fetch(bool force=false);
  const QString& name_by_id(int id);
private:
  bool m_fetched;
};

#endif // INC_IDENTITIES_H
