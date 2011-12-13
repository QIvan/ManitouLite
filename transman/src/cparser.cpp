#include "parser_I/cparser.h"
#include <QtGlobal>
#include <qiodevice.h>

namespace transman {
    namespace parser {
using namespace transman::I;
void CParser::setMode(transman::I::StartMode::Mode mode_)
{

}

transman::I::Token::PopToken CParser::currentState() const
{

}

transman::I::Token::PopToken CParser::waitAnswer(int timeout)
{

}

void CParser::connectTo(QIODevice* socket)
{
    Q_ASSERT( socket != NULL );
    Q_ASSERT( socket->isOpen() && socket->isReadable() );
    mSocket = socket;
}

CParser::CParser(QIODevice *input_socket, StartMode::Mode start_mode) :
    mSocket(NULL),
    mMode(start_mode)
{
    Q_ASSERT(input_socket != NULL );
    connectTo(input_socket);
}

CParser::~CParser()
{

}

    }
}
