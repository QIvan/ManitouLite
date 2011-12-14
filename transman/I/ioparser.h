#ifndef TRANSMAN_I_IOPARSER_H
#define TRANSMAN_I_IOPARSER_H
#include <boost/shared_ptr.hpp>
#include <Token>
#include <StartMode>
#include <QByteArray>
#include <QObject>

class QString;class QIODevice;
typedef QList<QByteArray> QByteArrayList;
namespace transman {

namespace I {
class IOParserTag;
typedef boost::shared_ptr<IOParserTag> IOParser;
class IOParserTag: public QObject
{
public:
    virtual int waitAnswer(int timeout = 0) = 0;
    virtual int currentState() const = 0;
    virtual QString answerComment() const = 0;
//tell parser what it wait for
  //POP3
    virtual void waitPOPAnswerStart() = 0;
    virtual void waitPOPComment() = 0;
    virtual void waitPOPMultiLine() = 0;
    virtual void waitPOPNumArg() = 0;
    virtual void waitPOPNumArg2() = 0;
  //POP3 values
    virtual QByteArrayList multiRowAnswer() const = 0;
    virtual bool popState() const = 0;
protected:
    virtual void connectTo(QIODevice * socket) = 0;
};

}

}

#endif // TRANSMAN_I_IOPARSER_H
