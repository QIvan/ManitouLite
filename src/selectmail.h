/* Copyright (C) 2004-2009 Daniel Verite

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

#ifndef INC_SELECTMAIL_H
#define INC_SELECTMAIL_H

#include <QComboBox>
#include <QDialog>
#include <QKeyEvent>
#include <QThread>
#include <QStringList>
#include <QDateTime>
#include <QTime>

#include <vector>
#include "db.h"
#include "sqlquery.h"
#include "mail_listview.h"

class QLineEdit;
class QSpinBox;
class QButtonGroup;
class QTimer;
class QDateTimeEdit;
class QCheckBox;
class QToolButton;
class tag_selector;
class QButtonGroup;

class mail_result
{
public:
  mail_id_t m_id;
  QString m_from;
  QString m_subject;
  QString m_date;
  int m_thread_id;
  int m_status;
  int m_in_replyto;
  int m_pri;
  QString m_sender_name;
  uint m_flags;
};

class fetch_thread: public QThread
{
public:
  fetch_thread();
  virtual void run();
  void release();
  void cancel();
  db_cnx* m_cnx;
  std::list<mail_result>* m_results;
  QString m_query;
  QString m_errstr;
  int m_exec_time;   // query exec time in milliseconds
};

class msgs_filter
{
public:
  msgs_filter();
  void init();
  virtual ~msgs_filter();
  const QString user_query() const {
    return m_user_query;
  }
  void set_user_query(const QString q) {
    m_user_query=q;
  }
  int exec_time() const;	// in milliseconds
  typedef std::list<mail_msg*> mlist_t;
  mlist_t m_list_msgs;
  // fetch the selection into a mail list widget
  int fetch(mail_listview*, bool fetch_more=false);
  int asynchronous_fetch (fetch_thread* t);
  void make_list(mail_listview*);
  void set_auto_refresh(bool s=true) {
    m_auto_refresh=s;
  }
  bool auto_refresh() const {
    return m_auto_refresh;
  }
  int parse_search_string(QString s, QStringList& words, QStringList& substrs);

  //private:

  enum recipient_type {
    rFrom=0,
    rTo,
    rCc,
    rAny
  };
  // search criteria
  uint m_mailId;
  int m_nAddrType;
  QString m_sAddress;
  QString m_subject;
  QString m_body_substring;
  QString m_addr_to;
  QString m_sql_stmt;
  QString m_tag_name;
  QDate m_date_min;
  QDate m_date_max;
  //  QString m_word;
  QStringList m_words;		// full-text search: words to find
  QStringList m_substrs;	// full-text search: substrings to find
  uint m_thread_id;

  int m_status;			// exact value wanted in mail.status
  int m_status_set;		// bitmask: OR'ed bits that have to be 1 in mail.status
  int m_status_unset;		// bitmask: OR'ed bits that have to be 0 in mail.status

  int m_newer_than;		/* newer than or N days old */
  // detailed user input
  struct {
    int days;
    int weeks;
    int months;
  } m_newer_details;

  int m_min_prio;
  uint m_tag_id;

  bool m_in_trash;

  void set_date_order(int o) {
    m_order=o;
  }
  uint m_max_results;		// max results per fetch iteration
  bool m_fetched;

  std::list<mail_result>* m_fetch_results;
  int build_query (sql_query&, bool fetch_more=false);
  //  mail_msg* in_list(mail_id_t id);
  static void load_result_list(sql_stream& query_result, std::list<mail_result>* l);

  QTime m_start_time;
  int m_exec_time;
  QString m_errmsg;

