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

#include "newmailwidget.h"
#include "app_config.h"
#include "edit_address_widget.h"
#include "notepad.h"
#include "attachment_listview.h"
#include "main.h"
#include "dragdrop.h"
#include "icons.h"
#include "users.h"
#include "notewidget.h"
#include "tagsbox.h"
#include "sqlstream.h"
#include "html_editor.h"
#include <set>

#include <QCloseEvent>
#include <QComboBox>
#include <QCursor>
#include <QTimer>
#include <QDropEvent>
#include <QFileDialog>
#include <QFontDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QToolButton>
#include <QSplitter>
#include <QTextDocument>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QRegExp>
#include <QDebug>

/***********************/
/* header_field_editor */
/***********************/
header_field_editor::header_field_editor(QWidget* parent)
{
  static const char* defaults[] = {
    // the contents must be aligned with enum header_index
    QT_TR_NOOP("To"),
    QT_TR_NOOP("Cc"),
    QT_TR_NOOP("Bcc"),
    QT_TR_NOOP("ReplyTo"),
    QT_TR_NOOP("Remove...")
  };

  m_cb = new QComboBox(parent);
  for (uint i=0; i<sizeof(defaults)/sizeof(defaults[0]); i++) {
    m_cb->addItem(tr(defaults[i]));
  }
  connect(m_cb, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_changed(int)));
  m_lineedit = new edit_address_widget(parent);
  m_more_button = new QPushButton(FT_MAKE_ICON(FT_ICON16_ADDRESSBOOK), "", parent);
  connect(m_more_button, SIGNAL(clicked()), this, SLOT(more_addresses()));
}

header_field_editor::~header_field_editor()
{
  /* The widgets have to be deleted explictly since our parent won't
     do it when this happens by the 'remove' combobox action */
  delete m_lineedit;
  delete m_more_button;
  delete m_cb;
}

/*
  Slot. Called on combobox changes */
void
header_field_editor::cb_changed(int index)
{
  if (index == index_header_remove) {
    emit remove();    
  }
}

void
header_field_editor::set_type(header_index type)
{
  m_cb->blockSignals(true);
  m_cb->setCurrentIndex((int)type);
  m_cb->blockSignals(false);
}

QString
header_field_editor::get_value() const
{
  return m_lineedit->text();
}

QString
header_field_editor::get_field_name() const
{
  if (!m_lineedit->text().isEmpty()) {
    switch(m_cb->currentIndex()) {
    case index_header_to:
      return "To";
    case index_header_cc:
      return "Cc";
    case index_header_bcc:
      return "Bcc";
    case index_header_replyto:
      return "ReplyTo";
    }
  }
  return QString::null;
}

void
header_field_editor::set_value(const QString val)
{
  m_lineedit->setText(val);
}

void
header_field_editor::grid_insert(QGridLayout* layout, int row, int column)
{
  layout->addWidget(m_cb, row, column++);
  layout->addWidget(m_lineedit, row, column++);
  layout->addWidget(m_more_button, row, column++);
}

void
header_field_editor::more_addresses()
{
  input_addresses_widget* w = new input_addresses_widget(get_value());
  connect(w, SIGNAL(addresses_available(QString)),
	   this, SLOT(addresses_offered(QString)));
  // try to open the window at the current mouse position
  w->move(QCursor::pos());
  w->show();  
}

/* Slot. Typically called when the user has OK'd the
   input_addresses_widget window. At this point we receive the new
   addresses, possibly reformat them and put them into our lineedit
   control */
void
header_field_editor::addresses_offered(QString addresses)
{
  mail_address::join_address_lines(addresses);
  m_lineedit->setText(addresses);
}

/***********************/
/* new_mail_widget     */
/***********************/

QString
new_mail_widget::m_last_attch_dir;

void
new_mail_widget::create_actions()
{
  m_action_send_msg = new QAction(FT_MAKE_ICON(FT_ICON16_SEND),
				  tr("Send mail"), this);
  connect(m_action_send_msg, SIGNAL(activated()), this, SLOT(send()));

  m_action_attach_file = new QAction(FT_MAKE_ICON(FT_ICON16_ATTACH),
				     tr("Attach file"), this);
  connect(m_action_attach_file, SIGNAL(activated()), this, SLOT(attach_files()));

  m_action_insert_file = new QAction(FT_MAKE_ICON(FT_ICON16_OPENFILE),
				     tr("Insert text file"), this);
  connect(m_action_insert_file, SIGNAL(activated()), this, SLOT(insert_file()));

  m_action_edit_note = new QAction(FT_MAKE_ICON(FT_ICON16_EDIT_NOTE),
				   tr("Edit note"), this);
  connect(m_action_edit_note, SIGNAL(activated()), this, SLOT(edit_note()));

  m_action_add_header = new QAction(FT_MAKE_ICON(ICON16_HEADER),
				   tr("Add header field"), this);
  connect(m_action_add_header, SIGNAL(activated()), this, SLOT(add_field_editor()));

  m_action_open_notepad = new QAction(FT_MAKE_ICON(ICON16_NOTEPAD),
				   tr("Open global notepad"), this);
  connect(m_action_open_notepad, SIGNAL(activated()), this, SLOT(open_global_notepad()));

}

void
new_mail_widget::make_toolbars()
{
  QToolBar* toolbar = addToolBar(tr("Message Operations"));
  toolbar->addAction(m_action_send_msg);
  toolbar->addAction(m_action_attach_file);
  toolbar->addAction(m_action_edit_note);
  toolbar->addAction(m_action_add_header);
  toolbar->addAction(m_action_open_notepad);

  m_toolbar_html1 = m_toolbar_html2 = NULL;

  if (m_html_edit) {
    QList<QToolBar*> toolbars = m_html_edit->create_toolbars();
    for (int i=0; i<toolbars.size(); i++) {
      if (i==0) m_toolbar_html1 = toolbars[i];
      if (i==1) {
	addToolBarBreak();
	m_toolbar_html2 = toolbars[i];
      }
      addToolBar(toolbars[i]);
    }
    // Add our own toggle button to the second HTML toolbar
    if (m_toolbar_html2 != NULL) {
      QToolButton* source = new QToolButton();
      source->setIcon(HTML_ICON("stock_view-html-source.png"));
      source->setToolTip(tr("Toggle between HTML source edit and visual edit"));
      source->setCheckable(true);
      connect(source, SIGNAL(toggled(bool)), this, SLOT(toggle_edit_source(bool)));
      m_toolbar_html2->addWidget(source);
    }
  }
}

