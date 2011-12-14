#ifndef TRANSMAN_PARSER_CPARSER_H
#define TRANSMAN_PARSER_CPARSER_H

#include <IOParser>
#undef yyFlexLexer
#include <FlexLexer.h>
#define yyFlexLexerOnce
#include <QByteArray>
#include <QList>

namespace transman {

namespace parser {

using namespace I;
class CParser : public transman::I::IOParserTag, public yyFlexLexer
{
    Q_OBJECT
public:
    virtual int currentState() const;
    virtual int waitAnswer(int timeout);
    virtual void connectTo(QIODevice* socket);
    CParser(QIODevice *input_socket);
    virtual ~CParser();
    static const int POP3_MAX_MESS_LENGTH = 512;
    static const int SMTP_MAX_MESS_LENGTH = 512;
    virtual void waitPOPAnswerStart();
    virtual void waitPOPComment();
    virtual void waitPOPMultiLine();
    virtual void waitPOPNumArg();
    virtual void waitPOPNumArg2();
    virtual QByteArrayList multiRowAnswer() const;
    virtual int firstArg() const;
    virtual int secondArg() const;
    virtual bool popState() const;
    virtual QString answerComment() const;
protected:
    void setComment( const QByteArray & comment_ );
    void setMultiRowAnswer( const QByteArrayList & new_list );
    void setFirstArg( int arg_ );
    void setSecondArg( int arg_ );
    void setPopState(bool);
    virtual void timerEvent(QTimerEvent *event);
    bool timedOut() const;
    virtual int LexerInput( char* buf, int max_size );
    void setTimedOut( bool out );
    bool verifyAnswer(int) const;
private:
    QIODevice *mSocket;
    StartMode::Mode mMode;
    int yylex();
    bool mPopState;
    QByteArray mComment;
    QByteArrayList mMultiRowAnswer;
    int mFirstArg;
    int mSecondArg;
    bool mTimedOut;
};
}

}

#endif // TRANSMAN_PARSER_CPARSER_H
