#ifndef TRANSMAN_I_IOPARSERCONTROLLER_H
#define TRANSMAN_I_IOPARSERCONTROLLER_H

#include <IOParser>
#include <StartMode>
class QIODevice;

namespace transman {

namespace parser {

using namespace I;
class IOParserController
{
    static IOParser createParser(QIODevice *input_socket );
};


}

namespace I {
    typedef parser::IOParserController IOParserController;
}


}

#endif // TRANSMAN_I_IOPARSERCONTROLLER_H
