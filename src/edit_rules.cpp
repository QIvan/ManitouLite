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

#include "main.h"
#include "filter_rules.h"
#include "edit_rules.h"
#include "tags.h"
#include "icons.h"
#include "helper.h"
#include "ui_controls.h"
#include "headers_groupview.h"
#include "tagsdialog.h"

#include <QMessageBox>
#include <QFrame>
#include <QHeaderView>
#include <QPushButton>
#include <QLayout>
#include <QLineEdit>
#include <QButtonGroup>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QStringList>
#include <QEvent>
#include <QFont>
#include <QPainter>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QGridLayout>
#include <QFocusEvent>

int expr_lvitem::m_max_id=0;

//
// expr_lvitem
//
expr_lvitem::expr_lvitem(QTreeWidget* parent, QString name) :
  QTreeWidgetItem(parent, QStringList(name))
{
  m_id = ++m_max_id;
  m_dirty=false;
}

expr_lvitem::~expr_lvitem()
{
}

//
// action_line
//
action_line::action_line(QWidget* parent, const QString& label, 
			 const QString& type) : QWidget(parent)
{
  m_rb = new action_radio_button(label, this);
  m_type = type;
  setContentsMargins(0,0,0,0);
}

action_line::~action_line()
{
}

void
action_line::set_checked(bool b)
{
  m_rb->blockSignals(true);
  m_rb->setChecked(b);
  m_rb->m_checked=b;
  m_rb->blockSignals(false);
}

//
// action_tag
//
action_tag::action_tag(QWidget* parent) :
  action_line(parent, tr("Assign tag"), "tag")
{
  QHBoxLayout* layout = new QHBoxLayout(this);
  m_qc_tag = new tag_selector(this);
  layout->addWidget(m_qc_tag);
  layout->setStretchFactor(m_qc_tag, 10);
  m_qc_tag->init(false);
  connect(m_qc_tag, SIGNAL(activated(int)), this, SLOT(newval(int)));
  layout->setSpacing(3);
  m_edit_btn = new QPushButton(tr("Edit"));
  layout->addWidget(m_edit_btn);
  layout->setStretchFactor(m_edit_btn, 1);
  connect(m_edit_btn, SIGNAL(clicked()), SLOT(edit_tags()));
}

/*
  Launch the tag editor in modal mode, and when it gets closed,
  refresh the tag list to reflect the modifications
*/
void
action_tag::edit_tags()
{
  tags_dialog w(0);
  int r = w.exec();
  if (r==QDialog::Accepted) {
    m_qc_tag->init(false);
  }
}

void
action_tag::newval(int index)
{
  if (index>=0) {
    emit new_value(m_type, m_qc_tag->itemText(index));
  }
}

QString
action_tag::get_param()
{
  return m_qc_tag->currentText();
}

void
action_tag::set_param(const QString& tag)
{
  int i = m_qc_tag->findText(tag);
  if (i == -1) { // if not found, then add the item
    m_qc_tag->addItem(tag);
    i = m_qc_tag->findText(tag);
  }
  m_qc_tag->setCurrentIndex(i);
}

void
action_tag::enable(bool b)
{
  m_qc_tag->setEnabled(b);
  m_edit_btn->setEnabled(b);
}

void
action_tag::reset()
{
  m_qc_tag->setCurrentIndex(-1);
}

//
// action_status
//
const char* action_status::status_text[nb_status] = {
  QT_TR_NOOP("Mark read"),
  QT_TR_NOOP("Archive"),
  QT_TR_NOOP("Trash"),
  QT_TR_NOOP("Delete")
};

const char action_status::status_letter[nb_status] = {
  'R', 'A', 'T', 'D'
};

action_status::action_status(QWidget* parent):
  action_line(parent, tr("Set status"), "status")
{
//  setSpacing(3);
  QHBoxLayout* layout = new QHBoxLayout(this);
  for (int i=0; i<nb_status; i++) {
    m_check[i] = new QCheckBox(tr(status_text[i]), this);
    CHECK_PTR(m_check[i]);
    layout->addWidget(m_check[i]);
    connect(m_check[i], SIGNAL(clicked()), this, SLOT(btn_clicked()));
  }  
}

