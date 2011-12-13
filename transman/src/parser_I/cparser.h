#ifndef TRANSMAN_PARSER_CPARSER_H
#define TRANSMAN_PARSER_CPARSER_H

#include <../../develop/Manitou/transman/I/IOParser>
#undef yyFlexLexer
#include <FlexLexer.h>
#define yyFlexLexerOnce

namespace transman {

namespace parser {

using namespace I;
class CParser : public transman::I::IOParserTag, public yyFlexLexer
{

public:
    virtual void setMode(transman::I::StartMode::Mode mode_);
    virtual transman::I::Token::PopToken currentState() const;
    virtual transman::I::Token::PopToken waitAnswer(int timeout = 0);
    virtual void connectTo(QIODevice* socket);
    CParser(QIODevice *input_socket, StartMode::Mode start_mode);
    virtual ~CParser();
private:
    QIODevice *mSocket;
    StartMode::Mode mMode;
    int yylex();
};

}

}

#endif // TRANSMAN_PARSER_CPARSER_H
