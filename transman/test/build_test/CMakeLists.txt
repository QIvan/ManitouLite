project(test_build)
cmake_minimum_required(VERSION 2.6.4)
find_package(Qt4 REQUIRED)
set(QT_DONT_USE_QTGUI TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})

set(test_build_SOURCES
    testbuild.cpp
)


set(test_build_MOC_HEADERS
)

# include_directories(
# )

qt4_wrap_cpp(test_build_MOC_SOURCES ${test_build_MOC_HEADERS})

add_executable(test_build ${test_build_SOURCES} ${test_build_MOC_SOURCES})

add_test(testbuild test_build)

target_link_libraries(test_build 
  ${QT_LIBRARIES}
)
