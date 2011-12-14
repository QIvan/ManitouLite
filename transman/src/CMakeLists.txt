project(transman)
cmake_minimum_required(VERSION 2.6.4)
find_package(Qt4 REQUIRED)
find_package(FLEX REQUIRED)
set(QT_DONT_USE_QTGUI TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})

set(transman_SOURCES
    cparser.cpp
    ioparsercontroller.cpp
    transman.cpp
)


set(transman_MOC_HEADERS
    parser_I/cparser.h
)

FLEX_TARGET(TranScanner transscanner.l ${CMAKE_CURRENT_BINARY_DIR}/transscanner.cpp COMPILE_FLAGS "-8 -++ --yyclass=transman::parser::CParser --noyywrap")
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/parser_I
)

qt4_wrap_cpp(transman_MOC_SOURCES ${transman_MOC_HEADERS})
add_executable(transman ${transman_SOURCES} ${transman_MOC_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/transscanner.cpp)
add_dependencies(transman test_build)

target_link_libraries(transman 
  ${QT_LIBRARIES}
)
