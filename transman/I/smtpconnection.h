#ifndef TRANSMAN_I_SMTPCONNECTION_H
#define TRANSMAN_I_SMTPCONNECTION_H

namespace transman {

namespace I {

class SmtpConnection
{
public:
    virtual QIODevice *socket() const = 0;
    virtual int portNumber() const = 0;
    virtual void setPortNumber( int number_ ) = 0;
    virtual void setHost( const QString & adress_name ) = 0;
};

}

}

#endif // TRANSMAN_I_SMTPCONNECTION_H
