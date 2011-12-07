#PostgreSQL
INCLUDEPATH += ./PostgreSQL
LIBS += -L/usr/lib/ -lpq
DEFINES += WITH_PGSQL=1
DEFINES += NO_SQLITE


SOURCES +=  \
    PostgreSQL/psql_stream.cpp \
    PostgreSQL/pgConnection.cpp

HEADERS += \
    PostgreSQL/psql_stream.h \
    PostgreSQL/pgConnection.h \



