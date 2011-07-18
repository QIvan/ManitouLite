#ifndef REGISTRYTEST_H
#define REGISTRYTEST_H
#include <stdlib.h>
#include <cppunit/extensions/HelperMacros.h>

namespace TestNames
{
    std::string db();// { return "DB"; }
    std::string connect();// { return "Connect"; }
};

//этот тест должен быть первым в DB
CPPUNIT_REGISTRY_ADD(TestNames::connect(), TestNames::db());


//этот тест должен быть последний DB
CPPUNIT_REGISTRY_ADD("Disconnect", TestNames::db());


#endif // REGISTRYTEST_H