void
action_status::enable(bool b)
{
  for (int i=0; i<nb_status; i++) {
    m_check[i]->setEnabled(b);
    //m_check[i]->setChecked(b);
  }
}

void
action_status::btn_clicked()
{
  emit new_value(m_type, get_param());
}

QString
action_status::get_param()
{
  QString res;
  for (int i=0; i<nb_status; i++) {
    if (m_check[i]->isChecked()) {
      if (!res.isEmpty())
	res.append('+');
      res.append(status_letter[i]);
    }
  }
  return res;
}

/*
  Set the status checkboxes according to 'param', which should be
  of the form R+A+... (read+archived+...)
*/
void
action_status::set_param(const QString& param)
{
  reset();
  QStringList l=param.split('+', QString::KeepEmptyParts);
  for (QStringList::Iterator it=l.begin(); it!=l.end(); ++it) {
    QChar s=(*it).at(0);
    for (int i=0; i<nb_status; i++) {
      if (s==status_letter[i])
	m_check[i]->setChecked(true);
    }
  }
}

void
action_status::reset()
{
  for (int i=0; i<nb_status; i++) {
    m_check[i]->setChecked(false);
  }
}

//
// action_prio
//
action_prio::action_prio(QWidget* parent):
  action_line(parent, tr("Priority"), "priority")
{
//  setSpacing(3);
  QHBoxLayout* layout = new QHBoxLayout(this);
  m_check_set=new QRadioButton(tr("Set priority"), this);
  connect(m_check_set, SIGNAL(toggled(bool)), this, SLOT(toggle_set(bool)));
  connect(m_check_set, SIGNAL(clicked()), this, SLOT(btn_clicked()));
  layout->addWidget(m_check_set);

  m_prio_set = new QSpinBox(this);
  connect(m_prio_set, SIGNAL(valueChanged(int)), this, SLOT(value_changed(int)));
  m_prio_set->setMinimum(-1000);
  m_prio_set->setMaximum(1000);
  layout->addWidget(m_prio_set);

  m_check_add = new QRadioButton(tr("Add priority"), this);
  connect(m_check_add, SIGNAL(toggled(bool)), this, SLOT(toggle_add(bool)));
  connect(m_check_add, SIGNAL(clicked()), this, SLOT(btn_clicked()));
  layout->addWidget(m_check_add);

  m_prio_add = new QSpinBox(this);
  connect(m_prio_add, SIGNAL(valueChanged(int)), this, SLOT(value_changed(int)));
  m_prio_add->setMinimum(-1000);
  m_prio_add->setMaximum(1000);
  layout->addWidget(m_prio_add);
}

void
action_prio::value_changed(int v)
{
  DBG_PRINTF(6, "action_prio::value_changed(%d)\n", v);
  emit new_value(m_type, get_param());
}

void
action_prio::btn_clicked()
{
  DBG_PRINTF(6, "action_prio::btn_clicked()\n");
  emit new_value(m_type, get_param());
}

void
action_prio::toggle_set(bool b)
{
  DBG_PRINTF(6, "action_prio::toggle_set(%d)\n", (int)b);
  if (b) {
    m_prio_add->setEnabled(false);
    m_check_add->setChecked(false);
    m_prio_set->setEnabled(true);
  }
}

void
action_prio::toggle_add(bool b)
{
  DBG_PRINTF(6, "action_prio::toggle_add(%d)\n", (int)b);
  if (b) {
    m_prio_set->setEnabled(false);
    m_check_set->setChecked(false);
    m_prio_add->setEnabled(true);
  }
}

void
action_prio::enable(bool b)
{
  DBG_PRINTF(6, "action_prio::enable(%d)\n", b);
  QWidget* w[]={m_check_set, m_prio_set, m_check_add, m_prio_add};
  for (unsigned int i=0; i<sizeof(w)/sizeof(w[0]); i++)
    w[i]->setEnabled(b);
  m_prio_add->setEnabled(b & m_check_add->isChecked());
  m_prio_set->setEnabled(b & m_check_set->isChecked());
}

