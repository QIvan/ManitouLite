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

#ifndef INC_TAGS_H
#define INC_TAGS_H

#include <QString>
#include <list>
#include <map>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>

class QStringList;


/*
  A message_tag, also called just 'tag', is an object that can be used
  to mark and classify mail.  Tags are organized in a tree-like
  hierarchy.
  This class manages the tag definition and properties, not
  the association with any particular mail message.
*/
class message_tag
{
public:
  message_tag(): m_tag_id(0), m_nMsgs(-1), m_parent_id(0) {}
  message_tag(int id, QString name) {
    m_name=name;
    m_tag_id=id;
    m_nMsgs=-1;
    m_parent_id=0;
  }
  virtual ~message_tag() {}
  bool remove();
  bool reload();
  int getId() const { return m_tag_id; }
  int id() const { return m_tag_id; }
  const QString& getName() const { return m_name; }
  const QString& get_name() const { return m_name; }
  const QString& name() const { return m_name; }
  void setName(const QString s) { m_name=s; }
  void set_name(const QString s) { m_name=s; }
  static int nameMaxLength() { return 300; }
  void setNbMsgs(int i) { m_nMsgs=i; }
  int nbMsgs() const { return m_nMsgs; }
  void set_parent_id(int id) { m_parent_id=id; }
  int parent_id() const { return m_parent_id; }
  bool store();
  static bool is_valid_name(const QString name, QString* errmsg=NULL);

  /* return a string representing the tag hierarchy with the separator
     between hierarchical levels as an ascii string, as opposed to
     an unicode character */
  static QString convert_separator_to_ascii(const QString&);

  /* Reverse of convert_separator_to_ascii() */
  static QString convert_separator_from_ascii(const QString&);

private:
  int m_tag_id;
  int m_nMsgs;
  int m_parent_id;
  QString m_name;
  // number of tagged messages, -1 if unknown
};

/*
  Container for a list of tags. Used to contain all the tags of the
  current mail database in static storage.  
  The tags contained are represented by their unique id (tag_id) and
  name, but (TODO) this class should really contain message_tag
  objects instead of those (int,QString)
*/
class tags_repository
{
public:
  static QStringList names_list(std::list<uint>&);
  static std::map<int,message_tag> m_tags_map;
  static int m_tags_map_fetched;
  static void fetch();
  static void reset();
  //  static void get_sorted_list(QStringList*);
  static QString name (int id);
  static QString hierarchy(int id);
};

/* Another container for a list of tags. FIXME: merge with tags_repository */
class tags_definition_list : public std::list<message_tag>
{
public:
  tags_definition_list() : m_bFetched(false) {}
  virtual ~tags_definition_list() {}
  bool fetch(bool force=false);
private:
  bool m_bFetched;
};

class tag_node
{
public:
  tag_node(tag_node* parent=0) : m_parent_id(0), m_id(0), m_parent_node(parent) {}
  virtual ~tag_node();
  const QString name() const {
    return m_name;
  }
  uint id() const {
    return m_id;
  }
  uint parent_id() const {
    return m_parent_id;
  }
  tag_node* parent_node() const {
    return m_parent_node;
  }
  void clear();
  QString hierarchy() const;
  void get_child_tags(tags_definition_list& l);
  const tag_node* find(uint tag_id) const;
  QString m_name;
  std::list<tag_node*> m_childs;
  uint m_parent_id;
  uint m_id;
  tag_node* m_parent_node;
};

class tag_qitem : public QListWidgetItem
{
public:
  tag_qitem(QListWidget* parent, tag_node* n, const QString txt_entry);
  virtual ~tag_qitem();
  uint tag_id() const {
    return m_id;
  }
  uint m_id;
  const tag_node* node() const {
    return m_node;
  }
private:
  tag_node* m_node;
};

class tag_selector : public QComboBox
{
public:
  tag_selector(QWidget* parent);
  virtual ~tag_selector();
  bool init(bool first_is_empty);
  const tag_node* selected_node() const;
  uint current_tag_id() const;
  void set_current_tag_id(uint id);
private:
  void insert_childs(tag_node* n, int level);
  tag_node m_root;
};

#endif // INC_TAGS_H
