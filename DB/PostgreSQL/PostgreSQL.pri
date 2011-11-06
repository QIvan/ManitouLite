#PostgreSQL
DEFINES  = WITH_PGSQL=1
SOURCES +=  \
    PostgreSQL/psql_stream.cpp \
    PostgreSQL/pgConnection.cpp

HEADERS += \
    PostgreSQL/psql_stream.h \
    PostgreSQL/pgConnection.h \