void
action_prio::reset()
{
  DBG_PRINTF(6, "action_prio::reset\n");
  blockSignals(true);
  m_check_set->setChecked(false);
  m_check_add->setChecked(false);
  m_prio_add->setValue(0);
  m_prio_set->setValue(0);
  m_prio_add->setEnabled(false);
  m_prio_set->setEnabled(false);
  blockSignals(false);
}

QString
action_prio::get_param()
{
  QString res;
  if (m_check_set->isChecked())
    res = "=" + m_prio_set->text();
  if (m_check_add->isChecked())
    res = "+=" + m_prio_add->text();
//  DBG_PRINTF(6, "action_prio::get_param returns '%s'\n", res.latin1());
  return res;
}

/*
  'param' should be "=value" or "+=value", value being an integer
*/
void
action_prio::set_param(const QString& param)
{
//  DBG_PRINTF(6, "action_prio::set_param('%s')\n", param.latin1());
  blockSignals(true);		// avoid signals for change of value
  if (param.at(0)=='=') {
    m_prio_set->setValue(param.mid(1).toInt());
    m_check_set->setChecked(true);
    m_check_add->setChecked(false);
  }
  else if (param.mid(0,2)=="+=") {
    m_prio_add->setValue(param.mid(2).toInt());
    m_check_add->setChecked(true);
    m_check_set->setChecked(false);
  }
  // else error
  blockSignals(false);
}


