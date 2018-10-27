project(tree)
cmake_minimum_required(VERSION 2.8.)

find_package(Catch2 REQUIRED)

include_directories(include)

set(TEST_LIST
  test/test.cpp)
set(TEST_EXE_NAME ${PROJECT_NAME}_test)

add_executable(${TEST_EXE_NAME} ${TEST_LIST})
target_link_libraries(${TEST_EXE_NAME} Catch2::Catch2)
set_property(TARGET ${TEST_EXE_NAME} PROPERTY CXX_STANDARD 17)

include(CTest)
include(Catch)
catch_discover_tests(${TEST_EXE_NAME})
