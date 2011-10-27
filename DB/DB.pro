#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T16:06:54
#
#-------------------------------------------------

#QT       += core, gui, network, sql
QT += network
CONFIG += staticlib
TARGET = DB
TEMPLATE = lib

LIBS += -L/usr/lib/ -lpq
LIBS += -L/usr/ -lsqlite3
INCLUDEPATH += /usr/include/ , /usr/include/c++/4.4.4/ , /usr/include/postgresql
INCLUDEPATH += ./PostgreSQL , ./SQLite



CONFIG(debug, debug|release) {
 } else {
    SOURCES += main.cpp
 }

SOURCES +=  \
    date.cpp \
    db.cpp \
    db_listener.cpp \
    database.cpp \
    sqlquery.cpp \
    creatorConnection.cpp \

HEADERS += \
    main.h \
    date.h \
    db.h \
    db_listener.h \
    sqlstream.h \
    sqlquery.h \
    dbtypes.h \
    config.h \
    database.h \
    creatorConnection.h \
    connection.h \

#SQLite
#SOURCES +=  \
#    SQLite/sqlite_stream.cpp \
#    SQLite/sqliteConnection.cpp

#HEADERS += \
#    SQLite/sqlite_stream.h \
#    SQLite/sqliteConnection.h

#PostgreSQL
DEFINES  = WITH_PGSQL=1
SOURCES +=  \
    PostgreSQL/psql_stream.cpp \
    PostgreSQL/pgConnection.cpp

HEADERS += \
    PostgreSQL/psql_stream.h \
    PostgreSQL/pgConnection.h \