//
// filter_edit
//
filter_edit::filter_edit(QWidget* parent): QWidget(parent)
{
  m_hd=NULL;
  setWindowTitle(tr("Filter rules"));
  QVBoxLayout* top_layout = new QVBoxLayout(this);
  top_layout->setMargin(5);
  QLabel* lexpr = new QLabel(tr("Conditions:"));
  QFont fnt1 = lexpr->font();
  QFont u_fnt = fnt1;
  fnt1.setBold(true);
  u_fnt.setUnderline(true);
  lexpr->setFont(fnt1);
  top_layout->addWidget(lexpr);

  QVBoxLayout* box_expr = new QVBoxLayout();
  top_layout->addLayout(box_expr);
  //  expr_sv->setMaximumHeight(150);

//  box_expr->setFrameStyle(QFrame::Panel|QFrame::Raised);
  box_expr->setMargin(2);

  lv_expr = new QTreeWidget();
  box_expr->addWidget(lv_expr);
//  lv_expr->setMultiSelection(false);
  QStringList labels;
  labels << tr("Name") << tr("Expression");
  lv_expr->setHeaderLabels(labels);
  lv_expr->header()->resizeSection(0, 100);	// width for "Name" column
  lv_expr->header()->resizeSection(1, 400);	// width for "Expression" column

  lv_expr->setRootIsDecorated(false);
  lv_expr->setAllColumnsShowFocus(true);
  connect(lv_expr, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
	  this, SLOT(expr_selection_changed(QTreeWidgetItem*,QTreeWidgetItem*)));

  QHBoxLayout* expr_btn_box = new QHBoxLayout();
  box_expr->addLayout(expr_btn_box);

  QPushButton* expr_new = new QPushButton(tr("New"));
  expr_btn_box->addWidget(expr_new);
  expr_btn_box->setStretchFactor(expr_new, 1);
  connect(expr_new, SIGNAL(clicked()), this, SLOT(new_expr()));

  expr_btn_delete = new QPushButton(tr("Delete"));
  expr_btn_box->addWidget(expr_btn_delete);
  expr_btn_box->setStretchFactor(expr_btn_delete, 1);

  connect(expr_btn_delete, SIGNAL(clicked()), this, SLOT(delete_expr()));
  expr_btn_delete->setEnabled(false);
  m_suggest_btn = new QPushButton(tr("Suggest"));
  expr_btn_box->addWidget(m_suggest_btn);
  m_suggest_btn->setEnabled(false);
  expr_btn_box->setStretchFactor(m_suggest_btn, 1);
  connect(m_suggest_btn, SIGNAL(clicked()), this, SLOT(suggest_filter()));
  expr_btn_box->addStretch(8);

  QFrame* expr_cont = new QFrame();
  expr_cont->setFrameStyle(QFrame::Box);
  box_expr->addWidget(expr_cont);
  QGridLayout* expr_grid = new QGridLayout(expr_cont);
  //  expr_grid->setSpacing(1);  
  int row=0;
  int col=0;

  lexpr_name = new QLabel(tr("Condition's name:"), expr_cont);
//  lexpr_name->setFont(u_fnt);	// underline
  expr_grid->addWidget(lexpr_name, row, col++);
  ql_expr_label = new QLabel(tr("Expression:"), expr_cont);
  expr_grid->addWidget(ql_expr_label, row, col++);

  row++; col=0;
  ql_expr_name = new focus_line_edit(expr_cont, "expr_name", this);
  expr_grid->addWidget(ql_expr_name, row, col++);
  ql_expr_full = new focus_line_edit(expr_cont, "expr_text", this);
  expr_grid->addWidget(ql_expr_full, row, col++);

  expr_grid->setColumnStretch(0, 1); // 1/5 for name
  expr_grid->setColumnStretch(1, 4); // 4/5 for expression

  row++; col=0;
  expr_grid->addWidget(new QLabel(tr("Filter's direction:")), row, col++);
  QHBoxLayout* dirlayout = new QHBoxLayout;
  m_dir = new button_group(QBoxLayout::LeftToRight);
  m_dir->setLineWidth(0); //ContentsMargins(1,1,1,1);
  m_dir->addButton(new QRadioButton(tr("Incoming mail")), 0);
  m_dir->addButton(new QRadioButton(tr("Outgoing mail")), 1);
  m_dir->addButton(new QRadioButton(tr("Both")), 2);
  dirlayout->addWidget(m_dir, 0);
  dirlayout->addStretch(10);
  expr_grid->addLayout(dirlayout, row, col++);
  connect(m_dir->group(), SIGNAL(buttonClicked(int)), this, SLOT(direction_changed(int)));

  // -- Actions --

  m_lrules = new QLabel(tr("Actions:"));
  m_lrules->setFont(fnt1);
  top_layout->addWidget(m_lrules);

  QHBoxLayout* box_rules = new QHBoxLayout();
  top_layout->addLayout(box_rules);
  //  box_rules->setFrameStyle(QFrame::Panel|QFrame::Raised);
  box_rules->setMargin(5);

  lv_actions = new action_listview();
  box_rules->addWidget(lv_actions);

  QFrame* rules_cont = new QFrame();
  rules_cont->setFrameStyle(QFrame::Panel|QFrame::Raised);
  box_rules->addWidget(rules_cont);
  QGridLayout* rules_grid = new QGridLayout(rules_cont);
  row=0;

  /* Instantiate one box for each kind of action, and place them
     into the grid, radiobutton in row 0 and the rest in row 1 */
  for (int idx=0; idx<action_tag::idx_max; idx++) {
    if (idx==action_tag::idx_tag)
      w_actions[idx] = new action_tag();
    else if (idx==action_tag::idx_status)
      w_actions[idx] = new action_status();
    else if (idx==action_tag::idx_prio)
      w_actions[idx] = new action_prio();
    else if (idx==action_tag::idx_stop)
      w_actions[idx] = new action_stop();
    else if (idx==action_tag::idx_redirect)
      w_actions[idx] = new action_redirect();

    connect(w_actions[idx]->m_rb, SIGNAL(toggled(bool)),
	    this, SLOT(action_radio_toggled(bool)));
    connect(w_actions[idx], SIGNAL(new_value(QString,QString)),
	    this, SLOT(action_new_val(QString,QString)));
    rules_grid->addWidget(w_actions[idx]->m_rb, row, 0);
    rules_grid->addWidget(w_actions[idx], row, 1);
    rules_grid->setColumnStretch(1, 10);
    row++;
  }

  lv_actions->setHeaderLabels(QStringList(tr("Actions list")));
  lv_actions->setSortingEnabled(false);	// should be kept sorted by the order of actions
  connect(lv_actions, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
	  this, SLOT(action_sel_changed(QTreeWidgetItem*,QTreeWidgetItem*)));
  connect(lv_actions, SIGNAL(key_del()), this, SLOT(delete_action()));

  // Help OK Cancel
  QHBoxLayout* hbox_valid = new QHBoxLayout();
  hbox_valid->setMargin(5);
  hbox_valid->setSpacing(5);
  top_layout->addLayout(hbox_valid);

  hbox_valid->addStretch(1);	// space at left
  QPushButton* whelp = new QPushButton(tr("Help"));
  hbox_valid->addWidget(whelp, 0);
  QPushButton* wok = new QPushButton(tr("OK"));
  hbox_valid->addWidget(wok, 0);
  wok->setDefault(true);
  QPushButton* wcancel = new QPushButton(tr("Cancel"));
  hbox_valid->addWidget(wcancel);

  connect(wok, SIGNAL(clicked()), this, SLOT(ok()));
  connect(wcancel, SIGNAL(clicked()), this, SLOT(cancel()));
  connect(whelp, SIGNAL(clicked()), this, SLOT(help()));
  clear_expr();
  disable_all_expr();
  untie_actions();
  load();
}

