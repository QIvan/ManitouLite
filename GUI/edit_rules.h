/* Copyright (C) 2004-2007 Daniel Vérité

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

#ifndef INC_EDIT_RULES_H
#define INC_EDIT_RULES_H

#include <QWidget>
#include <QDialog>
#include <QTreeWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QFocusEvent>
#include <QLabel>
#include <QKeyEvent>
#include <QTreeWidgetItem>
#include "filter_rules.h"

class headers_groupview;
class tag_selector;
class QCheckBox;
class QLabel;
class QSpinBox;
class QKeyEvent;
class QPainter;
class button_group;

class expr_lvitem : public QTreeWidgetItem
{
public:
  expr_lvitem(QTreeWidget* parent, QString name);
  virtual ~expr_lvitem();
  filter_expr* m_expr;
  int m_id;			// unique across the listview
  bool m_db;			// fetched from database (as opposed to created in memory and not yet stored)
  bool m_dirty;
private:
  static int m_max_id;
};

class filter_edit;

class focus_line_edit: public QLineEdit
{
  Q_OBJECT
public:
  focus_line_edit(QWidget* parent, const QString& name, filter_edit* fe);
  virtual ~focus_line_edit() {}
  void focusOutEvent(QFocusEvent*);
  void setText(const QString&);
private:
  filter_edit* m_form;		// parent form (owner)
  QString m_name;
  QString m_last_value;
public slots:
  void validate();
};

class action_listview : public QTreeWidget
{
  Q_OBJECT
public:
  action_listview(QWidget* parent=0) : QTreeWidget(parent) {}
  virtual ~action_listview() {}
protected:
  void keyPressEvent(QKeyEvent*);
signals:
  void key_del();
};

class action_lvitem : public QTreeWidgetItem
{
public:
  action_lvitem(action_listview* parent, action_lvitem* after=0) :
    QTreeWidgetItem(parent, after), m_act_ptr(NULL) {}
  filter_action* m_act_ptr;
};

class action_radio_button : public QRadioButton
{
  Q_OBJECT
public:
  action_radio_button(const QString label, QWidget* parent) :
    QRadioButton(label, parent), m_checked(false) { }
  virtual ~action_radio_button() {}
  bool m_checked;
  void setChecked(bool b);
#if QT_VERSION<0x040000
protected:
  void drawButtonLabel(QPainter*);
#endif
};

/*
  Base class to implement the boxes of widgets used as input for the
  action parameters. A new action can be created by implementing a
  subclass of action_line that deals internally with its own input
  widgets but exposes the interface defined here.
*/
class action_line : public QWidget
{
  Q_OBJECT
public:
  action_line(QWidget* parent=0L) : QWidget(parent) {}
  action_line(QWidget* parent, const QString& label, const QString& type);
  virtual ~action_line();

  // check the radiobutton associated with the action
  void set_checked(bool b);

  // enable or disable the widgets for input
  virtual void enable(bool)=0;
  // get a string representation from the widgets current contents
  virtual QString get_param();
  // set the widgets contents from a string representation
  virtual void set_param(const QString&);
  // reset the widgets
  virtual void reset();

  QString getval() {
    return m_type + ": " + get_param();
  }
  action_radio_button* m_rb;
  QString m_type;
  // enum of the different kinds of actions supported by the UI
  // idx_max is used by foreign classes to iterate through them
  // (see filter_edit::w_actions)
  enum {
    idx_tag=0,
    idx_status,
    idx_prio,
    idx_redirect,
    idx_stop,
    idx_max
  };
signals:
  // emit this signal when the user has changed an action's parameter
  void new_value(QString action_type, QString val);
};

class action_stop: public action_line
{
  Q_OBJECT
public:
  action_stop(QWidget* parent=0);
  void enable(bool);
};

class action_redirect: public action_line
{
  Q_OBJECT
public:
  action_redirect(QWidget* parent=0);
  void enable(bool b) {
    m_redirect->setEnabled(b);
  }
  QString get_param() {
    return m_redirect->text();
  }
  void set_param(const QString& address) {
    m_redirect->setText(address);
  }
  void reset() {
    m_redirect->setText("");
  }
public slots:
  void report_change(const QString& newval);
private:
  QLineEdit* m_redirect;
};


class action_tag: public action_line
{
  Q_OBJECT
public:
  action_tag(QWidget* parent=0);
  tag_selector* m_qc_tag;
  void enable(bool);
  QString get_param();
  void set_param(const QString&);
  void reset();
private slots:
  void newval(int);
  void edit_tags();
private:
  QPushButton* m_edit_btn;
};

class action_status: public action_line
{
  Q_OBJECT
public:
  action_status(QWidget* parent=0);
  void enable(bool);
  QString get_param();
  void set_param(const QString&);
  void reset();
  enum { nb_status=4 };
  static const char* status_text[nb_status];
  static const char status_letter[nb_status];
  QCheckBox* m_check[nb_status];
private slots:
  void btn_clicked();
};

class action_prio: public action_line
{
  Q_OBJECT
public:
  action_prio(QWidget* parent=0);
  void enable(bool);
  void reset();
  void set_param(const QString&);
  QString get_param();
  QRadioButton* m_check_set;
  QRadioButton* m_check_add;
  QSpinBox* m_prio_set;
  QSpinBox* m_prio_add;
private slots:
  void toggle_set(bool);
  void toggle_add(bool);
  void btn_clicked();
  void value_changed(int);
};

class filter_edit : public QWidget
{
  Q_OBJECT
public:
  filter_edit(QWidget* parent=0);
  ~filter_edit();
  void expr_update();
  bool expr_fields_filled() const;
  void set_sel_list(const std::list<unsigned int>& l);
private slots:
  void direction_changed(int);
  void action_new_val(QString type, QString val);
  void ok();
  void cancel();
  void help();
  void delete_expr();
  void suggest_filter();
  void new_expr();
  void expr_selection_changed(QTreeWidgetItem*,QTreeWidgetItem*);
  void expr_from_header(QTreeWidgetItem*); // create a new expr from an entry of a headers_groupview dialog
  void close_headers_groupview();
  void action_sel_changed(QTreeWidgetItem*,QTreeWidgetItem*);
  void action_radio_toggled(bool);
  void delete_action();
private:
  // expr
  std::list<unsigned int> m_sel_list;
  QTreeWidget* lv_expr;
  focus_line_edit* ql_expr_name;
  QPushButton* expr_btn_delete;
  QPushButton* m_suggest_btn;
  button_group* m_dir;
  focus_line_edit* ql_expr_full;
  QLabel* lexpr_name;
  QLabel* ql_expr_label;
  bool load();
  void clear_expr();
  void disable_all_expr();
  void enable_expr_edit(bool);
  void dlg_fields_to_filter_expr(filter_expr *e);
  QString compute_expr_text();
  // actions
  void untie_actions();
  void display_actions();
  void create_null_action();
  void reset_actions();
  action_listview* lv_actions;
  expr_list m_expr_list;
  action_line* w_actions[action_line::idx_max];
  QLabel* m_lrules;
  action_lvitem* m_current_action;
  filter_expr* m_current_expr;
  headers_groupview* m_hd;
};

// Local Variables: ***
// mode: c++ ***
// End: ***

#endif