void
new_mail_widget::toggle_edit_source(bool checked)
{
  if (!m_html_edit) {
    DBG_PRINTF(1, "ERR: no html editor is instantiated");
    return;
  }
  if (checked) {
    m_html_source_editor = new html_source_editor();
    m_html_source_editor->setPlainText(m_html_edit->html_text());
    m_edit_stack->addWidget(m_html_source_editor);
    m_edit_stack->setCurrentWidget(m_html_source_editor);
    m_html_edit->enable_html_controls(false);
    // disable some actions while we're editing html source
    m_action_send_msg->setEnabled(false);
    m_action_insert_file->setEnabled(false);
  }
  else {
    m_html_edit->set_html_text(m_html_source_editor->toPlainText());
    m_html_edit->enable_html_controls(true);
    delete m_html_source_editor;
    m_html_source_editor = NULL;
    m_action_send_msg->setEnabled(true);
    m_action_insert_file->setEnabled(true);
    m_edit_stack->setCurrentWidget(m_html_edit);
  }
}

new_mail_widget::new_mail_widget(mail_msg* msg, QWidget* parent)
  : QMainWindow(parent)
{
  lSubject = NULL;
  m_wSubject = NULL;

  setWindowTitle(tr("New message"));
  m_wrap_lines = true;
  m_close_confirm = true;
  m_msg = *msg;

  QWidget* main_hb = new QWidget(this);	// container
  setCentralWidget(main_hb);

  QHBoxLayout *topLayout = new QHBoxLayout(main_hb);
  topLayout->setSpacing(4);
  topLayout->setMargin(4);

  m_wtags=new tags_box_widget(main_hb);

  topLayout->addWidget(m_wtags);
  topLayout->setStretchFactor(m_wtags, 0);

  QSplitter* split=new QSplitter(Qt::Vertical, main_hb);

  m_edit_stack = new QStackedWidget(split);
  m_bodyw = new body_edit_widget();
  m_html_edit = new html_editor();
  m_html_edit->set_html_text("<html><body></body></html>");
  m_edit_stack->addWidget(m_bodyw);
  m_edit_stack->addWidget(m_html_edit);
  m_edit_mode = html_mode;
  m_edit_stack->setCurrentWidget(m_html_edit);

  QVBoxLayout *hLayout = new QVBoxLayout();
  topLayout->addLayout(hLayout);
  topLayout->setStretchFactor(hLayout, 1);

  gridLayout = new QGridLayout();
  hLayout->addLayout(gridLayout);
  hLayout->addWidget(split);

  //  QWidget* wcont=main_hb;
  int nRow=0;

  header_field_editor* ed = new header_field_editor(this);
  ed->set_type(header_field_editor::index_header_to);
  ed->grid_insert(gridLayout, nRow, 0);
  ed->set_value(m_msg.header().m_to);
  connect(ed, SIGNAL(remove()), this, SLOT(remove_field_editor()));
  //  connect(ed, SIGNAL(add()), this, SLOT(add_field_editor()));
  m_header_editors.append(ed);
  nRow++;

  QString initial_cc = m_msg.header().m_cc;
  if (!initial_cc.isEmpty()) {
    header_field_editor* ed1 = new header_field_editor(this);
    ed1->set_type(header_field_editor::index_header_cc);
    ed1->grid_insert(gridLayout, nRow++, 0);
    ed1->set_value(initial_cc);
    connect(ed1, SIGNAL(remove()), this, SLOT(remove_field_editor()));
    //    connect(ed1, SIGNAL(add()), this, SLOT(add_field_editor()));
    m_header_editors.append(ed1);
  }

  lSubject=new QLabel(tr("Subject:"), this);
  gridLayout->addWidget(lSubject, nRow, 0);
  m_wSubject=new QLineEdit(this);
  m_wSubject->setText(m_msg.header().m_subject);
  gridLayout->addWidget(m_wSubject, nRow, 1);
  nRow++;

  m_qAttch=new attch_listview(split);
  m_qAttch->allow_delete(true);
  m_qAttch->hide();
  m_qAttch->set_attch_list(&m_msg.attachments());

  resize(900,600);

  QList<int> lSizes;
  lSizes.append(400);
  lSizes.append(20);
  split->setSizes(lSizes);
  set_wrap_mode(); 

  create_actions();

  QMenu* pMsg=new QMenu(tr("&Message"), this);
  CHECK_PTR(pMsg);

  QIcon ico_print(FT_MAKE_ICON(FT_ICON16_PRINT));
  QIcon ico_note(FT_MAKE_ICON(FT_ICON16_EDIT_NOTE));
  QIcon ico_close(FT_MAKE_ICON(FT_ICON16_CLOSE_WINDOW));

  pMsg->addAction(m_action_send_msg);
  pMsg->addAction(m_action_attach_file);
  pMsg->addAction(m_action_insert_file);
  //  pMsg->addAction(tr("Keep for later"), this, SLOT(keep()));
  pMsg->addAction(ico_note, tr("Edit private note"), this, SLOT(edit_note()));
  pMsg->addAction(ico_print, tr("Print"), this, SLOT(print()));
  pMsg->addAction(ico_close, tr("&Cancel"), this, SLOT(cancel()));

  QIcon ico_cut(FT_MAKE_ICON(FT_ICON16_EDIT_CUT));
  QIcon ico_copy(FT_MAKE_ICON(FT_ICON16_EDIT_COPY));
  QIcon ico_paste(FT_MAKE_ICON(FT_ICON16_EDIT_PASTE));
  QIcon ico_undo(FT_MAKE_ICON(FT_ICON16_UNDO));
  QIcon ico_redo(FT_MAKE_ICON(FT_ICON16_REDO));

  QMenu* pEdit = new QMenu(tr("Edit"), this);
  pEdit->addAction(ico_cut, tr("Cut"), m_bodyw, SLOT(cut()));
  pEdit->addAction(ico_copy, tr("Copy"), m_bodyw, SLOT(copy()));
  pEdit->addAction(ico_paste, tr("Paste"), m_bodyw, SLOT(paste()));
  pEdit->addAction(ico_undo, tr("Undo"), m_bodyw, SLOT(undo()));
  pEdit->addAction(ico_redo, tr("Redo"), m_bodyw, SLOT(redo()));

  m_ident_menu = new QMenu(tr("Identity"), this);
  load_identities(m_ident_menu);
  if (m_ids.empty()) {
    m_errmsg=tr("No sender identity is defined.\nUse the File/Preferences menu and fill in an e-mail identity to be able to send messages.");
    QMessageBox::warning(NULL, tr("Error"), m_errmsg, QMessageBox::Ok, Qt::NoButton);
  }

  m_pMenuFormat = new QMenu(tr("&Format"), this);
  QAction* action_wrap_lines = m_pMenuFormat->addAction(tr("&Wrap lines"), this, SLOT(toggle_wrap(bool)));
  action_wrap_lines->setCheckable(true);
  action_wrap_lines->setChecked(m_wrap_lines);
  QIcon ico_font(FT_MAKE_ICON(FT_ICON16_FONT));
  m_pMenuFormat->addAction(ico_font, tr("Font"), this, SLOT(change_font()));

  QActionGroup* msg_format_group = new QActionGroup(this);
  msg_format_group->setExclusive(true);
  m_action_plain_text = m_pMenuFormat->addAction(tr("Plain text"), this, SLOT(to_format_plain_text()));
  m_action_plain_text->setCheckable(true);
  m_action_html_text = m_pMenuFormat->addAction(tr("HTML"), this, SLOT(to_format_html_text()));
  m_action_html_text->setCheckable(true);
  m_action_plain_text->setChecked(m_edit_mode == plain_mode);
  m_action_html_text->setChecked(m_edit_mode == html_mode);
  msg_format_group->addAction(m_action_plain_text);
  msg_format_group->addAction(m_action_html_text);

  m_pMenuFormat->addAction(tr("Store settings"), this, SLOT(store_settings()));

  QMenu* menu_display = new QMenu(tr("Display"), this);
  QAction* action_display_tags = menu_display->addAction(tr("Tags panel"), this, SLOT(toggle_tags_panel(bool)));
  action_display_tags->setCheckable(true);
  action_display_tags->setChecked(true);

  QMenuBar* menu=menuBar();
  menu->addMenu(pMsg);
  menu->addMenu(pEdit);
  menu->addMenu(m_ident_menu);
  menu->addMenu(m_pMenuFormat);
  menu->addMenu(menu_display);

  //  topLayout->setMenuBar(menu);
  make_toolbars();

  QString body_html = m_msg.get_body_html();
  if (!body_html.isEmpty()) {
    m_html_edit->set_html_text(body_html);
    m_html_edit->finish_load();
    m_html_edit->setFocus();
    format_html_text();
  }
  else {
    set_body_text(m_msg.get_body_text());
    format_plain_text();    
  }
}

