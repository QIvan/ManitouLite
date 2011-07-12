#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T16:06:54
#
#-------------------------------------------------

QT       += core, gui, network

TARGET = manitouPortotype
CONFIG   += console

LIBS += -L/usr/lib64/ -lpq
debug {
    DEFINES  = WITH-PQSQL=1
    LIBS += -L/usr/lib/ -lcppunit
}
release {

}



TEMPLATE = app


SOURCES += main.cpp \
    sqlstream.cpp \
    sqlquery.cpp \
    db.cpp \
    db_listener.cpp \
    date.cpp \
    addresses.cpp \

HEADERS += \
    sqlstream.h \
    sqlquery.h \
    dbtypes.h \
    db_listener.h \
    main.h \
    db.h \
    config.h \
    database.h \
    date.h \
    addresses.h \
