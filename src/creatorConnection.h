#ifndef CREATECONNECTION_H
#define CREATECONNECTION_H

class pgConnection;

class creatorConnection
{
public:
#ifdef WITH_PGSQL
    pgConnection* connection();
#endif
};

#endif // CREATECONNECTION_H