/* Switch to plain text format initiated by the user */
void
new_mail_widget::to_format_plain_text()
{
  if (m_html_edit->isModified()) {
    int res = QMessageBox::Ok;
    res = QMessageBox::warning(this, APP_NAME, tr("The Conversion to plain text format has the effect of loosing the rich text formating (bold, italic, colors, fonts, ...). Please confirm your choice."), QMessageBox::Ok, QMessageBox::Cancel|QMessageBox::Default, Qt::NoButton);
    if (res == QMessageBox::Ok) {
      set_body_text(m_html_edit->to_plain_text());
      format_plain_text();
    }
    else {
      m_action_html_text->setChecked(true);
    }
  }
  else {
    set_body_text(m_html_edit->to_plain_text());
    format_plain_text();
  }
}

/* Switch to html format initiated by the user */
void
new_mail_widget::to_format_html_text()
{
  QString text = mail_displayer::htmlize(m_bodyw->document()->toPlainText());
  text.replace("\n", "<br>\n");  
  m_html_edit->set_html_text(text);
  format_html_text();
}


void
new_mail_widget::format_plain_text()
{
  m_edit_stack->setCurrentWidget(m_bodyw);
  m_edit_mode = plain_mode;
  if (m_toolbar_html1)
    removeToolBar(m_toolbar_html1);
  if (m_toolbar_html2) {
    removeToolBarBreak(m_toolbar_html2);
    removeToolBar(m_toolbar_html2);
  }
  m_action_plain_text->setChecked(true);
}

void
new_mail_widget::format_html_text()
{
  m_edit_stack->setCurrentWidget(m_html_edit);
  m_edit_mode = html_mode;
  if (m_toolbar_html1 && m_toolbar_html1->isHidden()) {
    addToolBar(m_toolbar_html1);
    m_toolbar_html1->show();
  }
  if (m_toolbar_html2 && m_toolbar_html2->isHidden()) {
    addToolBarBreak();
    addToolBar(m_toolbar_html2);
    m_toolbar_html2->show();
  }
  m_action_html_text->setChecked(true);
}

/* Slot. Called when the user chooses to remove a header field editor */
void
new_mail_widget::remove_field_editor()
{
  header_field_editor* ed = dynamic_cast<header_field_editor*>(QObject::sender());
  if (ed != NULL) {
    m_header_editors.removeOne(ed);
    ed->deleteLater();
  }
}

/* Slot. Called when the user chooses to add a header field editor */
void
new_mail_widget::add_field_editor()
{
  // keep the subject at the end of the grid by removing/re-adding
  if (lSubject)
    gridLayout->removeWidget(lSubject);
  if (m_wSubject)
    gridLayout->removeWidget(m_wSubject);

  header_field_editor* ed = new header_field_editor(this);
  ed->set_type(header_field_editor::index_header_to);
  // insert before the last row of the grid, which is always occupied by the subject line
  ed->grid_insert(gridLayout, gridLayout->rowCount(), 0);
  ed->set_value(m_msg.header().m_to);
  connect(ed, SIGNAL(remove()), this, SLOT(remove_field_editor()));
  //  connect(ed, SIGNAL(add()), this, SLOT(add_field_editor()));
  m_header_editors.append(ed);

  int r=gridLayout->rowCount();
  if (lSubject)
    gridLayout->addWidget(lSubject, r , 0);
  if (m_wSubject)
    gridLayout->addWidget(m_wSubject, r, 1);
}

void
new_mail_widget::set_wrap_mode()
{
  if (m_wrap_lines) {
    m_bodyw->setWordWrapMode(QTextOption::WordWrap);
    // m_bodyw->setWrapColumnOrWidth(80);
    m_bodyw->setLineWrapMode(QPlainTextEdit::WidgetWidth);
  }
  else {
    m_bodyw->setLineWrapMode(QPlainTextEdit::NoWrap);
  }
}

