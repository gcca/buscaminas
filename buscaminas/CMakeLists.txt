add_library(buscaminas buscaminas.cpp)
target_include_directories(buscaminas PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(buscaminas PUBLIC cxx_std_14)

if (BUSCAMINAS_TESTS)
  add_executable(buscaminas-test
    buscaminas-test.cc
    board-properties-test.cc
    board-open-test.cc
    test-support-test.cc)
  target_compile_features(buscaminas-test PRIVATE cxx_std_17)
  target_link_libraries(buscaminas-test PRIVATE buscaminas GTest::gmock_main)
  gtest_discover_tests(buscaminas-test)
endif()
