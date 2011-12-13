QT += core

TARGET = test
TEMPLATE = app

INCLUDEPATH += ../ ../DB
DEPENDPATH += ../

LIBS += -L/usr/lib/ -lpq
LIBS += -L/usr/lib/ -lsqlite3
LIBS += -L/usr/lib/ -lcppunit
LIBS += -L../DB -lDB

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
