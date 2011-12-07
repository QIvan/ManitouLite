#ifndef DB_EXCPT_H
#define DB_EXCPT_H

#include <QString>
#include "db.h"

/// sql Exception class
class db_excpt
{
public:
  db_excpt() {}
  db_excpt(const QString query, db_cnx& d);
  db_excpt(const QString query, const QString msg, QString code=QString::null);
  virtual ~db_excpt() {}
  QString query() const { return m_query; }
  QString errmsg() const { return m_err_msg; }
  QString errcode() const { return m_err_code; }
  bool unique_constraint_violation() const {
    return m_err_code=="23505";
  }
private:
  QString m_query;
  QString m_err_msg;
  QString m_err_code;
};

void DBEXCPT(db_excpt& p);	// db.cpp


#endif // DB_EXCPT_H