void
new_mail_widget::load_identities(QMenu* m)
{
  m_identities_group = new QActionGroup(this);
  m_identities_group->setExclusive(true);

  /* Auto-select our identity as a sender.
     First, if we're replying to a message, try the identity to which this
     message was sent (envelope_from would have been set up by our caller).
     Otherwise get the default identity from the configuration */
  QString default_email = m_msg.header().m_envelope_from;
  if (default_email.isEmpty())
    default_email = get_config().get_string("default_identity");
  if (m_ids.fetch()) {
    identities::iterator iter;
    for (iter = m_ids.begin(); iter != m_ids.end(); ++iter) {
      mail_identity* p = &iter->second;
      QAction* action = m->addAction(p->m_email_addr, this, SLOT(change_identity()));
      m_identities_group->addAction(action);
      action->setCheckable(true);
      if (!default_email.isEmpty() && p->m_email_addr==default_email) {
        action->setChecked(true);
        m_from = default_email;
      }
      m_identities_actions.insert(action, p);
    }
    // if no identity is still defined, use the first one from the list
    if (default_email.isEmpty() && !m_ids.empty() && !m_identities_group->actions().isEmpty()) {
      iter=m_ids.begin();
      QAction* action = m_identities_group->actions().first();
      action->setChecked(true);
      m_from = iter->first;
    }
    // Add "Other" that lets the user enter an identity that is not defined
    // within the preferences
    m_action_identity_other = m->addAction(tr("Other..."), this, SLOT(other_identity()));
    m_identities_group->addAction(m_action_identity_other);
    m_action_identity_other->setCheckable(true);
    m_identities_actions.insert(m_action_identity_other, NULL);
    m_action_edit_other = m->addAction(tr("Edit other..."), this, SLOT(other_identity()));
    // m_action_edit_other is enabled only when the "Other" identity is selected
    m_action_edit_other->setEnabled(false);
  }
  setWindowTitle(tr("New mail from ")+m_from);
}

// User-input identity to be used only for this mail
void
new_mail_widget::other_identity()
{
  DBG_PRINTF(3, "other_identity()");
  identity_widget* w = new identity_widget(this);
  if (!m_other_identity_email.isEmpty())
    w->set_email_address(m_other_identity_email);
  if (!m_other_identity_name.isEmpty())
    w->set_name(m_other_identity_name);
  if (w->exec() == QDialog::Accepted) {
    m_other_identity_email = w->email_address();
    m_other_identity_name = w->name();
    m_action_identity_other->setChecked(true);
    m_action_edit_other->setEnabled(true);
    change_identity(); // will update 'm_from'
  }
  else {
    // if the dialog has been cancelled we need to restore the previous identity in
    // the menu through the action group
    QMap<QAction*,mail_identity*>::const_iterator it;
    for (it=m_identities_actions.begin(); it != m_identities_actions.end(); ++it) {
      if (it.value()!= NULL && it.value()->m_email_addr==m_from) {
	//	it.key()->blockSignals(true);
	it.key()->setChecked(true);
	//	it.key()->blockSignals(false);
	m_action_edit_other->setEnabled(false);
	break;
      }
    }
  }
  w->close();
}

void
new_mail_widget::change_identity()
{
  DBG_PRINTF(3, "change_identity()");
  QString old_from=m_from;
  QString old_sig;
  QString new_sig;

  identities::iterator iter;
  for (iter = m_ids.begin(); iter != m_ids.end(); ++iter) {
    mail_identity* p = &iter->second;
    if (old_from == p->m_email_addr) {
      old_sig = expand_signature(p->m_signature, *p);
    }
  }

  QAction* action = m_identities_group->checkedAction();
  // if the identity is the "other one" (user input) which
  // doesn't belong to m_ids
  if (action==m_action_identity_other) {
    new_sig="";
    m_from = m_other_identity_email;
  }
  else {
    // get at the identity by the action associated to it
    QMap<QAction*, mail_identity*>::const_iterator i = m_identities_actions.find(action);
    if (i!=m_identities_actions.constEnd()) {
      new_sig = (*i)->m_signature;
      new_sig = expand_signature(new_sig, **i);
      m_from = (*i)->m_email_addr;
    }
    m_action_edit_other->setEnabled(false);
  }
  if (old_sig != new_sig && !(old_sig.isEmpty() && new_sig.isEmpty())) {
    // try to locate the old signature at the end of the body
    QString body=m_bodyw->toPlainText();
    int idxb=body.lastIndexOf(old_sig);
    if (idxb>=0) {
      // if located, replace it with the signature of the new identity
      body.replace(idxb, old_sig.length(), new_sig);
      m_bodyw->setPlainText(body);
    }
  }
  DBG_PRINTF(3, "Changing from to %s", m_from.toLatin1().constData());
  setWindowTitle(tr("New mail from ")+m_from);
}

// Edit the current message's private note
void
new_mail_widget::edit_note()
{
  note_widget* w=new note_widget(this);
  w->set_note_text(m_note);
  int ret=w->exec();
  if (ret) {
    m_note=w->get_note_text();
    display_note();
  }
  w->close();
}

void
new_mail_widget::open_global_notepad()
{
  notepad* n = notepad::open_unique();
  if (n) {
    n->show();
    n->activateWindow();
    n->raise();
  }
}

void
new_mail_widget::display_note()
{
  attch_lvitem* lvpItem = dynamic_cast<attch_lvitem*>(m_qAttch->topLevelItem(0));
  uint index=0;
  while (lvpItem) {
    if (!lvpItem->get_attachment()) {
      break;			// note found
    }
    index++;
    lvpItem = dynamic_cast<attch_lvitem*>(m_qAttch->topLevelItem(index));
  }
  if (m_note.isEmpty()) {
    if (lvpItem) {		// case 2
      delete lvpItem;		// remove the note from the listview
      lvpItem=NULL;
    }
  }
  else {
    if (!lvpItem) {		// case 3
      lvpItem = new attch_lvitem(m_qAttch, NULL);
    }
    // case 3 & 4
    lvpItem->set_note(m_note);
    lvpItem->fill_columns();
  }

  // don't show the attachments & note's listview when it's empty
  if (m_qAttch->topLevelItem(0))
    m_qAttch->show();
  else
    m_qAttch->hide();
}


