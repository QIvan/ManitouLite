#include "parser_I/cparser.h"
#include <QtGlobal>
#include <qiodevice.h>
#include <Token>
namespace transman {
    namespace parser {
using namespace transman::I;

int CParser::waitAnswer(int timeout)
{
    setTimedOut( false );
    if( timeout )
	startTimer(timeout * 1000);
    return yylex();
}

void CParser::connectTo(QIODevice* socket)
{
    Q_ASSERT( socket != NULL );
    Q_ASSERT( socket->isOpen() && socket->isReadable() );
    mSocket = socket;
}

CParser::CParser(QIODevice *input_socket) :
    mPopState( false ),
    mFirstArg( 0 ),
    mSecondArg( 0 ),
    mTimedOut( true )
{
    Q_ASSERT(input_socket != NULL );
    connectTo(input_socket);
}

CParser::~CParser()
{

}

void CParser::setPopState(bool  result) {
    mPopState = result;
}

bool CParser::popState() const {
    return mPopState;
}

void CParser::setComment( const QByteArray & comment_ ) {
    mComment = comment_;
}
void CParser::setMultiRowAnswer( const QByteArrayList & new_list ) {
    mMultiRowAnswer = new_list;
}

QByteArrayList CParser::multiRowAnswer() const {
    return mMultiRowAnswer;
}

void CParser::setFirstArg( int arg_ ) {
    mFirstArg = arg_;
}

void CParser::setSecondArg( int arg_ ) {
    mSecondArg = arg_;
}

int CParser::firstArg() const {
    return mFirstArg;
}

int CParser::secondArg() const {
    return mSecondArg;
}

void CParser::timerEvent(QTimerEvent *event) {
    setTimedOut( true );
}

void CParser::setTimedOut( bool is_out ) {
    mTimedOut = is_out;
}

bool CParser::timedOut() const {
    return mTimedOut;
}

bool CParser::verifyAnswer(int num) const {
	if( ( num  == Token::STATUS_REPLY )
	|| ( num  == Token::HELP_MESSAGE )
	|| ( num  == Token::SERVICE_READY )
	|| ( num  == Token::SERVICE_CLOSING )
	|| ( num  == Token::ANSWER_OKAY )
	|| ( num  == Token::USER_FORWARD )
	|| ( num  == Token::CANNOT_VERIFY )
	|| ( num  == Token::START_INPUT )
	|| ( num  == Token::SERVICE_BROKEN )
	|| ( num  == Token::MAIL_ACT_FAIL )
	|| ( num  == Token::MAIL_ACT_ABORT )
	|| ( num  == Token::MAIL_ACT_SYS )
	|| ( num  == Token::SYNTAX_ERROR )
	|| ( num  == Token::SYNTAX_ERROR_ARG )
	|| ( num  == Token::COMMAND_FAIL )
	|| ( num  == Token::BAD_CMD_SEQUENCE )
	|| ( num  == Token::CMD_ARG_FAIL )
	|| ( num  == Token::MAILBOX_FAIL )
	|| ( num  == Token::USER_NOT_LOCAL )
	|| ( num  == Token::MAILBOX_ACT_STRG )
	|| ( num  == Token::MAILBOX_BAD_NAME )
	|| ( num  == Token::NO_SMTP ) )
	    return true;
	return false;
}

QString  CParser::answerComment() const {
    return mComment;
}

    }
}
