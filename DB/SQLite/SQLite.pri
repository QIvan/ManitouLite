#SQLite

INCLUDEPATH += ./SQLite c:/sqlite/ e:/PostgreSQL/include/
LIBS += c:/sqlite/libsqlite3.dll.a
DEFINES += NO_PG
DEFINES += WITH_SQLITE=1

SOURCES +=  \
    SQLite/sqlite_stream.cpp \
    SQLite/sqliteConnection.cpp \
    SQLite/db_excpt.cpp

HEADERS += \
    SQLite/sqlite_stream.h \
    SQLite/sqliteConnection.h
