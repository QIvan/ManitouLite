#PostgreSQL
INCLUDEPATH += ./PostgreSQL
LIBS += e:/PostgreSQL/lib/libpq.a
DEFINES += WITH_PGSQL=1
DEFINES += NO_SQLITE


SOURCES +=  \
    PostgreSQL/psql_stream.cpp \
    PostgreSQL/pgConnection.cpp \
    PostgreSQL/db_excpt.cpp

HEADERS += \
    PostgreSQL/psql_stream.h \
    PostgreSQL/pgConnection.h \
