#ifndef TRANSMAN_I_IOPARSER_H
#define TRANSMAN_I_IOPARSER_H
#include <boost/shared_ptr.hpp>
#include <Token>
#include <StartMode>
class QIODevice;
namespace transman {

namespace I {
class IOParserTag;
typedef boost::shared_ptr<IOParserTag> IOParser;
class IOParserTag
{
public:
    virtual Token::PopToken waitAnswer(int timeout = 0) = 0;
    virtual Token::PopToken currentState() const = 0;
    virtual void setMode(StartMode::Mode mode_) = 0;
protected:
    virtual void connectTo(QIODevice * socket) = 0;
};

}

}

#endif // TRANSMAN_I_IOPARSER_H
