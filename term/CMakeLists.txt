add_executable(buscaminas-term buscaminas-term.cc)
target_compile_features(buscaminas-term PRIVATE cxx_std_20)
find_library(TBOX2 NAMES libtermbox2.a
  PATHS "${CMAKE_CURRENT_SOURCE_DIR}" NO_DEFAULT_PATH REQUIRED)
target_link_libraries(buscaminas-term buscaminas ${TBOX2})