filter_edit::~filter_edit()
{
  if (m_hd)
    delete m_hd;
}

void
filter_edit::direction_changed(int id)
{
  Q_UNUSED(id);
  DBG_PRINTF(4, "direction_changed\n");
  if (m_current_expr) {
    m_current_expr->m_dirty=true;
    dlg_fields_to_filter_expr(m_current_expr);
  }
}

void
filter_edit::set_sel_list(const std::list<unsigned int>& l)
{
  m_sel_list = l;
  m_suggest_btn->setEnabled(true);
}

void
filter_edit::action_new_val(QString type, QString val)
{
  if (val.isNull())
    val=QString("");
//  DBG_PRINTF(6, "action_new_val type=%s, val=%s\n", type.latin1(), val.latin1());
//  QListViewItem* item=lv_actions->selectedItem();
  action_lvitem* p = m_current_action;
  if (!p) {
    DBG_PRINTF(2, "ERR: no current_action\n");
    return;
  }
  if (!p->m_act_ptr) {
    filter_action a;
    a.m_str_atype=val;
    m_current_expr->m_actions.push_back(a);
    p->m_act_ptr=&(m_current_expr->m_actions.back());
    // create another null action for future use
    create_null_action();
    m_current_action=p;
  }
  p->m_act_ptr->m_action_string = val;
  p->m_act_ptr->m_str_atype = type;
  p->setText(0, type + ": " + val);
  m_current_expr->m_dirty=true;
}

/*
  Emulate the behavior of a button group containing radiobuttons,
  except that the currently selected button can be deselected.
  (we don't use a QButtonGroup because its layout (a QVBox) doesn't
  fit into our grid)
*/
void
filter_edit::action_radio_toggled(bool b)
{
  const QObject* o = sender();
  DBG_PRINTF(6, "action_radio_toggled(b=%d,sender=%p)\n", b, o);
  for (int idx=0; idx<action_tag::idx_max; idx++) {
    if (o==w_actions[idx]->m_rb) {
      w_actions[idx]->enable(b);
      if (b) {
	action_new_val(w_actions[idx]->m_type, w_actions[idx]->get_param());
      }
    }
    else {
      w_actions[idx]->enable(false);
      w_actions[idx]->set_checked(false);
    }
  }
}

void
filter_edit::action_sel_changed(QTreeWidgetItem* item, QTreeWidgetItem* previous)
{
  Q_UNUSED(previous);
  DBG_PRINTF(6, "action_sel_changed(item=%p)\n", item);
  if (!item) return;
  action_lvitem* a = static_cast<action_lvitem*>(item);
  m_current_action=a;
  for (int i=0; i<action_line::idx_max; i++) {
    if (a->m_act_ptr && w_actions[i]->m_type == a->m_act_ptr->m_str_atype) {
      w_actions[i]->set_param(a->m_act_ptr->m_action_string);
      w_actions[i]->enable(true);
      w_actions[i]->set_checked(true);
      //      break;	// action types are unique, no need to continue
    }
    else {
      w_actions[i]->reset();
      w_actions[i]->set_checked(false);
    }
  }
}

/*
  Disable entirely the actions panel.
  To be called when no expression is selected.
*/
void
filter_edit::untie_actions()
{
  m_lrules->setEnabled(false);
  m_lrules->setText(tr("Actions:"));

  // uncheck all and disable each action widget
  reset_actions();

  // disable the actions list
  lv_actions->setEnabled(false);

  // also disable the radio buttons
  for (int i=0; i<action_line::idx_max; i++) {
    w_actions[i]->m_rb->setEnabled(false);
  }
}

