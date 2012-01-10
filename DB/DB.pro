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

INCLUDEPATH += e:/PostgreSQL/include/ e:/PostgreSQL/include/

CONFIG(debug, debug|release) {
 } else {
    SOURCES +=
 }

SOURCES +=  \
    date.cpp \
    db.cpp \
    db_listener.cpp \
    database.cpp \
    sqlquery.cpp \
    creatorConnection.cpp \
    sqlstream.cpp \
    mainfunc.cpp

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
    db_excpt.h \
    IStreamExec.h



#include(PostgreSQL/PostgreSQL.pri)
include(SQLite/SQLite.pri)

OTHER_FILES += \
    ../createDB/createSQLite_DB.sql \
    ../createDB/createPostgreSQL_DB.sql \
    ../createDB/test/Create_Test_DataBase_SQLite.sql \
    ../createDB/test/Create_Test_DataBase_PostgreSQL.sql
