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


DEBUG {
    #HEADERS +=  test/main.h
    #SOURCES +=  test/main.cpp
}
RELEASE {
    HEADERS += main.h
    SOURCES += main.cpp
}
SOURCES +=  \
    date.cpp \
    db.cpp \
    db_listener.cpp \
    sqlstream.cpp \
    sqlquery.cpp \
    addresses.cpp \

HEADERS += \
    date.h \
    db.h \
    db_listener.h \
    sqlstream.h \
    sqlquery.h \
    addresses.h \
    dbtypes.h \
    config.h \
    database.h \