void
new_mail_widget::toggle_wrap(bool wrap)
{
  m_wrap_lines = wrap;
  set_wrap_mode();
}

void
new_mail_widget::toggle_tags_panel(bool show_panel)
{
  if (show_panel)
    m_wtags->show();
  else
    m_wtags->hide();
}

void
new_mail_widget::show_tags()
{
  m_wtags->set_tags(m_msg.get_tags());
}

input_addresses_widget::input_addresses_widget(const QString& addresses)
{
  m_accel_type=0;
  QVBoxLayout *topLayout = new QVBoxLayout(this);
  topLayout->setSpacing(5);
  topLayout->setMargin(5);

  // search
  QHBoxLayout* hb = new QHBoxLayout();
  hb->setSpacing(3);
  topLayout->addLayout(hb);
  QLabel* lb1 = new QLabel(tr("Search for:"));
  hb->addWidget(lb1);
  m_wfind = new QLineEdit();
  hb->addWidget(m_wfind);
  connect(m_wfind, SIGNAL(returnPressed()), SLOT(find_contacts()));
  QPushButton* w_find_btn = new QPushButton(tr("Find"));
  hb->addWidget(w_find_btn);
  connect(w_find_btn, SIGNAL(clicked()), SLOT(find_contacts()));

  QLabel* lb = new QLabel(tr("Enter the email addresses (separated by comma or newline) :"), this);
  topLayout->addWidget(lb);

  m_wEdit = new QTextEdit();
  m_wEdit->setText(format_multi_lines(addresses));
  topLayout->addWidget(m_wEdit);

  QHBoxLayout* h1 = new QHBoxLayout();
  QPushButton* w_recent_to= new QPushButton(tr("Recent To:"));
  h1->addWidget(w_recent_to);
  QPushButton* w_recent_from= new QPushButton(tr("Recent From:"));
  h1->addWidget(w_recent_from);
  QPushButton* w_prio = new QPushButton(tr("Prioritized:"));
  h1->addWidget(w_prio);
  connect(w_recent_to, SIGNAL(clicked()), SLOT(show_recent_to()));
  connect(w_recent_from, SIGNAL(clicked()), SLOT(show_recent_from()));
  connect(w_prio, SIGNAL(clicked()), SLOT(show_prio_contacts()));
  topLayout->addLayout(h1);

  m_addr_list = new QTreeWidget();
  m_addr_list->hide();
  m_addr_list->setColumnCount(2);
  QStringList labels;
  labels << tr("Name and email") << tr("Last");
  m_addr_list->setHeaderLabels(labels);
  //m_addr_list->setAllColumnsShowFocus(true);
  m_addr_list->setRootIsDecorated(false);
/*
  // TODO: fix this code to resize section 0 the available width
  QFontMetrics font_metrics(m_addr_list->font());
  int date_width=font_metrics.width("0000/00/00 00:00");
  m_addr_list->header()->resizeSection(1, date_width);
  m_addr_list->header()->resizeSection(0, m_addr_list->header()->length()-date_width);
*/
  m_addr_list->header()->setResizeMode(QHeaderView::ResizeToContents);
  connect(m_addr_list, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
	  this, SLOT(addr_selected(QTreeWidgetItem*,int)));

  topLayout->addWidget(m_addr_list);

  QHBoxLayout* h = new QHBoxLayout();
  h->setSpacing(10);
  h->setMargin(10);
  QPushButton* wOk = new QPushButton(tr("OK"));
  h->addWidget(wOk);
  h->addStretch(3);
  QPushButton* wCancel = new QPushButton(tr("Cancel"));
  h->addWidget(wCancel);
  topLayout->addLayout(h);
  setWindowTitle("Mail addresses");
  connect(wOk, SIGNAL(clicked()), SLOT(ok()));
  connect(wCancel, SIGNAL(clicked()), SLOT(cancel()));
}

void
input_addresses_widget::find_contacts()
{
  QString s = m_wfind->text();
  if (s.isEmpty())
    return;
  
  m_addr_list->clear();

  mail_address_list result_list;
  if (result_list.fetch_from_substring(s)) {
    mail_address_list::const_iterator iter;
    for (iter = result_list.begin(); iter != result_list.end(); iter++) {
      QString name_and_email = iter->name() + " <" + iter->get() + ">";
      date date1 = iter->last_sent();
      date date2 = iter->last_recv();
      QString d;
      d = (date1.FullOutput() > date2.FullOutput()) ? date1.OutputHM(2):
	date2.OutputHM(2);
      QStringList string_items;
      string_items << name_and_email << d;
      (void)new QTreeWidgetItem(m_addr_list, string_items);
    }
  }
  m_addr_list->setMinimumHeight(130); // arbitrary
  //  m_addr_list->setSorting(1, false); // descending order
  //  m_addr_list->setShowSortIndicator(true);
  m_addr_list->show();
}

/*
  Add to the list an address which the user has selected
*/
void
input_addresses_widget::addr_selected(QTreeWidgetItem* item, int column)
{
  Q_UNUSED(column);
  if (!item) return;
  QString addresses = m_wEdit->toPlainText();
  if (!addresses.isEmpty() && addresses.right(1) != "\n") {
    addresses.append("\n");
  }
  QString new_addr=format_multi_lines(item->text(0));
  addresses.append(new_addr);
  m_wEdit->setText(addresses);
}

/*
  Display email addresses recently used in from or to recipients
  what: 1=to, 2=from, 3=contacts with a positive priority
*/
void
input_addresses_widget::show_recent(int what)
{
  const int fetch_step=10;
  struct accel* accel;
  mail_address_list result_list;
  switch(what) {
  case 1:
    accel=&m_recent_to;
    break;
  case 2:
    accel=&m_recent_from;
    break;
  case 3:
    accel=&m_prioritized;
    break;
  default:
    DBG_PRINTF(5,"ERR: impossible choice");
    return;
  }
  if (what!=m_accel_type) {
    accel->rows_displayed=0;
    m_addr_list->clear();
    //    m_addr_list->setSorting(1, false); // descending order
    //    m_addr_list->setShowSortIndicator(true);
    m_addr_list->setMinimumHeight(130); // arbitrary
  }
  if (result_list.fetch_recent(what, fetch_step, accel->rows_displayed)) {
    mail_address_list::const_iterator iter;
    for (iter = result_list.begin(); iter != result_list.end(); iter++) {
      QString name_and_email = iter->name() + " <" + iter->get() + ">";
      date d = (what==1)?iter->last_sent():iter->last_recv();
      QString sdate = d.OutputHM(2); // US format
      QString col1 = (what==3) ? QString("%1").arg(iter->recv_pri()) : sdate;
      QStringList string_items;
      string_items << name_and_email << col1;
      (void)new QTreeWidgetItem(m_addr_list, string_items);
      accel->rows_displayed++;
    }
    m_addr_list->show();
    m_accel_type=what;
  }
}
void
input_addresses_widget::set_header_col1(const QString& text)
{
  QTreeWidgetItem* item=m_addr_list->headerItem();
  if (item!=NULL) {
    item->setText(1, text);
  }
}


