#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T16:06:54
#
#-------------------------------------------------

#QT       += core, gui, network, sql
QT += network

TARGET = manitouPortotype

LIBS += -L/usr/lib64/ -lpq
LIBS += -L/usr/ -lsqlite3
INCLUDEPATH += /usr/include/ , /usr/include/c++/4.4.4/

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
    pgConnection.cpp \
    sqliteConnection.cpp \
    #PostgreSQL/psql_stream.cpp \
    SQLite/sqlite_stream.cpp

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
    pgConnection.h \
    sqliteConnection.h \
    connection.h \
    #PostgreSQL/psql_stream.h \
    SQLite/sqlite_stream.h
