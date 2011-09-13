#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T16:06:54
#
#-------------------------------------------------

#QT       += core, gui, network, sql
QT += network

TARGET = manitouPortotype

LIBS += -L/usr/lib/ -lpq
LIBS += -L/usr/ -lsqlite3
INCLUDEPATH += /usr/include/ , /usr/include/c++/4.4.4/ , /usr/include/postgresql
INCLUDEPATH += ./PostgreSQL , ./SQLite

TEMPLATE = app

CONFIG(debug, debug|release) {
    DEFINES  = WITH_PGSQL=1
    LIBS += -L/usr/lib/ -lcppunit
    include (test/test.pri)
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
    PostgreSQL/psql_stream.cpp \
    PostgreSQL/pgConnection.cpp \
    #SQLite/sqlite_stream.cpp \
    SQLite/sqliteConnection.cpp \

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
    PostgreSQL/psql_stream.h \
    PostgreSQL/pgConnection.h \
    #SQLite/sqlite_stream.h \
    SQLite/sqliteConnection.h \
