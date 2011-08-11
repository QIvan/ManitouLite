#include "creatorConnection.h"
#include "database.h"

#ifdef WITH_PGSQL
pgConnection*
creatorConnection::connection()
{
    return new pgConnection;
}
#endif