void
filter_edit::ok()
{
  ql_expr_full->validate();
  ql_expr_name->validate();
  //  accept();
  if (m_expr_list.update_db())
    close();
}

void
filter_edit::cancel()
{
  //  accept();
  close();
}

bool
filter_edit::load()
{
  expr_list* l = &m_expr_list;
  if (!l->fetch()) return false;
  std::list<filter_expr>::iterator it = l->begin();
  for (; it != l->end(); ++it) {
    expr_lvitem* item = new expr_lvitem(lv_expr, (*it).m_expr_name);
    item->setText(1, (*it).m_expr_text);
    item->m_expr = &(*it);
    item->m_db=true;
  }
  return true;
}

void
filter_edit::clear_expr()
{
  ql_expr_name->setText(QString::null);
  ql_expr_full->setText(QString::null);
  m_current_expr=NULL;
  lv_expr->clearSelection();
  untie_actions();
}

void
filter_edit::suggest_filter()
{
  if (!m_hd) {
    m_hd = new headers_groupview();
    m_hd->set_threshold(100);
    m_hd->init(m_sel_list);
    connect(m_hd->m_trview, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
	    this, SLOT(expr_from_header(QTreeWidgetItem*)));
    connect(m_hd, SIGNAL(close()), this, SLOT(close_headers_groupview()));
  }
  m_hd->show();
}

void
filter_edit::expr_from_header(QTreeWidgetItem* item)
{
  if (!item) return;
  DBG_PRINTF(3, "expr_from_header (item=%p)\n", item);
  const QString h = item->text(1);
  int sep_pos = h.indexOf(":");
  if (sep_pos>0) {
    QString header_name = h.left(sep_pos);
    QString header_value = h.mid(sep_pos+1);
    header_value = header_value.trimmed();
    new_expr();
    QString expr = "header(\""+ header_name + "\") eq \"" + header_value + "\"";
    ql_expr_full->setText(expr);
  }
}

void
filter_edit::close_headers_groupview()
{
}


void
filter_edit::delete_expr()
{
  if (!m_current_expr) {
    DBG_PRINTF(6, "no current expr\n");
    return;
  }
  int r=QMessageBox::warning(this, tr("Please confirm"), tr("Delete condition?"), tr("OK"), tr("Cancel"), QString::null);
  if (r==0) {
    expr_lvitem* item=static_cast<expr_lvitem*>(lv_expr->currentItem());
    if (!item) {
      DBG_PRINTF(1, "Couldn't find selected item\n");
      return;
    }
    item->m_expr->m_delete=true;
    delete item;
    clear_expr();
  }
}

void
filter_edit::expr_selection_changed(QTreeWidgetItem* i, QTreeWidgetItem* prev)
{
  Q_UNUSED(prev);
  if (!i) {
    DBG_PRINTF(6,"expr deselected\n");
    clear_expr();
    return;
  }
  expr_lvitem* item = static_cast<expr_lvitem*>(i);
  filter_expr* e = item->m_expr;
  m_current_expr = e;
  ql_expr_name->setText(e->m_expr_name);
  ql_expr_full->setText(e->m_expr_text);
  expr_btn_delete->setEnabled(item->m_id!=0);
  DBG_PRINTF(6, "expr sel=%d\n", item->m_id);
  enable_expr_edit(true);
  //grp_edit_method->setEnabled(false);
  switch(e->m_direction) {
  case 'I':
    m_dir->setButton(0);
    break;
  case 'O':
    m_dir->setButton(1);
    break;
  case 'B':
    m_dir->setButton(2);
    break;
  }
  reset_actions();
  display_actions();
}

