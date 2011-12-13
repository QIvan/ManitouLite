#SQLite
LIBS += -L/usr/lib/ -lsqlite3
INCLUDEPATH += ./SQLite

SOURCES +=  \
    SQLite/sqlite_stream.cpp \
    SQLite/sqliteConnection.cpp

HEADERS += \
    SQLite/sqlite_stream.h \
    SQLite/sqliteConnection.h

DEFINES += NO_PG
