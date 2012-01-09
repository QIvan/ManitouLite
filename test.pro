QT += core

TARGET = test
TEMPLATE = app

INCLUDEPATH += ../ , ../DB , c:/cppunit/include/ , e:/PostgreSQL/include/, c:/sqlite/
DEPENDPATH += ../

CPPUNIT_LIB_DIR = c:/cppunit/src/cppunit/.libs

LIBS += $${CPPUNIT_LIB_DIR}/libcppunit.a
CONFIG(debug) {
    LIBS += ../DB/debug/libDB.a
 } else {
    LIBS += ../DB/release/libDB.a
 }
LIBS += e:/PostgreSQL/lib/libpq.a
LIBS += c:/sqlite/libsqlite3.dll.a
#LIBS += -L/usr/lib/ -lpq
#LIBS += -L/usr/lib/ -lsqlite3
#LIBS += -L/c:/cppunit/include/ -lcppunit
#LIBS += -L../DB -lDB

HEADERS +=  main.h \
            RegistryTest.h \
            testConnectDisconnect.h \
            testSqlWriteFields.h \
            testDbListener.h \
            testBaseTestDB.h \
            testTransaction.h \
            testMaxCnx.h \
            testSqlQuery.h \
            testSqlStream.h

SOURCES +=  main.cpp \
            testConnectDisconnect.cpp \
            testSqlWriteFields.cpp \
            testDbListener.cpp \
            testBaseTestDB.cpp \
            testTransaction.cpp \
            testMaxCnx.cpp \
            testSqlQuery.cpp \
            testSqlStream.cpp
