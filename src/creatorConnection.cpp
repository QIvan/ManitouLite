#include "creatorConnection.h"
#ifdef WITH_PGSQL
#include "database.h"
#endif

#ifdef WITH_PGSQL
pgConnection *
creatorConnection::connection()
{
    return new pgConnection;
}
#endif
