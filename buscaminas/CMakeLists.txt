cmake_minimum_required(VERSION 3.25)
project(buscaminas VERSION 1.0 LANGUAGES CXX)

add_library(buscaminas buscaminas.cpp)

if (BUSCAMINAS_TESTS)
  find_package(GTest REQUIRED)
  add_executable(buscaminas-test buscaminas-test.cc)
  target_link_libraries(buscaminas-test GTest::gtest_main buscaminas)
  enable_testing()
  gtest_discover_tests(buscaminas-test)
endif()
