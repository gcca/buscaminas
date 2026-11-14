cmake_minimum_required(VERSION 4.3)
project(buscaminas VERSION 1.0 LANGUAGES CXX)

option(BUSCAMINAS_TERM "Build the terminal frontend" ON)
option(BUSCAMINAS_TESTS "Build the board tests" ON)

if (BUSCAMINAS_TESTS)
  enable_testing()
  include(FetchContent)
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.tar.gz
    URL_HASH SHA256=6e3191c1455468b3fc35a417fb565c1c5071aee1b7e7f85e30cf48a98d37d8b5
    FIND_PACKAGE_ARGS NAMES GTest)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  include(GoogleTest)
endif()

add_subdirectory(buscaminas)

if (BUSCAMINAS_TERM)
  add_subdirectory(term)
endif()
