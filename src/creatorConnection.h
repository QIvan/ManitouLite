#ifndef CREATECONNECTION_H
#define CREATECONNECTION_H

#include "database.h"

class pgConnection;
class creatorConnection
{
public:
#ifdef WITH_PGSQL
    static pgConnection* connection();

#endif
};

#endif // CREATECONNECTION_H
