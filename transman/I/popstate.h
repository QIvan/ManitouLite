#ifndef TRANSMAN_I_POPSTATE_H
#define TRANSMAN_I_POPSTATE_H
#include "pop3controller.h"
#include <boost/shared_ptr.hpp>

namespace transman {

namespace I {

class PopStateTag;
typedef boost::shared_ptr<PopStateTag> POP3State;
class PopStateTag
{
public:
    virtual void connect();
    virtual void sendUser();
    virtual void sendPass();
    virtual void commandStat();
    virtual void waitAnsStat();
    virtual void commandList();
    virtual void waitAnsList();
    virtual void commandRetr();
    virtual void waitAnsRetr();
    virtual void waitMessage();
    
    virtual void commandDele();
    virtual void waitAnsDele();
    
    virtual void commandQuit();
    virtual void waitAnsQuit();
    
};

}

}

#endif // TRANSMAN_I_POPSTATE_H