void
input_addresses_widget::show_recent_to()
{
  set_header_col1(tr("When"));
  show_recent(1);
}

void
input_addresses_widget::show_recent_from()
{
  set_header_col1(tr("When"));
  show_recent(2);
}

void
input_addresses_widget::show_prio_contacts()
{
  set_header_col1(tr("Priority"));
  show_recent(3);
}


QString
input_addresses_widget::format_multi_lines(const QString addresses)
{
  std::list<QString> emails_list;
  std::list<QString> names_list;
  QByteArray ba = addresses.toLatin1(); // TODO: replace this when ExtractAddresses takes a QString instead of char*
  mail_address::ExtractAddresses(ba.constData(), emails_list, names_list);
  QString result;

  std::list<QString>::const_iterator iter1,iter2;
  for (iter1 = emails_list.begin(), iter2 = names_list.begin();
       iter1!=emails_list.end() && iter2!=names_list.end();
       ++iter1, ++iter2)
    {
      mail_address addr;
      addr.set_email(*iter1);
      addr.set_name(*iter2);
      result.append(addr.email_and_name());
      result.append("\n");
    }
  return result;
}

void
input_addresses_widget::ok()
{
  emit addresses_available(m_wEdit->toPlainText());
  hide();
}

void
input_addresses_widget::cancel()
{
  hide();
}

void
new_mail_widget::closeEvent(QCloseEvent* e)
{
  int res=QMessageBox::Ok;
  if (m_close_confirm && (m_bodyw->document()->isModified() || m_html_edit->isModified())) {
    res=QMessageBox::warning(this, APP_NAME, tr("The message you are composing in this window will be lost if you confirm."), QMessageBox::Ok, QMessageBox::Cancel|QMessageBox::Default, Qt::NoButton);
  }
  if (res==QMessageBox::Ok)
    e->accept();
  else
    e->ignore();
}

void
new_mail_widget::cancel()
{
  close();
}

void
new_mail_widget::keep()
{
  // TODO
}

void
new_mail_widget::send()
{
  if (m_wtags)
    m_wtags->get_selected(m_msg.get_tags());

  // collect the addresses from the header field editors
  QMap<QString,QString> addresses;  
  QListIterator<header_field_editor*> iter(m_header_editors);
  while (iter.hasNext()) {
    header_field_editor* ed = iter.next();
    QString field_name = ed->get_field_name();
    QString value = expand_aliases(ed->get_value());
    if (!field_name.isEmpty() && !value.isEmpty()) {
      if (addresses.contains(field_name)) {
	// append the value
	addresses[field_name].append(QString(", %1").arg(value));
      }
      else {
	addresses.insert(field_name, value);
      }
    }
  }

  if (!addresses.contains("To") && !addresses.contains("Bcc")) {
    QMessageBox::critical(NULL, tr("Error"), tr("The To: and Bcc: field cannot be both empty"), QMessageBox::Ok, Qt::NoButton);
    return;
  }

  if (m_ids.empty()) {
    QMessageBox::critical(NULL, tr("Error"), tr("The message has no sender (create an e-mail identity in the Preferences)"), QMessageBox::Ok, Qt::NoButton);
    return;
  }

  if (m_wSubject->text().isEmpty()) {
    int res=QMessageBox::warning(this, APP_NAME, tr("The message has no subject.\nSend nonetheless?"), QMessageBox::Ok, QMessageBox::Cancel|QMessageBox::Default, Qt::NoButton);

    if (res!=QMessageBox::Ok)
      return;
  }

  if (m_edit_mode == html_mode) {
    // Collect the attachments refered to by the HTML contents, and generated
    // MIME content-id's for them.
    QStringList local_names = m_html_edit->collect_local_references();
    QMap<QString,QString> map_local_cid;
    attachments_list& alist = m_msg.attachments();
    QStringListIterator it(local_names);

    // For each reference to a distinct local file, we create a new attachment
    while (it.hasNext()) {
      QString filename = it.next();

      // Unicity test, because we don't want to create different attachments
      // for several references to the same file. That may happen for
      // smiley pictures, for example

      if (!map_local_cid.contains(filename)) {
	attachment attch;
	QString external_filename = filename;
	if (external_filename.startsWith("file://", Qt::CaseInsensitive))
	  external_filename = external_filename.mid(strlen("file://")); // remove the scheme
	// TODO: what if we have file:///home/file and /home/file in local_names?
#ifdef Q_OS_WIN
	if (external_filename.startsWith("/")) {
	  external_filename = external_filename.mid(1);
	}
#endif
	attch.set_filename(external_filename);
	attch.get_size_from_file();
	attch.set_mime_type(attachment::guess_mime_type(filename));
	attch.create_mime_content_id();
	alist.push_back(attch);
	map_local_cid[filename] = attch.mime_content_id();
      }
    }

    // Finally, translate the local names to the CIDs references in
    // the HTML document
    if (!map_local_cid.empty()) {
      m_html_edit->replace_local_references(map_local_cid);
    }
  }

  try {
    make_message(addresses);
  }
  catch(QString error_msg) {
    QMessageBox::critical(NULL, tr("Error"), error_msg);
    return;
  }


  if (m_msg.store()) {
    /* If this message was a reply, then tell to the originator mailitem
       to update it's status */
    if (m_msg.inReplyTo() != 0) {
      DBG_PRINTF(5, "refresh_request for %d", m_msg.inReplyTo());
      emit refresh_request(m_msg.inReplyTo());
    }
    else if (m_msg.forwardOf().size() != 0) {
      const std::vector<mail_id_t>& v = m_msg.forwardOf();
      for (uint i=0; i < v.size(); i++) {
	emit refresh_request(v[i]);
      }
    }
    m_close_confirm=false;
    close();
  }
  else {
    QMessageBox::critical(NULL, tr("Error"), "Error while saving the message");
  }
}


