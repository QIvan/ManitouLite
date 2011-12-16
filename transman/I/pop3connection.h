#ifndef TRANSMAN_I_POP3CONNECTION_H
#define TRANSMAN_I_POP3CONNECTION_H
#include <token.h>

class QString;class QIODevice;
namespace transman {

namespace I {

class POP3Connection
{
public:
    virtual QIODevice *socket() const = 0;
    virtual int portNumber() const = 0;
    virtual void setPortNumber( int number_ ) = 0;
    virtual void setHost( const QString & adress_name ) = 0;
    virtual Token::PopToken sendCommand( const QString & cmd_, int time_out ) = 0;
    virtual Token::PopToken nextToken() = 0;
};

}

}

#endif // TRANSMAN_I_POP3CONNECTION_H
