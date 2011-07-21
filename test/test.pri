    LIBS += -L/usr/lib/ -lcppunit

    HEADERS +=  test/main.h \
    test/RegistryTest.h \
    test/testConnectDisconnect.h \
    test/testSqlWriteFields.h \
    test/testDbListener.h
    SOURCES +=  test/main.cpp \
    test/testConnectDisconnect.cpp \
    test/testSqlWriteFields.cpp \
    test/testDbListener.cpp