/*
  update the 'Actions' panel with the actions connected to the
  currently selected expression
*/
void
filter_edit::display_actions()
{
  m_lrules->setEnabled(true);
  lv_actions->clear();
  lv_actions->setEnabled(true);
  m_current_action=NULL;
  filter_expr* e = m_current_expr;
  if (!e) {
    DBG_PRINTF(1, "ERR: no current expression\n");
    return;
  }

  if (e->m_expr_name.isEmpty())
    m_lrules->setText(tr("Actions connected to current condition"));
  else
    m_lrules->setText(tr("Actions connected to condition '%1':").arg(e->m_expr_name));

  std::list<filter_action>::iterator iter;
  action_lvitem* after=NULL;
  for (iter=e->m_actions.begin(); iter!=e->m_actions.end(); ++iter) {
    after = new action_lvitem(lv_actions, after);
    CHECK_PTR(after);
//    DBG_PRINTF(6, "add action %s:%s\n", iter->m_str_atype.latin1(),
//	       iter->m_action_string.latin1());
    after->m_act_ptr = &(*iter);
    after->setText(0, iter->m_str_atype + ": " + iter->m_action_string);
  }
  create_null_action();
  if (lv_actions->topLevelItem(0)) {
    // pre-select the first element
    DBG_PRINTF(6, "setSelected(0x%p)\n", lv_actions->topLevelItem(0));
    lv_actions->setCurrentItem(lv_actions->topLevelItem(0));
    m_current_action=static_cast<action_lvitem*>(lv_actions->topLevelItem(0));
  }
  for (int i=0; i<action_line::idx_max; i++) {
    w_actions[i]->m_rb->setEnabled(true);
  }
}

// Add the (New action) entry
void
filter_edit::create_null_action()
{
  int count=lv_actions->topLevelItemCount();
  action_lvitem* last=NULL;
  if (count>0)
    last = static_cast<action_lvitem*>(lv_actions->topLevelItem(count-1));
  action_lvitem* item = new action_lvitem(lv_actions, last);
  CHECK_PTR(item);
  item->setText(0, tr("(New action)"));
  item->m_act_ptr = NULL;	// no real associated action
}

/*
  Reset the 'Actions' panel
*/
void
filter_edit::reset_actions()
{
  for (int i=0; i<action_line::idx_max; i++) {
    w_actions[i]->enable(false);
    w_actions[i]->set_checked(false);
  }

  lv_actions->clear();
}


void
filter_edit::enable_expr_edit(bool enable)
{
  ql_expr_full->setEnabled(enable);
  ql_expr_label->setEnabled(enable);
  ql_expr_name->setEnabled(enable);
  lexpr_name->setEnabled(enable);
}

void
filter_edit::new_expr()
{
  clear_expr();
  enable_expr_edit(true);
  ql_expr_name->setFocus();
}

bool
filter_edit::expr_fields_filled() const
{
  return !ql_expr_full->text().isEmpty();
}

QString
filter_edit::compute_expr_text()
{
  return ql_expr_full->text();
}

void
filter_edit::dlg_fields_to_filter_expr(filter_expr *e)
{
  e->m_element.truncate(0);
  e->m_value.truncate(0);
  e->m_match_type=0;
  e->m_expr_text = ql_expr_full->text();
  static char dir[]={'I','O','B'};
  e->m_direction = dir[m_dir->selected_id()];
}

void
filter_edit::expr_update()
{
  bool select_item=false;
  // change the text in the expressions listview

  QList<QTreeWidgetItem*> list_selected = lv_expr->selectedItems();
  expr_lvitem* item = list_selected.count()==1 ? 
    static_cast<expr_lvitem*>(list_selected.first()) : NULL;

  if (!item) {
    item = new expr_lvitem(lv_expr, ql_expr_name->text());
    CHECK_PTR(item);
    select_item=true;
    filter_expr new_expr;
    m_expr_list.push_back(new_expr);
    item->m_expr = &(m_expr_list.back());
    DBG_PRINTF(6, "creating new expr\n");
    item->m_expr->m_new=true;
    item->m_expr->m_expr_id=0;
    m_current_expr = item->m_expr;
  }
  QString expr_text = compute_expr_text();
  item->setText(0, ql_expr_name->text());
  item->setText(1, expr_text);
  ql_expr_full->setText(expr_text);

  filter_expr* e = item->m_expr; // should be m_current_expr
  e->m_expr_name = ql_expr_name->text();
  e->m_expr_text = expr_text;
  dlg_fields_to_filter_expr(e);
  e->m_dirty=true;
  item->m_dirty=true;
  DBG_PRINTF(6, "expr_id %d is marked dirty\n", e->m_expr_id);
  if (select_item)
    lv_expr->setCurrentItem(item);
}