void
new_mail_widget::attach_files()
{
  QStringList l=QFileDialog::getOpenFileNames(this, tr("Select files to attach"), m_last_attch_dir);

  QStringList::Iterator it;
  for (it=l.begin(); it!=l.end(); it++) {
    attachment attch;
    attch.set_filename((*it));
    attch.get_size_from_file();
    attch.set_mime_type(attachment::guess_mime_type(*it));
    m_msg.attachments().push_back(attch);
    attachment& attch1 = m_msg.attachments().back();

    // Insert the attachment's listviewitem at the end of the listview
    attch_lvitem* pItem=new attch_lvitem(m_qAttch, &attch1);
    pItem->fill_columns();
  }
  m_qAttch->show();
}

void
new_mail_widget::insert_file()
{
  QString file_contents;
  QString name = QFileDialog::getOpenFileName(this);
  if (name.isEmpty())
    return;

  QFile f(name);
  bool opened=f.open(QIODevice::ReadOnly | QIODevice::Text);
  if (opened) {
    while (!f.atEnd() && !f.error()) {
      char buf[8192+1];
      qint64 n_read;
      while ((n_read=f.read(buf, sizeof(buf)-1)) >0) {
	buf[(uint)n_read] = '\0';
	file_contents += buf;
      }
    }
  }

  if (f.error() || !opened) {
    file_contents=QString::null;
    QString errstr;
    if (f.error()==QFile::OpenError || !opened) {
      errstr=tr("Unable to open file '%1'").arg(name);
    }
    else {
      errstr=tr("Unable to read file '%1': error #%2").arg(name).arg(f.error());
    }
    QMessageBox::information(this, APP_NAME, errstr);
  }

  if (!file_contents.isEmpty()) {
    m_bodyw->insertPlainText(file_contents);
  }
}


void
new_mail_widget::make_message(const QMap<QString,QString>& user_headers)
{
  mail_header& h=m_msg.header();

  m_msg.set_note(m_note);
  QTextDocument* doc = m_bodyw->document();
  if (m_edit_mode == plain_mode) {
    m_msg.set_body_text(doc->toPlainText());
    m_msg.set_body_html("");
  }
  else {
    m_msg.set_body_html(m_html_edit->html_text());
    m_msg.set_body_text(m_html_edit->to_plain_text());
  }

  mail_address addr;
  identities::iterator iter;
  for (iter = m_ids.begin(); iter != m_ids.end(); ++iter) {
    mail_identity* p = &iter->second;
    if (m_from == p->m_email_addr) {
      addr.set_name(p->m_name);
      addr.set_email(p->m_email_addr);
      h.m_xface = p->m_xface;
      break;
    }
  }

  if (iter==m_ids.end()) {
    // if no identity has been found with 'm_ids', it has to mean
    // that the "other" identity has been selected
    if (!m_other_identity_email.isEmpty()) {
      addr.set_email(m_other_identity_email);
      addr.set_name(m_other_identity_name);
    }
    else {
      throw(tr("Error: no sender can be assigned to the message"));
    }
  }

  h.m_from = addr.email_and_name();
  h.m_sender = addr.email();
  h.m_sender_fullname = addr.name();
  h.m_to = user_headers.value("To");
  /* Remove problematic characters from the subject. Despite being a
     single-line edit, it is apparently possible to paste newlines
     into the text. */
  h.m_subject = m_wSubject->text().trimmed();
  h.m_subject.replace("\n\r", " ");
  h.m_subject.replace("\n", " ");
  h.m_subject.replace("\r", " ");
  h.m_subject.replace("\t", " ");

  h.m_cc = user_headers.value("Cc");
  h.m_replyTo = user_headers.value("ReplyTo");
  h.m_bcc = user_headers.value("Bcc");
  m_msg.setStatus(mail_msg::statusOutgoing + mail_msg::statusRead);
}

QString
new_mail_widget::expand_aliases(const QString addresses)
{
  if (addresses.isEmpty())
    return QString("");

  std::list<QString> emails_list;
  std::list<QString> names_list;
  QByteArray ba = addresses.toLatin1(); // TODO: replace this when ExtractAddresses takes a QString instead of char*
  mail_address::ExtractAddresses(ba.constData(), emails_list, names_list);
  QString result;

  std::list<QString>::const_iterator iter1,iter2;
  for (iter1 = emails_list.begin(), iter2 = names_list.begin();
       iter1!=emails_list.end() && iter2!=names_list.end();
       ++iter1, ++iter2)
    {
      mail_address addr;
      bool found;
      if (!(iter1->indexOf('@')==-1 &&
	    addr.fetch_by_nickname(*iter1, &found) && found)) {
	addr.set_email(*iter1);
	addr.set_name(*iter2);
      }
      // else fetch_by_nickname has set up addr contents
      result.append(addr.email_and_name());
      result.append(",");
    }
  // remove the trailing ',' if necessary

  if (!result.isEmpty() && result.at(result.length()-1) == ',')
    result.truncate(result.length()-1);
  return result;
}

void
new_mail_widget::start_edit()
{
  m_bodyw->setFocus();
  m_bodyw->document()->setModified(false);
}

const mail_identity*
new_mail_widget::get_current_identity()
{
  identities::const_iterator iter = m_ids.find(m_from);
  return (iter!=m_ids.end() ? &iter->second : NULL);
}

void
new_mail_widget::insert_signature()
{
  const mail_identity* id = get_current_identity();
  if (id) {
    QString sig = expand_signature(id->m_signature, *id);
    if (sig.at(0)!='\n')
      sig.prepend("\n");
    if (m_edit_mode == plain_mode) {
      // insert the signature and leave the cursor just above
      QTextCursor cursor = m_bodyw->textCursor();
      cursor.movePosition(QTextCursor::End);
      int pos = cursor.position();
      m_bodyw->appendPlainText(sig);
      cursor.setPosition(pos);
      m_bodyw->setTextCursor(cursor);
    }
    else if (m_edit_mode == html_mode) {
      QString html_sig = mail_displayer::htmlize(sig);
      html_sig = "<p><div class=\"manitou-sig\">" + html_sig + "</div>";
      m_html_edit->append_paragraph(html_sig);
    }
  }
}

