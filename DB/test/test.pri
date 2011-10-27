    LIBS += -L/usr/lib/ -lcppunit

    HEADERS +=  test/main.h \
    test/RegistryTest.h \
    test/testConnectDisconnect.h \
    test/testSqlWriteFields.h \
    test/testDbListener.h \
    test/testBaseTestDB.h \
    test/testTransaction.h \
    test/testMaxCnx.h \
    test/testSqlQuery.h \
    test/testSqlStream.h
    SOURCES +=  test/main.cpp \
    test/testConnectDisconnect.cpp \
    test/testSqlWriteFields.cpp \
    test/testDbListener.cpp \
    test/testBaseTestDB.cpp \
    test/testTransaction.cpp \
    test/testMaxCnx.cpp \
    test/testSqlQuery.cpp \
    test/testSqlStream.cpp