void
filter_edit::disable_all_expr()
{
  enable_expr_edit(false);
//  grp_edit_method->setEnabled(false);
  ql_expr_name->setEnabled(false);
  lexpr_name->setEnabled(false);
  expr_btn_delete->setEnabled(false);
}

/*
  Delete the current action
*/
void
filter_edit::delete_action()
{
  if (!m_current_action || 
      m_current_action!=lv_actions->currentItem() ||
      !m_current_action->m_act_ptr)
    {
      return;
    }
  filter_expr* e=m_current_expr;
  std::list<filter_action>::iterator iter;
  for (iter=e->m_actions.begin(); iter!=e->m_actions.end(); ++iter) {
    if (m_current_action->m_act_ptr==&(*iter)) {
      DBG_PRINTF(6, "found\n");
      e->m_actions.erase(iter);
      e->m_dirty=true;
      break;
    }
  }
  delete m_current_action;
  m_current_action=NULL;
#if 0   // huh?
  if (!lv_actions->currentItem()) {
    if (lv_actions->currentItem()) { // mutually exclusive with the IF above
      lv_actions->setSelected(lv_actions->currentItem(), true);
    }
  }
#endif
}

void
filter_edit::help()
{
#if 0
  filter_expr* e=m_current_expr;
  std::list<filter_action>::iterator iter;
  DBG_PRINTF(6, "current actions (m_current_action=0x%p\n", m_current_action);
  for (iter=e->m_actions.begin(); iter!=e->m_actions.end(); ++iter) {
    DBG_PRINTF(6,"\tm_str_atype=%s, m_action_string=%s\n",
	       iter->m_str_atype.latin1(), iter->m_action_string.latin1());
  }
#endif
  helper::show_help("filters");
}

//
// focus_line_edit
//
focus_line_edit::focus_line_edit(QWidget* parent, const QString& name, 
				 filter_edit* form) :
  QLineEdit(parent)
{
  m_form=form;
  m_name=name;
  connect(this, SIGNAL(returnPressed()), this, SLOT(validate()));
}

void
focus_line_edit::validate()
{
  if (text()!=m_last_value && m_form->expr_fields_filled()) {
    m_form->expr_update();
    m_last_value=text();
  }
}

void
focus_line_edit::setText(const QString& t)
{
  m_last_value=t;
  QLineEdit::setText(t);
}

void
focus_line_edit::focusOutEvent(QFocusEvent * e)
{
  DBG_PRINTF(6, "focusOutEvent()\n");
  validate();
  QLineEdit::focusOutEvent(e);
}

void
action_listview::keyPressEvent(QKeyEvent* e)
{
  if (e->modifiers()!=0 || e->key()!=Qt::Key_Delete) {
    QTreeWidget::keyPressEvent(e);
  }
  else
    emit key_del();
}

//
// action_radio_button
//
#if QT_VERSION<0x040000
void
action_radio_button::drawButtonLabel(QPainter* p)
{
  if (isChecked()) {
    QFont f=p->font();
    f.setBold(true);
    p->setFont(f);
  }
  QRadioButton::drawButtonLabel(p);
}
#endif

void
action_radio_button::setChecked(bool b)
{
  QRadioButton::setChecked(b);
  repaint();
}

action_stop::action_stop(QWidget* parent) :
  action_line(parent, tr("Stop filters"), "stop")
{
}

void
action_stop::enable(bool b)
{
  Q_UNUSED(b);
}

QString
action_line::get_param()
{
  return QString();
}

void
action_line::reset()
{
}

void
action_line::set_param(const QString& param)
{
  Q_UNUSED(param);		// base action has no parameter
}

/* action_redirect */

action_redirect::action_redirect(QWidget* parent) :
  action_line(parent, tr("Redirect to"), "redirect")
{
  m_redirect = new QLineEdit();
  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->addWidget(m_redirect);
  //  m_redirect->setMinimumWidth(300);
  connect(m_redirect, SIGNAL(textEdited(const QString&)), this, SLOT(report_change(const QString&)));
}

void
action_redirect::report_change(const QString& newval)
{
  emit new_value(m_type, newval);
}
