#ifndef CREATECONNECTION_H
#define CREATECONNECTION_H

#include "database.h"

class creatorConnection
{
public:
#ifdef WITH_PGSQL
    static pgConnection* connection()
    {
        return new pgConnection;
    }

#endif
};

#endif // CREATECONNECTION_H
