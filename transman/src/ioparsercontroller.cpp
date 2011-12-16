#include "../I/ioparsercontroller.h"
#include "cparser.h"
#include <QtGlobal>

using namespace transman::I;
namespace transman {
    namespace parser {
IOParser IOParserController::createParser(QIODevice *input_socket ) {
    Q_ASSERT( input_socket != NULL );
    CParser * ParserItem = new CParser( input_socket);
    return IOParser( (IOParserTag *) ParserItem );
}
    }
}
