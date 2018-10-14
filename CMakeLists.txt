project(tree)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
cmake_minimum_required(VERSION 2.8)

find_package(Catch2 REQUIRED)

include_directories(include)

set(TEST_LIST
  test/test.cpp)
set(TEST_EXE_NAME ${PROJECT_NAME}_test)

add_executable(${TEST_EXE_NAME} ${TEST_LIST})
target_link_libraries(${TEST_EXE_NAME} Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(${TEST_EXE_NAME})
