#ifndef ISTREAM_H
#define ISTREAM_H

#include <QString>


class IStreamExec
{
public:
  /** send the query to the server */
  virtual void execute(QString query) = 0;

  /** true if there are no more results to read from the stream */
  virtual int isEmpty() = 0; //no const

  /** count rows  return query */
  virtual int affected_rows() const = 0;

  /** Iterator function */
  virtual QString next_result() = 0;
private:
  /** Iterator function */
  virtual void increment_position() = 0;
};

#endif // ISTREAM_H
