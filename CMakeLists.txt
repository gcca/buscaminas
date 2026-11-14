cmake_minimum_required(VERSION 4.3)
project(buscaminas VERSION 1.0 LANGUAGES CXX)

option(BUSCAMINAS_TERM "Build the terminal frontend" ON)
option(BUSCAMINAS_TESTS "Build the board tests" OFF)

if (BUSCAMINAS_TESTS)
  enable_testing()
endif()

add_subdirectory(buscaminas)

if (BUSCAMINAS_TERM)
  add_subdirectory(term)
endif()