QString
new_mail_widget::expand_signature(const QString sig, const mail_identity& identity)
{
  QString esig (sig);
  if (esig.isEmpty())
    esig = "\n";
  bool user_fetched = false;
  user u;
  int pos=0;
  QRegExp rx("\\{(\\w+)\\}");
  while ((pos=rx.indexIn(esig, pos))>=0) {
    const QString field = rx.cap(1);
    if (!user_fetched && field.startsWith("operator")) {
      int user_id = user::current_user_id();
      if (user_id>0)
      u.fetch(user_id);
    }

    QString field_val;
    bool use_field = true;

    if (field=="operator_login") {
      field_val = u.m_login;
    }
    else if (field=="operator_firstname") {
      int bpos = u.m_fullname.indexOf(' ');
      if (bpos>=1) {
        field_val = u.m_fullname.left(bpos);
      }
    }
    else if (field=="operator_fullname") {
      field_val = u.m_fullname;
    }
    else if (field=="operator_email") {
      field_val = u.m_email;
    }
    else if (field=="operator_custom_field1") {
      field_val = u.m_custom_field1;
    }
    else if (field=="operator_custom_field2") {
      field_val = u.m_custom_field2;
    }
    else if (field=="operator_custom_field3") {
      field_val = u.m_custom_field3;
    }
    else if (field=="sender_email") {
      field_val = identity.m_email_addr;
    }
    else if (field=="sender_name") {
      field_val = identity.m_name;
    }
    else
      use_field=false;

    if (use_field) {
      esig.replace(pos, rx.matchedLength(), field_val);
      pos += field_val.length();
    }
    else
      pos += rx.matchedLength();
  }
  return esig;
}

void
new_mail_widget::change_font()
{
  bool ok;
  QFont f=QFontDialog::getFont(&ok, m_bodyw->font());
  if (ok) {
    get_config().set_string("newmail/font", f.toString());
    m_bodyw->setFont(f);
  }
}

void
new_mail_widget::store_settings()
{
  // Save the font
  app_config& conf=get_config();
  if (conf.store("newmail/font")) {
    QString user_msg;
    if (conf.name().isEmpty())
      user_msg = QString(tr("The display settings have been saved in the default configuration."));
    else
      user_msg = QString(tr("The display settings have been saved in the '%1' configuration.")).arg(conf.name());
    QMessageBox::information(NULL, tr("Confirmation"), user_msg); 
  }
}

body_edit_widget::body_edit_widget(QWidget* p)
  : QPlainTextEdit(p)
{
//  setTextFormat(Qt::PlainText);
  QString fontname=get_config().get_string("newmail/font");
  if (!fontname.isEmpty() && fontname!="xft") {
    QFont f;
    if (fontname.at(0)=='-')
      f.setRawName(fontname);		// for pre-0.9.5 entries
    else
      f.fromString(fontname);
    setFont(f);
  }
  else {
//    setFont(QFont("courier", 12));
  }
}


void
new_mail_widget::print()
{
  QTextDocument* doc = m_bodyw->document();
  QPrinter printer;
  QPrintDialog *dialog = new QPrintDialog(&printer, this);
  dialog->setWindowTitle(tr("Print Document"));
  if (dialog->exec() != QDialog::Accepted)
    return;
  doc->print(&printer);
}

///
/// identity_widget
///
identity_widget::identity_widget(QWidget* parent): QDialog(parent)
{
  setWindowTitle(tr("Other identity"));
  QVBoxLayout *layout = new QVBoxLayout(this);
  QGridLayout* grid = new QGridLayout();
  layout->addLayout(grid);
  w_email = new QLineEdit();
  w_email->setMinimumWidth(200);
  w_name = new QLineEdit();
  int row=0;
  grid->addWidget(new QLabel(tr("Email address")), row, 0);
  grid->addWidget(w_email, row, 1);
  row++;
  grid->addWidget(new QLabel(tr("Name (optional)")), row, 0);
  grid->addWidget(w_name, row, 1);
  
  QHBoxLayout* buttons_layout = new QHBoxLayout();
  layout->addLayout(buttons_layout);
  buttons_layout->addStretch(3);
  QPushButton* wok = new QPushButton(tr("OK"));
  buttons_layout->addStretch(2);
  QPushButton* wcancel = new QPushButton(tr("Cancel"));
  buttons_layout->addStretch(3);
  connect(wok, SIGNAL(clicked()), SLOT(ok()));
  connect(wcancel, SIGNAL(clicked()), SLOT(cancel()));
  buttons_layout->addWidget(wok);
  buttons_layout->addWidget(wcancel);
}

identity_widget::~identity_widget()
{
}

void
identity_widget::set_email_address(const QString email)
{
  w_email->setText(email);
}

void
identity_widget::set_name(const QString name)
{
  w_name->setText(name);
}

QString
identity_widget::name()
{
  return w_name->text();
}

QString
identity_widget::email_address()
{
  return w_email->text();
}

void
identity_widget::cancel()
{
  this->reject();
}

void
identity_widget::ok()
{
  this->accept();
}

//
// html_source_editor
//

html_source_editor::html_source_editor(QWidget* parent) : QPlainTextEdit(parent)
{
  m_sticker = new QLabel(this);
  m_sticker->setTextFormat(Qt::RichText);
  m_sticker->setText(QString(" <font size=+2>%1</font> ").arg(tr("Source editor")));
  m_sticker->setToolTip(tr("Use the toggle button of the toolbar to go back to visual editor"));
  m_sticker->setAutoFillBackground(true);
  QPalette pal = m_sticker->palette();
  pal.setColor(QPalette::WindowText, QColor(255, 10, 20, 128));
  pal.setColor(QPalette::Background, QColor(255, 239, 52, 100));
  m_sticker->setPalette(pal);
  m_sticker->setFrameStyle(QFrame::StyledPanel);
  QTimer::singleShot(0, this, SLOT(position_label()));
}

void
html_source_editor::position_label()
{
  QRect r = viewport()->contentsRect();
  m_sticker->move(r.x()+r.width()-3-m_sticker->width(),
		  r.y()+5);
}

void
html_source_editor::resizeEvent(QResizeEvent* e)
{
  QPlainTextEdit::resizeEvent(e);
  position_label();
}
