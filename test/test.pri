    LIBS += -L/usr/lib/ -lcppunit

    HEADERS +=  test/main.h \
    test/RegistryTest.h \
    test/testConnectDisconnect.h \
    test/testSqlWriteFields.h
    SOURCES +=  test/main.cpp \
    test/testConnectDisconnect.cpp \
    test/testSqlWriteFields.cpp
