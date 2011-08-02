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

#ifndef INC_NEWMAILWIDGET_H
#define INC_NEWMAILWIDGET_H

#include "db.h"
#include "identities.h"
#include "mail_displayer.h"

#include <QPlainTextEdit>
#include <QMainWindow>
#include <QDialog>
#include <QLineEdit>
#include <QStringList>
#include <QMap>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QPushButton>

#include <map>

class tags_box_widget;
class attch_listview;
class edit_address_widget;
class html_editor;

class QComboBox;
class QGridLayout;
class QMenu;
class QCloseEvent;
class QResizeEvent;
class QTreeWidgetItem;
class QTreeWidget;
class QAction;
class QActionGroup;
class QLabel;
class QToolBar;
class QStackedWidget;

class body_edit_widget : public QPlainTextEdit
{
public:
  body_edit_widget (QWidget* parent=NULL);
  virtual ~body_edit_widget() {}
};

class input_addresses_widget : public QWidget
{
  Q_OBJECT
public:
  input_addresses_widget(const QString& addresses);
  virtual ~input_addresses_widget() {}
public slots:
  void ok();
  void cancel();
  void show_recent_to();
  void show_recent_from();
  void show_prio_contacts();
  void addr_selected(QTreeWidgetItem*,int);
  void find_contacts();
signals:
  void addresses_available(QString s);
private:
  void set_header_col1(const QString& text);
  /* For query-based access to selected parts of the address book
     (addresses to which mail was recently sent, or from which mail was
     recently received, or other criteria) */
  struct accel {
/*    QPushButton* b;*/
    int rows_displayed;
  };
  QTextEdit* m_wEdit;
  QString format_multi_lines (const QString);
  void show_recent(int what);
  struct accel m_recent_to;
  struct accel m_recent_from;
  struct accel m_prioritized;
  QTreeWidget* m_addr_list;	// in which accel results are displayed
  /* type of accel info currently displayed in m_addr_list
     0: nothing, 1: recent to, 2: recent from  */
  int m_accel_type;
  QLineEdit* m_wfind;
};

#if 0
// old version
class edit_address_widget : public QLineEdit
{
public:
  edit_address_widget (QWidget* parent);
  virtual ~edit_address_widget() {}
protected:
  void keyPressEvent (QKeyEvent*);
private:
  void auto_complete();
  mail_address_list m_addr_completion_list;
  QString m_text_after_completion;
  QString m_text_before_completion;
  int m_completion_pos;
  QString m_email_to_complete;
  //  QSize sizeHint() const;
};
#else
// new version is in edit_address_widget.h

#endif

class identity_widget: public QDialog
{
  Q_OBJECT
public:
  identity_widget(QWidget* parent);
  virtual ~identity_widget();
  void set_email_address(const QString email);
  void set_name(const QString name);
  QString email_address();
  QString name();
private slots:
  void cancel();
  void ok();
private:
  QLineEdit* w_email;
  QLineEdit* w_name;
};

/* This class glues together (in a outer grid layout):
   - a combobox offering different types of header fields
   - a qlineedit with an address autocompletion feature
   - and a button that pops up a non-modal input_addresses_widget window
*/
class header_field_editor: public QObject
{
  Q_OBJECT
public:
  header_field_editor(QWidget* parent);
  ~header_field_editor();
  enum header_index {
    index_header_to=0,
    index_header_cc,
    index_header_bcc,
    index_header_replyto,
    index_header_remove,
  };
  void grid_insert(QGridLayout* layout, int row, int column);
  void set_type(header_index type);

  /* return a non-localized field name such as "To", "Cc", when the
     value is not empty and the combobox index is mapped to a real
     header field. Otherwise return an empty string. */
  QString get_field_name() const;

  /* return the current value from the lineedit */
  QString get_value() const;

  /* set the lineedit value */
  void set_value(const QString);
private:
  edit_address_widget* m_lineedit;
  QComboBox* m_cb;
  QPushButton* m_more_button;
public slots:
  void addresses_offered(QString addresses);
  void more_addresses();
  void cb_changed(int);
signals:
  void remove();
};

class html_source_editor : public QPlainTextEdit
{
  Q_OBJECT
public:
  html_source_editor(QWidget* parent=NULL);
protected:
  void resizeEvent(QResizeEvent*);
public slots:
  void position_label();
private:
  QLabel* m_sticker;
};

class new_mail_widget : public QMainWindow
{
  Q_OBJECT
public:
  new_mail_widget(mail_msg*, QWidget *parent=0);
  virtual ~new_mail_widget() {}
  void set_body_text(const QString& b) {
    m_bodyw->setPlainText(b);
  }
  void insert_signature();
  // interpolate variables inside the signature
  QString expand_signature(const QString signature, const mail_identity& identity);

  const mail_identity* get_current_identity();
  void start_edit();
  // message handling
  void make_message(const QMap<QString,QString>& user_headers);
  mail_msg& get_message() { return m_msg; }
  void show_tags();

  // returns empty or an error message produced at init time
  QString errmsg() const {
    return m_errmsg;
  }

  enum edit_mode {
    plain_mode=1,
    html_mode=2
  };
  void format_html_text();
  void format_plain_text();
public slots:
  void print();
  void send();
  void cancel();
  void keep();
  void closeEvent(QCloseEvent*);
  void remove_field_editor();
  void add_field_editor();
  void attach_files();
  void insert_file();
  void change_identity();
  //  void tag_selected(int);
  void toggle_edit_source(bool);
  void toggle_wrap(bool);
  void toggle_tags_panel(bool);
  void change_font();
  void store_settings();
  void edit_note();
  void other_identity();
  void open_global_notepad();

  void to_format_html_text();
  void to_format_plain_text();

signals:
/*  void change_status_request (uint id, uint mask_set, uint mask_unset);*/
  void refresh_request (mail_id_t m_id);
private:
  void set_wrap_mode();
  void join_address_lines (QString&);
  QString expand_aliases (const QString addresses);

  mail_msg m_msg;
  body_edit_widget* m_bodyw;
  html_editor* m_html_edit;
  html_source_editor* m_html_source_editor;
  QStackedWidget* m_edit_stack;
  edit_mode m_edit_mode;

  attch_listview* m_qAttch;
  std::map<QString,QString> m_suffix_map;
  QList<header_field_editor*> m_header_editors;
  QLineEdit* m_wSubject;
  QLabel* lSubject;
  QGridLayout* gridLayout;
  tags_box_widget* m_wtags;
//  std::map<int,input_addresses_widget*> m_map_ia_widgets;

  bool m_wrap_lines;
  bool m_close_confirm;		/* ask user confirmation on close? */
  QMenu* m_pMenuFormat;
  QMenu* m_ident_menu;
  QToolBar* m_toolbar_html1;
  QToolBar* m_toolbar_html2;
  void load_identities(QMenu*);
  QString m_from;		// email only (no name, has to match an identity)
  QString m_other_identity_email;
  QString m_other_identity_name;
  identities m_ids;
  void make_toolbars();
  void create_actions();
  QString m_note;
  QString m_errmsg;
  void display_note();

  QAction* m_action_send_msg;
  QAction* m_action_attach_file;
  QAction* m_action_insert_file;
  QAction* m_action_edit_note;
  QAction* m_action_identity_other;
  QAction* m_action_edit_other;
  QAction* m_action_add_header;
  QAction* m_action_open_notepad;
  QAction* m_action_plain_text;
  QAction* m_action_html_text;
  QActionGroup* m_identities_group;
  QMap<QAction*,mail_identity*> m_identities_actions;
public:
  static QString m_last_attch_dir;
};

#endif
