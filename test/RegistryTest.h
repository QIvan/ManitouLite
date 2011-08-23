#ifndef REGISTRYTEST_H
#define REGISTRYTEST_H
#include <cppunit/extensions/HelperMacros.h>

namespace TestNames
{
    std::string db_api();
    std::string connect();
    std::string disconnect();
    std::string sql_wirte_fields();
    std::string listener();
    std::string transaction();
    std::string max_db_cnx();
    std::string sql_query();
}










//этот тест должен быть первым в DB
CPPUNIT_REGISTRY_ADD(TestNames::connect(), TestNames::db_api());
CPPUNIT_REGISTRY_ADD(TestNames::max_db_cnx(), TestNames::db_api());
CPPUNIT_REGISTRY_ADD (TestNames::sql_query(), TestNames::db_api());
CPPUNIT_REGISTRY_ADD(TestNames::sql_wirte_fields(), TestNames::db_api());
CPPUNIT_REGISTRY_ADD(TestNames::listener(), TestNames::db_api());
CPPUNIT_REGISTRY_ADD(TestNames::transaction(), TestNames::db_api());
//этот тест должен быть последний DB
CPPUNIT_REGISTRY_ADD(TestNames::disconnect(), TestNames::db_api());


#endif // REGISTRYTEST_H
