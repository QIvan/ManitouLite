#SQLite

INCLUDEPATH += ./SQLite
LIBS += -L/usr/lib/ -lsqlite3
DEFINES += NO_PG

SOURCES +=  \
    SQLite/sqlite_stream.cpp \
    SQLite/sqliteConnection.cpp

HEADERS += \
    SQLite/sqlite_stream.h \
    SQLite/sqliteConnection.h