private:
  bool m_auto_refresh;
  int add_address_selection (sql_query& q, const QString email_addr, int addr_type);
  /* number of criteria that needs to match an address from
     the mail_addresses table. For each one of these we have to use
     a different sql alias for the mail_addresses table */
  unsigned int m_addresses_count;
  static const int max_possible_prio;

  /* escape % and _ for LIKE clauses and add % at the start and end */
  QString quote_like_arg(const QString&);

  /* the part of the query that's made visible to the user: it
     includes a select-list that retrieves only the mail_id column,
     and also  shouldn't have ORDER BY or LIMIT clauses. Typically,
     it's aimed at being included in a subquery, i.e:

     SELECT <columns> FROM mail WHERE mail_id IN (m_user_query) ORDER BY
     ... LIMIT ... */
  QString m_user_query;

  /* used to fetch another set of results that are older/newer (depending on m_order) */
  QString m_date_bound;
  int m_mail_id_bound;

  /* ordering of msg_date (+1=ASC, -1=DESC) column for the fetch */
  int m_order;
};

// Array of checkboxes for viewing, editing or selecting
// a combination of all possible status
class select_status_box : public QFrame
{
  Q_OBJECT
public:
  select_status_box (bool either, QWidget* parent=0);
  virtual ~select_status_box() {}
  int mask_yes() const;
  int mask_no() const;
  void set_mask(int maskYes, int maskNo);
  int status() const;
public slots:
  void status_changed(int);
private:
  struct st_status {
    const char* name;
    int value;
  };
  static st_status m_status_tab[];
  int m_mask_set;		/* the bits we want to be 1 */
  int m_mask_unset;		/* the bits we want to be 0 */

  // true: the buttons are "yes, no, either"
  // false: the buttons are "yes", no"
  bool m_either;

  std::vector<QButtonGroup*> m_button_groups;
};

// a modal dialog that embeds a select_status_box with OK and Cancel
// buttons
class status_dialog : public QDialog
{
  Q_OBJECT
public:
  status_dialog(QWidget* parent=0);
  select_status_box* m_statusBox;
};

// selection of email addresses with autocompletion
class combobox_addresses : public QComboBox
{
public:
  combobox_addresses(QWidget* parent=0) : QComboBox(parent) {
    setEditable(true);
    m_address_type=0;
  }
  void set_address_type (int a) { m_address_type=a; }
  virtual ~combobox_addresses() {}
protected:
  void keyPressEvent(QKeyEvent*);
private:
  int m_address_type;
};

class msg_select_dialog : public QDialog
{
  Q_OBJECT
public:
  msg_select_dialog(bool open_new=true);
  virtual ~msg_select_dialog() {}

  /* fill in the dialog controls with the values provided by the message
     filter */
  void filter_to_dialog (const msgs_filter* cFilter);

public slots:
  void ok();
  void cancel();
  void help();
private slots:
  void more_status();
  void zoom_on_sql();
  void timer_done();
  void addr_type_changed(int);
private:
  QString str_status_mask();
  combobox_addresses* m_wAddress;
  QSpinBox* m_wdate_spin;
  QButtonGroup* m_wdate_button_group;
  QComboBox* m_wAddrType;
//  QComboBox* m_qtag_sel;
  tag_selector* m_qtag_sel;

  combobox_addresses* m_wTo;
  QLineEdit* m_wSubject;
  QLineEdit* m_wString;
  QLineEdit* m_wSqlStmt;
  QDateTimeEdit* m_wmin_date;
  QDateTimeEdit* m_wmax_date;
  QCheckBox* m_chk_datemin;
  QCheckBox* m_chk_datemax;
  QLineEdit* m_wStatus;
  QLineEdit* m_wMaxResults;
  QPushButton* m_wCancelButton;
  QPushButton* m_wOkButton;
  QPushButton* m_wStatusMoreButton;
  QToolButton* m_zoom_button;
  QCheckBox* m_trash_checkbox;
  int m_status_set_mask;
  int m_status_unset_mask;
  void to_filter(msgs_filter*);
  msgs_filter m_filter;
  void enable_inputs (bool enable);

  fetch_thread m_thread;
  QTimer* m_timer;
  bool m_waiting_for_results;
  bool m_new_selection;
signals:
  void fetch_done(msgs_filter*);
};



#endif // INC_SELECTMAIL_H
