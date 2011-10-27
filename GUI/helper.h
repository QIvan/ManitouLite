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

#ifndef INC_HELPER_H
#define INC_HELPER_H

#include <QObject>
#include <QString>
#include <QAssistantClient>

class qassistant : public QAssistantClient
{
  Q_OBJECT
public:
  qassistant();
  virtual ~qassistant();
private slots:
  void error(const QString& msg);
};

class helper
{
public:
  /* Open the helper window and show help for the topic, or a warning
     message if it can't be found. That function is expected to be called 
     when the user presses a "Help" button inside a window providing
     functionalities that might need to be explained */
  static void show_help(const QString topic);

  /* If the helper window is open and in track-mode, go to the topic.
     That function is expected to be called at open time by any window
     for which there is an entry in the help file */
  static void track(const QString topic);

  /* Set or reset the track-mode */
  static void auto_track(bool);

  /* Close any open assistant */
  static void close();

  /* returns whether auto_track is on or off */
  static bool auto_track_status();

private:
  static QAssistantClient* m_qassistant;

  /* If true, help is always displayed and automatically updated to follow
     the application context */
  static bool m_track_mode;

  struct topic {
    const char* name;
    const char* page;
  };
  static struct topic m_topics[];
};

#endif // INC_HELPER_H
