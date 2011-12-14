%{
#include <cparser.h>
#include <Token>
#include <QIODevice>
using namespace transman::I;
%}
DIGIT [0-9]
/*LETTER [A-Za-z_]*/
CRLF \r\n
%Start POP POP_W_COMMENT POP_MANY_ROWS POP_W_NUM_ARG POP_W_NUM_ARG2
%Start SMTP SMTP_W_COMMENT
%%
<POP>\+OK" "			{ setPopState( true ); return Token::POP_OK; }
<POP>\-ERR" "			{ setPopState( false ); return Token::POP_ERR; }
<POP_W_COMMENT>.*{CRLF}		{ setComment( QByteArray ( yytext, POP3_MAX_MESS_LENGTH )); return Token::POP_COMM_LINE; }
<POP_MANY_ROWS>\.{CRLF}		{ return Token::POP_ROWS_OK; }
<POP_MANY_ROWS>{DIGIT}+" "{DIGIT}+{CRLF} {
				  QByteArray Text( yytext, POP3_MAX_MESS_LENGTH );
				  QByteArrayList Args = Text.split(' ');
				  setFirstArg( Args[0].toInt() );
				  setSecondArg( Args[1].toInt() );
				  return Token::POP_PAIR_ROW;
				}
<POP_MANY_ROWS>.*{CRLF}		{ QByteArrayList List = multiRowAnswer();
				  List.append( QByteArray( yytext, POP3_MAX_MESS_LENGTH ));
				  setMultiRowAnswer( List );
				}
<POP_W_NUM_ARG>{DIGIT}+" "	{
				setFirstArg(QByteArray(yytext, POP3_MAX_MESS_LENGTH).toInt());
				return Token::POP_FIRST_NUM;
				}
<POP_W_NUM_ARG2>{DIGIT}+" "	{
				setSecondArg(QByteArray(yytext, POP3_MAX_MESS_LENGTH).toInt());
				return Token::POP_SECOND_NUM;
				}
<SMTP>{DIGIT}{DIGIT}{DIGIT}" "	{
				QByteArray Answer (yytext, SMTP_MAX_MESS_LENGTH);
				setFirstArg(Answer.mid(0, 3).toInt());
				BEGIN( SMTP_W_COMMENT );
				}
<SMTP_W_COMMENT>.*{CRLF}	{
				setComment( QByteArray ( yytext, SMTP_MAX_MESS_LENGTH )); 
				if( verifyAnswer( firstArg() ) )
				    return firstArg();
				else
				    return Token::BAD_ANSWER;
				}
%%
namespace transman {
    namespace parser {
void CParser::waitPOPAnswerStart() {
    BEGIN( POP );
}

void CParser::waitPOPComment() {
    BEGIN( POP_W_COMMENT );
}

void CParser::waitPOPMultiLine() {
    BEGIN( POP_MANY_ROWS );
}


void CParser::waitPOPNumArg() {
    BEGIN( POP_W_NUM_ARG );
}

void CParser::waitPOPNumArg2() {
    BEGIN( POP_W_NUM_ARG2 );
}

int CParser::currentState() const
{
    return YY_START;
}

int CParser::LexerInput( char* buf, int max_size ) {
    Q_ASSERT( buf != NULL );
    Q_ASSERT( max_size != 0 );
    Q_ASSERT( mSocket != 0 );
    while( !mSocket->bytesAvailable() && !timedOut() )
	sleep(1);
    if( !mSocket->bytesAvailable() )
	return YY_NULL;
    else
	return mSocket->read( buf, max_size );
}

    }
}
