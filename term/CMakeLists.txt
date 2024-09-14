cmake_minimum_required(VERSION 3.25)
project(buscaminas VERSION 1.0 LANGUAGES CXX)

add_subdirectory(../buscaminas libbuscaminas)

add_executable(buscaminas-term buscaminas-term.cc)
find_library(TBOX2 libtermbox2.a PATHS .)
target_include_directories(buscaminas-term PRIVATE ../buscaminas)
target_link_libraries(buscaminas-term buscaminas ${TBOX2})
