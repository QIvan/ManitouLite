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
#include "app_config.h"
#include "helper.h"
#include <QMessageBox>
#include <QStringList>
#include <QFile>
#include "prog_chooser.h"
#include <QLocale>

qassistant::qassistant(): QAssistantClient("")
{
  connect(this, SIGNAL(error(const QString&)),
	  this, SLOT(error(const QString&)));
}

qassistant::~qassistant()
{
}

void
qassistant::error(const QString& msg)
{
  QString err=tr("The Qt assistant couldn't be launched:");
  err.append("\n");
  err.append(msg);
#ifndef Q_OS_WIN
  QString fullpath=prog_chooser::find_in_path("assistant");
  if (fullpath.isEmpty()) {
    err.append(tr("\n(No program named 'assistant' found in PATH)"));
  }
#endif
  QMessageBox::critical(NULL, tr("Help error"), err);
}

// translate help topics into HTML help files
//static
struct helper::topic helper::m_topics[] = {
  {"help", "user-interface.html"},
  {"connecting", "ui.invocation.html"},
  {"preferences/display", "ui.preferences.html"},
  {"preferences/identities", "ui.preferences.html#ui.preferences.identities"},
  {"preferences/fetching", "ui.preferences.html#ui.preferences.fetching"},
  {"preferences/mimeviewers", "ui.preferences.html#ui.preferences.mimeviewers"},
  {"preferences/paths", "ui.preferences.html#ui.preferences.paths"},
  {"query selection", "ui.query-form.html"},
  {"filters", "ui.filter-editor.html"},
  {"sql", "sql.html"}
};

//static
bool helper::m_track_mode=false;

//static
QAssistantClient* helper::m_qassistant;

//static
void
helper::show_help(const QString topic)
{
  QString path = get_config().get_string("help_path");
  QString path_tried[3];
  if (path.isEmpty()) {
    extern QString gl_help_path;
    path = gl_help_path;
  }

  if (!path.isEmpty() && path.right(1) == "/")
    path.truncate(path.length()-1); // remove trailing slash

  /* The help path normally refers to the 'help' directory in which
     there are per-language directory like 'en', 'fr',
     but if the user makes it point directly into one of these
     sub-directories, then let's use that instead of guessing the language */

  path_tried[0] = path;
  if (!QFile::exists(path+"/manitou.adp")) {
    QLocale locale;
#if 0 // temporarily disable the help translation until we get it translated :)
    QString lname = locale.name();
    if (lname == "C") 
      lname="en_us";
#else
    QString lname="en_us";
#endif
    // try help_path + language_country
    path_tried[1] = path+"/"+lname;
    if (!QFile::exists(path_tried[1]+"/manitou.adp")) {
      // try help_path + language
      int sep_pos = lname.indexOf('_');
      if (sep_pos>=0) {
	path_tried[2]=path+"/"+lname.left(sep_pos);
	if (!QFile::exists(path_tried[2]+"/manitou.adp")) {
	  path.truncate(0);
	}
	else {
	  path = path_tried[2];
	}
      }
      else
	path.truncate(0);
    }
    else {
      path = path_tried[1];
    }
  }
  // else use path as is

  if (path.isEmpty()) {
    QString msg=QObject::tr("The file 'manitou.adp' could not be found in any of the following directories:\n");
    for (int ii=0; ii<3 && !path_tried[ii].isEmpty(); ii++) {
      msg.append(path_tried[ii]);
      msg.append("\n");
    }
    msg.append(QObject::tr("Please use the Preferences (Paths tab) to enter the directory where help files are located."));
    QMessageBox::warning(NULL, QObject::tr("Unable to locate help"), msg);
    return;
  }

  if (!m_qassistant) {
    m_qassistant = new qassistant();
    QStringList args;
    args << "-profile" << path + "/manitou.adp";
    m_qassistant->setArguments(args);
  }
  QString page;
  for (uint i=0; i<sizeof(m_topics)/sizeof(m_topics[0]); i++) {
    if (topic==m_topics[i].name) {
      page=m_topics[i].page;
      break;
    }
  }
  if (page.isEmpty()) {
//    DBG_PRINTF(2, "WRN: topic '%s' not found\n", (const char*)topic.local8Bit());
    return;
  }
  QString p=path + "/" + page;
//  DBG_PRINTF(4, "showPage('%s')\n", (const char*)p.local8Bit());
  m_qassistant->showPage(path + "/" + page);
}

bool
helper::auto_track_status()
{
  return m_track_mode;
}

void
helper::auto_track(bool on)
{
  m_track_mode=on;
}

void
helper::track(const QString topic)
{
  if (m_track_mode)
    show_help(topic);
}

void
helper::close()
{
  if (m_qassistant)
    m_qassistant->closeAssistant();
}

